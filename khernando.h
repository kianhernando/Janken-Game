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
        int base_x, base_y;

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
void render_text(Rect *rec, const char* lines[], const int num_lines);
void render_box();
void render_player();

extern const char* intro[];
extern const char* lorem[];