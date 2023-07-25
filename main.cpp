#include <Magick++.h>
#include <iostream>
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

using namespace std;


// uint32_t imageWidth, imageHeight;
// map<string, PALETTE_ENTRY> palette;
// float zoomx = 0.0f, zoomy = 0.0f, zoom = 0.0f;

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



Image writeColormap(string filename, const map<string, PALETTE_ENTRY> &palette, vector<Color> &colorArray)
{
    Image colormap(Geometry(DITHER_SIZE * 8, 8), "white");
    int idx = 0;
    // Color colorArray[DITHER_SIZE];
    for (auto it = palette.begin(); it != palette.end(); it++) {
        int x = idx * 8;
        DrawableRectangle d(x, 0, x + 8, 8);
        Color c = it->second.color;
        colormap.fillColor(c);
        colormap.draw(d);
        colorArray[idx] = c;
        idx++;
    }
    // Write colormap
    colormap.write(filename.c_str());
    return colormap;
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


int ditherImage(string fullpath, int countIndex)
{
    uint32_t imageWidth, imageHeight;
    map<string, PALETTE_ENTRY> palette;
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
            std::cout << "Zoom x|y : " << zoomx << "|" << zoomy << std::endl;
            zoom = std::max(zoomx, zoomy);
            std::cout << "Selected zoom : " << zoom << std::endl;
        }

        image.resize(Geometry(imageWidth / zoom, imageHeight / zoom));
        cout << "Image new size:" << image.columns() << "," << image.rows() << endl;

        originalImage = image;


        // quantize image to find optimal palette
        image.quantizeDither(false);
        image.quantizeColors(DITHER_SIZE);
        image.quantize();

        // create palette structure
        getPaletteFromImage(image, palette);


        cout << "*** original Palette [" << palette.size() << "]***"  << endl;
        for (auto p = palette.begin(); p != palette.end(); p++) {
            cout << "  Color " << p->first << " at " << (int)(p->second.index) << endl;
        }

        // debug informations
        originalImage.write("original.png");
        vector<Color> colorArray(palette.size());
        writeColormap("colormap.gif", palette, colorArray);
        image.write("quantized.gif");

        // dithering originalImage
        image = floydSteinbergDither(originalImage, palette, &floyd_matrix[USE_MATRIX]);
        image.write("dithered1.gif");


        // Find ImageMagick palette in Thomson color space
        std::map<string, PALETTE_ENTRY> thomsonPalette;
        createThomsonPaletteFromRGB(palette, thomsonPalette);
        Image thomsonColormap = writeColormap("thomsonColormap.gif", thomsonPalette, colorArray);
        cout << "*** Thomson Palette [" << thomsonPalette.size() << "]***"  << endl;


        // Dither original with the updated palette
        image = floydSteinbergDither(originalImage, thomsonPalette, &floyd_matrix[USE_MATRIX]);
        image.write("dithered2.gif");

        // Create all possible couples from 16 colors colormap
        std::vector<std::map<string, PALETTE_ENTRY>> paletteCouples;
        string k;
        for (int i = 0; i < DITHER_SIZE; i++) {
            for (int j = i; j < DITHER_SIZE; j++) {
                if (i == j) continue;
                Color c1 = colorArray[i];
                Color c2 = colorArray[j];

                std::map<string, PALETTE_ENTRY> pc;
                k = getPaletteKey(c1);
                pc.insert(pair<string, PALETTE_ENTRY>(k, {c1, 0}));
                k = getPaletteKey(c2);
                pc.insert(pair<string, PALETTE_ENTRY>(k, {c2, 1}));
                paletteCouples.push_back(pc);
            }
        }

        // debug all dithered fragments
        // int idx = 0;
        // for (auto mapImageIterator = couples.begin(); mapImageIterator != couples.end(); mapImageIterator++) {
        //     string coupleName = "couple";
        //     coupleName.append(std::to_string(idx));
        //     coupleName.append(".gif");
        //     mapImageIterator->write(coupleName.c_str());
        //     idx++;
        // }
        cout << "Couples found:" << paletteCouples.size() << endl;

        // Process color clash by re-dithering 8 pixels length fragments having color count > 2
        // Try all possible couples, keep best result for each
        Image clashFragment(Geometry(CLASH_SIZE, 1), "white");
        Image reprocessed(Geometry(image.columns(), image.rows()), "white");
        int totalPixelsClash = 0;
        int totalClashed = 0;
        MAP_SEG map_40; // SNAP-TO thomson image format container (http://collection.thomson.free.fr/code/articles/prehisto_bulletin/page.php?XI=0&XJ=13)
        uint8_t currentBloc[8];
        for (int y = 0; y < image.rows(); y++) {
            for (int x = 0; x < image.columns(); x += CLASH_SIZE) {
                // cout << "x:" << x << endl;
                int length = x + CLASH_SIZE > image.columns() ? image.columns() - x : CLASH_SIZE;


                for (int i = 0; i < length; i++) {
                    Color c = image.pixelColor(x + i, y);
                    // clashFragment.pixelColor(i, 0, *(pp + i));
                    clashFragment.pixelColor(i, 0, c);
                }
                int countClash = countColors(clashFragment);
                // printf("%d %d -> %d (%d)\n", x, y, length, countClash);
                totalPixelsClash++;
                if (countClash > 2) {
                    totalClashed++;
                    findNearestClashedFragment(paletteCouples, clashFragment);
                } else {
                    // 2 colors
                    // findNearestClashedFragment(couples, clashFragment);
                    // clashFragment.write("clash.gif");
                }

                // convert fragment to thomson planes
                convertClashFragmentToPaletteIndexedBloc(clashFragment, thomsonPalette, currentBloc, CLASH_SIZE);
                uint8_t ret[3];
                convertBlocToThomson(currentBloc, ret);
                map_40.rama.push_back(ret[0]);
                map_40.ramb.push_back(ret[1]);

                // reprocessed image construction
                for (int i = 0; i < length; i++) {
                    reprocessed.pixelColor(x + i, y, clashFragment.pixelColor(i, 1));
                }
            }
        }
        // adjust map40 sizes
        map_40.lines = reprocessed.rows();
        map_40.columns = reprocessed.columns() / CLASH_SIZE + (reprocessed.columns() % CLASH_SIZE == 0 ? 0 : 1);

        cout << endl << totalClashed << "/" << totalPixelsClash << " to reprocess" << std::endl;
        reprocessed.write("thomsonReprocessed.gif");

        save_map_40_col(name.c_str(), map_40, thomsonPalette);

        // Create thomson sap floppy
        if (countIndex == 0)
            writeImagesListOnDisk("dithtomo.sap", name.append(".map"), 1);
        else
            writeImagesListOnDisk("dithtomo.sap", name.append(".map"), 0);
        countIndex++;
    } catch (Exception &error_) {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
    }
    return 0;
}


int main(int argc, char **argv)
{
    InitializeMagick(*argv);
    initThomsonPalette();
    initThomsonCompensation();

    ditherImage("/home/rodoc/develop/projects/DitherToMo/images/arton5254.jpg", 0);
    ditherImage("/home/rodoc/develop/projects/DitherToMo/images/nimoy.jpg", 1);

    return 0;
}

