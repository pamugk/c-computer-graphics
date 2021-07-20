#include "image.h"
#include <stdio.h>

int main(int argc, const char **argv) {
    int res = 0;
    image image = readHeightmap("../gebco_08_rev_elev_21600x10800.png");
    if (image.contents != NULL) {
        printf("%ix%i\n", image.width, image.height);
    } else {
        printf("Exeption occurred while loading image\n");
        res = 1;
    }
    freeImage(&image);
    return res;
}