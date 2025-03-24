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
extern void stopBackground();
Box box(576, 75);
Box top(576, 40);

Box::Box(float w, float h)
{
    width = w;
    height = h;
    pos[0] = 576/2.0f;
    pos[1] = 0;
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
}

Player::Player()
{
    // Set sprite coordinates
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
    hp.backImage = nullptr;
    hp_xres = 0;
    hp_yres = 0;
    hp_x = 10;
    hp_y = 294;
}

void Player::init(const char* imagePath)
{
    tex.backImage = new Image(imagePath);

    // Call datatypes to store width and height for manipulation
    int image_width = tex.backImage->width;
    int image_height = tex.backImage->height;
    
    // Scale it 3x original size
    float scale = 3.0f;
    xres = image_width * scale;
    yres = image_height * scale;

    // Set player sprite coordinates
    base_x = 576/4 - xres/2;
    base_y = 324/2.75 - yres/2;
    pos_x = base_x;
    pos_y = base_y;

    // Initalize sprite textures
    glGenTextures(1, &tex.backTexture);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, buildAlphaData(tex.backImage));
}

void Player::init_hp(int health)
{
    hp.backImage = new Image("assets/health/100.png");

    int image_width = hp.backImage->width;
    int image_height = hp.backImage->height;
    
    // Scale it 3x original size
    float scale = 3.0f;
    hp_xres = image_width * scale;
    hp_yres = image_height * scale;
    
    // Initalize health bar textures
    glGenTextures(1, &hp.backTexture);
    glBindTexture(GL_TEXTURE_2D, hp.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hp.backImage->width, 
            hp.backImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
            buildAlphaData(hp.backImage));
}

void Player::render_player()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render player sprite textures
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(tex.xc[0], tex.yc[1]);
    glVertex2i(pos_x, pos_y);
    glTexCoord2f(tex.xc[0], tex.yc[0]);
    glVertex2i(pos_x, pos_y + yres);
    glTexCoord2f(tex.xc[1], tex.yc[0]);
    glVertex2i(pos_x + xres, pos_y + yres);
    glTexCoord2f(tex.xc[1], tex.yc[1]);
    glVertex2i(pos_x + xres, pos_y);
    glEnd();

    glDisable(GL_BLEND);
}

void Player::render_hp()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render health bar textures
    glBindTexture(GL_TEXTURE_2D, hp.backTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2i(hp_x, hp_y);
    glTexCoord2f(0.0, 0.0);
    glVertex2i(hp_x, hp_y + hp_yres);
    glTexCoord2f(1.0, 0.0);
    glVertex2i(hp_x + hp_xres, hp_y + hp_yres);
    glTexCoord2f(1.0, 1.0);
    glVertex2i(hp_x + hp_xres, hp_y);
    glEnd();

    glDisable(GL_BLEND);
}

void Player::update(float amplitude)
{
    static float time = 0.0f;
    static const float speed = 2.0f;

    // Apply sinodal wave to player's original coords
    float offset = amplitude * sin(speed * time);
    pos_y = base_y + offset;

    time += 0.1f;
}

void Player::changeImage(const char* imagePath)
{
    // Conditional to check if player sprite is already initalized
    if (tex.backImage != nullptr) {
        delete tex.backImage;
        glDeleteTextures(1, &tex.backTexture);
    }

    // Insert new sprite textures
    tex.backImage = new Image(imagePath);
    int image_width = tex.backImage->width;
    int image_height = tex.backImage->height;
    
    float scale = 3.0f;
    xres = image_width * scale;
    yres = image_height * scale;

    // Render new sprite textures
    glGenTextures(1, &tex.backTexture);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, buildAlphaData(tex.backImage));
    
    // Set coordinates of new sprite textures
    tex.xc[0] = 0.0;
    tex.xc[1] = 1.0;
    tex.yc[0] = 0.0;
    tex.yc[1] = 1.0;
}

