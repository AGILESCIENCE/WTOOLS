WTOOLS
======

Wavelet tools for AGILE satellite gamma-ray data analysis

## Dependencies
These env variables must be set:
* AGILE
* CFITSIO
* GSL

## Installing
* git clone https://github.com/AGILESCIENCE/WTOOLS
* cd WTOOLS
* <load modules AGILE,CFITSIO, GSL>
* ./install.sh

The installation procedure will create ccl, cwt2 and met executables under $AGILE/bin and will copy the script files under $AGILE/scripts. 
