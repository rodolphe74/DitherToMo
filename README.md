# DitherToMo


## Description
Dither image to the Thomson bitmap 40 (320x200 16 colors with color clash) graphic mode.

<img src="images/original.png">   <img src="images/thomsonReprocessed.gif">

Or to the Thomson bitmap 16 (320x200 16 colors without constraint) graphic mode.

<img src="images/original.png">   <img src="images/ditheredth16.gif">


## Compilation
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

## Binaries
Binaries|Utilisation
---|---
dtm40|MAP image creation in 320x200x16[^1]
dtm16|MAP image creation in 160x200x16[^1]
dtm1677|lz77 image creation in 160x200x16[^3]
mapc16|C header creation in 160x200x16[^2]
mapc1677|C header creation in 160x200x16 with lz77 compression[^2]
[^1]:[Les fichiers graphiques Thomson](http://collection.thomson.free.fr/code/articles/prehisto_bulletin/page.php?XI=0&XJ=13)
[^2]:[lz77](https://github.com/rodolphe74/lz77)
[^3]:see l77 image proprietary format description below


## 160x200x16 lz77 image format

### Description
Offset|Length|Description
---|---|---
0|3|Magic number [0x01 0x06 0x07]
3|1|X resolution / 4 (40 max)
4|1|X resolution (160 max)
5|1|Y resolution (200 max)
6|32|Palette 16 * 2 bytes (16 colors, each on 2 bytes)
38|1|Number A of RAMA lz77 (or not[^4]) chunks
39|1|Number B of RAMB lz77 (or not[^4]) chunks
40|2|1st RAMA chunk size
41|1|Boolean: compressed chunk or not.
42|size of chunk|lz77 (or not[^4]) RAMA bytes data
42+size of chunk|2|Next RAMA chunk size
...|...|...
N|2|1st RAMB chunk size
N+1|1|Boolean: compressed chunk (or not[^4]).
N+2|size of chunk|lz77 (or not[^4]) RAMB bytes data
...|...|...

[^4]:sometimes compressed chunks are bigger than uncompressed ones

### Realtime image decompression (TO8)
<img src="loader.gif" width=352>

See [loader.c](thomson/loader.c) for a sample on how to load an image from a floppy on a TO8D.

See [Makefile](thomson/Makefile) for a sample on how to compile it with [cmoc](http://perso.b2b2c.ca/~sarrazip/dev/cmoc.html).
