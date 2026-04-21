/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <vector>

#define G 2000
#define PLAYER_JUMP_SPD 1000.0f
#define PLAYER_ACC 1000.0f
#define PLAYER_FRICTION 600.0f
#define PLAYER_MAX_SPEED 400.0f
#define JUMP_HOLD_FORCE 1200.0f
#define MAX_JUMP_TIME 0.2f

#define TILE_EMPTY 0
#define TILE_SOLID 1
#define TILE_BREAK 2
#define TILE_NEGRO 3
#define TILE_SOLID_HERBA 4
#define TILE_HERBA_R     5
#define TILE_HERBA_L     6
#define TILE_TERRA_R     7
#define TILE_TERRA_L     8
#define TILE_INTERROGANT 9
#define TILE_ESTRELLA    10
#define TILE_EMOTICONOCALAVERAGROC    11
#define TILE_EMOTICONOCALAVERAROSA    12
#define TILE_BOSSACOLLONS    13
#define TILE_BOSSACOLLONSPETIT 14
#define TILE_PORTA 15

#define BLAU  CLITERAL(Color){8, 9, 250}

//----------------------------------------------------------------------------------
// TEXTURES
//----------------------------------------------------------------------------------
Texture2D background;
Texture2D nuvol;
Texture2D AlexKiddIdleR;
Texture2D AlexKiddIdleL;
Texture2D AlexKiddWalkR;
Texture2D AlexKiddWalkL;
Texture2D AlexKiddJumpR;
Texture2D AlexKiddJumpL;
Texture2D AlexKiddPunyR;
Texture2D AlexKiddPunyL;
Texture2D AlexKiddCrouchR;
Texture2D AlexKiddCrouchL;
Texture2D AlexKiddDeath;

Texture2D MonsterBirdR;
Texture2D MonsterBirdL;

Texture2D blockSolidTerra;
Texture2D blockBreak;
Texture2D blockTerraR;
Texture2D blockTerraL;
Texture2D blockHerbaR;
Texture2D blockHerbaL;
Texture2D blockSolidHerba;
Texture2D blockInterrogant;
Texture2D blockEstrella;
Texture2D blockPorta;

Texture2D blockCalaveraGroc;
Texture2D blockCalaveraRosa;
Texture2D blockBossaCollons;
Texture2D blockBossaCollonsPetit;

Texture2D MIAU;
Texture2D negro;

Texture2D mapImage;

// AUDIO
Music titleMusic;
Music gameMusic;
Sound jumpSound;
Sound levelStartSound;
Sound coinSound;
Sound punchSound;
Sound coinBlockSound;
Music gameOverMusic;

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    float speedY;
    float velX;
    bool canJump;

    bool alive;
    float respawnTimer;
    Vector2 spawn;
    bool isJumping;
    float jumpTime;
    int coins;

    int lives;
    bool deathAnim;
    float deathY;
    float deathX;
    int deathFrame;
    int deathFrameCounter;
} Player;

typedef enum {
    BLOCK_SOLID,
    BLOCK_BREAKABLE
} BlockType;

typedef enum {
    DROP_NONE,
    DROP_COIN,
    DROP_STAR
} DropType;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Texture2D texture;
    BlockType type;
    bool active;
    DropType drop;
    bool collectible;
    float lifetime;
    int tileID;
} EnvItem;

typedef struct enemic {
    bool vida;
    int velocitat;
    bool siToca;
    Vector3 posicio;
} pterodactil, escorpi, mono;

typedef enum {
    STATE_MENU,
    STATE_MAP,
    STATE_PLAYING
} GameState;

//----------------------------------------------------------------------------------
// FUNCIONS
//----------------------------------------------------------------------------------
void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraDownOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void PterodactilMoviment(enemic* pterodactil, EnvItem* envItems, int envItemsLength, float delta);
void PlayerBreakBlock(Player* player, EnvItem* envItems, int envItemsLength, int LeftOrRight);
void PlayerHitEnemy(Player* player, enemic* pterodactil, int LeftOrRight);
void EnemyHitPlayer(Player* player, enemic* pterodactil);
void PlayerAttackEnemy(Player* player, enemic* ptero, int LeftOrRight);

