#include <Magick++.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <limits>
#include <string.h>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "Structs.h"
#include "ThomsonStuff.h"
#include "ErrorDiffusionDither.h"
#include "Floppy.h"
#include "KMean.h"

using namespace std;

static int iterAnim = 0;
#define RESULT_SIZE_X 320.0
#define RESULT_SIZE_Y 200.0

void debugPalette(const map<string, PALETTE_ENTRY> &palette, const string &paletteName)
{
    cout << endl << "*** " << paletteName << "[" << palette.size() << "] ***"  << endl;
    for (auto p = palette.begin(); p != palette.end(); p++) {
        cout << "Color " << p->first << " at " << (int)(p->second.index) << " th=" << (int)(p->second.thomsonIndex) << endl;
    }
    cout << endl;
}

void waitAnim()
{
    switch (iterAnim % 4) {
    case 0:
        std::cout << "\b\\" << std::flush;
        break;
    case 1:
        std::cout << "\b|" << std::flush;
        break;
    case 2:
        std::cout << "\b/" << std::flush;
        break;
    case 3:
        std::cout << "\b-" << std::flush;
        break;
    default:
        break;
    }
    iterAnim++;
}


void getPaletteFromImage(const Image &im, map<string, PALETTE_ENTRY> &pal)
{
    uint8_t paletteIdx = 0;
    for (uint32_t y = 0; y < im.rows(); y++) {
        for (uint32_t x = 0;  x < im.columns(); x++) {
            Color c = im.pixelColor(x, y);
            string k = getPaletteKey(c);
            PALETTE_ENTRY pe = {c, 0};
            pal.insert(pair<string, PALETTE_ENTRY>(k, pe));
        }
    }
    for (auto p = pal.begin(); p != pal.end(); p++) {
        p->second.index = paletteIdx++;
    }
}



Image writeColormap(string filename, const map<string, PALETTE_ENTRY> &palette/*, vector<Color> &colorArray*/)
{
    Image colormap(Geometry(DITHER_SIZE * 8, 8), "white");
    int idx = 0;
    for (auto it = palette.begin(); it != palette.end(); it++) {
        int x = idx * 8;
        DrawableRectangle d(x, 0, x + 8, 8);
        Color c = it->second.color;
        // cout << filename << " - writing color " << c.quantumRed() << "," << c.quantumGreen() << "," << c.quantumBlue() << "," << c.quantumAlpha() << endl;
        colormap.fillColor(c);
        colormap.draw(d);
        // colorArray[idx] = c;
        // colorArray.push_back(c);
        idx++;
    }
    // Write colormap
    colormap.write(filename.c_str());
    return colormap;
}

void createPaletteArray(const map<string, PALETTE_ENTRY> &palette, vector<Color> &colorArray)
{
    for (auto it = palette.begin(); it != palette.end(); it++) {
        Color c = it->second.color;
        colorArray.push_back(c);
    }
}

void convertClashFragmentToPaletteIndexedBloc(const Image &fragment, const map<string, PALETTE_ENTRY> &palette, uint8_t *bloc, int blocSize)
{
    for (int i = 0; i < blocSize; i++) {
        Color c = fragment.pixelColor(i, 0);
        string k = getPaletteKey(c);
        int idx = palette.at(k).index;
        bloc[CLASH_SIZE - 1 - i] = idx;
    }
}


uint32_t countColors(const Image &image)
{
    std::set<Color> colorSet;
    for (uint32_t y = 0; y < image.rows(); y++) {
        for (uint32_t x = 0;  x < image.columns(); x++) {
            Color c = image.pixelColor(x, y);
            colorSet.insert(c);
        }
    }
    return (uint32_t) colorSet.size();
}


double ColorCompare(int r1, int g1, int b1, int r2, int g2, int b2)
{
    double range = (float) INT16_MAX;

    double luma1 = (r1 * 299 + g1 * 587 + b1 * 114) / (range * 1000);
    double luma2 = (r2 * 299 + g2 * 587 + b2 * 114) / (range * 1000);
    double lumadiff = luma1 - luma2;
    double diffR = (r1 - r2) / 255.0, diffG = (g1 - g2) / 255.0, diffB = (b1 - b2) / 255.0;
    return (diffR * diffR * 0.299 + diffG * diffG * 0.587 + diffB * diffB * 0.114) * 0.75
           + lumadiff * lumadiff;
}


