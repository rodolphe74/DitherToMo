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

Binaries|Utilisation
---|---
dtm40|MAP image creation in 320x200x16[^1]
dtm16|MAP image creation in 160x200x16[^1]
mapc16|C header creation in 160x200x16[^2]
mapc1677|C header creation in 160x200x16 with lz77 compression[^2]

[^1]:[Les fichier graphiques Thomson](http://collection.thomson.free.fr/code/articles/prehisto_bulletin/page.php?XI=0&XJ=13).
[^2]:[lz77](https://github.com/rodolphe74/lz77).
