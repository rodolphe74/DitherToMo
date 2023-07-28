#include "KMean.h"


static int initialized = 0;

int rand_int(int n)
{
    struct timeval current_time;
    if (!initialized) {
        gettimeofday(&current_time, NULL);
        srand(current_time.tv_usec);
        initialized = 1;
    }
    int random_value = rand() % (n + 1);
    return random_value;
}

int nearest_color(Color c, const vector<Color> &color_vector)
{
    float distMin = std::numeric_limits<float>::max();
    float dist = 0;
    int idx;
    int i = 0;
    for (auto it = color_vector.begin(); it != color_vector.end(); it++) {
        dist = preciseColorDelta(*it, c);
        if (dist < distMin) {
            distMin = dist;
            idx = i;
        }
        i++;
    }

    return idx;
}

Color colors_mean(const vector<Color> &color_vector)
{
    int sz = color_vector.size();

    if (sz == 0) return Color(0,0,0);

    Color mean;
    int r = 0, g = 0, b = 0;

    for (auto it = color_vector.begin(); it != color_vector.end(); it++) {
        r += (*it).quantumRed();
        g += (*it).quantumGreen();
        b += (*it).quantumBlue();
    }

    mean.quantumRed(r / sz);
    mean.quantumGreen(g / sz);
    mean.quantumBlue(b / sz);

    return mean;
}


float color_variance(const vector<Color> &color_vector)
{
    int sz = color_vector.size();
    Color mean = colors_mean(color_vector);
    float dist = 0, dist_sum = 0;
    for (auto it = color_vector.begin(); it != color_vector.end(); it++) {
        dist = preciseColorDelta(*it, mean);
        dist_sum += /*(dist * dist)*/dist;
    }
    return dist_sum / (float) sz;
}

void new_cluster_list(int k, vector<CLUSTER_LIST> &cluster_vector)
{
    for (int i = 0; i < k; i++) {
        CLUSTER_LIST cl;
        cluster_vector.push_back(cl);
    }
}


// void get_cluster_centroid(const Image &image, vector<CLUSTER_LIST> &cluster_vector, vector<Color> &centroid_vector)
// {
//     CLUSTER_LIST current_cluster;
//     int cl_size = cluster_vector.size();
//
//     // vector_clear(*centroid_vector);
//     centroid_vector.clear();
//     for (int i = 0; i < cl_size; i++) {
//         Color no_color = Color(0, 0, 0);
//         // vector_add_last(*centroid_vector, &no_color);
//         centroid_vector.push_back(no_color);
//     }
//
//     for (int i = 0; i < cl_size; i++) {
//         // vector_get_at(&current_cluster, *cluster_vector, i);
//         current_cluster = cluster_vector[i];
//         if (current_cluster.cluster_vector.size() > 0) {
//             Color c_mean = colors_mean(current_cluster.cluster_vector);
//             centroid_vector[i] = c_mean;
//         } else {
//             int xr = rand_int(image.columns());
//             int yr = rand_int(image.rows());
//             Color c = image.pixelColor(xr, yr);
//             centroid_vector[i] = c;
//         }
//     }
// }

void get_cluster_centroid(const Image &image, CLUSTER_LIST **clustersList, int cl_size, vector<Color> &centroid_vector)
{
    CLUSTER_LIST *current_cluster;
    // int cl_size = cluster_vector.size();

    printf("3 %p - %p\n", clustersList[0], clustersList[7]);

    // vector_clear(*centroid_vector);
    centroid_vector.clear();
    for (int i = 0; i < cl_size; i++) {
        Color no_color = Color(0, 0, 0);
        // vector_add_last(*centroid_vector, &no_color);
        centroid_vector.push_back(no_color);
    }

    for (int i = 0; i < cl_size; i++) {
        // vector_get_at(&current_cluster, *cluster_vector, i);
        current_cluster = clustersList[i];
        if (current_cluster->cluster_vector.size() > 0) {
            Color c_mean = colors_mean(current_cluster->cluster_vector);
            centroid_vector[i] = c_mean;
        } else {
            int xr = rand_int(image.columns());
            int yr = rand_int(image.rows());
            Color c = image.pixelColor(xr, yr);
            centroid_vector[i] = c;
        }
    }
}

