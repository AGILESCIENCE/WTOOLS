import sys
import argparse
import numpy as np
from astropy.io import fits
from cwttools import MexicanHat2d, cwt2d, icwt2d


DESCRIPTION = 'Real Isotropic Continuous Wavelet Trasform 2D - Version 3.0'


if __name__ == "__main__":

    #----------------------------------
    # Parse inputs
    #----------------------------------

    #Configure input arguments
    my_parser = argparse.ArgumentParser(prog='cwt2d', description=DESCRIPTION)
    my_parser.add_argument('-w', '--wavelet', action='store', type=str, required=False, default='log',
                           help='wavelet family, for the time being only Laplacian of Gaussian (log) is implemented')
    my_parser.add_argument('-s', '--scales', action='store', type=str, required=True,
                           help='wavelet scales, type:num:min:max, type can be linear or dyadic, size is the number of scales, min is the minimum scale in px, max is the max scale in px')
    my_parser.add_argument('-i', '--input', action='store', type=str, required=True, help='input file')
    my_parser.add_argument('-o', '--output', action='store', type=str, required=True, help='ouput file')
    my_parser.add_argument('-v', '--verbose', action='store_true', help='verbose output')

    # Parse arguments and stop in case of help
    args = my_parser.parse_args(sys.argv[1:])

    # ----------------------------------
    # Configure transform and scales
    # ----------------------------------

    print(DESCRIPTION)

    # Wavelet type
    if args.verbose:
        print('Wavelet type: %s' % args.wavelet)

    psi = None
    if args.wavelet == 'log':
        psi = MexicanHat2d()
    else:
        print('ERROR: unsupported wavelet type')
        quit()

    # Scales
    if args.verbose:
        print('Scales: %s' % args.scales)

    tok = args.scales.split(':')
    if len(tok) != 4:
        print('ERROR: unable to split scales parameters')
        quit()

    stype = tok[0]
    snum = int(tok[1])
    smin = float(tok[2])
    smax = float(tok[3])

    scales = None
    if stype == 'linear':
        scales = np.linspace(smin, smax, num=snum)
    elif stype == 'dyadic':
        smin = np.log2(smin)
        smax = np.log2(smax)
        x = np.linspace(smin, smax, num=snum)
        scales = np.exp2(x)
    else:
        print('ERROR: unsupported scales type')
        quit()

    if args.verbose:
        for i in range(0, len(scales)):
            print('    [%04d] %e' % (i, scales[i]))

    # ----------------------------------
    # Load input data
    # ----------------------------------

    if args.verbose:
        print('Load input data from: %s' % args.input)

    try:
        in_hdul = fits.open(args.input)
    except Exception as e:
        print('ERROR: loading input data')
        print(e)
        quit()

    in_hdr = in_hdul[0].header
    g = in_hdul[0].data

    # ----------------------------------
    # Compute CWT
    # ----------------------------------

    if args.verbose:
        print("Compute CWT")

    gcwt, gmean = cwt2d(psi, scales, g, 1.0)

    # ----------------------------------
    # Save output data
    # ----------------------------------

    if args.verbose:
        print('Save CWT to: %s' % args.output)

    # Create primary HDU and copy the header from the input file
    out_hdu = fits.PrimaryHDU(gcwt, in_hdr)

    # Add custom keywords
    out_hdu.header.set('WTYPE', args.wavelet, 'Wavelet type')
    out_hdu.header.set('WSCALE', stype, 'Wavelet scale')
    out_hdu.header.set('WSIZE', snum, 'Number of scales considered')
    out_hdu.header.set('WSMIN', smin, 'Min scale')
    out_hdu.header.set('WSMAX', smax, 'Max scale')
    for i in range(0, len(scales)):
        out_hdu.header.set('WS%03d' % i, scales[i])

    # Save also the mean value of the input data
    out_hdu.header.set('WMEAN', gmean, 'Mean of the input data')

    # Add comment to mark the custom
    out_hdu.header.add_comment('Wavelet transform specific parameters', 'WTYPE')

    # Save
    out_hdu.writeto(args.output, overwrite=True)

    print("End")