void Player::changeHealthBar(int health)
{
    // Conditional to check if health bar is already initalized
    if (tex.backImage != nullptr) {
        delete hp.backImage;
        glDeleteTextures(1, &hp.backTexture);
    }

    // Conditional to check player's HP amount
    if (health == 100) {
        hp.backImage = new Image("assets/health/100.png");
    } else if (health == 90) {
        hp.backImage = new Image("assets/health/90.png");
    } else if (health == 80) {
        hp.backImage = new Image("assets/health/80.png");
    } else if (health == 70) {
        hp.backImage = new Image("assets/health/70.png");
    } else if (health == 60) {
        hp.backImage = new Image("assets/health/60.png");
    } else if (health == 50) {
        hp.backImage = new Image("assets/health/50.png");
    } else if (health == 40) {
        hp.backImage = new Image("assets/health/40.png");
    } else if (health == 30) {
        hp.backImage = new Image("assets/health/30.png");
    } else if (health == 20) {
        hp.backImage = new Image("assets/health/20.png");
    } else if (health == 10) {
        hp.backImage = new Image("assets/health/10.png");
    } else if (health == 0) {
        hp.backImage = new Image("assets/health/0.png");
    }

    // Render health bar textures
    glGenTextures(1, &hp.backTexture);
    glBindTexture(GL_TEXTURE_2D, hp.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hp.backImage->width, 
            hp.backImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            buildAlphaData(hp.backImage));
}

Enemy::Enemy()
{
    // Set sprite coordinates
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
    hp.backImage = nullptr;
    hp_xres = 0;
    hp_yres = 0;
    hp_x = 449;
    hp_y = 294;
    hitBarrier = false;
}

void Enemy::init(const char* imagePath)
{
    tex.backImage = new Image(imagePath);
    
    // Call datatypes to store width and height
    int image_width = tex.backImage->width;
    int image_height = tex.backImage->height;
   
    // Scale it 3x original size
    float scale = 3.0f;
    xres = image_width * scale;
    yres = image_height * scale;

    // Set enemy sprite coordinates
    base_x = 576 + xres;
    base_y = 324/2.75 - yres/2;
    pos_x = base_x;
    pos_y = base_y;

    hitBarrier = false;

    // Initalize sprite textures
    glGenTextures(1, &tex.backTexture);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, buildAlphaData(tex.backImage));
}

void Enemy::init_hp() 
{
    hp.backImage = new Image("assets/health/100.png");

    int image_width = hp.backImage->width;
    int image_height = hp.backImage->height;
    
    // Scale it 3x original size
    float scale = 3.0f;
    hp_xres = image_width * scale;
    hp_yres = image_height * scale;
    
    // Initalize health bar textures
    glGenTextures(1, &hp.backTexture);
    glBindTexture(GL_TEXTURE_2D, hp.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hp.backImage->width, 
            hp.backImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
            buildAlphaData(hp.backImage));
}

void Enemy::render_enemy()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render enemy sprite textures
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(tex.xc[0], tex.yc[1]);
    glVertex2i(pos_x, pos_y);
    glTexCoord2f(tex.xc[0], tex.yc[0]);
    glVertex2i(pos_x, pos_y + yres);
    glTexCoord2f(tex.xc[1], tex.yc[0]);
    glVertex2i(pos_x + xres, pos_y + yres);
    glTexCoord2f(tex.xc[1], tex.yc[1]);
    glVertex2i(pos_x + xres, pos_y);
    glEnd();

    glDisable(GL_BLEND);
}

void Enemy::render_hp()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Render health bar textures
    glBindTexture(GL_TEXTURE_2D, hp.backTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0);
    glVertex2i(hp_x, hp_y);
    glTexCoord2f(1.0, 0.0);
    glVertex2i(hp_x, hp_y + hp_yres);
    glTexCoord2f(0.0, 0.0);
    glVertex2i(hp_x + hp_xres, hp_y + hp_yres);
    glTexCoord2f(0.0, 1.0);
    glVertex2i(hp_x + hp_xres, hp_y);
    glEnd();

    glDisable(GL_BLEND);
}

void Enemy::update(float amplitude)
{
    static float time = 0.0f;
    static const float speed = 2.0f;
    // Set barrier
    static const float barrier = 3 * 576/4 - xres/2;

    float offset = amplitude * sin(speed * time);
    pos_y = base_y + offset;

    if (!hitBarrier) {
        // Move the enemy left from off-screen
        pos_x -= 4.0f;

        if (pos_x <= barrier) {
            hitBarrier = true;
            pos_x = barrier;
        }

        time += 0.1f;
    } else {
        time += 0.1f;
    }
}

