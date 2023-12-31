#include "ThomsonStuff.h"
#include <fstream>

void initThomsonPalette()
{
    int index = 0;
    for (int b = 0; b < 16; b++) {
        for (int g = 0; g < 16; g++) {
            for (int r = 0; r < 16; r++) {
                Color currentColor(RED_255[r].color.quantumRed() * 257, GREEN_255[g].color.quantumGreen() * 257, BLUE_255[b].color.quantumBlue() * 257);
                index = RED_255[r].thomsonIndex + GREEN_255[g].thomsonIndex + BLUE_255[b].thomsonIndex;
                // cout << "Color[" << r * g * b << "] = " << endl;
                THOMSON_PALETTE[index] = {index, currentColor};
                // cout << "Color[" << index << "] = " << THOMSON_PALETTE[index].color.quantumRed() << "," << THOMSON_PALETTE[index].color.quantumGreen() << "," << THOMSON_PALETTE[index].color.quantumBlue() << endl;
                index++;
            }
        }
    }
}

Image createThomsonImageFromRGBImage(const Image &rgbImage)
{
    float delta = 0;
    Color selectedColor;
    Image thomsonImage(Geometry(rgbImage.columns(), rgbImage.rows()), "white");
    for (int y = 0; y < rgbImage.rows(); y++) {
        cout << (int) y << endl;
        for (int x = 0; x < rgbImage.columns(); x++) {
            Color c = rgbImage.pixelColor(x, y);
            float shortest = std::numeric_limits<float>::max();
            for (int i = 0; i < 4096; i++) {
                delta = preciseColorDelta(c, THOMSON_PALETTE[i].color);
                if (delta < shortest) {
                    selectedColor = THOMSON_PALETTE[i].color;
                    shortest = delta;
                }
            }
            thomsonImage.pixelColor(x, y, selectedColor);
        }

    }
    cout << endl;
    return thomsonImage;
}

// void removeExtraColorsFromThomsonPalette(const map<string, PALETTE_ENTRY> &palette, map<string, PALETTE_ENTRY> &thomsonPalette)
// {
//     map<string, PALETTE_ENTRY> paletteCopy;
//     if (palette.size() <= DITHER_SIZE) {
//         for (auto it = palette.begin(); it != palette.end(); it++) {
//             thomsonPalette.insert(*it);
//         }
//         return;
//     }
//
//     // too much colors
//     for (auto it = palette.begin(); it != palette.end(); it++)
//         paletteCopy.insert(*it);
//     float delta;
//     Color lastColor = std::prev(paletteCopy.end())->second.color;
//     // cout << "###Last color" << getPaletteKey(lastColor) << endl;
//     do {
//         // Color lastColor = std::prev(paletteCopy.end())->second.color;
//         thomsonPalette.clear();
//         float shortest = std::numeric_limits<float>::max();
//         string selectedKey;
//         for (auto it = paletteCopy.begin(); it != paletteCopy.end(); it++) {
//             delta = preciseColorDelta(lastColor, it->second.color);
//             if (delta < shortest && it->second.color != lastColor) {
//                 selectedKey = it->first;
//                 shortest = delta;
//             }
//         }
// // cout << "###shortest" << shortest << " selectedKey"  << selectedKey << endl;
//         // rebuild palette less selectedKey
//         for (auto it = paletteCopy.begin(); it != paletteCopy.end(); it++) {
//             if (it->first == selectedKey) {
//                 // cout << "###Removing color" << selectedKey << endl;
//                 continue;
//             } else {
//                 thomsonPalette.insert(*it);
//             }
//         }
//         cout << "thomsonPalette size:" << thomsonPalette.size() << endl;
//         paletteCopy.clear();
//         for (auto it = thomsonPalette.begin(); it != thomsonPalette.end(); it++)
//             paletteCopy.insert(*it);
//     } while (thomsonPalette.size() > DITHER_SIZE);
// }


