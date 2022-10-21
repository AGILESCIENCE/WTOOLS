#!/usr/bin/env bash

trap exit ERR

### check environment variables
if [ -z "$AGILE" ] || [ -z $(env | grep "AGILE=") ] ; then
    echo "AGILE environment variable not set. Abort."
    exit
fi

echo "install scripts"
test -d $AGILE/scripts || mkdir -p $AGILE/scripts
cp -rf PYWTOOLS $AGILE/scripts
