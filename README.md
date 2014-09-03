LineSpace
========

A 3D cuve editor using head tracking and leapmotion to enhance the experience. (based on Holotouch)

Installation instructions
=========================

Linux
=====

install qt5 for linux with qtcreator included
install libcv2.4 and libcv2.4-dev, maybe libcv-highui if not already included,
install lib-glu-mesa-dev
install leapmotion software, the drivers, and download the sdk

You need to copy the file libLeap.so from the sdk into the code/leapmotion folder.

Windows
=======
For using with QtCreator and Mingw compiler, use the libLeap.dll from https://leapmotion.app.box.com/s/4lg9frhquuvu89xd0z4j

OS X
====
install opencv (macport, source)
install Leap Motion SDK
sudo ln -s LeapDeveloperKit/LeapSDK/lib/libc++/libLeap.dylib /usr/local/lib/libLeap.dylib
sudo ln -s LeapDeveloperKit/LeapSDK/include/Leap.h /usr/local/include/Leap.h
sudo ln -s LeapDeveloperKit/LeapSDK/include/LeapMath.h /usr/local/include/LeapMath.h