void Enemy::changeImage(const char* imagePath)
{
    // Conditional to check if enemy sprite is already initalized
    if (tex.backImage != nullptr) {
        delete tex.backImage;
        glDeleteTextures(1, &tex.backTexture);
    }

    // Insert new sprite textures
    tex.backImage = new Image(imagePath);
    int image_width = tex.backImage->width;
    int image_height = tex.backImage->height;
    
    float scale = 3.0f;
    xres = image_width * scale;
    yres = image_height * scale;

    // Render new sprite textures
    glGenTextures(1, &tex.backTexture);
    glBindTexture(GL_TEXTURE_2D, tex.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, buildAlphaData(tex.backImage));
    
    // Set coordinates of new sprite textures
    tex.xc[0] = 0.0;
    tex.xc[1] = 1.0;
    tex.yc[0] = 0.0;
    tex.yc[1] = 1.0;
}

void Enemy::changeHealthBar(int health)
{
    // Conditional to check if health bar is already initalized
    if (tex.backImage != nullptr) {
        delete hp.backImage;
        glDeleteTextures(1, &hp.backTexture);
    }

    // Conditional to check player's HP amount
    if (health == 100) {
        hp.backImage = new Image("assets/health/100.png");
    } else if (health == 90) {
        hp.backImage = new Image("assets/health/90.png");
    } else if (health == 80) {
        hp.backImage = new Image("assets/health/80.png");
    } else if (health == 70) {
        hp.backImage = new Image("assets/health/70.png");
    } else if (health == 60) {
        hp.backImage = new Image("assets/health/60.png");
    } else if (health == 50) {
        hp.backImage = new Image("assets/health/50.png");
    } else if (health == 40) {
        hp.backImage = new Image("assets/health/40.png");
    } else if (health == 30) {
        hp.backImage = new Image("assets/health/30.png");
    } else if (health == 20) {
        hp.backImage = new Image("assets/health/20.png");
    } else if (health == 10) {
        hp.backImage = new Image("assets/health/10.png");
    } else if (health == 0) {
        hp.backImage = new Image("assets/health/0.png");
    }

    // Render health bar textures
    glGenTextures(1, &hp.backTexture);
    glBindTexture(GL_TEXTURE_2D, hp.backTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, hp.backImage->width, 
            hp.backImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            buildAlphaData(hp.backImage));
}

void render_box()
{
    // Render black box
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

void render_top()
{
    glColor3ub(top.color[0], top.color[1], top.color[2]);
    glPushMatrix();
    glTranslatef(top.pos[0], 324 - top.height, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-top.width/2, 0);
    glVertex2f(-top.width/2, top.height);
    glVertex2f(top.width/2, top.height);
    glVertex2f(top.width/2, 0);
    glEnd();
    glPopMatrix();
}

const char* intro[] = {
    "HI MY NAME IS HANDEL!",
    "THIS IS MY DEBUT GAME!", 
    "IT IS CALLED JANKEN!"
};

const char* controls[] = {
    "PRESS 'M' TO SHOW CONTRIBUTORS", 
    "PRESS 'SPACE' TO SHOW ENEMY (WILL AUTOMATICALLY STOP BACKGROUND)",
    "PRESS 'R', 'P', OR 'S' FOR ROCK-PAPER-SCISSORS, 'N' FOR NORMAL"
};

void render_text(Rect *rec, const char* lines[], const int num_lines)
{
    // Set text box position
    float box_center_x = box.pos[0];
    float box_bottom = box.pos[1];
    
    const int char_width = 6;
    const int line_height = 12;
    
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
        // Edited to fix passed-in text
        rec->bot = start_y + ((num_lines - 1 - i) * line_height);
        rec->center = 0;
        ggprint8b(rec, 12, 0xFFFFFF, lines[i]);
    }
}

void controlText(Rect* rControl)
{
    ggprint8b(rControl, 16, 0xFFFFFF, "HIT C FOR CONTROLS");
}

void kianText(Rect* rKian)
{
    ggprint8b(rKian, 16, 0xFFFFFF, "Kian");
}
