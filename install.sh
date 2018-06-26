#!/usr/bin/env bash

trap exit ERR

if [ "$1" == "-j" ] ; then
    parallel=$1
fi

### check environment variables
if [ -z "$AGILE" ] || [ -z $(env | grep "AGILE=") ] ; then
    echo "AGILE environment variable not set. Abort."
    exit
fi
if [ -z "$CFITSIO" ] || [ -z $(env | grep "CFITSIO=") ] ; then
    echo "CFITSIO environment variable not set. Abort."
    exit
fi
if [ -z "$GSL" ] || [ -z $(env | grep "GSL=") ] ; then
    echo "GSL environment variable not set. Abort."
    exit
fi

CPP_WTOOLS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/C++WTOOLS"


if [ "$1" == "clean" ] ; then
    cd $CPP_WTOOLS_DIR/CCL
    make clean
    cd $CPP_WTOOLS_DIR/CWT2
    make clean
    cd $CPP_WTOOLS_DIR/MET
    make clean
    exit
fi

echo "install CCL"
cd $CPP_WTOOLS_DIR/CCL
make ${parallel} install prefix=$AGILE
cd ..

echo "install CWT2"
cd $CPP_WTOOLS_DIR/CWT2
make ${parallel} install prefix=$AGILE
cd ..

echo "install MET"
cd $CPP_WTOOLS_DIR/MET
make ${parallel} install prefix=$AGILE
cd ..


echo "install scripts"
test -d $AGILE/scripts || mkdir -p $AGILE/scripts
cp -rf scripts/* $AGILE/scripts