//------------------------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------------------------
int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 900;

    GameState gameState = STATE_MENU;

    InitWindow(screenWidth, screenHeight, "Alex Kidd");
    InitAudioDevice();

    background = LoadTexture("resources/Alex-Kidd-assets.png");
    nuvol = LoadTexture("resources/nuvol.png");

    AlexKiddIdleR = LoadTexture("resources/AlexKiddIdleR.png");
    AlexKiddIdleL = LoadTexture("resources/AlexKiddIdleL.png");
    AlexKiddWalkR = LoadTexture("resources/AlexKiddWalkR.png");
    AlexKiddWalkL = LoadTexture("resources/AlexKiddWalkL.png");
    AlexKiddJumpR = LoadTexture("resources/AlexKiddJumpR.png");
    AlexKiddJumpL = LoadTexture("resources/AlexKiddJumpL.png");
    AlexKiddPunyR = LoadTexture("resources/AlexKiddPunyR.png");
    AlexKiddPunyL = LoadTexture("resources/AlexKiddPunyL.png");
    AlexKiddCrouchR = LoadTexture("resources/AlexKiddCrouchR.png");
    AlexKiddCrouchL = LoadTexture("resources/AlexKiddCrouchL.png");
    AlexKiddDeath = LoadTexture("resources/AlexKiddDeath.png");

    MonsterBirdR = LoadTexture("resources/MonsterBirdR.png");
    MonsterBirdL = LoadTexture("resources/MonsterBirdL.png");

    blockSolidTerra = LoadTexture("resources/BlocSolidTerra.png");
    blockBreak = LoadTexture("resources/BlocBreakable.png");
    blockTerraR = LoadTexture("resources/triangledretaterra.png");
    blockTerraL = LoadTexture("resources/triangleesquerraterra.png");
    blockHerbaR = LoadTexture("resources/triangledretaherba.png");
    blockHerbaL = LoadTexture("resources/triangleesquerraherba.png");
    blockSolidHerba = LoadTexture("resources/blocherba.png");
    blockInterrogant = LoadTexture("resources/interrogant.png");
    blockEstrella = LoadTexture("resources/estrella.png");
    blockCalaveraGroc = LoadTexture("resources/emoticonocalaveragroga.png");
    blockCalaveraRosa = LoadTexture("resources/emoticonocalaverarosa.png");
    blockBossaCollons = LoadTexture("resources/bossadecollons.png");
    blockBossaCollonsPetit = LoadTexture("resources/bossadecollonspetit.png");
    blockPorta = LoadTexture("resources/Puerta.png");

    MIAU = LoadTexture("resources/Patricio.png");
    negro = LoadTexture("resources/Negro.png");

    mapImage = LoadTexture("resources/mapa.png");

    // AUDIO - Cargar
    titleMusic = LoadMusicStream("resources/TitleScreen.wav");
    titleMusic.looping = false;
    SetMusicVolume(titleMusic, 0.4f);
    PlayMusicStream(titleMusic);

    gameMusic = LoadMusicStream("resources/MainTheme.wav");
    gameMusic.looping = true;
    SetMusicVolume(gameMusic, 0.3f);

    jumpSound = LoadSound("resources/Jump.wav");
    SetSoundVolume(jumpSound, 0.5f);

    levelStartSound = LoadSound("resources/LevelStart.wav");
    SetSoundVolume(levelStartSound, 0.6f);

    coinSound = LoadSound("resources/CoinCollection.wav");
    SetSoundVolume(coinSound, 0.7f);

    punchSound = LoadSound("resources/Punch.wav");
    SetSoundVolume(punchSound, 0.6f);

    coinBlockSound = LoadSound("resources/CoinBlock.wav");
    SetSoundVolume(coinBlockSound, 0.6f);

    gameOverMusic = LoadMusicStream("resources/GameOver.wav");
    gameOverMusic.looping = false;
    SetMusicVolume(gameOverMusic, 0.6f);


   

    Rectangle frameRecR = { 0.0f, 0.0f, ((float)AlexKiddWalkR.width / 4), ((float)AlexKiddWalkR.height) };
    Rectangle frameRecL = { 0.0f, 0.0f, ((float)AlexKiddWalkL.width / 4), ((float)AlexKiddWalkL.height) };
    Rectangle frameRecJump = { 0.0f, 0.0f, ((float)AlexKiddJumpR.width), ((float)AlexKiddJumpR.height) };
    Rectangle frameRecPuny = { 0.0f, 0.0f, ((float)AlexKiddPunyR.width), ((float)AlexKiddPunyR.height) };
    Rectangle frameRecCrouch = { 0.0f, 0.0f, ((float)AlexKiddPunyR.width), ((float)AlexKiddPunyR.height) };
    Rectangle framePterodactil = { 0.0f, 0.0f, ((float)MonsterBirdR.width / 2), ((float)MonsterBirdR.height) };
    Rectangle frameRecDeath = { 0.0f, 0.0f, ((float)AlexKiddDeath.width / 3), ((float)AlexKiddDeath.height) };

    int playerFrame = 0;
    int playerCounter = 0;
    int pteroFrame = 0;
    int pteroCounter = 0;
    int miauCounter = 0;
    int miauFrame = 0;
    int framesSpeed = 8;

    int LeftOrRight = NULL;
    int var = 0;

    bool attacking = false;
    int attackTimer = 0;
    int FramesPuny = 0;

    // AUDIO - Control de transición
    bool levelStarting = false;
    float levelStartTimer = 0.0f;

    Player player = { 0 };
    player.position = Vector2{ 600, 200 };
    player.speedY = 0;
    player.velX = 0;
    player.canJump = false;
    player.isJumping = false;
    player.jumpTime = 0;
    player.coins = 0;
    player.lives = 3;
    player.deathAnim = false;
    player.deathY = 0;
    player.deathX = 0;
    player.deathFrame = 0;
    player.deathFrameCounter = 0;
    player.spawn = player.position;
    player.alive = true;
    player.respawnTimer = 2.0f;

    

    Vector2 petPosition = player.position;

    std::vector<enemic> pterodactilos;

    enemic p1 = { 0 }; p1.posicio = Vector3{ 600, 1600 }; p1.velocitat = 2; p1.vida = true; pterodactilos.push_back(p1);
    enemic p2 = { 0 }; p2.posicio = Vector3{ 900, 2160 }; p2.velocitat = 2; p2.vida = true; pterodactilos.push_back(p2);
    enemic p3 = { 0 }; p3.posicio = Vector3{ 600, 2800 }; p3.velocitat = 2; p3.vida = true; pterodactilos.push_back(p3);
    enemic p4 = { 0 }; p4.posicio = Vector3{ 600, 3040 }; p4.velocitat = 2; p4.vida = true; pterodactilos.push_back(p4);
    enemic p5 = { 0 }; p5.posicio = Vector3{ 800, 3780 }; p5.velocitat = 2; p5.vida = true; pterodactilos.push_back(p5);
    enemic p6 = { 0 }; p6.posicio = Vector3{ 600, 4100 }; p6.velocitat = 2; p6.vida = true; pterodactilos.push_back(p6);
    enemic p7 = { 0 }; p7.posicio = Vector3{ 800, 4480 }; p7.velocitat = 2; p7.vida = true; pterodactilos.push_back(p7);
    enemic p8 = { 0 }; p8.posicio = Vector3{ 600, 5210 }; p8.velocitat = 2; p8.vida = true; pterodactilos.push_back(p8);
    enemic p9 = { 0 }; p9.posicio = Vector3{ 600, 6000 }; p9.velocitat = 2; p9.vida = true; pterodactilos.push_back(p9);
    enemic p10 = { 0 }; p10.posicio = Vector3{ 500, 6580 }; p10.velocitat = 2; p10.vida = true; pterodactilos.push_back(p10);

    player.position = Vector2{ 550, 200 };
    player.spawn = player.position;
    player.alive = true;
    player.respawnTimer = 2.0f;

