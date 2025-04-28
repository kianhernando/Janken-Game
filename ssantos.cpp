// Original author: Simon Santos
// Date: March 08, 2025
#include <cstdio>
#include <unistd.h>
#include <GL/gl.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include "ssantos.h"
#include "khernando.h"

extern Global g;
extern Player player;
extern Enemy enemy;
extern void init_hp(int health);
extern void changeHealthBar(int health);
std::random_device rd;
std::mt19937 gen(rd());
static int roundCounter = 0;

struct battleState{
    int playerLoseInteraction;
    int enemyLoseInteraction;
    int playerDeath;
    int enemyDeath;
} bState;
int initScore = 100;
int totalScore = 0;

Item::Item(const std::string& itemName) : 
    boostedDamage(0), 
    protectedDamage(0), 
    crit(0), 
    thorns(0),
    name(itemName) 
{
}

void Item::swordItem()
{
    boostedDamage = 1;
}

void Item::bowItem()
{
    boostedDamage = 1;
}

void Item::spearItem()
{
    boostedDamage = 1;
}

void Item::axeItem()
{
    boostedDamage = 1;
}

void Item::hammerItem()
{
    boostedDamage = 1;
}

void Item::shieldItem()
{
    protectedDamage = 1;
}

void Item::armorItem()
{
    protectedDamage = 1;
}

void Item::helmItem()
{
    protectedDamage = 1;
}

void Item::serratedSword()
{
    crit = 2;
}

void Item::thornedArmor()
{
    thorns = 2;
}

// Player inventory management functions
void Player::addItem(Item* item) {
    if (item) {
        if (item) {
            // optional: verify that the item's important stats are reasonable
            inventory.push_back(item);
            applyItemEffects();
        }
    }
}

void Player::clearInventory() {
    for (Item* item : inventory) {
        delete item;  // Clean up dynamically allocated memory
    }
    inventory.clear();  // Clear the vector
}

void Player::removeItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(), 
        [&itemName](Item* item) { return item->getName() == itemName; });
    
    if (it != inventory.end()) {
        inventory.erase(it);
        // Recalculate effects when an item is removed
        applyItemEffects();
    }
}

Item* Player::getItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(), 
        [&itemName](Item* item) { return item->getName() == itemName; });
    
    if (it != inventory.end()) {
        return *it;
    }
    return nullptr;
}

void Player::applyItemEffects() {
    // Reset stats to base values
    currentDamage = baseDamage;
    currentDefense = baseDefense;
    critChance = 0;
    thornsAmount = 0;
    
    // Apply all item effects
    for (Item* item : inventory) {
        if (item) { // <- SAFETY CHECK
            currentDamage += item->boostedDamage;
            currentDefense += item->protectedDamage;
            critChance += item->crit;
            thornsAmount += item->thorns;

             // Debug prints
            std::cout << "Item: " << item->getName() << std::endl;
            std::cout << "Boosted Damage: " << item->boostedDamage << std::endl;
            std::cout << "Current Damage: " << currentDamage << std::endl;
        }
    }
}

// Enemy inventory management functions
void Enemy::addItem(Item* item) {
    if (item) {
        if (item) {
            // optional: verify that the item's important stats are reasonable
            inventory.push_back(item);
            applyItemEffects();
        }
    }
}

void Enemy::clearEnemyInventory() {
    for (Item* item : inventory) {
        delete item;  // Clean up dynamically allocated memory
    }
    inventory.clear();  // Clear the vector
}

void Enemy::removeItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(), 
        [&itemName](Item* item) { return item->getName() == itemName; });
    
    if (it != inventory.end()) {
        inventory.erase(it);
        // Recalculate effects when an item is removed
        applyItemEffects();
    }
}

Item* Enemy::getItem(const std::string& itemName) {
    auto it = std::find_if(inventory.begin(), inventory.end(), 
        [&itemName](Item* item) { return item->getName() == itemName; });
    
    if (it != inventory.end()) {
        return *it;
    }
    return nullptr;
}

void Enemy::applyItemEffects() {
    // Reset stats to base values
    currentDamage = baseDamage;
    currentDefense = baseDefense;
    critChance = 0;
    thornsAmount = 0;
    
    // Apply all item effects
    for (Item* item : inventory) {
        if (item) { // <- SAFETY CHECK
            currentDamage += item->boostedDamage;
            currentDefense += item->protectedDamage;
            critChance += item->crit;
            thornsAmount += item->thorns;

             // Debug prints
            std::cout << "Item: " << item->getName() << std::endl;
            std::cout << "Boosted Damage: " << item->boostedDamage << std::endl;
            std::cout << "Current Damage: " << currentDamage << std::endl;
        }
    }
}

