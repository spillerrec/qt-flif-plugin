# qt-flif-plugin

Enables Qt4 and Qt5 applications to load images in the FLIF image format.
FLIF is a new lossless image format providing superior compression performance compared to outdated formats like PNG. For more information on FLIF, see their website: http://flif.info/

### Features

- Reading FLIF images
- Animation

### TODO

- Making sure loop count and frame delay is interpreted correctly
- Support setScale and resize
- Writing FLIF images
- Avoiding loading the entire image in the constructor

### Building

**Dependencies**

- Qt
- qmake
- libflif [AUR](https://aur.archlinux.org/packages/flif-git/)

**Building**

1. qmake
2. make

**Installation**

After successful compilation you should have a libflif.* file (extension dependent on OS). You can install this globally or application specific. To install it globally, copy it to *QTDIR/plugins/imageformats/*. To only use it with a specific application, create a folder called *imageformats* in the executable's folder and place the plug-in there.