#define TILE_SIZE 80

    int map[105][24] = {
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,3,3,3,3},
{3,3,3,3,4,4,4,4,4,6,0,0,0,0,0,5,4,4,4,4,3,3,3,3},
{3,3,3,3,1,1,1,1,8,0,0,0,0,0,0,0,7,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,0,0,0,0,0,0,0,0,0,7,1,1,3,3,3,3},
{3,3,3,3,1,1,1,10,0,0,5,4,4,4,6,0,0,0,7,1,3,3,3,3},
{3,3,3,3,1,1,1,10,0,0,0,7,1,8,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,1,1,4,6,0,0,0,0,0,0,0,0,2,9,1,3,3,3,3},
{3,3,3,3,1,1,1,1,0,0,0,0,0,5,4,4,4,4,4,1,3,3,3,3},
{3,3,3,3,1,1,1,1,0,0,0,0,2,0,7,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,0,0,0,0,2,0,0,7,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,0,5,4,4,6,0,0,0,7,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,8,0,0,7,8,0,0,0,0,0,7,1,1,3,3,3,3},
{3,3,3,3,1,1,8,0,0,0,0,0,0,0,0,0,0,0,7,1,3,3,3,3},
{3,3,3,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0,10,1,3,3,3,3},
{3,3,3,3,1,1,0,0,0,0,0,2,5,4,4,4,6,0,10,1,3,3,3,3},
{3,3,3,3,1,10,0,0,0,0,0,2,0,7,1,8,2,0,0,1,3,3,3,3},
{3,3,3,3,1,2,2,0,0,0,0,0,0,0,0,0,0,2,0,1,3,3,3,3},
{3,3,3,3,1,4,6,0,0,0,0,0,0,0,0,0,0,0,2,1,3,3,3,3},
{3,3,3,3,1,1,4,4,4,4,6,0,0,0,0,10,0,0,0,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,8,0,0,0,5,4,4,4,4,4,1,3,3,3,3},
{3,3,3,3,1,1,1,10,2,0,0,0,0,0,7,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,2,0,0,0,0,0,0,0,7,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,8,0,2,0,0,0,0,0,0,0,7,1,1,1,3,3,3,3},
{3,3,3,3,1,8,2,0,2,0,0,0,0,0,0,0,0,7,1,1,3,3,3,3},
{3,3,3,3,1,0,2,10,0,2,2,0,0,0,0,0,0,0,1,1,3,3,3,3},
{3,3,3,3,1,0,2,0,0,0,2,0,0,0,0,9,0,0,1,1,3,3,3,3},
{3,3,3,3,1,0,10,0,0,0,10,10,5,4,4,6,0,0,1,1,3,3,3,3},
{3,3,3,3,1,0,2,0,0,0,10,0,0,7,8,0,0,0,1,1,3,3,3,3},
{3,3,3,3,1,0,5,4,4,4,6,0,0,0,0,0,0,0,10,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,2,2,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,5,4,4,4,4,4,4,1,3,3,3,3},
{3,3,3,3,1,2,2,2,2,0,0,0,0,7,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,10,0,0,2,0,0,0,0,0,7,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,11,0,0,2,0,0,0,0,0,0,7,1,1,1,1,3,3,3,3},
{3,3,3,3,1,10,0,0,2,0,0,0,0,0,0,0,0,0,7,1,3,3,3,3},
{3,3,3,3,1,4,4,4,4,4,4,6,0,0,0,0,0,0,2,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,8,0,0,0,0,0,0,0,2,0,1,3,3,3,3},
{3,3,3,3,1,1,1,1,8,0,0,0,0,0,0,0,2,0,0,1,3,3,3,3},
{3,3,3,3,1,1,1,1,0,0,0,0,5,4,4,4,6,0,0,1,3,3,3,3},
{3,3,3,3,1,1,1,8,0,0,0,0,0,7,1,8,0,0,0,1,3,3,3,3},
{3,3,3,3,1,8,13,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,5,4,4,4,12,4,4,4,4,4,6,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,7,1,1,1,1,1,1,1,8,0,10,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,7,1,1,1,1,1,8,0,0,10,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,2,2,10,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,2,0,0,0,0,0,5,4,4,4,4,1,3,3,3,3},
{3,3,3,3,1,0,0,0,2,0,0,0,0,0,0,7,1,1,1,1,3,3,3,3},
{3,3,3,3,1,4,4,4,4,4,6,0,0,0,0,0,0,7,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,8,0,0,0,0,0,0,0,0,7,1,3,3,3,3},
{3,3,3,3,1,1,1,1,8,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,1,1,8,0,0,0,0,0,0,0,0,0,0,2,1,3,3,3,3},
{3,3,3,3,1,10,2,0,0,0,0,0,13,0,0,0,0,2,0,1,3,3,3,3},
{3,3,3,3,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,4,6,0,0,0,0,0,0,0,0,0,10,10,10,1,3,3,3,3},
{3,3,3,3,1,8,0,0,0,0,0,0,0,0,0,0,2,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,2,0,2,1,3,3,3,3},
{3,3,3,3,1,0,0,5,4,4,4,4,4,4,4,4,4,4,4,1,3,3,3,3},
{3,3,3,3,1,0,0,0,7,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,7,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,4,6,0,0,0,0,0,0,10,7,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,8,0,0,0,0,0,0,0,0,0,0,0,7,1,1,3,3,3,3},
{3,3,3,3,1,10,0,0,0,0,0,0,0,0,0,0,0,0,7,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,5,4,4,6,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,5,4,4,6,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,10,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,5,6,0,0,1,3,3,3,3},
{3,3,3,3,1,0,13,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,13,0,0,0,0,0,0,0,0,0,0,0,10,1,3,3,3,3},
{3,3,3,3,1,6,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,5,4,6,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,5,6,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,5,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,5,4,6,0,5,4,6,0,5,4,6,0,5,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,2,13,0,5,6,0,10,0,2,13,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,2,13,0,0,0,0,2,0,2,13,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,2,0,5,4,6,0,10,0,2,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,2,0,0,0,0,0,2,0,2,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,3,3,3},
{3,3,3,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,15,1,3,3,3,3},
{3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3}
    };

    std::vector<EnvItem> envItems;

    for (int y = 0; y < 101; y++)
    {
        for (int x = 0; x < 24; x++)
        {
            int tile = map[y][x];
            if (tile == 0) continue;

            EnvItem item = { 0 };
            item.rect = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            item.blocking = 1;
            item.active = true;

            if (tile == TILE_SOLID) { item.texture = blockSolidTerra; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_BREAK) { item.texture = blockBreak; item.type = BLOCK_BREAKABLE; item.drop = DROP_COIN; }
            else if (tile == TILE_NEGRO) { item.texture = negro; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_PORTA) { item.texture = blockPorta; item.type = BLOCK_SOLID; item.blocking = 0; item.collectible = false; item.drop = DROP_NONE; }
            else if (tile == TILE_SOLID_HERBA) { item.texture = blockSolidHerba; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_HERBA_R) { item.texture = blockHerbaR; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_HERBA_L) { item.texture = blockHerbaL; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_TERRA_R) { item.texture = blockTerraR; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_TERRA_L) { item.texture = blockTerraL; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_INTERROGANT) { item.texture = blockInterrogant; item.type = BLOCK_BREAKABLE; item.drop = DROP_STAR; }
            else if (tile == TILE_ESTRELLA) { item.texture = blockEstrella; item.type = BLOCK_BREAKABLE; item.drop = DROP_STAR; }
            else if (tile == TILE_EMOTICONOCALAVERAGROC) { item.texture = blockCalaveraGroc; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_EMOTICONOCALAVERAROSA) { item.texture = blockCalaveraRosa; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
            else if (tile == TILE_BOSSACOLLONS) { item.texture = blockBossaCollons; item.type = BLOCK_SOLID; item.blocking = 0; item.collectible = true; item.drop = DROP_NONE; }
            else { item.texture = blockSolidTerra; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }

            item.tileID = tile;
            envItems.push_back(item);
            envItems.back().lifetime = 0.0f;
            
        }
    }

    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);
    std::vector<EnvItem> originalEnvItems = envItems;

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
        UpdateCameraCenter,
        UpdateCameraCenterInsideMap,
        UpdateCameraCenterSmoothFollow,
        UpdateCameraEvenOutOnLanding,
        UpdateCameraPlayerBoundsPush,
        UpdateCameraDownOnly
    };

    int cameraOption = 4;

    SetTargetFPS(60);

    static float x = 400, y = 400;

    bool gameOver = false;
    float gameOverTimer = 0.0f;

    while (!WindowShouldClose())
    {
        // AUDIO - Actualizar stream según estado
        if (gameState == STATE_MENU) {
            UpdateMusicStream(titleMusic);
        }
        else if (gameState == STATE_PLAYING) {
            UpdateMusicStream(gameMusic);
        }

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // MENU: detectar ENTER y arrancar transición de audio
        if (gameState == STATE_MENU && IsKeyPressed(KEY_ENTER)) {
            StopMusicStream(titleMusic);
            PlaySound(levelStartSound);
            levelStarting = true;
            levelStartTimer = 0.0f;
        }

        // AUDIO - Transición: esperar sonido de inicio y arrancar música de juego
        if (levelStarting) {
            gameState = STATE_MAP;        // va al mapa inmediatamente
            levelStarting = false;
            levelStartTimer = 0.0f;
        }
        if (gameOver)
        {
            UpdateMusicStream(gameOverMusic);
        }

        int cameraOption = 5;

        // Animaciones
        if (attacking)
        {
            attackTimer--;
            if (attackTimer <= 0) attacking = false;
        }

        playerCounter++;
        if (IsKeyDown(KEY_D)) {
            if (playerCounter >= (60 / framesSpeed)) {
                playerCounter = 0;
                playerFrame++;
                if (playerFrame > 3) playerFrame = 0;
                frameRecR.x = (float)playerFrame * (float)AlexKiddWalkR.width / 4;
            }
        }
        if (IsKeyDown(KEY_A)) {
            if (playerCounter >= (60 / framesSpeed)) {
                playerCounter = 0;
                playerFrame++;
                if (playerFrame > 3) playerFrame = 0;
                frameRecL.x = (float)playerFrame * (float)AlexKiddWalkR.width / 4;
            }
        }

        pteroCounter++;
        if (pteroCounter >= (150 / framesSpeed)) {
            pteroCounter = 0;
            pteroFrame++;
            if (pteroFrame > 1) pteroFrame = 0;
            framePterodactil.x = (float)pteroFrame * (float)MonsterBirdR.width / 2;
        }

        // UPDATE
        if (gameState == STATE_PLAYING) {
            float deltaTime = GetFrameTime();
            UpdatePlayer(&player, envItems.data(), envItems.size(), deltaTime);

            // Mascota
            static float teleportCooldown = 0.0f;
            const float MAX_FOLLOW_DISTANCE = 1000.0f;

            if (teleportCooldown > 0.0f) teleportCooldown -= deltaTime;

            float side = (player.velX >= 0) ? -80 : 80;
            Vector2 targetPos = Vector2Add(player.position, Vector2{ side, 0 });
            float distance = Vector2Length(Vector2Subtract(player.position, petPosition));

            if (distance > MAX_FOLLOW_DISTANCE && teleportCooldown <= 0.0f && player.alive)
            {
                petPosition = targetPos;
                petPosition.y -= 30.0f;
                teleportCooldown = 1.2f;
            }
            else
            {
                float followSpeed = 5.0f;
                Vector2 dir = Vector2Subtract(targetPos, petPosition);
                Vector2 velocity = { dir.x * followSpeed * deltaTime, dir.y * followSpeed * deltaTime };

                Rectangle petRect = { petPosition.x - 20, petPosition.y - 20, 40, 40 };

                petRect.x += velocity.x;
                for (int i = 0; i < (int)envItems.size(); i++) {
                    EnvItem* ei = &envItems[i];
                    if (ei->blocking && ei->active && CheckCollisionRecs(petRect, ei->rect)) {
                        if (velocity.x > 0) petRect.x = ei->rect.x - petRect.width;
                        else if (velocity.x < 0) petRect.x = ei->rect.x + ei->rect.width;
                    }
                }

                petRect.y += velocity.y;
                for (int i = 0; i < (int)envItems.size(); i++) {
                    EnvItem* ei = &envItems[i];
                    if (ei->blocking && ei->active && CheckCollisionRecs(petRect, ei->rect)) {
                        if (velocity.y > 0) petRect.y = ei->rect.y - petRect.height;
                        else if (velocity.y < 0) petRect.y = ei->rect.y + ei->rect.height;
                    }
                }

                petPosition.x = petRect.x + petRect.width / 2;
                petPosition.y = petRect.y + petRect.height / 2;
            }

            // Muerte + animación + game over
            if (!player.alive && !player.deathAnim && !gameOver)
            {
                player.deathAnim = true;
                player.deathY = player.position.y - 80;
                player.deathX = player.position.x - 40;
                player.deathFrame = 0;
                player.deathFrameCounter = 0;
                player.lives--;

                if (player.lives <= 0) {
                    gameOver = true;

                    StopMusicStream(gameMusic);

                    PlayMusicStream(gameOverMusic);
                    
                }
            }

            if (player.deathAnim)
            {
                player.deathY -= 300.0f * deltaTime;
                player.deathFrameCounter++;
                if (player.deathFrameCounter >= 8) {
                    player.deathFrameCounter = 0;
                    player.deathFrame++;
                    if (player.deathFrame > 2) player.deathFrame = 2;
                }

                float screenTopWorld = camera.target.y - (screenHeight / camera.zoom);
                if (player.deathY < screenTopWorld && !gameOver)
                {
                    player.deathAnim = false;
                    player.alive = true;
                    player.speedY = 0;
                    player.velX = 0;

                    Vector2 spawnPos = { camera.target.x+100, camera.target.y - 250 };
                    const float maxSearchDown = 600.0f;
                    bool foundSafeSpot = false;

                    for (float testY = spawnPos.y; testY < spawnPos.y + maxSearchDown; testY += 40.0f)
                    {
                        Rectangle testRect = { spawnPos.x - 20, testY - 80, 40, 160 };
                        bool collision = false;
                        for (int i = 0; i < (int)envItems.size(); i++) {
                            EnvItem* ei = &envItems[i];
                            if (ei->active && ei->blocking && CheckCollisionRecs(testRect, ei->rect)) { collision = true; break; }
                        }
                        if (!collision)
                        {
                            Rectangle feetRect = { spawnPos.x - 20, testY + 75, 40, 20 };
                            bool hasGround = false;
                            for (int i = 0; i < (int)envItems.size(); i++) {
                                EnvItem* ei = &envItems[i];
                                if (ei->active && ei->blocking && CheckCollisionRecs(feetRect, ei->rect)) { hasGround = true; break; }
                            }
                            if (hasGround) { spawnPos.y = testY; foundSafeSpot = true; break; }
                        }
                    }
                    if (!foundSafeSpot) spawnPos.y = camera.target.y - 100;
                    player.position = spawnPos;
                }
                else if (player.deathY < screenTopWorld && gameOver)
                {
                    player.deathAnim = false;
                }
            }

            for (int i = 0; i < (int)pterodactilos.size(); i++)
                PterodactilMoviment(&pterodactilos[i], envItems.data(), envItems.size(), deltaTime);

            cameraUpdaters[cameraOption](&camera, &player, envItems.data(), envItems.size(), deltaTime, screenWidth, screenHeight);

            Rectangle playerRect = { player.position.x - 20, player.position.y - 80, 40, 80 };

            for (int i = 0; i < (int)envItems.size(); i++)
            {
                EnvItem* ei = &envItems[i];
                if (!ei->active) continue;

                if (ei->tileID == TILE_PORTA && CheckCollisionRecs(playerRect, ei->rect))
                {
                    StopMusicStream(gameMusic);
                    PlayMusicStream(titleMusic);

                    gameState = STATE_MENU;
                    gameOver = false;
                    player.lives = 3;
                    player.alive = true;
                    player.speedY = 0;
                    player.velX = 0;
                    player.coins = 0;
                    player.deathAnim = false;
                    player.position = Vector2{ 550, 200 };
                    camera.target = Vector2{ 550, 200 };
                    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
                    envItems = originalEnvItems;
                    for (int j = 0; j < (int)pterodactilos.size(); j++)
                        pterodactilos[j].vida = true;
                    break;
                }
                if (ei->collectible)
                {
                    if (CheckCollisionRecs(playerRect, ei->rect))
                    {
                        PlaySound(coinSound);

                        ei->active = false;

                        if (ei->texture.id == blockBossaCollons.id) player.coins += 100;
                        else if (ei->texture.id == blockBossaCollonsPetit.id) player.coins += 50;

                        continue;
                    }
                    if (ei->lifetime > 0)
                    {
                        ei->lifetime -= deltaTime;
                        if (ei->lifetime <= 0) ei->active = false;
                    }
                }
            }
        }

        // DRAW
        BeginDrawing();
        ClearBackground(BLAU);

        if (gameState == STATE_PLAYING)
        {
            BeginMode2D(camera);

            DrawTextureEx(nuvol, Vector2{ 200, -100 }, 0, 0.2f, WHITE);
            DrawTextureEx(nuvol, Vector2{ 700, 150 }, 0, 0.2f, WHITE);
            DrawTextureEx(nuvol, Vector2{ 950, 0 }, 0, 0.2f, WHITE);
            DrawTextureEx(MIAU, Vector2{ petPosition.x - 40, petPosition.y - 85 }, 0, 1.0f, WHITE);

            for (int i = 0; i < (int)envItems.size(); i++)
            {
                if (!envItems[i].active) continue;
                DrawTexturePro(
                    envItems[i].texture,
                    Rectangle{ 0, 0, (float)envItems[i].texture.width, (float)envItems[i].texture.height },
                    envItems[i].rect,
                    Vector2{ 0, 0 }, 0.0f, WHITE
                );
            }

            for (int i = 0; i < (int)pterodactilos.size(); i++)
                EnemyHitPlayer(&player, &pterodactilos[i]);

            if (player.deathAnim)
            {
                Rectangle frameRecDeathDraw = {
                    (float)player.deathFrame * (AlexKiddDeath.width / 3.0f),
                    0,
                    AlexKiddDeath.width / 3.0f,
                    (float)AlexKiddDeath.height
                };
                DrawTextureRec(AlexKiddDeath, frameRecDeathDraw, Vector2{ player.deathX, player.deathY }, WHITE);
            }

            if (player.alive)
            {
                if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D)) LeftOrRight = 0;
                else if (IsKeyPressed(KEY_A) || IsKeyDown(KEY_A)) LeftOrRight = 1;

                if (attacking && LeftOrRight == 0) DrawTextureRec(AlexKiddPunyR, frameRecPuny, Vector2{ player.position.x - 35, player.position.y - 129 }, WHITE);
                else if (attacking && LeftOrRight == 1) DrawTextureRec(AlexKiddPunyL, frameRecPuny, Vector2{ player.position.x - 75, player.position.y - 129 }, WHITE);
                else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) && player.canJump && !IsKeyPressed(KEY_ENTER) && LeftOrRight == 0 && !IsKeyDown(KEY_S)) DrawTextureRec(AlexKiddIdleR, frameRecR, Vector2{ player.position.x - 40, player.position.y - 128 }, WHITE);
                else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) && player.canJump && !IsKeyPressed(KEY_ENTER) && LeftOrRight == 1 && !IsKeyDown(KEY_S)) DrawTextureRec(AlexKiddIdleL, frameRecR, Vector2{ player.position.x - 40, player.position.y - 128 }, WHITE);
                else if (IsKeyDown(KEY_D) && player.canJump && !IsKeyDown(KEY_S)) DrawTextureRec(AlexKiddWalkR, frameRecR, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
                else if (IsKeyDown(KEY_A) && player.canJump && !IsKeyDown(KEY_S)) DrawTextureRec(AlexKiddWalkL, frameRecL, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
                else if (LeftOrRight == 0 && !player.canJump) DrawTextureRec(AlexKiddJumpR, frameRecJump, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
                else if (LeftOrRight == 1 && !player.canJump) DrawTextureRec(AlexKiddJumpL, frameRecJump, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
                else if (IsKeyDown(KEY_S) && player.canJump && LeftOrRight == 0) DrawTextureRec(AlexKiddCrouchR, frameRecJump, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
                else if (IsKeyDown(KEY_S) && player.canJump && LeftOrRight == 1) DrawTextureRec(AlexKiddCrouchL, frameRecJump, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);

                if (IsKeyPressed(KEY_ENTER) && !attacking)
                {
                    attacking = true;
                    attackTimer = 20;

                    PlaySound(punchSound);

                    PlayerBreakBlock(&player, envItems.data(), envItems.size(), LeftOrRight);

                    for (int i = 0; i < (int)pterodactilos.size(); i++)
                        PlayerAttackEnemy(&player, &pterodactilos[i], LeftOrRight);
                }
            }

            for (int i = 0; i < (int)pterodactilos.size(); i++)
            {
                enemic* p = &pterodactilos[i];
                if (!p->vida) continue;
                if (p->velocitat > 0)
                    DrawTextureRec(MonsterBirdR, framePterodactil, Vector2{ p->posicio.x, p->posicio.y }, WHITE);
                else
                    DrawTextureRec(MonsterBirdL, framePterodactil, Vector2{ p->posicio.x, p->posicio.y }, WHITE);
            }

            EndMode2D();

            // HUD: vidas
            for (int i = 0; i < player.lives; i++)
                DrawTextureEx(AlexKiddIdleR, Vector2{ 20.0f + i * 45.0f, 60.0f }, 0, 0.35f, WHITE);

            DrawText(TextFormat("Coins: %d", player.coins), 20, 20, 30, YELLOW);

            // GAME OVER
            if (gameOver && !player.deathAnim)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Color{ 0, 0, 0, 180 });

                int fontSize = 120;
                const char* text = "GAME OVER";
                int textW = MeasureText(text, fontSize);
                DrawText(text, screenWidth / 2 - textW / 2, screenHeight / 2 - 80, fontSize, RED);

                const char* sub = "Prem R per tornar al menu";
                int subW = MeasureText(sub, 40);
                DrawText(sub, screenWidth / 2 - subW / 2, screenHeight / 2 + 70, 40, WHITE);

                if (IsKeyPressed(KEY_R))
                {
                    // AUDIO - Volver a música de menú
                    StopMusicStream(gameMusic);
                    StopMusicStream(gameOverMusic);
                    PlayMusicStream(titleMusic);

                    gameOver = false;
                    gameOverTimer = 0.0f;
                    gameState = STATE_MENU;

                    player.lives = 3;
                    player.alive = true;
                    player.speedY = 0;
                    player.velX = 0;
                    player.coins = 0;
                    player.deathAnim = false;
                    player.position = Vector2{ 550, 200 };
                    camera.target = Vector2{ 550, 200 };
                    camera.offset = Vector2{ screenWidth / 3.3f, screenHeight / 1.5f };

                    envItems = originalEnvItems;
                    for (int i = 0; i < (int)pterodactilos.size(); i++)
                        pterodactilos[i].vida = true;
                }
            }
        }

        if (gameState == STATE_MAP)
        {
            levelStartTimer += GetFrameTime();

            ClearBackground(BLACK);

            // Dibuja la imagen centrada en pantalla
            DrawTexturePro(
                mapImage,
                Rectangle{ 0, 0, (float)mapImage.width, (float)mapImage.height },
                Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                Vector2{ 0, 0 }, 0.0f, WHITE
            );

            if (levelStartTimer >= 3.0f)
            {
                levelStartTimer = 0.0f;
                PlayMusicStream(gameMusic);
                gameState = STATE_PLAYING;
            }
        }
        // MENÚ
        if (gameState == STATE_MENU)
        {
            ClearBackground(BLAU);

            const char* title = "ALEX KIDD";
            int titleSize = 100;
            int titleW = MeasureText(title, titleSize);
            DrawText(title, screenWidth / 2 - titleW / 2, screenHeight / 2 - 180, titleSize, YELLOW);

            if ((int)(GetTime() * 2) % 2 == 0) {
                const char* prompt = "Prem ENTER per jugar";
                int promptSize = 45;
                int promptW = MeasureText(prompt, promptSize);
                DrawText(prompt, screenWidth / 2 - promptW / 2, screenHeight / 2, promptSize, WHITE);
            }

            const char* controls = "A / D  ->  Moure   |   ESPAI  ->  Saltar   |   ENTER  ->  Atacar";
            int ctrlW = MeasureText(controls, 28);
            DrawText(controls, screenWidth / 2 - ctrlW / 2, screenHeight / 2 + 100, 28, LIGHTGRAY);
        }

        EndDrawing();
    }

    // Unload
    UnloadTexture(background);
    UnloadSound(jumpSound);
    UnloadSound(levelStartSound);
    UnloadSound(coinSound);
    UnloadSound(punchSound);
    UnloadSound(coinBlockSound);
    UnloadMusicStream(titleMusic);
    UnloadMusicStream(gameMusic);
    UnloadMusicStream(gameOverMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta)
{
    if (!player->alive) return;

    Rectangle playerRect = {
        player->position.x - 35,
        player->position.y - 100,
        70,
        100
    };

    if (IsKeyPressed(KEY_SPACE) && player->canJump && !IsKeyDown(KEY_S))
    {
        player->speedY = -PLAYER_JUMP_SPD;
        PlaySound(jumpSound); // AUDIO
        player->canJump = false;
        player->isJumping = true;
        player->jumpTime = 0;
    }
    if (IsKeyReleased(KEY_SPACE) && player->speedY < 0)
        player->speedY *= 0.4f;

    if (IsKeyDown(KEY_SPACE) && player->isJumping)
    {
        player->jumpTime += delta;
        if (player->jumpTime < MAX_JUMP_TIME)
            player->speedY -= JUMP_HOLD_FORCE * delta;
    }
    if (IsKeyReleased(KEY_SPACE))
        player->isJumping = false;

    // Movimiento horizontal (primer bloque, mantener compatibilidad)
    if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_S))
        player->velX += PLAYER_ACC * delta;
    else if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_S))
        player->velX -= PLAYER_ACC * delta;
    else
    {
        if (player->velX > 0) { player->velX -= PLAYER_FRICTION * delta; if (player->velX < 0) player->velX = 0; }
        else if (player->velX < 0) { player->velX += PLAYER_FRICTION * delta; if (player->velX > 0) player->velX = 0; }
    }

    // Movimiento horizontal PRO++
    float acc = PLAYER_ACC;
    float friction = PLAYER_FRICTION;

    if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_S))
    {
        if (player->velX < 0) player->velX += friction * delta;
        else player->velX += acc * delta;
    }
    else if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_S))
    {
        if (player->velX > 0) player->velX -= friction * delta;
        else player->velX -= acc * delta;
    }
    else
    {
        if (player->velX > 0) { player->velX -= friction * delta; if (player->velX < 0) player->velX = 0; }
        else if (player->velX < 0) { player->velX += friction * delta; if (player->velX > 0) player->velX = 0; }
    }

    if (player->velX > PLAYER_MAX_SPEED) player->velX = PLAYER_MAX_SPEED;
    if (player->velX < -PLAYER_MAX_SPEED) player->velX = -PLAYER_MAX_SPEED;

    float moveX = player->velX * delta;
    playerRect.x += moveX;

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = &envItems[i];
        if (ei->blocking && ei->active && CheckCollisionRecs(playerRect, ei->rect))
        {
            if (moveX > 0) playerRect.x = ei->rect.x - playerRect.width;
            else if (moveX < 0) playerRect.x = ei->rect.x + ei->rect.width;
        }
    }

    player->speedY += G * delta;
    float moveY = player->speedY * delta;

    int steps = (int)fabs(moveY / 5.0f) + 1;
    float stepSize = moveY / steps;
    player->canJump = false;

    for (int s = 0; s < steps; s++)
    {
        playerRect.y += stepSize;
        for (int i = 0; i < envItemsLength; i++)
        {
            EnvItem* ei = &envItems[i];
            if (ei->blocking && ei->active && CheckCollisionRecs(playerRect, ei->rect))
            {
                if (stepSize > 0) { playerRect.y = ei->rect.y - playerRect.height; player->speedY = 0; player->canJump = true; player->isJumping = false; }
                if (stepSize < 0) { playerRect.y = ei->rect.y + ei->rect.height; player->speedY = 0; }
                s = steps;
                break;
            }
        }
    }

    player->position.x = playerRect.x + playerRect.width / 2;
    player->position.y = playerRect.y + playerRect.height;
}

