import sys
import argparse
import numpy as np
from astropy.io import fits
from cwttools import met2d


def save_fits(mask,in_hdr,bin_number,vmin,vmax,th,fname, gcwt):

    # Create primary HDU and copy the header from the input file
    out_hdu = fits.PrimaryHDU(mask, in_hdr)

    # Add custom keywords
    out_hdu.header.set('MBINN', bin_number, 'Histogram bin number')

    for i in range(0, gcwt.shape[0]):
        out_hdu.header.set('MM%03d' % i, vmin[i])
        out_hdu.header.set('MX%03d' % i, vmax[i])
        out_hdu.header.set('MT%03d' % i, th[i])

    # Add comment to mark the custom
    out_hdu.header.add_comment('MET specific parameters', 'MBINN')

    # Save
    out_hdu.writeto(fname, overwrite=True)


DESCRIPTION = 'Maximum Entropy Thresholding - Version 2.0'

def main(input, output, bin_number, save_sum, save_or, verbose):

    # Set optional parameters default values
    if bin_number is None:
        bin_number = 100

    # ----------------------------------
    # Load input data
    # ----------------------------------

    print(DESCRIPTION)

    if verbose:
        print('Load input data from: %s' % input)

    try:
        in_hdul = fits.open(input)
    except Exception as e:
        print('ERROR: loading input data')
        print(e)
        quit()

    in_hdr = in_hdul[0].header
    gcwt = in_hdul[0].data

    # ----------------------------------
    # Apply MET
    # ----------------------------------

    if verbose:
        print("Apply MET")

    vmin, vmax, th, mask = met2d(gcwt, bin_number)

    if verbose:
        for i in range(0, gcwt.shape[0]):
            print('    [%04d] min %8.3e max %8.3e th %8.3e' % (i, vmin[i], vmax[i], th[i]))

    # ----------------------------------
    # Save output data
    # ----------------------------------

    # Save MET mask --------------------
    if verbose:
        print('Save MET mask to: %s' % output)

    save_fits(mask, in_hdr, bin_number, vmin, vmax, th, output, gcwt)

    # Save sum of the MET mask ---------
    if save_sum:

        if verbose:
            print('Save sum of MET mask to: %s' % output+'.sum')

        mask_sum = np.zeros_like(mask[0], dtype='b')
        for i in range(0, mask.shape[0]):
            mask_sum += mask[i]

        save_fits(mask_sum, in_hdr, bin_number, vmin, vmax, th, output+'.sum', gcwt)

    # Save or of the MET mask ----------
    if save_or:

        if verbose:
            print('Save or of MET mask to: %s' % output + '.or')

        mask_or = np.zeros_like(mask[0], dtype='b')
        for i in range(0, mask.shape[0]):
            mask_or += mask[i]

        mask_or[mask_or > 0] = 1

        save_fits(mask_or, in_hdr, bin_number, vmin, vmax, th, output + '.or', gcwt)


if __name__ == "__main__":

    #----------------------------------
    # Parse inputs
    #----------------------------------

    # Configure input arguments
    my_parser = argparse.ArgumentParser(prog='met2d', description=DESCRIPTION)
    my_parser.add_argument('-i', '--input', action='store', type=str, required=True, help='input file')
    my_parser.add_argument('-o', '--output', action='store', type=str, required=True, help='ouput file')
    my_parser.add_argument('-n', '--bin-number', action='store', type=int, required=False, help='histogram bin number')
    my_parser.add_argument('-s', '--save-sum', action='store_true', help='save sum mask')
    my_parser.add_argument('-r', '--save-or', action='store_true', help='save or mask')
    my_parser.add_argument('-v', '--verbose', action='store_true', help='verbose output')

    # Parse arguments and stop in case of help
    args = my_parser.parse_args(sys.argv[1:])

    main(args.input, args.output, args.bin_number, args.save_sum, args.save_or, args.verbose)
