#!/bin/sh
#< build-me.sh - 20130401 - test-btg project
BN=`basename $0`
TMPPRJ="test-btg"
TMPLOG="bldlog-1.txt"

if [ -f "$TMPLOG" ]; then
    rm -f -v $TMPLOG
fi

echo "$BN: Build of $TMPPRJ" > $TMPLOG

CMOPTS="-DCMAKE_PREFIX_PATH=/media/Disk2/FG/fg21/install/simgear"
export OSG_DIR=/media/Disk2/FG/fg21/install/OSGtrunk
echo "$BN: exported OSG_DIR=$OSG_DIR"
echo "$BN: exported OSG_DIR=$OSG_DIR" >> $TMPLOG
export SIMGEAR_DIR=/media/Disk2/FG/fg21/install/simgear
echo "$BN: exported SIMGEAR_DIR=$SIMGEAR_DIR"
echo "$BN: exported SIMGEAR_DIR=$SIMGEAR_DIR" >> $TMPLOG

echo "$BN: doing 'cmake .. $CMOPTS'"
echo "$BN: doing 'cmake .. $CMOPTS'" >> $TMPLOG
cmake .. $CMOPTS >> $TMPLOG 2>&1
if [ ! "$?" = "0" ]; then
    echo "$BN: Appears a cmake configuration or generation error - see $TMPLOG"
    exit 1
fi

if [ ! -f "Makefile" ]; then
    echo "$BN: No Makefile generated!"
    exit 1
fi

echo "$BN: doing 'make'"
echo "$BN: doing 'make'" >> $TMPLOG
make >> $TMPLOG 2>&1
if [ ! "$?" = "0" ]; then
    echo "$BN: Appears a make error - see $TMPLOG"
    exit 1
fi

echo "$BN: Appears a successful build of $TMPPRJ"

# eof

