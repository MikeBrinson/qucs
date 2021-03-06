#!/bin/bash


if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(date +"%y%m%d")
  RELEASE="0.0.17."${RELEASE:0:6}
fi
echo Building release: $RELEASE

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi

echo exporting git tree...
git clone ./ release/
mv release/qucs-core release/qucs/
mv release/qucs release/qucs-$RELEASE
rm -rf release/.git

cd release/qucs-$RELEASE
sed -i 's/# AC_CONFIG_SUBDIRS(qucs-core)/AC_CONFIG_SUBDIRS(qucs-core)/g' configure.ac
sed -i 's/# RELEASEDIRS="qucs-core"/RELEASEDIRS="qucs-core"/g' configure.ac
./autogen.sh
make distclean
rm -rf autom4te.cache


cd qucs-core
./autogen.sh
make
./configure
make distclean
rm -rf autom4te.cache
cd ..
cd ..

echo creating source archive...

tar -zcvf qucs-$RELEASE.tar.gz qucs-$RELEASE

DISTS="precise quantal"

#wget https://launchpad.net/~fransschreuder1/+archive/qucs/+files/qucs_0.0.17.orig.tar.gz
cp qucs-$RELEASE.tar.gz qucs_$RELEASE.orig.tar.gz

cd qucs-$RELEASE
COUNT=-0 #last version number in repository
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	dch -D $DIST -m -v $RELEASE$COUNT -b
	debuild -S -k8AD5905E
	./configure 
done
