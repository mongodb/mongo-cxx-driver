#!/bin/sh

#
# Test mongocxx's Debian packaging scripts.
#
# Supported/used environment variables:
#   IS_PATCH    If "true", this is an Evergreen patch build.

set -o errexit

on_exit () {
  if [ -e ./unstable-chroot/debootstrap/debootstrap.log ]; then
    echo "Dumping debootstrap.log"
    cat ./unstable-chroot/debootstrap/debootstrap.log
  fi
}
trap on_exit EXIT

if [ ! -z "${DEB_BUILD_PROFILES}" ]; then
   echo "DEB_BUILD_PROFILES was set; building with profiles: ${DEB_BUILD_PROFILES}"
fi

if [ "${IS_PATCH}" = "true" ]; then
  git diff HEAD -- . ':!debian' > ../upstream.patch
  git diff HEAD -- debian > ../debian.patch
  git clean -fdx
  git reset --hard HEAD
  if [ -s ../upstream.patch ]; then
    [ -d debian/patches ] || mkdir debian/patches
    mv ../upstream.patch debian/patches/
    echo upstream.patch >> debian/patches/series
    git add debian/patches/*
    git commit -m 'Evergreen patch build - upstream changes'
    git log -n1 -p
  fi
  if [ -s ../debian.patch ]; then
    git apply --index ../debian.patch
    git commit -m 'Evergreen patch build - Debian packaging changes'
    git log -n1 -p
  fi
fi
if [ "${DEB_BUILD_PROFILES#*pkg.mongo-cxx-driver.mnmlstc}" != "${DEB_BUILD_PROFILES}" ]; then
   MNMLSTC_DEPS="git ca-certificates"
fi


cd ..

git clone https://salsa.debian.org/installer-team/debootstrap.git debootstrap.git
export DEBOOTSTRAP_DIR=`pwd`/debootstrap.git
sudo -E ./debootstrap.git/debootstrap unstable ./unstable-chroot/ http://cdn-aws.deb.debian.org/debian
cp -a mongo-cxx-driver ./unstable-chroot/tmp/
sudo DEB_BUILD_PROFILES="${DEB_BUILD_PROFILES}" chroot ./unstable-chroot /bin/bash -c "
  (apt-get install -y build-essential git-buildpackage fakeroot debhelper cmake curl ca-certificates libboost-dev libsasl2-dev libicu-dev doxygen ${MNMLSTC_DEPS} && \
  mkdir /tmp/mongo-c-driver && \
  curl -o deb.tar.gz -L https://s3.amazonaws.com/mciuploads/mongo-c-driver/master/mongo-c-driver-debian-packages-latest.tar.gz && \
  tar zxvf deb.tar.gz && \
  apt-get install -y ./*.deb && \
  cd /tmp/mongo-cxx-driver && \
  git clean -fdx && \
  git reset --hard HEAD && \
  python3 etc/calc_release_version.py > build/VERSION_CURRENT && \
  git add --force build/VERSION_CURRENT && \
  git commit build/VERSION_CURRENT -m 'Set current version' && \
  LANG=C /bin/bash -x ./debian/build_snapshot.sh && \
  debc ../*.changes && \
  dpkg -i ../*.deb && \
  /usr/bin/g++ -I/usr/include/bsoncxx/v_noabi -I/usr/include/mongocxx/v_noabi -o runcommand_examples examples/mongocxx/mongodb.com/runcommand_examples.cpp -lmongocxx -lbsoncxx && \
  /usr/bin/g++ -I/usr/include/bsoncxx/v_noabi -I/usr/include/mongocxx/v_noabi -o aggregation_examples examples/mongocxx/mongodb.com/aggregation_examples.cpp -lmongocxx -lbsoncxx && \
  /usr/bin/g++ -I/usr/include/bsoncxx/v_noabi -I/usr/include/mongocxx/v_noabi -o index_examples examples/mongocxx/mongodb.com/index_examples.cpp -lmongocxx -lbsoncxx && \
  /usr/bin/g++ -I/usr/include/bsoncxx/v_noabi -I/usr/include/mongocxx/v_noabi -o documentation_examples examples/mongocxx/mongodb.com/documentation_examples.cpp -lmongocxx -lbsoncxx )"

[ -e ./unstable-chroot/tmp/mongo-cxx-driver/runcommand_examples ] || (echo "Example 'runcommand_examples' was not built!" ; exit 1)
[ -e ./unstable-chroot/tmp/mongo-cxx-driver/aggregation_examples ] || (echo "Example 'aggregation_examples' was not built!" ; exit 1)
[ -e ./unstable-chroot/tmp/mongo-cxx-driver/index_examples ] || (echo "Example 'index_examples' was not built!" ; exit 1)
[ -e ./unstable-chroot/tmp/mongo-cxx-driver/documentation_examples ] || (echo "Example 'documentation_examples' was not built!" ; exit 1)
(cd ./unstable-chroot/tmp/ ; tar zcvf ../../deb.tar.gz *.dsc *.orig.tar.gz *.debian.tar.xz *.build *.deb)

