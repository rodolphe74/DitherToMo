#pragma once

#include "ColorStuff.h"
#include <sys/time.h>
#include <limits>
#include <vector>

struct CLUSTER_LIST_STRUCT {
    vector<Color> cluster_vector;
};
typedef struct CLUSTER_LIST_STRUCT CLUSTER_LIST;

void kmean(const Image &image, int reducSize, map<string, PALETTE_ENTRY> &palette);
