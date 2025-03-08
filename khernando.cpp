// Kian Hernando
// CMPS 3350 - Janken Game Project
// Last edited: March 2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "khernando.h"
#include "image.h"

// Kian Hernando
// CMPS 3350 - Janken Game Project
//
// TO-DO:
// -> add enemy and player interaction to 
//    call int check_keys() to stop the background
// -> start creating architecture for game (rock, paper, scissors)

class Global {
public:
    int xres, yres;
};

extern Global g;

Player::Player() {
    tex.xc[0] = 0.0;
    tex.xc[1] = 1.0;
    tex.yc[0] = 0.0;
    tex.yc[1] = 1.0;
    xres = 100;  
    yres = 100;  

    // For test purposes; put it in center of screen;
    pos_x = g.xres/2 - xres/2;
    pos_y = g.yres/2 - yres/2;
    tex.backImage = nullptr;
}

void Player::init(const char* imagePath = "assets/idle.png") {
    // Load the image first
    tex.backImage = new Image(imagePath);
    
    // Generate one texture
    glGenTextures(1, &tex.backTexture);

    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int w = tex.backImage->width;
    int h = tex.backImage->height;

    // Take raw data and create texture with specific formatting
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
        GL_RGB, GL_UNSIGNED_BYTE, tex.backImage->data);
}

void Player::render_hand() {
    // Draw the hand; similar to drawing the box in waterlab
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(tex.xc[0], tex.yc[1]); glVertex2i(pos_x,         pos_y);
        glTexCoord2f(tex.xc[0], tex.yc[0]); glVertex2i(pos_x,         pos_y + yres);
        glTexCoord2f(tex.xc[1], tex.yc[0]); glVertex2i(pos_x + xres,  pos_y + yres);
        glTexCoord2f(tex.xc[1], tex.yc[1]); glVertex2i(pos_x + xres,  pos_y);
    glEnd();
}

void kian_text(Rect* r)
{
    ggprint8b(r, 16, 0x000000, "Jankenpoi! -Kian Hernando");
};

Image* newImage(const char* fname) {
    return new Image(fname);
}


