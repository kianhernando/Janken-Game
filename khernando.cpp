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
#include "stran.h"
#include "image.h"

bool animationDone = false;
bool isRPSMoving = false;

Box box(576, 75);
Box top(576, 40);
Box selection1(90, 65);
Box selection2(90, 65);
Box selection3(90, 65);

Box::Box(float w, float h)
{
    width = w;
    height = h;
    pos[0] = 576/2.0f;
    pos[1] = 0;
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    boxImage = nullptr;
    texture = 0;
    xc[0] = 0.0;
    xc[1] = 1.0;
    yc[0] = 0.0;
    yc[1] = 1.0;
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
    if (health <= 100 && health > 90) {
        hp.backImage = new Image("assets/health/100.png");
    } else if (health <= 90 && health > 80) {
        hp.backImage = new Image("assets/health/90.png");
    } else if (health <= 80 && health > 70) {
        hp.backImage = new Image("assets/health/80.png");
    } else if (health <= 70 && health > 60) {
        hp.backImage = new Image("assets/health/70.png");
    } else if (health <= 60 && health > 50) {
        hp.backImage = new Image("assets/health/60.png");
    } else if (health <= 50 && health > 40) {
        hp.backImage = new Image("assets/health/50.png");
    } else if (health <= 40 && health > 30) {
        hp.backImage = new Image("assets/health/40.png");
    } else if (health <= 30 && health > 20) {
        hp.backImage = new Image("assets/health/30.png");
    } else if (health <= 20 && health > 10) {
        hp.backImage = new Image("assets/health/20.png");
    } else if (health <= 10 && health > 0) {
        hp.backImage = new Image("assets/health/10.png");
    } else if (health <= 0) {
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

void initSelections() {
    float totalWidth = box.width;
    float spacing = totalWidth / 4;
    
    float yPos = box.pos[1] + (box.height - selection1.height) / 2;
    
    selection1.pos[0] = box.pos[0] - spacing;
    selection1.pos[1] = yPos;
    
    selection2.pos[0] = box.pos[0];
    selection2.pos[1] = yPos;
    
    selection3.pos[0] = box.pos[0] + spacing;
    selection3.pos[1] = yPos;

    // Pass the image paths for rock, paper, scissors
    renderBox(selection1, "assets/player/rock_y.png");
    renderBox(selection2, "assets/player/paper_y.png");
    renderBox(selection3, "assets/player/scissors_y.png");
}

void controlText(Rect* rControl)
{
    ggprint8b(rControl, 16, 0xFFFFFF, "HIT C FOR CONTROLS");
}

void kianText(Rect* rKian)
{
    ggprint8b(rKian, 16, 0xFFFFFF, "Kian");
}

void renderBox(Box& sel, const char* imagePath) {
    glColor3ub(sel.color[0], sel.color[1], sel.color[2]);
    glPushMatrix();
    glTranslatef(sel.pos[0], sel.pos[1], 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-sel.width/2, 0);
    glVertex2f(-sel.width/2, sel.height);
    glVertex2f(sel.width/2, sel.height);
    glVertex2f(sel.width/2, 0);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2.0);
    glPushMatrix();
    glTranslatef(sel.pos[0], sel.pos[1], 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-sel.width/2, 0);
    glVertex2f(-sel.width/2, sel.height);
    glVertex2f(sel.width/2, sel.height);
    glVertex2f(sel.width/2, 0);
    glEnd();
    glPopMatrix();
    
    if (imagePath != nullptr) {
        glEnable(GL_TEXTURE_2D);
        
        if (sel.boxImage != nullptr) {
            delete sel.boxImage;
            glDeleteTextures(1, &sel.texture);
        }
        
        sel.boxImage = new Image(imagePath);
        
        sel.xc[0] = 0.0;
        sel.xc[1] = 1.0;
        sel.yc[0] = 0.0;
        sel.yc[1] = 1.0;
        
        glGenTextures(1, &sel.texture);
        glBindTexture(GL_TEXTURE_2D, sel.texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sel.boxImage->width, 
            sel.boxImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
            buildAlphaData(sel.boxImage));
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, sel.texture);
        glPushMatrix();
        glTranslatef(sel.pos[0], sel.pos[1], 0.0f);
        
        float imgWidth = (float)sel.boxImage->width;
        float imgHeight = (float)sel.boxImage->height;
        float boxWidth = sel.width;
        float boxHeight = sel.height;
        
        float scaleX = boxWidth / imgWidth;
        float scaleY = boxHeight / imgHeight;
        float scale = std::min(scaleX, scaleY) * 0.8f;
        
        float newWidth = imgWidth * scale;
        float newHeight = imgHeight * scale;
        
        float xOffset = (boxWidth - newWidth) / 2.0f;
        float yOffset = (boxHeight - newHeight) / 2.0f;
        
        glBegin(GL_QUADS);
        glTexCoord2f(sel.xc[0], sel.yc[1]);
        glVertex2f(-boxWidth/2 + xOffset, yOffset);
        glTexCoord2f(sel.xc[0], sel.yc[0]);
        glVertex2f(-boxWidth/2 + xOffset, yOffset + newHeight);
        glTexCoord2f(sel.xc[1], sel.yc[0]);
        glVertex2f(-boxWidth/2 + xOffset + newWidth, yOffset + newHeight);
        glTexCoord2f(sel.xc[1], sel.yc[1]);
        glVertex2f(-boxWidth/2 + xOffset + newWidth, yOffset);
        glEnd();
        
        glPopMatrix();
        glDisable(GL_BLEND);
    }
}