void simonText(Rect *rSimon)
{
    ggprint8b(rSimon, 16, 0xffffff, "Simon");
}

const char* battleChoice[] = {
    "You won the round! Choose an Action!",
    "'A' to attack enemy",
    "'B' to block next hit"
};

const char* simplifyControls[] = {
    "PRESS 'R', 'P', OR 'S' FOR ROCK-PAPER-SCISSORS"
};

int battleChoiceFunc(int &pHealth, int &eHealth)
{
    // Calculate player's damage based on their base damage and item bonuses
    int damage = player.currentDamage;
    
    // Apply critical hit chance
    if (player.critChance > 0) {
        std::uniform_int_distribution<> critRoll(1, 10);
        if (critRoll(gen) <= player.critChance) {
            damage *= 2;
            printf("Critical hit!\n");
        }
    }
    
    // Apply enemy defense from items
    int finalDamage = std::max(1, damage - enemy.currentDefense);
    eHealth -= finalDamage;
    printf("Enemy takes %d damage!\n", finalDamage);
    printf("enemy health %i\n", eHealth);
    
    // Apply thorns damage if enemy has thorned armor
    if (enemy.thornsAmount > 0) {
        int thornsDamage = enemy.thornsAmount;
        pHealth -= thornsDamage;
        printf("You took %d thorns damage from enemy's thorns!\n", 
            thornsDamage);
        player.changeHealthBar(pHealth);
    }
    
    printf("Amount of times enemy has lost: %i\n", bState.enemyLoseInteraction);
    enemy.changeHealthBar(eHealth);
    if (eHealth == 0) {
        printf("Enemy has died!\n");
        scoreCalculator(bState.playerLoseInteraction, 
            bState.enemyLoseInteraction);
        ++bState.enemyDeath;
        genNewEnemy(bState.enemyDeath);
        eHealth = 100;
        pHealth = 100;
        enemy.changeHealthBar(eHealth);
        player.changeHealthBar(pHealth);
        player.init("assets/player/normal_x.png");
        roundCounter = 0;
    }
    grabEnemyHealth(pHealth);
    return eHealth;
}

int grabEnemyHealth(int &eHealth)
{
    return eHealth;
}

int grabPlayerHealth(int &pHealth)
{
    return pHealth;
}

int compareHealth(int &pHealth, int &eHealth)
{
    int player = pHealth;
    int enemy = eHealth;

    if (player > enemy) {
        printf("player won!\n");
    }
    if (player < enemy) {
        printf("enemy won!\n");
    }
    if (player == enemy) {
        printf("tie!\n");
    }
    return 0;
}

bool blockDamage(bool block)
{
    printf("is blocked %i\n", block);
    return block;
}