double compareFragment(const Image &first, const Image &second)
{
    double result = 0.0;
    // double multiplier[] = {1.0 * 256.0, 2.0 * 256.0, 4.0 * 256.0, 8.0 * 256.0, 16.0 * 256.0, 32.0 * 256.0, 64.0 * 256.0, 128.0 * 256.0};
    for (int x = 0; x < 8; x++) {
        Color c1 = first.pixelColor(x, 0);
        Color c2 = second.pixelColor(x, 0);
        double compare = ColorCompare(c1.quantumRed(), c1.quantumGreen(), c1.quantumBlue(),
                                      c2.quantumRed(), c2.quantumGreen(), c2.quantumBlue());
        //compare *= multiplier[x];
        result += compare;
    }
    return result;
}



void findNearestClashedFragment(std::vector<std::map<string, PALETTE_ENTRY>> paletteCouples, Image &fragment)
{
    double diff = std::numeric_limits<double>::max();
    Image selectedFragment;
    // try to find the best dithered color couple which match with current clashed fragment
    for (auto paletteIterator = paletteCouples.begin(); paletteIterator != paletteCouples.end(); paletteIterator++) {
        Image ditherFragment = fragment;
        ditherFragment = floydSteinbergDither(ditherFragment, *paletteIterator, &floyd_matrix[USE_MATRIX]);
        // ditherFragment = ostromoukhov_dither(ditherFragment, *paletteIterator);
        double currentDiff = compareFragment(ditherFragment, fragment);
        if (currentDiff < diff) {
            diff = currentDiff;
            selectedFragment = ditherFragment;
        }
    }
    fragment = selectedFragment;
}

void extractFilenameFromPath(const string &fullpath, string &name, string &ext)
{
    std::string path = fullpath;
    // input >> path;

    size_t sep = path.find_last_of("\\/");
    if (sep != std::string::npos)
        path = path.substr(sep + 1, path.size() - sep - 1);

    size_t dot = path.find_last_of(".");
    if (dot != std::string::npos) {
        name = path.substr(0, dot);
        ext  = path.substr(dot, path.size() - dot);
    } else {
        name = path;
        ext  = "";
    }
}


