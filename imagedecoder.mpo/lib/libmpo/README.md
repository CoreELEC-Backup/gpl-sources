A future library to decode and encode MPO (multiple picture object) files.
Those are used mainly for 3D cameras and the Nintendo 3DS.

##Current Status : [![Build Status](https://travis-ci.org/Lectem/libmpo.svg?branch=master)](https://travis-ci.org/Lectem/libmpo)

##Main goal :
- Load / Write Stereoscopic 3D images (Multi-frame Disparity) through libjpeg.
- Give others enough material to work with
- No exif library needed (though you'll probably want to use one)

###Currently working :
- Decompression (see examples/decompress.c)
- Basic compression
- Compression of 3d stereo files, compatible with Nintendo 3DS and NVidia 3D viewer.
  Look at the example for more details

##Requirements :
- libjpeg8 or later

##Documentation

The doxygen documentation is automatically generated after each commit.
You can find it [here](http://lectem.github.io/libmpo/).

##TODO (In *order* of priority):
- Give a good interface for compression
- Simplify usage of MPF attributes
- Decompression : fix the image attributes parsing
- A good refactoring
- Remove printf and other debugging stuff
- Everything else

##Will not be done :
- Exif and other markers support (you should be able to write/read it trough your own handlers and libjpeg)

###Might not be done :
- Helper functions for non stereoscopic images