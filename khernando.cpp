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

extern Global g;
Box box(576, 75);

Box::Box() {
    width = 50;
    height = 75;
    pos[0] = 576/2.0f;
    pos[1] = 0;
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
}

Box::Box(float w, float h) {
    width = w;
    height = h;
    pos[0] = 576/2.0f;
    pos[1] = 0;
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
}

Player::Player() {
    tex.xc[0] = 0.0;
    tex.xc[1] = 1.0;
    tex.yc[0] = 0.0;
    tex.yc[1] = 1.0;
    
    // Empty values to be set in init();
    xres = 0;  
    yres = 0;  
    pos_x = 0;
    pos_y = 0;
    tex.backImage = nullptr;
}

void Player::init(const char* imagePath) {
    tex.backImage = new Image(imagePath);

    int og_w = tex.backImage->width;
    int og_h = tex.backImage->height;

    float scale = 3.0f;

    xres = og_w * scale;
    yres = og_h * scale;

    // Use hard-coded values for window dimensions (576x324)
    pos_x = 576/2 - xres/2;
    pos_y = 324/2 - yres/2;

    // Generate one texture
    glGenTextures(1, &tex.backTexture);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Get alpha channel data to allow transparency
    unsigned char *alphaData = buildAlphaData(tex.backImage);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, og_w, og_h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, alphaData);

    // Free alphaData since OpenGL has a copy
    free(alphaData);
}

void Player::render_hand() {
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw the hand; similar to drawing the box in waterlab
    // UDATE: Changed to 4 channels to allow alpha data
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(tex.xc[0], tex.yc[1]); glVertex2i(pos_x,         pos_y);
        glTexCoord2f(tex.xc[0], tex.yc[0]); glVertex2i(pos_x,         pos_y + yres);
        glTexCoord2f(tex.xc[1], tex.yc[0]); glVertex2i(pos_x + xres,  pos_y + yres);
        glTexCoord2f(tex.xc[1], tex.yc[1]); glVertex2i(pos_x + xres,  pos_y);
    glEnd();

    // Optional: Disable blending if you don't need it for other rendering
    glDisable(GL_BLEND);
}

void render_box()
{
    glColor3ub(box.color[0], box.color[1], box.color[2]);
    glPushMatrix();
    glTranslatef(box.pos[0], box.pos[1], 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-box.width/2, 0);
        glVertex2f(-box.width/2, box.height);
        glVertex2f(box.width/2, box.height);
        glVertex2f(box.width/2, 0);
    glEnd();
    glPopMatrix();
}

void render_text(Rect *rec)
{
    float box_center_x = box.pos[0];
    float box_bottom = box.pos[1];
    
    const int char_width = 6;
    const int line_height = 12;
    
    const char* lines[] = {
        "HI MY NAME IS HANDEL!",
        "THIS IS MY DEBUT GAME!",
        "IT IS CALLED JANKEN!"
    };
    const int num_lines = 3;
    
    // Find the longest line
    int max_length = 0;
    for (int i = 0; i < num_lines; i++) {
        int line_length = strlen(lines[i]);
        if (line_length > max_length) {
            max_length = line_length;
        }
    }
    
    // Calculate total height and starting position
    float total_height = line_height * num_lines;
    float start_y = box_bottom + (box.height - total_height) / 2;
    
    // Calculate left alignment position based on longest line
    float text_block_width = max_length * char_width;
    float start_x = box_center_x - (text_block_width / 2);
    
    // Render each line from the same starting x position
    for (int i = 0; i < num_lines; i++) {
        rec->left = start_x;
        rec->bot = start_y + (i * line_height);
        rec->center = 0;
        
        ggprint8b(rec, 12, 0xFFFFFF, lines[i]);
    }
}

void kian_text(Rect* r)
{
    ggprint8b(r, 16, 0xFFFFFF, "Graphics done by: Kian Hernando");
    ggprint8b(r, 16, 0xFFFFFF, "Press '-' to stop, Press '=' to start");

}

Image* newImage(const char* fname) {
    return new Image(fname);
}


