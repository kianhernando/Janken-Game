#pragma once
#include "fonts.h"
#include "image.h"
#include "textures.h"

class Global;

// Declare the external global variable
extern Global g;

class Player {
    public:
        Player();
        void init(const char* imagePath);
        void init_hp();
        void render_hand();
        void render_hp();
        void update();
        Texture tex;
        Texture hp;  // Add health bar texture
        int xres, yres;
        float pos_x, pos_y;
        float base_y;
        float hp_x, hp_y;  // Add health bar position
};

class Enemy {
    public:
        Enemy();
        void init(const char* imagePath);
        void init_hp();
        void render_enemy();
        void render_hp();
        void update();
        Texture tex;
        Texture hp;
        int xres, yres;
        float pos_x, pos_y;
        float base_x, base_y;
        float hp_x, hp_y;  // Add health bar position
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
void kianText(Rect *rKian);
void render_text(Rect *rec, const char* lines[], const int num_lines);
void render_box();
void render_player();

extern const char* intro[];
extern const char* instructions[];