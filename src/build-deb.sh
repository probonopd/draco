#!/bin/bash

# This creates a deb for easy testing.
# This deb is not made according to the Debian policies.
# This is intentional, because Debian policy would require us to split up this
# package into multiple packages, which would make everything more complicated.
# Any improvement that does not make the build process more complicated is welcome.

rm -rf debian/ || true

# FIXME: Why do we need to specify -DCMAKE_INSTALL_LIBDIR="lib/x86_64-linux-gnu"?
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR="lib/x86_64-linux-gnu" -DCMAKE_INSTALL_LIBEXECDIR=bin -DCMAKE_BUILD_TYPE=Release ..
make -j ${nproc}

PROGRAMNAME=draco

mkdir -p debian/usr/share/doc/${PROGRAMNAME}/
echo "" | gzip -9 - -c -f > debian/usr/share/doc/${PROGRAMNAME}/changelog.gz
chmod 0644 debian/usr/share/doc/${PROGRAMNAME}/changelog.gz

make DESTDIR=debian install

# Only build deb for one of the builds
if [  "$CXX" != "clang++" ] ; then
  exit 0
fi

VERSION=$(ls debian/usr/lib/x86_64-linux-gnu/libDraco.so.* | tail -n 1 | cut -d / -f 5 |  sed  -e 's|libDraco.so.||g')
ARCH=amd64
OUTPUT_FILENAME="${PROGRAMNAME}_${VERSION}_${ARCH}.deb"

mkdir -v -p debian/DEBIAN
echo "Package: ${PROGRAMNAME}
Version: ${VERSION}
Priority: optional
Architecture: ${ARCH}
Depends: bash (>=4.0), libxcb-ewmh2, libxcb-composite0, libxcb-damage0, libqt5concurrent5, qterminal, pnmixer, openbox, hicolor-icon-theme, xscreensaver, xdg-utils, openbox, udisks2, upower
Installed-Size: ${INSTALLED_SIZE}
Maintainer: No Name <noname@gmail.com>
Description: Draco is a simple and lightweight desktop environment.
 While small still features XDG integration, freedesktop services and integration,
 power and storage management, desktop, panels, multi-monitor support and much more.
 Draco does not include any user applications." > debian/DEBIAN/control

find debian/usr/bin -type f -exec strip {} \;
find debian/usr/lib -type f -exec strip {} \;

chmod +x debian/etc/X11/xinit/xinitrc.draco

INSTALLED_SIZE=$(du -s debian/usr | awk '{x=$1/1024; i=int(x); if ((x-i)*10 >= 5) {f=1} else {f=0}; print i+f}')
echo "size=${INSTALLED_SIZE}"

sudo chown -R root debian/
fakeroot dpkg-deb --build debian

mv -v debian.deb "${OUTPUT_FILENAME}"

# Show the remaining deficiencies
lintian "${OUTPUT_FILENAME}" || true