int logicSimon(int choice, int enChoice, int &pHealth, bool &blocked) 
{
    int playerWonInteraction = 1;
    if (roundCounter == 40) {
        grabPlayerHealth(pHealth);
        printf("battle over!\n");
        return 0;
    }
    if (choice == ROCK) {
        roundCounter++;
        printf("rounds: %i\n", roundCounter);
        player.changeImage("assets/player/rock_x.png");
        if (enChoice == 2) {
            enemy.changeImage("assets/enemy/scissors.png");
            ++bState.enemyLoseInteraction;
            printf("Player Wins! Enemy loses 10 HP!\n");
            fflush(stdout);
            return playerWonInteraction;
        } else if (enChoice == 1) {
            enemy.changeImage("assets/enemy/paper.png");
            ++bState.playerLoseInteraction;
            printf("Player Loses!\n");
        
            int baseDamage = enemy.currentDamage; // <- Enemy's current attack power
        
            if (blocked == 1) {
                baseDamage -= 3; // If blocking, reduce incoming damage by 3
                printf("Blocking reduced damage by 3!\n");
            }
            blocked = 0;
        
            // Apply player's defense
            int finalDamage = baseDamage - player.currentDefense;
            if (finalDamage < 1) {
                finalDamage = 1;
            }
        
            // Deal damage
            pHealth -= finalDamage;
        
            printf("Enemy deals %d damage to player (after defense).\n", finalDamage);
            printf("Player's current health: %d\n", pHealth);
            printf("Amount of times player has lost: %d\n", bState.playerLoseInteraction);
        
            player.changeHealthBar(pHealth);
            fflush(stdout);
        } else if (enChoice == choice) {
            enemy.changeImage("assets/enemy/rock.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    else if (choice == PAPER) {
        roundCounter++;
        printf("rounds: %i\n", roundCounter);
        player.changeImage("assets/player/paper_x.png");
        if (enChoice == 0) {
            enemy.changeImage("assets/enemy/rock.png");
            ++bState.enemyLoseInteraction;
            printf("Player Wins! Enemy loses 10 HP!\n");
            fflush(stdout);
            return playerWonInteraction;
        } else if (enChoice == 2) {
            enemy.changeImage("assets/enemy/scissors.png");
            ++bState.playerLoseInteraction;
            printf("Player Loses!\n");
        
            int baseDamage = enemy.currentDamage; // <- Enemy's current attack power
        
            if (blocked == 1) {
                baseDamage -= 3; // If blocking, reduce incoming damage by 3
                printf("Blocking reduced damage by 3!\n");
            }
            blocked = 0;
        
            // Apply player's defense
            int finalDamage = baseDamage - player.currentDefense;
            if (finalDamage < 1) {
                finalDamage = 1;
            }
        
            // Deal damage
            pHealth -= finalDamage;
        
            printf("Enemy deals %d damage to player (after defense).\n", finalDamage);
            printf("Player's current health: %d\n", pHealth);
            printf("Amount of times player has lost: %d\n", bState.playerLoseInteraction);
        
            player.changeHealthBar(pHealth);
            fflush(stdout);
        } else if (enChoice == choice) {
            enemy.changeImage("assets/enemy/paper.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    } else if (choice == SCISSORS) {
        roundCounter++;
        printf("rounds: %i\n", roundCounter);
        player.changeImage("assets/player/scissors_x.png");
        if (enChoice == 1) {
            enemy.changeImage("assets/enemy/paper.png");
            ++bState.enemyLoseInteraction;
            printf("Player Wins! Enemy loses 10 HP!\n");
            fflush(stdout);
            return playerWonInteraction;
        } else if (enChoice == 0) {
            enemy.changeImage("assets/enemy/rock.png");
            ++bState.playerLoseInteraction;
            printf("Player Loses!\n");
        
            int baseDamage = enemy.currentDamage; // <- Enemy's current attack power
        
            if (blocked == 1) {
                baseDamage -= 3; // If blocking, reduce incoming damage by 3
                printf("Blocking reduced damage by 3!\n");
            }
            blocked = 0;
        
            // Apply player's defense
            int finalDamage = baseDamage - player.currentDefense;
            if (finalDamage < 1) {
                finalDamage = 1;
            }
        
            // Deal damage
            pHealth -= finalDamage;
        
            printf("Enemy deals %d damage to player (after defense).\n", finalDamage);
            printf("Player's current health: %d\n", pHealth);
            printf("Amount of times player has lost: %d\n", bState.playerLoseInteraction);
        
            player.changeHealthBar(pHealth);
            fflush(stdout);
        } else if (enChoice == choice) {
            enemy.changeImage("assets/enemy/scissors.png");
            printf("Tie! Nothing happens!\n");
            fflush(stdout);
        }
    }
    if (pHealth == 0) {
        printf("Player has died!\n");
        scoreCalculator(bState.playerLoseInteraction, 
            bState.enemyLoseInteraction);
        ++bState.playerDeath;
        roundCounter = 0;
    }
    printf("\n");
    usleep(700000);
    return 0;
}

void genNewEnemy(int eDeath)
{
    // Clear previous enemy's inventory
    enemy.clearEnemyInventory();
    if (eDeath == 1) {
        enemy.init("assets/enemy/grey-yaroi.png");
        // Basic enemy with simple equipment
        Item* sword = new Item("Enemy Sword");
        sword->swordItem();
        enemy.addItem(sword);
    } else if (eDeath == 2) {
        enemy.init("assets/enemy/yellow-yaroi.png");
        // Stronger enemy with better equipment
        Item* axe = new Item("Enemy Axe");
        axe->axeItem();
        Item* shield = new Item("Enemy Shield");
        shield->shieldItem();
        enemy.addItem(axe);
        enemy.addItem(shield);
    } else if (eDeath == 3) {
        enemy.init("assets/enemy/blue-yaroi.png");
        // Advanced enemy with rare items
        Item* serratedSword = new Item("Enemy Serrated Sword");
        serratedSword->serratedSword();
        enemy.addItem(serratedSword);
    } else if (eDeath == 4) {
        enemy.init("assets/enemy/red-yaroi.png");
        // Boss enemy with the best equipment
        Item* serratedSword = new Item("Enemy Serrated Sword");
        serratedSword->serratedSword();
        Item* thornedArmor = new Item("Enemy Thorned Armor");
        thornedArmor->thornedArmor();
        enemy.addItem(serratedSword);
        enemy.addItem(thornedArmor);
    }
}

//Random Gen for enemy choice
int randGen()
{
    //======================================================================
    //Optimization Change
    //previous code:
    //  srand(time(0));
    //  int randomNum  = rand() % 3;
    //to code below
    //change was made to make randGen more random
    //======================================================================
    //set a range for the num to generate between 0 and 2
    std::uniform_int_distribution<> dis(0, 2);
    int randNum = dis(gen);

    return randNum;
}

int randItemGen()
{
    std::uniform_int_distribution<> dis(1, 10);
    int randItem = dis(gen);
    Item* newItem = nullptr;
    
    if (randItem <= 10 && randItem > 9) {
        printf("Ultra Rare Item: %i\n", randItem);
        newItem = new Item("Serrated Sword");
        newItem->serratedSword();
        printf("Serrated sword acquired\n");
    } else if (randItem <= 9 && randItem > 8) {
        printf("Rare Item: %i\n", randItem);
        newItem = new Item("Thorned Armor");
        newItem->thornedArmor();
        printf("Thorned armor acquired\n");
    } else if (randItem <= 8 && randItem > 5) {
        printf("Uncommon Item: %i\n", randItem);
        int uncommonType = randUncommonItem();
        if (uncommonType == 0) {
            newItem = new Item("Shield");
            newItem->shieldItem();
        } else if (uncommonType == 1) {
            newItem = new Item("Helm");
            newItem->helmItem();
        } else if (uncommonType == 2) {
            newItem = new Item("Armor");
            newItem->armorItem();
        }
    } else if (randItem <= 5 && randItem > 0) {
        printf("Common Item: %i\n", randItem);
        int commonType = randCommonItem();
        if (commonType == 0) {
            newItem = new Item("Sword");
            newItem->swordItem();
        } else if (commonType == 1) {
            newItem = new Item("Bow");
            newItem->bowItem();
        } else if (commonType == 2) {
            newItem = new Item("Spear");
            newItem->spearItem();
        } else if (commonType == 3) {
            newItem = new Item("Axe");
            newItem->axeItem();
        } else if (commonType == 4) {
            newItem = new Item("Hammer");
            newItem->hammerItem();
        }
    }
    
    if (newItem) {
        player.addItem(newItem);
        std::cout << newItem->getName() << " added to inventory!" << std::endl;
    }
    
    printf("Item: %i\n", randItem);
    fflush(stdout);

    return randItem;
}
 
int randCommonItem()
{
    std::uniform_int_distribution<> dis(0, 4);
    int randCommon = dis(gen);
    if (randCommon == 0) {
        printf("sword acquired\n");
    } else if (randCommon == 1) {
        printf("bow acquired\n");
    } else if (randCommon == 2) {
        printf("spear acquired\n");
    } else if (randCommon == 3) {
        printf("axe acquired\n");
    } else if (randCommon == 4) {
        printf("hammer acquired\n");
    }

    return randCommon;
}
 
int randUncommonItem()
{
    std::uniform_int_distribution<> dis(0, 2);
    int randUncommon = dis(gen);
    if (randUncommon == 0) {
        printf("shield acquired\n");
    } else if (randUncommon == 1) {
        printf("helm acquired\n");
    } else if (randUncommon == 2) {
        printf("armor acquired\n");
    }
    
    return randUncommon;
}

int scoreCalculator(int &pLoseCondition, int &eLoseCondition)
{
    printf("initial score is: %i\n", initScore);
    int scoreMultCondition = eLoseCondition - pLoseCondition;
    if (scoreMultCondition <= 10 && scoreMultCondition > 8) {
        totalScore = initScore * 5;
    } else if (scoreMultCondition <= 8 && scoreMultCondition > 6) {
        totalScore = initScore * 4;
    } else if (scoreMultCondition <= 6 && scoreMultCondition > 4) {
        totalScore = initScore * 3;
    } else if (scoreMultCondition <= 4 && scoreMultCondition > 2) {
        totalScore = initScore * 2;
    } else if (scoreMultCondition <= 2 && scoreMultCondition > 0) {
        totalScore = initScore * 1;
    }
    printf("the total score is: %i\n", totalScore);
    return totalScore;
}