int ditherImage(string fullpath, int countIndex, int backCompleteColor)
{
    uint32_t imageWidth, imageHeight;
    map<string, PALETTE_ENTRY> palette, kmeanPalette;
    float zoomx = 0.0f, zoomy = 0.0f, zoom = 0.0f;
    Image image;
    Image originalImage;

    string name, ext;
    extractFilenameFromPath(fullpath, name, ext);

    string prefix = name.substr(0, name.length() < 6 ? name.length() : 6);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << countIndex;
    std::string s = ss.str();
    name = prefix.append(s);

    cout << "Working on " << name << endl;

    try {
        // Read a file into image object
        image.read(fullpath.c_str());

        imageWidth = (uint32_t) image.columns();
        imageHeight = (uint32_t) image.rows();

        if (imageWidth > RESULT_SIZE_X) {
            zoomx = imageWidth / RESULT_SIZE_X;
        }
        if (imageHeight > RESULT_SIZE_Y) {
            zoomy = imageHeight / RESULT_SIZE_Y;
        }
        if (zoomx != 0 || zoomy != 0) {
            // std::cout << "Zoom x|y : " << zoomx << "|" << zoomy << std::endl;
            zoom = std::max(zoomx, zoomy);
            // std::cout << "Selected zoom : " << zoom << std::endl;
            string size = to_string(imageWidth / (zoom * 2)).append("x").append(to_string(imageHeight / zoom).append("!"));
            image.resize(size);
            cout << "Image new size:" << image.columns() << "," << image.rows() << endl;
        } else {
            zoom = 1;
            string size = to_string(imageWidth / 2).append("x").append(to_string(imageHeight).append("!"));
            image.resize(size);
            image.write("resized.png");
            cout << "Image new size:" << image.columns() << "," << image.rows() << endl;
        }

        originalImage = image;

        // // Check KMean
        // kmean(originalImage, DITHER_SIZE, kmeanPalette);
        // debugPalette(kmeanPalette, "kmeanPalette");
        // vector<Color> ca(kmeanPalette.size());
        // writeColormap("colormapk.gif", kmeanPalette, ca);


        // quantize image to find optimal palette
        // loop until thomson palette >= DITHER_SIZE
        // (we can have some duplicate colors (24 bits to 12 bits))
        // at each iteration, increment magick quantize colors
        // and convert palette to thomson colospace
        std::map<string, PALETTE_ENTRY> thomsonPalette;
        std::map<string, PALETTE_ENTRY> thomsonPaletteCleaned;
        vector<Color> colorArray;
        for (int i = DITHER_SIZE; i < 2 * DITHER_SIZE; i++) {
            image = originalImage;
            image.quantizeDither(false);
            image.quantizeColors(i);
            image.quantize();
            // create palette structure
            getPaletteFromImage(image, palette);
            colorArray.clear();
            writeColormap("colormap.gif", palette/*, colorArray*/);
            cout << "Palette count:" << palette.size() << endl;

            // debugPalette(palette, "thomson Image palette");
            int thomsonColorsNotFound = createThomsonPaletteFromRGB(palette, thomsonPalette);
            cout << "Thomson remaining:" << thomsonColorsNotFound << endl;
            colorArray.clear();
            Image thomsonColormap = writeColormap("thomsonColormap.gif", thomsonPalette/*, colorArray*/);
            if (thomsonColorsNotFound <= 0) break;
        }

        // if thomson colors count is more than DITHER_SIZE, need to remove extra colors
        removeExtraColorsFromThomsonPalette(thomsonPalette, thomsonPaletteCleaned);

        // set thomson indexes on palette
        updateThomsonColorIndexes(thomsonPaletteCleaned);
        cout << "Final Thomson palette size:" << thomsonPaletteCleaned.size() << endl;

        Image thomsonColormap = writeColormap("thomsonPaletteCleaned.gif", thomsonPaletteCleaned);

        // Some debug informations
        originalImage.write("original.png");
        writeColormap("colormap.gif", palette);
        // image.write("quantized.gif");

        // dithering original Image in RGB colorspace
        image = floydSteinbergDither(originalImage, palette, &floyd_matrix[USE_MATRIX]);
        // image = ostromoukhov_dither(originalImage, palette);
        image.write("ditheredrgb.gif");

        // Dither original Image with the Thomson colorspace
        image = floydSteinbergDither(originalImage, thomsonPaletteCleaned, &floyd_matrix[USE_MATRIX]);
        // image = ostromoukhov_dither(originalImage, thomsonPaletteCleaned);
        image.write("ditheredth.gif");

        // Sauvegarde TO-SNAP BM16
        MAP_SEG map_16;
        uint8_t two_pixels;
        int hdiv = 1;
        map_16.lines = image.rows() / hdiv;
        map_16.columns = image.columns() / 2;

        if (image.columns() % 2) map_16.columns++;

        int idxOne, idxTwo;
        int xCount = 0, yCount = 0;
        for (int y = 0; y < map_16.lines * hdiv; y++) {
            xCount = 0;
            for (int x = 0; x < map_16.columns * 2; x += 4) {

                Color c;
                string k;

                // RAMA
                if (x < image.columns()) {
                    c = image.pixelColor(x, y);
                    k = getPaletteKey(c);
                    idxOne = thomsonPaletteCleaned.at(k).index;
                } else {
                    cout << x + 0 << "," << y << "=" << "0" << endl;
                    idxOne = backCompleteColor;
                }

                if (x + 1 < image.columns()) {
                    c = image.pixelColor(x + 1, y);
                    k = getPaletteKey(c);
                    idxTwo = thomsonPaletteCleaned.at(k).index;
                } else {
                    cout << x + 1 << "," << y << "=" << "0" << endl;
                    idxTwo = backCompleteColor;
                }

                two_pixels = idxOne << 4 | idxTwo;
                map_16.rama.push_back(two_pixels);

                if (x + 2 < image.columns()) {
                    c = image.pixelColor(x + 2, y);
                    k = getPaletteKey(c);
                    idxOne = thomsonPaletteCleaned.at(k).index;
                } else {
                    cout << x + 2 << "," << y << "=" << "0" << endl;
                    idxOne = backCompleteColor;
                }

                if (x + 3 < image.columns()) {
                    c = image.pixelColor(x + 3, y);
                    k = getPaletteKey(c);
                    idxTwo = thomsonPaletteCleaned.at(k).index;
                } else {
                    cout << x + 3 << "," << y << "=" << "0" << endl;
                    idxTwo = backCompleteColor;
                }

                two_pixels = idxOne << 4 | idxTwo;
                map_16.ramb.push_back(two_pixels);

                xCount++;
            }
            yCount++;
        }

        cout << "xCount=" << xCount << endl;

        string cname = name + ".h";
        ofstream os(cname);
        // Structs
        os << "struct " << name << "_struct {" << endl;

        // struct declaration
        os << "\t" << "unsigned int " << "pal[16];" << endl;
        os << "\t" << "unsigned char " << "x;" << endl;
        os << "\t" << "unsigned char " << "w;" << endl;
        os << "\t" << "unsigned char " << "h;" << endl;
        os << "\t" << "unsigned char/*unsigned int*/ mapa[" << map_16.rama.size() << "];" << endl;
        os << "\t" << "unsigned int " << "lena;" << endl;
        os << "\t" << "unsigned char/*unsigned int*/ mapb[" << map_16.ramb.size() << "];" << endl;
        os << "\t" << "unsigned int " << "lenb;" << endl;
        os << "}; " << endl;

        // struct initialization
        os << "struct " << name << "_struct " << name << " = {" << endl;
        os << "\t" << "{ ";
        for (auto p = thomsonPaletteCleaned.begin(); p != thomsonPaletteCleaned.end(); p++) {
            uint16_t thomsonIndex = p->second.thomsonIndex;
            os << "\t" <<  thomsonIndex << (next(p) == thomsonPaletteCleaned.end() ? "" : ",");
        }
        os << " }," << endl;
        os << "\t" << xCount << "," << endl;
        os << "\t" << xCount * 4 << "," << endl;
        os << "\t" << /*map_16.lines * 8*/yCount << "," << endl;
        os << endl;
        streamMapC(os, map_16.rama);
        os << endl;
        streamMapC(os, map_16.ramb);
        os << "};";
        os.close();

    } catch (Exception &error_) {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
    }
    return 0;
}


