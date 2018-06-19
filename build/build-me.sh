#!/bin/sh
#< build-me.sh - 20161010 - 20130401 - test-btg project
# 20180619 - Add user options
BN=`basename $0`
TMPPRJ="test-btg"
TMPLOG="bldlog-1.txt"
TMPINST="$HOME"
TMPSG="/home/geoff/fg/next/install/simgear"
#TMPSG="/media/Disk2/FG/fg21/install/simgear"
TMPOSG="/home/geoff/fg/next/install/openscenegraph"
TMPSRC=".."

if [ -f "$TMPLOG" ]; then
    rm -f -v $TMPLOG
fi
if [ ! -d "$TMPSG" ]; then
    echo "$BN: Unable to locate SG install $TMPSG"
    exit 1
fi

wait_for_input()
{
    if [ "$#" -gt "0" ] ; then
        echo "$1"
    fi
    echo -n "Enter y to continue : "
    read char
    if [ "$char" = "y" -o "$char" = "Y" ]
    then
        echo "Got $char ... continuing ..."
    else
        if [ "$char" = "" ] ; then
            echo "Aborting ... no input!"
        else
            echo "Aborting ... got $char!"
        fi
        exit 1
    fi
    # exit 0
}
BLDDBG=0
CMOPTS=""
##############################################
### ***** NOTE THIS INSTALL LOCATION ***** ###
### Change to suit your taste, environment ###
TMPINST="$HOME"
CMOPTS="$CMOPTS -DCMAKE_INSTALL_PREFIX=$TMPINST"
##############################################

CMOPTS="$CMOPTS -DCMAKE_PREFIX_PATH=$TMPSG"
# CMOPTS="$CMOPTS -DCMAKE_INSTALL_PREFIX=$TMPINST"
CMOPTS="$CMOPTS -DUSE_EXTERN_ZLIB:BOOL=ON"
BLDDBG=0
TMPOPTS=""
### Accept user argument
for arg in $@; do
      case $arg in
         VERBOSE) TMPOPTS="$TMPOPTS -DCMAKE_VERBOSE_MAKEFILE=ON" ;;
         DEBUG) BLDDBG=1 ;;
         SHARED) TMPOPTS="$TMPOPTS -DBUILD_SHARED_LIB:BOOL=TRUE" ;;
         *) TMPOPTS="$TMPOPTS $arg" ;;
      esac
done

if [ "$BLDDBG" = "1" ]; then
    TMPOPTS="$TMPOPTS -DCMAKE_BUILD_TYPE=Debug"
    # -DENABLE_DEBUG_SYMBOLS:BOOL=TRUE
else
    TMPOPTS="$TMPOPTS -DCMAKE_BUILD_TYPE=Release"
fi

CMOPTS="$CMOPTS $TMPOPTS"

echo "$BN: Will do: 'cmake $TMPSRC $CMOPTS' to $TMPLOG"

wait_for_input

echo "$BN: Build of $TMPPRJ" > $TMPLOG

# 
export OSG_DIR="$TMPOSG"
### export OSG_DIR=/media/Disk2/FG/fg21/install/OSGtrunk
echo "$BN: exported OSG_DIR=$OSG_DIR"
echo "$BN: exported OSG_DIR=$OSG_DIR" >> $TMPLOG
export SIMGEAR_DIR=$TMPSG
echo "$BN: exported SIMGEAR_DIR=$SIMGEAR_DIR"
echo "$BN: exported SIMGEAR_DIR=$SIMGEAR_DIR" >> $TMPLOG

echo "$BN: doing 'cmake $TMPSRC $CMOPTS'"
echo "$BN: doing 'cmake $TMPSRC $CMOPTS'" >> $TMPLOG
cmake $TMPSRC $CMOPTS >> $TMPLOG 2>&1
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

echo "" >> $TMPLOG
echo "$BN: Appears a successful build of $TMPPRJ" >> $TMPLOG
echo "" >> $TMPLOG
echo "$BN: Time for 'make install' to install to $TMPINST/bin, if desired..." >> $TMPLOG
echo "" >> $TMPLOG

echo ""
echo "$BN: Appears a successful build of $TMPPRJ"
echo ""
echo "$BN: Time for 'make install' to install to $TMPINST/bin, if desired..."
echo ""

# eof

