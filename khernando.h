#pragma once
#include "fonts.h"
#include "image.h"
#include "textures.h"
#include <GL/glx.h>
#include <vector>
#include <algorithm>
#include <string>
#include "ssantos.h"

class Global;

// Declare the external global variable
extern Global g;

class Player {
    public:
        Player();
        void init(const char* imagePath);
        void init_hp(int health);
        void render_player();
        void render_hp();
        void update(float amplitude);
        void changeImage(const char* imagePath);
        void changeHealthBar(int health);

        // Add inventory functions
        void addItem(Item* item);
        void removeItem(const std::string& itemName);
        void clearInventory();
        Item* getItem(const std::string& itemName);
        void applyItemEffects(); // Apply all item effects to player stats
        
        Texture tex;
        Texture hp;
        int xres, yres;
        int hp_xres, hp_yres;
        float pos_x, pos_y;
        float base_x, base_y;
        float hp_x, hp_y;

        //stats
        int baseDamage;
        int baseDefense;
        int currentDamage;
        int currentDefense;
        int critChance;
        int thornsAmount;

    private:
        std::vector<Item*> inventory;

};

class Enemy {
    public:
        Enemy();
        void init(const char* imagePath);
        void init_hp();
        void render_enemy();
        void render_hp();
        void update(float amplitude);
        void changeImage(const char* imagePath);
        void changeHealthBar(int health);

        // Add inventory functions
        void addItem(Item* item);
        void clearEnemyInventory();
        void removeItem(const std::string& itemName);
        Item* getItem(const std::string& itemName);
        void applyItemEffects(); // Apply all item effects to enemy stats

        Texture tex;
        Texture hp;
        int xres, yres;
        int hp_xres, hp_yres;
        float pos_x, pos_y;
        float base_x, base_y;
        float hp_x, hp_y;
        bool hitBarrier;
            
        //stats
        int baseDamage;
        int baseDefense;
        int currentDamage;
        int currentDefense;
        int critChance;
        int thornsAmount;
    
    private:
        std::vector<Item*> inventory;
};

// Originally from waterfall (lab 6)
class Box {
    public:
        Box(float w, float h);
        float pos[2];
        float width, height;
        unsigned char color[3];
        Image *boxImage;
        GLuint texture;
        float xc[2];
        float yc[2];
};

extern Box box;
extern Box topBox;
void render_box();
void render_top();

void kianText(Rect *rKian);
void controlText(Rect *rControl);

extern const char* intro[];
extern const char* enemyAppeared[];
void render_text(Rect *rec, const char* lines[], const int num_lines);

void initSelections();
void renderBox(Box& sel, const char* imagePath = nullptr, bool isSelected = false);

bool renderAnimation(Player &player, Enemy &enemy);
void renderStart(int startMenuSelection);

extern bool animationDone;