void kmean(const Image &image, int reducSize, map<string, PALETTE_ENTRY> &palette)
{
    // vector<CLUSTER_LIST> clusters_vector;
    CLUSTER_LIST **clustersList = new CLUSTER_LIST*[reducSize];
    for (int i = 0; i < reducSize; i++) {
        CLUSTER_LIST *cl = new CLUSTER_LIST;
        clustersList[i] = cl;
    }

    vector<Color> centroid_vector;
    // new_cluster_list(reducSize, clusters_vector);

    int iter = 0, max_iter = 100;
    float *previous_variance = new float[reducSize];
    for (int i = 0; i < reducSize; i++) previous_variance[i] = 1.0;
    float variance = 0.0, delta = 0.0, delta_max = 0.0, /*threshold = 0.00005*/  threshold = 80.0f;
    CLUSTER_LIST *current_cluster;

    while (1) {
        // Génération des clusters
        // get_cluster_centroid(image, clusters_vector, centroid_vector);
        printf("1 %p - %p\n", clustersList[0], clustersList[7]);
        get_cluster_centroid(image, clustersList, reducSize, centroid_vector);

        // clusters_vector.clear();
        for (int i = 0; i < reducSize; i++) {
            delete clustersList[i];
        }
        delete [] clustersList;

        // new_cluster_list(reducSize, clusters_vector);
        clustersList = new CLUSTER_LIST*[reducSize];
        for (int i = 0; i < reducSize; i++) {
            clustersList[i] = new CLUSTER_LIST;
        }
printf("2 %p - %p\n", clustersList[0], clustersList[7]);

        for (int y = 0; y < image.rows(); y++) {
            for (int x = 0; x < image.columns(); x++) {
                Color c = image.pixelColor(x, y);
                int nc_idx = nearest_color(c, centroid_vector);
                // current_cluster = clusters_vector[nc_idx];
                current_cluster = clustersList[nc_idx];
                current_cluster->cluster_vector.push_back(c);
                // clusters_vector[nc_idx] = current_cluster;
                // cout << x << ":" << nc_idx << endl;
            }
            // cout << y << endl;
        }

        // Test de la convergence
        delta_max = 0;
        // int i = 0;
        // for (auto it = clusters_vector.begin(); it != clusters_vector.end(); it++) {
        //     variance = color_variance(it->cluster_vector);
        //     delta = std::fabs(previous_variance[i] - variance);
        //     delta_max = std::max(delta, delta_max);
        //     previous_variance[i] = variance;
        //     i++;
        // }

        for (int i = 0; i < reducSize; i++) {
            current_cluster = clustersList[i];
            variance = color_variance(current_cluster->cluster_vector);
            delta = std::fabs(previous_variance[i] - variance);
            delta_max = std::max(delta, delta_max);
            previous_variance[i] = variance;
        }

        printf("Kmean iteration %d - variance %f\n", iter, delta_max);
        if (delta_max < threshold || iter++ > max_iter) {
            break;
        }
        printf("4 %p - %p\n", clustersList[0], clustersList[7]);
    }
    uint8_t j = 0;
    for (auto it = centroid_vector.begin(); it != centroid_vector.end(); it++) {
        string k = getPaletteKey(*it);
        cout << (int) j << "=" << k << " -> " << it->quantumRed() << "," << it->quantumGreen() << "," << it->quantumBlue() <<  endl;
        palette.insert(std::pair<string, PALETTE_ENTRY>(k, { Color(it->quantumRed(), it->quantumGreen(), it->quantumBlue()), j, 0}));
        j++;
    }
    delete[] previous_variance;
}
