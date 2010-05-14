#! /bin/bash

#
# grab the complete source tree from CVS at sourceforge
#

mkdir trunk
pushd trunk

cvs -z3 -d:pserver:anonymous@otl.cvs.sourceforge.net:/cvsroot/otl co -R .


popd

