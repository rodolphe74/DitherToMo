#include "ThomsonStuff.h"

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
                // cout << "Color[" << index << "] = " << currentColor.quantumRed() << "," << currentColor.quantumGreen() << "," << currentColor.quantumBlue() << endl;
                cout << "Color[" << index << "] = " << THOMSON_PALETTE[index].color.quantumRed() << "," << THOMSON_PALETTE[index].color.quantumGreen() << "," << THOMSON_PALETTE[index].color.quantumBlue() << endl;
                index++;
            }
        }
    }
}

void createThomsonPaletteFromRGB(const map<string, PALETTE_ENTRY> &palette, map<string, PALETTE_ENTRY> &thomsonPalette)
{
    float delta = 0;
    Color selectedColor;
    uint16_t selectedThomsonIndex = 0;
    uint8_t idx = 0;
    map<string, int> colorsCount;
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
        cout << "Color " << c.quantumRed() << "," << c.quantumGreen() << "," << c.quantumBlue() << " <-> " << selectedColor.quantumRed() << "," << selectedColor.quantumGreen() << ","  << selectedColor.quantumBlue() << endl;
        string k = getPaletteKey(selectedColor);
        auto inserted = thomsonPalette.insert(std::pair<string, PALETTE_ENTRY>(k, {selectedColor, idx, selectedThomsonIndex}));
        if (inserted.second == true) {
            idx++;
        }
    }
    // for (auto it = thomsonPalette.begin(); it != thomsonPalette.end(); it++) {
    //     cout << "  " << it->first << " = " << it->second.thomsonIndex << endl;
    // }

    // padding
    // for (int i = thomsonPalette.size(); i < DITHER_SIZE; i++) {
    idx = thomsonPalette.size();
    while (thomsonPalette.size() != DITHER_SIZE) {
        Color c = THOMSON_PALETTE[2048 + idx].color;
        selectedThomsonIndex = THOMSON_PALETTE[2048 + idx].thomsonIndex;
        cout << "  Padding:" << c.quantumRed() << "," << c.quantumGreen() << "," << c.quantumBlue() <<  " at " <<  (int) idx <<endl;
        string k = getPaletteKey(c);
        thomsonPalette.insert(std::pair<string, PALETTE_ENTRY>(k, {c, idx++, selectedThomsonIndex}));
    }

    for (auto it = thomsonPalette.begin(); it != thomsonPalette.end(); it++) {
        cout << (int)  it->second.index << " = " << it->first << " = " << it->second.thomsonIndex << endl;
    }
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
            cout << "idx:" << idx << "=(" << THOMSON_PALETTE[idx].color.quantumRed() << "," << THOMSON_PALETTE[idx].color.quantumGreen() << "," << THOMSON_PALETTE[idx].color.quantumBlue() << ")" << endl;

            // if (THOMSON_PALETTE[idx].color.quantumRed()!= 0 && THOMSON_PALETTE[idx].color.quantumRed()!= 24672 && THOMSON_PALETTE[idx].color.quantumRed() == THOMSON_PALETTE[idx].color.quantumBlue() && THOMSON_PALETTE[idx].color.quantumRed() == THOMSON_PALETTE[idx].color.quantumGreen()) {
            //     cout << "**********" << endl;
            //     return paletteImage;
            // }

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
    int r, g, b;

    unsigned char current;

    FILE *fout;
    char map_filename[256];

    sprintf(map_filename, "%s.map", filename.c_str());
    if ((fout = fopen(map_filename, "wb")) == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier données en écriture\n");
        return;
    }

    transposeDataMap40(map_40.columns, map_40.lines, map_40.rama, buffer_list);
    compress(target_buffer_list, fout, buffer_list, 1);

    buffer_list.clear();

    transposeDataMap40(map_40.columns, map_40.lines, map_40.ramb, buffer_list);
    compress(target_buffer_list, fout, buffer_list, 1);

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

    cout << "ToSnap resolution:" << (int) header[6] << "*" << (int) header[7] << endl;

    fwrite(header, sizeof(uint8_t), 8, fout);

    // Ecriture du buffer map compressé dans le fichier de sortie
    cout << "ToSnap buffer size:" << target_buffer_list.size() << endl;
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
        // Color c = getColorForPaletteIndex(palette, i);
        // r = c.quantumRed();
        // g = c.quantumGreen();
        // b = c.quantumBlue();
        // short thomson_palette_value = getPaletteThomsonValue(r, g, b);
        uint16_t thomson_palette_value = getThomsonIndexForPaletteIndex(palette, i);
        cout << "thomson_palette_value:" << thomson_palette_value << endl;
        // printf("BM40 - Couleur %d %d %d = %d\n", r, g, b, thomson_palette_value);
        to_snap[5 + i * 2] = (thomson_palette_value >> 8) & 255;
        to_snap[5 + i * 2 + 1] = thomson_palette_value & 255;

        // to_snap[5 + i * 2] = (thomson_palette_value >> 8) & 255;
        // to_snap[5 + i * 2 + 1] = thomson_palette_value & 255;
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

    printf("Le fichier TO-SNAP est créé\n");

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


void convertBlocToThomson(uint8_t bloc[8], uint8_t thomson_bloc[3])
{
    // Conversion du bloc en valeur thomson to/mo
    // en sortie :
    // thomson_bloc[0] = forme
    // thomson_bloc[1] = couleurs format TO
    // thomson_bloc[2] = couleurs format MO
    // En basic, le format de la oouleur est spécifié en fonction de la config TO/MO
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
            // list_get_at(&current, *src, y * columns + x);
            current = src.at(y * columns + x);
            // if (y == 1 && x > 30) {
            // 	printf("transpose (%d %d) = %d\n", x, current, list_size(*target));
            // }
            // list_add_last(*target, &current);
            target.push_back(current);
        }

        // if (padding)
        // 	for (int y = 0; y < add_line; y++)
        // 		list_add_last(*target, &zero);

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


void compress(vector<uint8_t> &target, FILE *f, const vector<uint8_t> &buffer_list, int enclose)
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
