#pragma once

// ORIGINALLY FROM BACKGROUND.CPP
// ORIGINAL AUTHOR: GORDON GRIESEL
class Image {
public:
    int width, height;
    unsigned char *data;
    ~Image() { delete [] data; }
    Image(const char *fname) {
        if (fname[0] == '\0')
            return;
        char name[40];
        strcpy(name, fname);
        int slen = strlen(name);
        name[slen-4] = '\0';
        char ppmname[80];
        sprintf(ppmname,"%s.ppm", name);
        char ts[100];
        sprintf(ts, "convert %s %s", fname, ppmname);
        system(ts);
        FILE *fpi = fopen(ppmname, "r");
        if (fpi) {
            char line[200];
            fgets(line, 200, fpi);
            fgets(line, 200, fpi);
            //skip comments and blank lines
            while (line[0] == '#' || strlen(line) < 2)
                fgets(line, 200, fpi);
            sscanf(line, "%i %i", &width, &height);
            fgets(line, 200, fpi);
            //get pixel data
            int n = width * height * 3;
            data = new unsigned char[n];
            for (int i = 0; i < n; i++)
                data[i] = fgetc(fpi);
            fclose(fpi);
        } else {
            printf("ERROR opening image: %s\n", ppmname);
            exit(0);
        }
        unlink(ppmname);
    }
};

// ORIGINALLY FROM RAINFOREST.CPP
// ORIGINAL AUTHOR: GORDON GRIESEL
// EDITED TO ALLOW BLACK AND WHITE IMAGES TO BE TRANSPARENT
// (ORIGINALLY WOULD MAKE THE BLACK PARTS OF AN IMAGE TRANSPARENT)
inline unsigned char *buildAlphaData(Image *img)
{
    // Add 4th component to an RGB stream...
    // RGBA
    // When you do this, OpenGL is able to use the A component to determine
    // transparency information.
    // It is used in this application to erase parts of a texture-map from view.
    int i;
    int a, b, c;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *)img->data;
    newdata = (unsigned char *)malloc(img->width * img->height * 4);
    ptr = newdata;
    for (i = 0; i < img->width * img->height * 3; i += 3) {
        a = *(data+0);
        b = *(data+1);
        c = *(data+2);
        *(ptr+0) = a;
        *(ptr+1) = b;
        *(ptr+2) = c;

        // Make magenta (255,0,255) transparent
        if (a == 255 && b == 0 && c == 255) {
            *(ptr+3) = 0;
        } else {
            *(ptr+3) = 255;
        }

        ptr += 4;
        data += 3;
    }
    return newdata;
}