bool renderAnimation(Player &player, Enemy &enemy)
{
    static bool isInitialized = false;
    static bool hasCollided = false; 
    static float playerStart = -36;
    static float enemyStart = 600;
    static float moveSpeed = 15.0f;
    static float time = 0.0f;
    static const float speed = 0.5f;
    static const float amplitude = 20.0f;
    static const float cycles = 3.0f;
    static const float duration = (2.0f * M_PI * cycles) / speed;
    static float base_y = 0.0f;
    static bool RPSdone = false;
    
    static int handtype = 0;
    static float lastChanged = 0.0f;
    static Image* playerSprites[3] = {nullptr, nullptr, nullptr};
    static Image* enemyImages[3] = {nullptr, nullptr, nullptr};
    static GLuint playerTextures[3] = {0, 0, 0};
    static GLuint enemyTextures[3] = {0, 0, 0};
    static const char* playerPaths[3] = {
        "assets/player/rock_x.png",
        "assets/player/paper_x.png",
        "assets/player/scissors_x.png"
    };
    static const char* enemyPaths[3] = {
        "assets/enemy/rock.png",
        "assets/enemy/paper.png",
        "assets/enemy/scissors.png"
    };
    
    static bool showExplosion = false;
    static int explosionFrame = 0;
    static const int explosionFrames = 12;
    static float explosionSize = 450.0f;
    static float explosionX = 0.0f;
    static float explosionY = 0.0f;
    
    static Image* explosionImage = nullptr;
    static GLuint explosionTexture = 0;
    static const int GRID_SIZE = 12;
    static float explosionDelay = 1.3f;
    static float explosionCounter = 0.0f;
    
    if (animationDone) {
        return true;
    }
    
    if (!isInitialized) {
        for (int i = 0; i < 3; i++) {
            playerSprites[i] = new Image(playerPaths[i]);
            glGenTextures(1, &playerTextures[i]);
            glBindTexture(GL_TEXTURE_2D, playerTextures[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, playerSprites[i]->width, 
                playerSprites[i]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                buildAlphaData(playerSprites[i]));
            
            enemyImages[i] = new Image(enemyPaths[i]);
            glGenTextures(1, &enemyTextures[i]);
            glBindTexture(GL_TEXTURE_2D, enemyTextures[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, enemyImages[i]->width, 
                enemyImages[i]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                buildAlphaData(enemyImages[i]));
        }
        
        player.init(playerPaths[0]);
        enemy.init(enemyPaths[0]);
        
        if (explosionImage == nullptr) {
            explosionImage = new Image("assets/effects/explosion.png");
            
            glGenTextures(1, &explosionTexture);
            glBindTexture(GL_TEXTURE_2D, explosionTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, explosionImage->width, 
                explosionImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                buildAlphaData(explosionImage));
        }
        
        player.pos_x = playerStart - 35;
        enemy.pos_x = enemyStart - 35;
        
        base_y = player.pos_y;
        
        isInitialized = true;
        isRPSMoving = false;
        RPSdone = false;
        hasCollided = false;
        handtype = 0;
    }
    
    float center = 265;
    float playerEdge = center - 32;
    float enemyEdge = center + 1;
    
    float playerPoint = playerEdge - 30;
    float enemyPoint = enemyEdge + 30;
    
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(0, 324);
        glVertex2i(576, 324);
        glVertex2i(576, 0);
    glEnd();
    
    if (!isRPSMoving && !RPSdone && !hasCollided) {
        if (player.pos_x < playerPoint) {
            player.pos_x += moveSpeed;
            if (player.pos_x > playerPoint) {
                player.pos_x = playerPoint;
            }
        }
        
        if (enemy.pos_x > enemyPoint) {
            enemy.pos_x -= (moveSpeed * 1.2f);
            if (enemy.pos_x < enemyPoint) {
                enemy.pos_x = enemyPoint;
            }
        }
        
        if (player.pos_x >= playerPoint && enemy.pos_x <= enemyPoint) {
            isRPSMoving = true;
            time = 0.0f;
            lastChanged = 0.0f;
        }
    }
    
    if (isRPSMoving && !hasCollided) {
        float offset = amplitude * sin(time * speed);
        player.pos_y = base_y + offset;
        enemy.pos_y = base_y + offset;
        
        float sinValue = sin(time * speed);
        
        if (sinValue > 0.95f && (time - lastChanged) > (M_PI / speed)) {
            handtype = (handtype + 1) % 3;
            
            player.tex.backImage = playerSprites[handtype];
            player.tex.backTexture = playerTextures[handtype];
            
            enemy.tex.backImage = enemyImages[handtype];
            enemy.tex.backTexture = enemyTextures[handtype];
            
            lastChanged = time;
        }
        
        time += 1.0f;
        
        if (time >= duration) {
            isRPSMoving = false;
            RPSdone = true;
            
            player.pos_y = base_y;
            enemy.pos_y = base_y;
            
            handtype = 0;
            player.tex.backImage = playerSprites[handtype];
            player.tex.backTexture = playerTextures[handtype];
            
            enemy.tex.backImage = enemyImages[handtype];
            enemy.tex.backTexture = enemyTextures[handtype];
        }
    }
    
    if (RPSdone && !hasCollided) {
        if (player.pos_x < playerEdge) {
            player.pos_x += moveSpeed;
            if (player.pos_x > playerEdge) {
                player.pos_x = playerEdge;
            }
        }
        
        if (enemy.pos_x > enemyEdge) {
            enemy.pos_x -= (moveSpeed * 1.2f);
            if (enemy.pos_x < enemyEdge) {
                enemy.pos_x = enemyEdge;
            }
        }
        
        if (player.pos_x >= playerEdge && enemy.pos_x <= enemyEdge) {
            hasCollided = true;
            showExplosion = true;
            explosionFrame = 0;
            
            explosionX = center + 30;
            explosionY = player.pos_y + player.yres / 2.0f + 40;
        }
    }
    
    if (!showExplosion) {
        player.render_player();
        enemy.render_enemy();
    } else {
        player.render_player();
        enemy.render_enemy();
        
        if (explosionTexture != 0) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            glBindTexture(GL_TEXTURE_2D, explosionTexture);
            
            float frameWidth = 1.0f / GRID_SIZE;
            float texLeft = frameWidth * (explosionFrame % GRID_SIZE);
            float texRight = texLeft + frameWidth;
            
            float halfWidth = explosionSize / 2.0f;
            float halfHeight = explosionSize / 2.0f;
            
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
                glTexCoord2f(texLeft, 1.0f);
                glVertex2f(explosionX - halfWidth, explosionY - halfHeight);
                
                glTexCoord2f(texLeft, 0.0f);
                glVertex2f(explosionX - halfWidth, explosionY + halfHeight);
                
                glTexCoord2f(texRight, 0.0f);
                glVertex2f(explosionX + halfWidth, explosionY + halfHeight);
                
                glTexCoord2f(texRight, 1.0f);
                glVertex2f(explosionX + halfWidth, explosionY - halfHeight);
            glEnd();
            
            glDisable(GL_BLEND);
            
            explosionCounter += 1.0f;
            if (explosionCounter >= explosionDelay) {
                explosionFrame++;
                explosionCounter = 0.0f;
            }
            
            if (explosionFrame >= explosionFrames) {
                showExplosion = false;
            }
        }
    }
    
    if (hasCollided) {
        if (!showExplosion) {
            hasCollided = false;
            isRPSMoving = false;
            RPSdone = false;
            
            player.pos_x = player.base_x;
            enemy.pos_x = enemy.base_x;
            player.pos_y = base_y;
            enemy.pos_y = base_y;
            
            animationDone = true;
            
            return true;
        }
    }
    
    return animationDone;
}

void renderStart(int startMenuSelection)
{
    static bool titleInitialized = false;
    static GLuint titleTexture;
    static int titleWidth, titleHeight;
    static Image *titleImage = NULL;
    
    if (!titleInitialized) {
        titleImage = new Image("./assets/title.png");
        titleWidth = titleImage->width;
        titleHeight = titleImage->height;
        
        glGenTextures(1, &titleTexture);
        glBindTexture(GL_TEXTURE_2D, titleTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, titleWidth, titleHeight, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, buildAlphaData(titleImage));
        
        titleInitialized = true;
    }
    
    const int numOptions = 3; 
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    float titleScale = 4.0f;
    float scaledTitleWidth = titleWidth * titleScale;
    float scaledTitleHeight = titleHeight * titleScale;
    float titleX = (576 - scaledTitleWidth) / 2;
    float titleY = 160;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, titleTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(titleX, titleY);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(titleX, titleY + 
            scaledTitleHeight);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(titleX + scaledTitleWidth, titleY 
            + scaledTitleHeight);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(titleX + scaledTitleWidth, titleY);
    glEnd();
    
    Rect subtitleText;
    subtitleText.center = 1;
    subtitleText.left = 576 / 2;
    subtitleText.bot = titleY - 20;
    ggprint16(&subtitleText, 32, 0xffffff, "A One-Handed Journey");
    
    float boxWidth = 200.0f;
    float boxHeight = 100.0f;
    float x = (576 - boxWidth) / 2;
    float y = 20;

    glColor4f(1.0, 1.0, 1.0, 0.15);
    for (int i = 0; i < 6; i++) {
        float offset = i * 2.0f;
        glBegin(GL_QUADS);
            glVertex2f(x - offset, y - offset);
            glVertex2f(x + boxWidth + offset, y - offset);
            glVertex2f(x + boxWidth + offset, y + boxHeight + offset);
            glVertex2f(x - offset, y + boxHeight + offset);
        glEnd();
    }
    
    glColor4f(0.0, 0.0, 0.0, 0.85);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + boxWidth, y);
        glVertex2f(x + boxWidth, y + boxHeight);
        glVertex2f(x, y + boxHeight);
    glEnd();
    glDisable(GL_BLEND);
    
    const unsigned int COLOR_MENU_SELECTED = 0xffffff;
    const unsigned int COLOR_MENU_DEFAULT = 0x888888;
    
    Rect rText;
    rText.center = 1;
    rText.left = 576 / 2;
    
    const char* options[] = {"Play Game", "Controls", "Exit Game"};
    for (int i = 0; i < numOptions; ++i) {
        rText.bot = y + boxHeight - 35 - (i * 25);
        int color = (i == startMenuSelection) ? COLOR_MENU_SELECTED : 
            COLOR_MENU_DEFAULT;
        ggprint16(&rText, 32, color, options[i]);
    }
}