void removeExtraColorsFromThomsonPalette(const map<string, PALETTE_ENTRY> &palette, map<string, PALETTE_ENTRY> &thomsonPalette)
{

    if (palette.size() <= DITHER_SIZE) {
        for (auto it = palette.begin(); it != palette.end(); it++) {
            thomsonPalette.insert(*it);
        }
        return;
    }

    // too much colors
    float delta;
    string selectedKey, otherKey;
    map<string, PALETTE_ENTRY> paletteCopy;
    for (auto it = palette.begin(); it != palette.end(); it++)
        paletteCopy.insert(*it);

    do {
        // Color lastColor = std::prev(paletteCopy.end())->second.color;
        thomsonPalette.clear();

        for (auto i = paletteCopy.begin(); i != paletteCopy.end(); i++) {
            float shortest = std::numeric_limits<float>::max();
            for (auto j = paletteCopy.begin(); j != paletteCopy.end(); j++) {
                delta = preciseColorDelta(i->second.color, j->second.color);
                if (delta < shortest && i->first != j->first) {
                    selectedKey = i->first;
                    shortest = delta;
                    otherKey = j->first;
                }
            }
        }
        cout << "Found 2 near color candidates:" << selectedKey << " & " << otherKey << endl;
        cout << "Removing " << selectedKey << endl;
        for (auto it = paletteCopy.begin(); it != paletteCopy.end(); it++) {
            if (it->first == selectedKey) {
                // cout << "###Removing color" << selectedKey << endl;
                continue;
            } else {
                thomsonPalette.insert(*it);
            }
        }
        cout << "thomsonPalette size:" << thomsonPalette.size() << endl;
        paletteCopy.clear();
        for (auto it = thomsonPalette.begin(); it != thomsonPalette.end(); it++)
            paletteCopy.insert(*it);
    } while (thomsonPalette.size() > DITHER_SIZE);
}


void updateThomsonColorIndexes(map<string, PALETTE_ENTRY> &palette)
{
    float delta = 0;
    Color selectedColor;
    uint16_t selectedThomsonIndex = 0;
    uint8_t idx = 0;
    for (auto it = palette.begin(); it != palette.end(); it++) {
        Color c = it->second.color;

        float shortest = std::numeric_limits<float>::max();
        for (int i = 0; i < 4096; i++) {
            delta = preciseColorDelta(c, THOMSON_PALETTE[i].color);
            if (delta < shortest) {
                selectedColor = THOMSON_PALETTE[i].color;
                selectedThomsonIndex = THOMSON_PALETTE[i].thomsonIndex;
                shortest = delta;
            }
        }
        // cout << "shortest " << shortest <<  "  -  "  << selectedThomsonIndex <<  endl;
        string k = getPaletteKey(selectedColor);
        palette[k] = {selectedColor, idx, selectedThomsonIndex};
        idx++;
    }
}


int createThomsonPaletteFromRGB(const map<string, PALETTE_ENTRY> &palette, map<string, PALETTE_ENTRY> &thomsonPalette)
{
    float delta = 0;
    Color selectedColor;
    uint16_t selectedThomsonIndex = 0;
    uint8_t idx = 0;
    map<string, int> colorsCount;
    thomsonPalette.clear();
    for (auto it = palette.begin(); it != palette.end(); it++) {
        Color c = it->second.color;

        float shortest = std::numeric_limits<float>::max();
        for (int i = 0; i < 4096; i++) {
            delta = preciseColorDelta(c, THOMSON_PALETTE[i].color);
            if (delta < shortest) {
                selectedColor = THOMSON_PALETTE[i].color;
                selectedThomsonIndex = THOMSON_PALETTE[i].thomsonIndex;
                shortest = delta;
            }
        }
        string k = getPaletteKey(selectedColor);
        auto inserted = thomsonPalette.insert(std::pair<string, PALETTE_ENTRY>(k, {selectedColor, idx, selectedThomsonIndex}));
        if (inserted.second == true) {
            idx++;
        }
    }

    int colorsLeft = DITHER_SIZE - thomsonPalette.size();

    // padding
    idx = thomsonPalette.size();
    while (thomsonPalette.size() < DITHER_SIZE) {
        Color c = THOMSON_PALETTE[2048 + idx].color;
        selectedThomsonIndex = THOMSON_PALETTE[2048 + idx].thomsonIndex;
        // cout << "Padding " << c.quantumRed() << "," << c.quantumGreen() << "," << c.quantumBlue() <<  " at " << (int) idx << endl;
        string k = getPaletteKey(c);
        thomsonPalette.insert(std::pair<string, PALETTE_ENTRY>(k, {c, idx++, selectedThomsonIndex}));
    }
    return colorsLeft;
}

