#pragma once
#include "fonts.h"
#include "image.h"
#include "textures.h"

class Player {
public:
    Texture tex;
    int xres, yres;
    int pos_x, pos_y;

    Player();
    void init(const char* imagePath);
    void render_hand();
};

void kian_text(Rect *r);