void PlayerBreakBlock(Player* player, EnvItem* envItems, int envItemsLength, int LeftOrRight)
{
    float width = 50;
    float height = 40;
    float offsetX = (LeftOrRight == 0) ? 20 : -width;
    float offsetY = -60;

    Rectangle actionRect = { player->position.x + offsetX, player->position.y + offsetY, width, height };

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = &envItems[i];
        if (!ei->active) continue;

        if (CheckCollisionRecs(actionRect, ei->rect))
        {
            if (ei->drop == DROP_STAR && ei->texture.id == blockEstrella.id)
            {
                PlaySound(coinBlockSound);
                
                bool esGran = GetRandomValue(0, 1) == 0;
                if (esGran) { ei->texture = blockBossaCollons; }
                else { ei->texture = blockBossaCollonsPetit; }

                ei->type = BLOCK_SOLID;
                ei->blocking = 0;
                ei->collectible = true;
                ei->drop = DROP_NONE;
                ei->lifetime = 8.0f;
                continue;
            }
            if (ei->type == BLOCK_BREAKABLE) {
                
                ei->active = false;
            }
        }
    }
}

void PterodactilMoviment(enemic* pterodactil, EnvItem* envItems, int envItemsLength, float delta)
{
    Rectangle pteroRect = { pterodactil->posicio.x, pterodactil->posicio.y, 80, 40 };
    float moveX = pterodactil->velocitat * delta * 100;
    pteroRect.x += moveX;

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = &envItems[i];
        if (ei->blocking && ei->active && CheckCollisionRecs(pteroRect, ei->rect))
        {
            pterodactil->velocitat *= -1;
            if (moveX > 0) pteroRect.x = ei->rect.x - pteroRect.width;
            else pteroRect.x = ei->rect.x + ei->rect.width;
            break;
        }
    }
    pterodactil->posicio.x = pteroRect.x;
}