Image createImageFromThomsonPalette()
{
    int step = 16;
    int idx = 0;
    Image paletteImage(Geometry(64 * step, 64 * step), "white");
    for (int y = 0; y < 64 * step; y += step) {
        for (int x = 0; x < 64 * step; x += step) {
            paletteImage.fillColor(THOMSON_PALETTE[idx].color);
            DrawableRectangle rect(x, y, x + step, y + step);
            paletteImage.draw(rect);
            // cout << "idx:" << idx << "=(" << THOMSON_PALETTE[idx].color.quantumRed() << "," << THOMSON_PALETTE[idx].color.quantumGreen() << "," << THOMSON_PALETTE[idx].color.quantumBlue() << ")" << endl;
            idx++;
        }
    }
    return paletteImage;
}


Color getColorForPaletteIndex(const map<string, PALETTE_ENTRY> &palette, int index)
{
    for (auto p = palette.begin(); p != palette.end(); p++) {
        if (p->second.index == index)
            return p->second.color;
    }
    return Color(65535, 0, 65535);
}


uint16_t getThomsonIndexForPaletteIndex(const map<string, PALETTE_ENTRY> &palette, int index)
{
    for (auto p = palette.begin(); p != palette.end(); p++) {
        if (p->second.index == index)
            return p->second.thomsonIndex;
    }
    return 0;
}

void initThomsonCompensation()
{
    float r;
    float dm = std::numeric_limits<float>::max();;
    int cm = 0;

    // Thomson colors equivalence
    cm = 0;
    for (int i = 0; i < 65536; i++) {
        dm = std::numeric_limits<float>::max();
        r = i;
        for (int c = 0; c < 16; c++) {
            float d = fabs(thomson_level_magick[c] - r);

            if (d < dm) {
                cm = c;
                dm = d;
            }
        }
        rgb_to_to_comp[i] = cm;
    }
}

int getPaletteThomsonValue(int r, int g, int b)
{
    int r_16 = rgb_to_to_comp[r];
    int g_16 = rgb_to_to_comp[g];
    int b_16 = rgb_to_to_comp[b];

    printf("Color <-> TColor : (%d %d %d) <-> (%d %d %d) = %d\n", r, g, b, r_16, g_16, b_16,
           (r_16 + 1) + (g_16 + 1) * 16 + (b_16 + 1) * 256 - 273);

    return (r_16 + 1) + (g_16 + 1) * 16 + (b_16 + 1) * 256 - 273;
}

