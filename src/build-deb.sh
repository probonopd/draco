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

##########################################################################################
# For a more complete out-of-the-box experience, also build and package QtFM
##########################################################################################

sudo apt-get -y install libqt5widgets5 libqt5gui5 libqt5concurrent5 libqt5dbus5 libqt5core5a ffmpeg imagemagick udisks2 hicolor-icon-theme adwaita-icon-theme libavdevice57 libswscale4 libavformat57 libavcodec57 libavutil55 libmagickwand-dev libmagickcore-dev libmagick++-dev tree cmake git build-essential qtbase5-dev qt5-qmake libmagick++-dev libavdevice-dev libswscale-dev libavformat-dev libavcodec-dev libavutil-dev autogen

git clone https://github.com/rodlie/qtfm --depth 1
cd qtfm

mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=/usr  -DENABLE_MAGICK=false -DENABLE_FFMPEG=false -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
make DESTDIR=../../debian install

cd ../..

##########################################################################################

INSTALLED_SIZE=$(du -s debian/usr | awk '{x=$1/1024; i=int(x); if ((x-i)*10 >= 5) {f=1} else {f=0}; print i+f}')
echo "size=${INSTALLED_SIZE}"

sudo chown -R root debian/
fakeroot dpkg-deb --build debian

mv -v debian.deb "${OUTPUT_FILENAME}"

# Show the remaining deficiencies
lintian "${OUTPUT_FILENAME}" || true