void PlayerHitEnemy(Player* player, enemic* pterodactil, int LeftOrRight)
{
    float width = 50, height = 40;
    float offsetX = (LeftOrRight == 0) ? 20 : -width;
    float offsetY = -60;
    Rectangle attackRect = { player->position.x + offsetX, player->position.y + offsetY, width, height };
    Rectangle pteroRect = { pterodactil->posicio.x, pterodactil->posicio.y, 80, 40 };
    if (pterodactil->vida && CheckCollisionRecs(attackRect, pteroRect))
        pterodactil->vida = false;
}

void EnemyHitPlayer(Player* player, enemic* pterodactil)
{
    if (!player->alive || !pterodactil->vida) return;
    Rectangle playerRect = { player->position.x - 20, player->position.y - 80, 40, 80 };
    Rectangle pteroRect = { pterodactil->posicio.x, pterodactil->posicio.y, 80, 40 };
    if (CheckCollisionRecs(playerRect, pteroRect))
    {
        player->alive = false;
        player->respawnTimer = 2.0f;
    }
}

void PlayerAttackEnemy(Player* player, enemic* ptero, int LeftOrRight)
{
    if (!ptero->vida) return;
    float punchWidth = 50, punchHeight = 40;
    float offsetX = (LeftOrRight == 0) ? 20 : -punchWidth;
    float offsetY = -60;
    Rectangle punchHitbox = { player->position.x + offsetX, player->position.y + offsetY, punchWidth, punchHeight };
    Rectangle pteroHitbox = { ptero->posicio.x, ptero->posicio.y, MonsterBirdR.width / 2.0f, MonsterBirdR.height };
    if (CheckCollisionRecs(punchHitbox, pteroHitbox))
        ptero->vida = false;
}

