#include <limits>
#include "ErrorDiffusionDither.h"


float colorDelta(Color c1, Color c2)
{
    float dr = std::abs(c1.quantumRed() - c2.quantumRed());
    float dg = std::abs(c1.quantumGreen() - c2.quantumGreen());
    float db = std::abs(c1.quantumBlue() - c2.quantumBlue());

    // printf("     %f %f %f=%f\n", dr, dg, db, std::sqrt((dr * dr) + (dg * dg) + (db * db)));
    return sqrt(30.0f * (dr * dr) + 59.0f * (dg * dg) + 11.0f * (db * db));
    // return std::sqrt((dr * dr) + (dg * dg) + (db * db));
}


Color findClosestColorFromPalette(Color color, const map<string, PALETTE_ENTRY> &palette)
{
    float distance;
    float diff = std::numeric_limits<float>::max();
    Color nearest;

    for (auto p = palette.begin(); p != palette.end(); p++) {
        // Color current = p->second.color;
        distance = colorDelta(p->second.color, color);
        if (distance < diff) {
            nearest = p->second.color;
            diff = distance;
        }
    }

    return nearest;
}

Color apply_error(Color p, float ratio, int32_t error[3])
{
    int32_t new_value;
    new_value = (int) std::round(p.quantumRed() + ratio * error[0]);
    p.quantumRed((int)std::min(std::max(new_value, 0), 65535));
    new_value = (int) std::round(p.quantumGreen() + ratio * error[1]);
    p.quantumGreen((int)std::min(std::max(new_value, 0), 65535));
    new_value = (int) std::round(p.quantumBlue() + ratio * error[2]);
    p.quantumBlue((int)std::min(std::max(new_value, 0), 65535));
    return p;
}

Image floydSteinbergDither(const Image &source, const map<string, PALETTE_ENTRY> &palette, const FLOYD_MATRIX *f_mat)
{
    int w = source.columns();
    int h = source.rows();
    Image target(source);

    Color oldPixel, newPixel;
    int32_t error_propagation[3];

    float *matrix = f_mat->matrix;
    int serpentine = 1;
    int left_to_right = 1;
    int matrix_size = (int)matrix[0];

    int addQuantisationPass = (int)matrix[matrix_size * 3 + 1];

    int serpentine_x = 0;
    int matrix_shift, yy;
    float error;


    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (serpentine) {
                if ((y + 1) % 2) {
                    serpentine_x = x;
                    left_to_right = 1;
                } else {
                    serpentine_x = w - 1 - x;
                    left_to_right = 0;
                }
            } else {
                serpentine_x = x;
            }

            oldPixel.quantumRed(target.pixelColor(serpentine_x, y).quantumRed());
            oldPixel.quantumGreen(target.pixelColor(serpentine_x, y).quantumGreen());
            oldPixel.quantumBlue(target.pixelColor(serpentine_x, y).quantumBlue());

            Color p(oldPixel.quantumRed(), oldPixel.quantumGreen(), oldPixel.quantumBlue());
            newPixel = findClosestColorFromPalette(p, palette);
            target.pixelColor(serpentine_x, y, newPixel);
            // cout << "("<< serpentine_x << "," << y << ")" << oldPixel.quantumRed()<< "," << oldPixel.quantumGreen()<< "," << oldPixel.quantumBlue() << " -> "<< newPixel.quantumRed()<< "," << newPixel.quantumGreen()<< "," << newPixel.quantumBlue() << endl;

            error_propagation[0] = oldPixel.quantumRed() - newPixel.quantumRed();
            error_propagation[1] = oldPixel.quantumGreen() - newPixel.quantumGreen();
            error_propagation[2] = oldPixel.quantumBlue() - newPixel.quantumBlue();

            for (int i = 0; i < matrix_size; i++) {
                if (left_to_right)
                    matrix_shift = (int)matrix[1 + (i % matrix_size) * 3];
                else
                    matrix_shift = 0 - (int)matrix[1 + (i % matrix_size) * 3];
                yy = (int)matrix[1 + (i % matrix_size) * 3 + 1];
                error = matrix[1 + (i % matrix_size) * 3 + 2];

                if (serpentine_x + matrix_shift >= 0 && serpentine_x + matrix_shift < w && y + yy >= 0 && y + yy < h) {
                    Color c = apply_error(target.pixelColor(matrix_shift + serpentine_x, y + yy), error, error_propagation);
                    target.pixelColor(matrix_shift + serpentine_x, y + yy, c);
                }

            }
        }
    }

    if (addQuantisationPass) {
        // cout << "Quant+" << endl;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                target.pixelColor(x, y, findClosestColorFromPalette(target.pixelColor(x, y), palette));
            }
        }
    }

    return target;
}

Image ostromoukhov_dither(const Image &source, const map<string, PALETTE_ENTRY> &palette)
{
    int w = source.columns();
    int h = source.rows();
    Image edImage(Geometry(w, h), "white");
    return edImage;
}