void save_map_40_col(const string &filename, const MAP_SEG &map_40, const map<string, PALETTE_ENTRY> &palette)
{
    vector<uint8_t> buffer_list, target_buffer_list;
    unsigned char current;

    FILE *fout;
    char map_filename[256];

    sprintf(map_filename, "%s.map", filename.c_str());
    if ((fout = fopen(map_filename, "wb")) == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier données en écriture\n");
        return;
    }

    transposeDataMap40(map_40.columns, map_40.lines, map_40.rama, buffer_list);
    compress(target_buffer_list, buffer_list, 1);

    buffer_list.clear();

    transposeDataMap40(map_40.columns, map_40.lines, map_40.ramb, buffer_list);
    compress(target_buffer_list, buffer_list, 1);

    // Ecriture de l'entete
    // unsigned short size = (unsigned short)list_size(target_buffer_list) + 3 + 39;
    uint16_t size = (uint16_t) target_buffer_list.size() + 3 + 39;

    if (size % 2 == 1) {
        // Apparement, la taille doit être paire
        unsigned char zero = 0;
        // list_add_last(target_buffer_list, &zero);
        target_buffer_list.push_back(zero);
        size++;
    }

    unsigned char header[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    header[2] = size & 255;
    header[1] = (size >> 8) & 255;


    header[6] = map_40.columns - 1;
    header[7] = (map_40.lines - 1) / 8;  // Le fichier map ne fonctionne que sur multiple de 8

    // cout << "ToSnap resolution:" << (int) header[6] << "*" << (int) header[7] << endl;

    fwrite(header, sizeof(uint8_t), 8, fout);

    // Ecriture du buffer map compressé dans le fichier de sortie
    // cout << "ToSnap buffer size:" << target_buffer_list.size() << endl;
    for (int i = 0; i < target_buffer_list.size(); i++) {
        current = target_buffer_list.at(i);
        fwrite(&current, sizeof(uint8_t), 1, fout);
    }

    // Ecriture footer TO-SNAP
    uint8_t to_snap[40];

    memset(to_snap, 0, 39);
    to_snap[0] = 0; // 16 couleurs 40 colonnes
    to_snap[2] = 0; // tour de l'écran
    to_snap[4] = 0; // mode 3 console

    for (int i = 0; i < palette.size(); i++) {
        uint16_t thomson_palette_value = getThomsonIndexForPaletteIndex(palette, i);
        cout << "thomson[" << i << "]=" << thomson_palette_value << endl;
        to_snap[5 + i * 2] = (thomson_palette_value >> 8) & 255;
        to_snap[5 + i * 2 + 1] = thomson_palette_value & 255;
    }

    to_snap[37] = 0xA5;
    to_snap[38] = 0x5A;
    fwrite(to_snap, sizeof(unsigned char), 39, fout);

    // Ecriture du footer
    unsigned char footer[] = { 0, 0, 0, 0, 0 };

    footer[0] = 255;
    fwrite(footer, sizeof(unsigned char), 5, fout);


    fflush(fout);
    fclose(fout);

    printf("TO-SNAP created\n");

    // // Ecriture du chargeur TO-SNAP
    // char fname_snap_out[256];
//
    // sprintf(fname_snap_out, "%s.bld", filename);
    // FILE *tosnap_out = fopen(fname_snap_out, "w");
//
    // fprintf(tosnap_out, "10 DIM T%%(10000)\n");
    // fprintf(tosnap_out, "20 DEFFNC(R)=MAX(-R-1,R)\n");
    // fprintf(tosnap_out, "30 LOADP \"%s\",T%%(10000)\n", map_filename);
    // fprintf(tosnap_out, "40 T=T%%(10000)\n");
    // fprintf(tosnap_out, "50 T=T+1 : IF T%%(T)<>-23206 THEN END\n");
    // fprintf(tosnap_out, "60 FOR I=15 TO 0 STEP -1:T=T+1:PALETTE I,FNC(T%%(T)):NEXT\n");
    // fprintf(tosnap_out, "70 T=T+1 : CONSOLE,,,,T%%(T)\n");
    // fprintf(tosnap_out, "80 T=T+1 : SCREEN,,T%%(T)\n");
    // fprintf(tosnap_out, "90 T=T+1 : POKE &H605F,T%%(T)\n");
    // fprintf(tosnap_out, "100 PUT(0,0),T%%(10000)\n");
    // fflush(tosnap_out);
    // fclose(tosnap_out);
//
    // fflush(stdout);
}


void save_map_16(const string &filename, const MAP_SEG &map_16, int x_count, const map<string, PALETTE_ENTRY> &palette)
{
    int lines_count = map_16.lines * 8;

    int r, g, b;

    FILE *fout;
    char map_filename[256];

    sprintf(map_filename, "%s.map", filename.c_str());
    if ((fout = fopen(map_filename, "wb")) == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier données en écriture\n");
        return;
    }

    vector<uint8_t> buffer_list, target_buffer_list;

    for (int x = 0; x < x_count; x++) {
        for (int y = 0; y < lines_count; y++) {
            uint8_t two_pixels_a;
            two_pixels_a =  map_16.rama[x * lines_count + y];
            buffer_list.push_back(two_pixels_a);
        }

        for (int y = 0; y < lines_count; y++) {
            uint8_t two_pixels_b;
            two_pixels_b = map_16.ramb[x * lines_count + y];
            buffer_list.push_back(two_pixels_b);
        }
    }

    // Compression des RAMA/RAMB entrelacés sans cloture
    compress(target_buffer_list, buffer_list, 1);

    // cloture rama/ramb
    uint8_t cloture[4] = { 0, 0, 0, 0 };
    target_buffer_list.push_back(cloture[0]);
    target_buffer_list.push_back(cloture[1]);
    target_buffer_list.push_back(cloture[2]);
    target_buffer_list.push_back(cloture[3]);

    // Ecriture de l'entete
    uint16_t size = (uint16_t)(target_buffer_list.size() + 3 + 39);

    if (size % 2 == 1) {
        // Apparement, la taille doit être paire
        unsigned char zero = 0;
        target_buffer_list.push_back(zero);
        size++;
    }

    uint8_t header[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    header[2] = size & 255;
    header[1] = (size >> 8) & 255;
    header[5] = 0x40;       // BM16
    //header[6] = map_16.columns - 1;
    header[6] = (x_count * 2) - 1;
    header[7] = map_16.lines - 1;
    fwrite(header, sizeof(unsigned char), 8, fout);

    // Ecriture du buffer map compressé dans le fichier de sortie
    uint8_t current;

    for (int i = 0; i < target_buffer_list.size(); i++) {
        current = target_buffer_list[i];
        fwrite(&current, sizeof(unsigned char), 1, fout);
    }

    // Ecriture footer TO-SNAP
    uint8_t to_snap[40];

    memset(to_snap, 0, 39);
    // to_snap[0] = 0x40; // BM16
    to_snap[0] = 0x80;      // ?
    to_snap[2] = 0;         // tour de l'écran
    to_snap[4] = 3;         // mode 3 console

    for (int i = 0; i < palette.size(); i++) {
        uint16_t thomson_palette_value = getThomsonIndexForPaletteIndex(palette, i);
        cout << "thomson[" << i << "]=" << thomson_palette_value << endl;
        to_snap[5 + i * 2] = (thomson_palette_value >> 8) & 255;
        to_snap[5 + i * 2 + 1] = thomson_palette_value & 255;
    }

    to_snap[37] = 0xA5;
    to_snap[38] = 0x5A;
    fwrite(to_snap, sizeof(unsigned char), 39, fout);

    // Ecriture du footer
    unsigned char footer[] = { 0, 0, 0, 0, 0 };

    footer[0] = 255;
    fwrite(footer, sizeof(unsigned char), 5, fout);

    fflush(fout);
    fclose(fout);
}


void streamMapC(ofstream &os, const vector<uint8_t> &map)
{
    int charLineCount = 0;
    int charCount = 0;
    char hex[8] = {0};
    int asInt = 0;
    os << "\t" << "{" << endl;
    for (auto i = map.begin(); i != map.end(); i++) {
        if (charLineCount == 12) {
            os << endl;
            charLineCount = 0;
        }
        uint8_t u = (uint8_t) * i;
        sprintf(hex, "0x%02x", u);
        os << "\t" << hex << (charCount < map.size() - 1 ?  "," : "");

        charLineCount++;
        charCount++;
    }
    os << endl << "\t" << "}," << endl;
    os << "\t" << map.size() << "," << endl;
}


void convertBlocToThomson(uint8_t bloc[8], uint8_t thomson_bloc[3])
{
    // Conversion du bloc en valeur thomson to/mo
    // en sortie :
    // thomson_bloc[0] = forme
    // thomson_bloc[1] = couleurs format TO
    // thomson_bloc[2] = couleurs format MO
    // En basic, le format de la couleur est spécifié en fonction de la config TO/MO
    // En SNAP-TO, le format de la couleur est toujours TO

    // recherche des couleurs
    int fd = bloc[0];
    int fo = -1;
    int val = 0/*, coul = 0*/;

    for (int i = 0; i < 8; i++)
        if (bloc[i] != fd)
            fo = bloc[i];

    // Calcul forme
    for (int i = 7; i >= 0; i--)
        if (bloc[i] == fo)
            val += std::pow(2, i);

    // Couleur MO / TO
    thomson_bloc[1] = getIndexColorThomsonTo(fd, fo <= 0 ? 0 : fo);
    thomson_bloc[2] = getIndexColorThomsonMo(fd, fo <= 0 ? 0 : fo);

    thomson_bloc[0] = val;
}

void transposeDataMap40(int columns, int lines, const vector<uint8_t> &src, vector<uint8_t> &target)
{
    u_int8_t current;
    uint8_t zero = 0;

    // Le nombre de lignes doit être un multiple de 8
    // La hauteur est inscrite dans l'entêtte du map
    // sous la forme (map_40->lines - 1) / 8

    int padding = lines % 8;
    int add_line = 8 - padding;

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < lines; y++) {
            current = src.at(y * columns + x);
            target.push_back(current);
        }

        if (padding)
            for (int y = 0; y < add_line; y++)
                target.push_back(zero);


        add_line = 8 - padding;
    }
}

