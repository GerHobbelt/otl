#! /bin/bash

#
# generates each ex123 vcproj project files from a common template: otl_ex___.vcproj
# with a bit of SED and FIND we're on our merry way...
#

for f in ../../otl4_examples/ex*.cpp ; do
    e=`echo $f | sed -e 's,.*/\([^/.]*\)\.cpp,\1,' `
    echo "example: $f --> $e"
    cat otl_ex___.vcproj | sed -e "s,otl_ex6789,$e,g" > $e.vcproj
done

