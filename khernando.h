#pragma once
#include "fonts.h"
#include "image.h"
#include "textures.h"

class Global;
extern Global g;

class Player {
    public:
        Texture tex;
        int xres, yres;
        int pos_x, pos_y;
        int base_y;

        Player();
        void init(const char* imagePath);
        void render_hand();
        void update();
};

class Enemy {
    public:
        Texture tex;
        int xres, yres;
        int pos_x, pos_y;
        int base_x;
        float speed;

        Enemy();
        void init(const char* imagePath);
        void render_enemy();
        void update();
};

// Originally from waterfall (lab 6)
class Box {
    public:
        float width, height;
        float pos[2];
        unsigned char color[3];
        
        Box();
        Box(float w, float h);
};

extern Box box;
void kian_text(Rect *r);
void render_text(Rect *rec);
void render_box();
void render_player();