int readAhead(const vector<uint8_t> &buffer_list, int idx)
{
    uint8_t current;
    uint8_t compare_to;

    compare_to = buffer_list.at(idx);
    int repeat = 0;

    for (int i = idx + 1; i < buffer_list.size(); i++) {
        current = buffer_list.at(i);
        if (compare_to != current || repeat > 253)
            break;
        repeat++;
    }
    return repeat;
}

void writeSegment(vector<uint8_t> &target, const vector<uint8_t> &buffer_list, int i, uint8_t seg_size)
{
    uint8_t current;
    uint8_t header[2];

    header[0] = 0;
    header[1] = seg_size;

    target.push_back(header[0]);
    target.push_back(header[1]);

    for (int j = i - seg_size; j < i; j++) {
        current = buffer_list.at(j);
        target.push_back(current);
    }
}


void compress(vector<uint8_t> &target, const vector<uint8_t> &buffer_list, int enclose)
{
    // Traitement du buffer;
    int i = 0;
    int seg = 0;
    unsigned char current;

    while (i < buffer_list.size()) {
        int repeat = readAhead(buffer_list, i);

        if (repeat == 0) {
            i += 1;
            seg++;

            if (seg > 254) {
                writeSegment(target, buffer_list, i, seg);
                seg = 0;
            }
        } else {
            if (seg > 0)
                writeSegment(target, buffer_list, i, (unsigned char)seg);

            i += (repeat + 1);
            seg = 0;

            unsigned char rep_count;
            rep_count = repeat + 1;
            target.push_back(rep_count);
            current = buffer_list.at(i - repeat - 1);
            target.push_back(current);
        }
    }

    // flush
    if (seg > 0)
        writeSegment(target, buffer_list, i, seg);

    // cloture ?
    if (enclose) {
        unsigned char cloture[2] = { 0, 0 };

        target.push_back(cloture[0]);
        target.push_back(cloture[1]);
    }
}

