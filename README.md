# DitherToMo
Dither image to the Thomson bitmap 40 (320x200 16 colors with color clash) graphic mode.


<img src="images/original.png">   <img src="images/thomsonReprocessed.gif">


Rely on [ImageMagick 7](https://github.com/ImageMagick/ImageMagick) :
```
git clone https://github.com/ImageMagick/ImageMagick.git
git checkout tags/7.1.1-13
./configure --with-quantum-depth=16 --disable-hdri
make; make install
sudo ldconfig
cmake .
make
```