int main(int argc, const char **argv)
{
    InitializeMagick(*argv);
    initThomsonPalette();
    initThomsonCompensation();

    // Read command line arguments
    string fsOption = "-fs=";
    string kOption = "-k";
    string bOption = "-b=";
    int fs = -1;
    int k = -1;
    int b = -1;  // background complete color when width is not 8 divisor
    vector<string> filesList;
    for (int x = 0; x < argc; x++) {
        // printf("%d = %s\n", x, argv[x]);
        bool noflag = true;
        if (string(argv[x]).compare(0, fsOption.size(), fsOption) == 0) {
            std::string argumentValue = string(argv[x]).substr(fsOption.size());
            try {
                fs = std::stoi(argumentValue);
            } catch (...) {}
            noflag = false;
        }
        if (string(argv[x]).compare(0, bOption.size(), bOption) == 0) {
            std::string argumentValue = string(argv[x]).substr(bOption.size());
            try {
                b = std::stoi(argumentValue);
            } catch (...) {}
            noflag = false;
        }
        if (string(argv[x]).compare(0, kOption.size(), kOption) == 0) {
            k = 1;
            noflag = false;
        }
        if (noflag && x != 0) {
            filesList.push_back(string(argv[x]));
        }
    }
    if (b == -1) b = 0;
    cout << "background complete color index:" << b << endl;
    cout << "Images list" << endl;
    for (auto it = filesList.begin(); it != filesList.end(); it++) {
        cout << *it << endl;
    }
    cout << endl;

    int i = 0;
    for (auto it = filesList.begin(); it != filesList.end(); it++) {
        ditherImage(*it, i++, b);
    }

    // ditherImage("/home/rodoc/develop/tomo/saucer/saucer2.gif", 0, 15);
    // ditherImage("/home/rodoc/develop/projects/DitherToMo/build/cat160.png", 0);
    // ditherImage("/home/rodoc/develop/projects/DitherToMo/images/fouAPiedRouge.jpg", 0);
    // ditherImage("/home/rodoc/develop/projects/DitherToMo/images/fouAPiedBleu.jpg", 0);
    // ditherImage("/home/rodoc/develop/projects/DitherToMo/images/nimoy.jpg", 1);
    // ditherImage("/home/rodoc/develop/projects/DitherToMo/images/beast01.png", 0);
    // ditherImage("/home/rodoc/develop/tomo/saucer/saucer.png", 0);

    return 0;
}