int getIndexColorThomsonTo(int back_index, int fore_index)
{
    // Palette thomson TO xyBVRBVR | x = 0 : fd pastel | y = 0 fo pastel
    // N,R,V,J,B,M,C,BL (fonce)
    // x,x,x,x,x,x,x,OR (pastel)

    // couleur > 7 = pastel
    int subst_back = (back_index > 7 ? 8 : 0);
    int subst_fore = (fore_index > 7 ? 8 : 0);
    unsigned char idx = (back_index > 7 ? 0 : 1) << 7
                        | (fore_index > 7 ? 0 : 1) << 6
                        | (fore_index - subst_fore) << 3
                        | (back_index - subst_back);

    return idx;
}

int getIndexColorThomsonMo(int back_index, int fore_index)
{
    // Palette thomson MO5/6 xBVRyBVR | x = 1 : fd pastel | y = 1 fo pastel
    // N,R,V,J,B,M,C,BL (fonce)
    // x,x,x,x,x,x,x,OR (pastel)

    // couleur > 7 = pastel
    unsigned char idx = (fore_index > 7 ? 1 : 0) << 7
                        | (fore_index) << 4
                        | (back_index > 7 ? 1 : 0) << 3
                        | (back_index);

    return idx;
}


#define _putc(__ch, __out) *__out++ = (__ch)
#define _getc(in, in_) (in<in_?(*in++):-1)
#define _rewind(in,_in) in = _in

int mrlec(uint8_t *in,  int inlen, uint8_t *out)
{
    unsigned char *ip = in, *in_ = in + inlen, *op = out;
    int i;
    int c, pc = -1;  			// current and last char
    long long t[256] = {0};  	// byte -> savings
    long long run = 0;  		// current run length
    // Pass 1: determine which chars will compress
    while ((c = _getc(ip, in_)) != -1) {
        if (c == pc) t[c] += (++run % 255) != 0;
        else --t[c], run = 0;
        pc = c;
    }
    for (i = 0; i < 32; ++i) {
        int j;
        c = 0;
        for (j = 0; j < 8; ++j) c += (t[i * 8 + j] > 0) << j;
        _putc(c, op);
    }
    _rewind(ip, in);
    c = pc = -1;
    run = 0;
    do {
        c = _getc(ip, in_);
        if (c == pc) ++run;
        else if (run > 0 && t[pc] > 0) {
            _putc(pc, op);
            for (; run > 255; run -= 255) _putc(255, op);
            _putc(run - 1, op);
            run = 1;
        } else for (++run; run > 1; --run) _putc(pc, op);
        pc = c;
    } while (c != -1);

    return op - out;
}

int mrled(uint8_t *in, uint8_t *out, int outlen)
{
    unsigned char *ip = in, *op = out;
    int i;

    int c, pc = -1;  			// current and last char
    long long t[256] = {0};  	// byte -> savings
    long long run = 0;  		// current run length
    for (i = 0; i < 32; ++i) {
        int j;
        c = *ip++;//_getc(ip,in_);
        for (j = 0; j < 8; ++j)
            t[i * 8 + j] = (c >> j) & 1;
    }
    while (op < out + outlen) {
        c = *ip++;//(c=_getc(ip,in_))!=-1) {
        if (t[c]) {
            for (run = 0; (pc = *ip++/*_getc(ip,in_)*/) == 255; run += 255);
            run += pc + 1;
            for (; run > 0; --run) _putc(c, op);
        } else _putc(c, op);
    }
    return ip - in;
}

