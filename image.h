#pragma once

// ORIGINALLY FROM BACKGROUND.CPP
// ORIGINAL AUTHOR: GORDON GRIESEL
class Image 
{
public:
    int width, height;
    unsigned char *data;
    Image(const char *fname);
    ~Image();
};

// ORIGINALLY FROM RAINFOREST.CPP
// ORIGINAL AUTHOR: GORDON GRIESEL
// EDITED TO ALLOW BLACK AND WHITE IMAGES TO BE TRANSPARENT
// (ORIGINALLY WOULD MAKE THE BLACK PARTS OF AN IMAGE TRANSPARENT)
unsigned char *buildAlphaData(Image *img);