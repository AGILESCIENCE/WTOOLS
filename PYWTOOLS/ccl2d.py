import sys
import argparse
import cc3d
from astropy.io import fits


DESCRIPTION = 'Connected Components Labelling - Version 2.0'


if __name__ == "__main__":

    #----------------------------------
    # Parse inputs
    #----------------------------------

    # Configure input arguments
    my_parser = argparse.ArgumentParser(prog='ccl2d', description=DESCRIPTION)
    my_parser.add_argument('-i', '--input', action='store', type=str, required=True, help='input file')
    my_parser.add_argument('-o', '--output', action='store', type=str, required=True, help='ouput file')
    my_parser.add_argument('-f', '--filtering', action='store', type=str, required=False, help='filtering parameters, a sequence of param:min:max with param = voxel-no, spatial-ext or scale-ext')
    my_parser.add_argument('-v', '--verbose', action='store_true', help='verbose output')

    # Parse arguments and stop in case of help
    args = my_parser.parse_args(sys.argv[1:])

    # Set filtering parameters default values
    min_voxel_no = -1
    max_voxel_no = -1
    min_spatial_ext = -1
    max_spatial_ext = -1
    min_scale_ext = -1
    max_scale_ext = -1

    # Parse filtering string
    if args.filtering is not None:
        tok = args.filtering.split(':')
        n = 0
        while n < len(tok):
            if tok[n] == 'voxel-no':
                n = n + 1
                min_voxel_no = int(tok[n])
                n = n + 1
                max_voxel_no = int(tok[n])
                n = n + 1
            elif tok[n] == 'spatial-ext':
                n = n + 1
                min_spatial_ext = int(tok[n])
                n = n + 1
                max_spatial_ext = int(tok[n])
                n = n + 1
            elif tok[n] == 'scale-ext':
                n = n + 1
                min_scale_ext = int(tok[n])
                n = n + 1
                max_scale_ext = int(tok[n])
                n = n + 1
            else:
                print('ERROR: wrong parameter name %s' % tok[n])
                quit()

    # ----------------------------------
    # Load input data
    # ----------------------------------

    print(DESCRIPTION)

    if args.verbose:
        print('Load input data from: %s' % args.input)

    try:
        in_hdul = fits.open(args.input)
    except Exception as e:
        print('ERROR: loading input data')
        print(e)

    in_hdr = in_hdul[0].header
    mask = in_hdul[0].data

    # ----------------------------------
    # Get labels
    # ----------------------------------

    # By default 3D connectivity is set to 26
    labels, labels_n = cc3d.connected_components(mask, return_N=True)
    stats = cc3d.statistics(labels)

    if args.verbose:
        print('Number of regions %d' % labels_n)

    # ----------------------------------
    # Filter regions
    # ----------------------------------

    if args.verbose:
        print('Filter regions')
        if min_voxel_no > 0:
            print(' min. voxel no.: %d' % min_voxel_no)
        if max_voxel_no > 0:
            print(' max. voxel no.: %d' % max_voxel_no)
        if min_spatial_ext > 0:
            print(' min. spatial ext.: %d' % min_spatial_ext)
        if max_spatial_ext > 0:
            print(' max. spatial ext.: %d' % max_spatial_ext)
        if min_scale_ext > 0:
            print(' min. scale ext.: %d' % min_scale_ext)
        if max_scale_ext > 0:
            print(' max. scale ext.: %d' % max_scale_ext)

    sel_labels_no = 0
    reg_list = []
    for i in range(0, labels_n):
        voxel_no = stats['voxel_counts'][i]
        if min_voxel_no > 0:
            if voxel_no < min_voxel_no:
                continue
        if max_voxel_no > 0:
            if voxel_no > max_voxel_no:
                continue
        slice_list = stats['bounding_boxes'][i]
        dx = slice_list[0].stop - slice_list[0].start
        dy = slice_list[1].stop - slice_list[1].start
        spatial_ext = max(dx, dy)
        if min_spatial_ext > 0:
            if spatial_ext < min_spatial_ext:
                continue
        if max_spatial_ext > 0:
            if spatial_ext > max_spatial_ext:
                continue
        scale_ext = slice_list[2].stop - slice_list[2].start
        if min_scale_ext > 0:
            if scale_ext < min_scale_ext:
                continue
        if max_scale_ext > 0:
            if scale_ext > max_scale_ext:
                continue
        sel_labels_no += 1

        ct = stats['centroids'][i]

        if args.verbose:
            print('[%06d,%06d] %8d %4d %4d %10.2f %10.2f %10.2f' % (sel_labels_no, i, voxel_no, spatial_ext, scale_ext, ct[0], ct[1], ct[2]))

        radius = spatial_ext / 2

        reg_list.append((ct[2], ct[1], radius))

    # ----------------------------------
    # Save output data
    # ----------------------------------

    if args.verbose:
            print('Save regions to: %s' % args.output)

    f = open(args.output, 'w')
    f.write('#Region file format: DS9 version 4.0\n')
    f.write('#Input: %s\n' % args.input)
    if args.filtering is None:
        f.write('#Filtering: none\n')
    else:
        f.write('#Filtering: %s\n' % args.filtering)

    for reg in reg_list:
        f.write('physical; circle %f %f %f\n' % (reg[0] + 1, reg[1] + 1, reg[2]))



