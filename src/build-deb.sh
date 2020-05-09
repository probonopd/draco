#!/bin/bash

# This creates a deb for easy testing.
# This deb is not made according to the Debian policies.
# Any improvement that does not make the build process more complicated is welcome.

sudo rm -rf debian/ || true

# -DCMAKE_INSTALL_LIBDIR="lib/x86_64-linux-gnu"
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBEXECDIR=bin -DCMAKE_BUILD_TYPE=Release ..
make -j ${nproc}

PROGRAMNAME=draco
mkdir -v -p debian/DEBIAN
sudo make DESTDIR=debian install

# Only build deb for one of the builds
if [  "$CXX" != "clang" ] ; then
  exit 0
fi

VERSION=$(ls debian/usr/lib/x86_64-linux-gnu/libDraco.so.* | tail -n 1 | cut -d / -f 5 |  sed  -e 's|libDraco.so.||g')
ARCH=amd64
OUTPUT_FILENAME="${PROGRAMNAME}_${VERSION}_${ARCH}.deb"

sudo find debian/usr/bin -type f -exec strip {} \;
sudo find debian/usr/lib -type f -exec strip {} \;

sudo chmod +x debian/etc/X11/xinit/xinitrc.draco

echo "" | gzip -9 - -c -f > debian/usr/share/doc/${PROGRAMNAME}/changelog.gz
chmod 0644 debian/usr/share/doc/${PROGRAMNAME}/changelog.gz

INSTALLED_SIZE=$(du -s debian/usr | awk '{x=$1/1024; i=int(x); if ((x-i)*10 >= 5) {f=1} else {f=0}; print i+f}')
echo "size=${INSTALLED_SIZE}"

echo "Package: ${PROGRAMNAME}
Version: ${VERSION}
Priority: optional
Architecture: ${ARCH}
Depends: libc, bash (>=4.0)
Installed-Size: ${INSTALLED_SIZE}
Maintainer: No Name <noname@gmail.com>
Description: Draco is a simple and lightweight desktop environment.
 While small still features XDG integration, freedesktop services and integration,
 power and storage management, desktop, panels, multi-monitor support and much more.
 Draco does not include any user applications." > debian/DEBIAN/control

fakeroot dpkg-deb --build debian

mv -v debian.deb "${OUTPUT_FILENAME}"

# Show the remaining deficiencies
lintian "${OUTPUT_FILENAME}" || true
