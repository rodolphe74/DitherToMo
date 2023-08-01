# DitherToMo
Dither image to the Thomson bitmap 40 (320x200 16 colors with color clash) graphic mode.

<img src="images/original.png">   <img src="images/thomsonReprocessed.gif">

Or to the Thomson bitmap 16 (320x200 16 colors without constraint) graphic mode.

<img src="images/original.png">   <img src="images/ditheredth16.gif">

Rely on [ImageMagick 7](https://github.com/ImageMagick/ImageMagick) :

```shell
sudo apt install libjpeg-dev
sudo apt install libpng-dev
sudo apt install libgif-dev
git clone https://github.com/ImageMagick/ImageMagick.git
git checkout tags/7.1.1-13
./configure --with-quantum-depth=16 --disable-hdri
make
sudo make install
sudo ldconfig
git clone https://github.com/rodolphe74/DitherToMo.git
cmake .
make
```