// CAMERAS
void UpdateCameraCenter(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    camera->target = player->position;
}

void UpdateCameraCenterInsideMap(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    camera->target = player->position;
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;
    for (int i = 0; i < envItemsLength; i++) {
        EnvItem* ei = envItems + i;
        minX = fminf(ei->rect.x, minX); maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY); maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
    }
    Vector2 max = GetWorldToScreen2D(Vector2{ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D(Vector2{ minX, minY }, *camera);
    if (max.x < width) camera->offset.x = width - (max.x - (float)width / 2);
    if (max.y < height) camera->offset.y = height - (max.y - (float)height / 2);
    if (min.x > 0) camera->offset.x = (float)width / 2 - min.x;
    if (min.y > 0) camera->offset.y = (float)height / 2 - min.y;
}

void UpdateCameraCenterSmoothFollow(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static float minSpeed = 30, minEffectLength = 10, fractionSpeed = 0.8f;
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);
    if (length > minEffectLength) {
        float speed = fmaxf(fractionSpeed * length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / length));
    }
}

void UpdateCameraEvenOutOnLanding(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static float evenOutSpeed = 700;
    static int eveningOut = false;
    static float evenOutTarget;
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    camera->target.x = player->position.x;
    if (eveningOut) {
        if (evenOutTarget > camera->target.y) { camera->target.y += evenOutSpeed * delta; if (camera->target.y > evenOutTarget) { camera->target.y = evenOutTarget; eveningOut = 0; } }
        else { camera->target.y -= evenOutSpeed * delta; if (camera->target.y < evenOutTarget) { camera->target.y = evenOutTarget; eveningOut = 0; } }
    }
    else {
        if (player->canJump && (player->speedY == 0) && (player->position.y != camera->target.y)) { eveningOut = 1; evenOutTarget = player->position.y; }
    }
}

void UpdateCameraPlayerBoundsPush(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static Vector2 bbox = { 0.2f, 0.2f };
    Vector2 bboxWorldMin = GetScreenToWorld2D(Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D(Vector2{ (1 + bbox.x) * 0.5f * width, (1 + bbox.y) * 0.5f * height }, *camera);
    camera->offset = Vector2{ (1 - bbox.x) * 0.5f * width, (1 - bbox.y) * 0.5f * height };
    if (player->position.x < bboxWorldMin.x) camera->target.x = player->position.x;
    if (player->position.y < bboxWorldMin.y) camera->target.y = player->position.y;
    if (player->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    if (player->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}

void UpdateCameraDownOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static float lowestY = 0;
    static float fixedX = 0;
    static bool initialized = false;
    camera->offset = Vector2{ width / 3.3f, height / 1.5f };
    if (player->position.y < 300 && player->position.x < 600) { lowestY = 0; initialized = false; }
    if (!initialized || lowestY == 0) { lowestY = player->position.y + 400; fixedX = 582; initialized = true; }
    if (player->position.y > lowestY) lowestY = player->position.y;
    camera->target.x = fixedX;
    camera->target.y = lowestY;
}
