/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

Modified by: Alan, Ian, Yarley, Lluc
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

#define TILE_PEDRA 16
#define TILE_WARP 17

#define TILE_SHOP_ENTER 18   // Entrar a la tienda (activa con W)
#define TILE_SHOP_EXIT  19   // Salir de la tienda (activa con W)

// Tiles de cueva (nivel de cuevas)
#define TILE_TERRA_COVA      20
#define TILE_TERRA_COVA_L    21
#define TILE_TERRA_COVA_R    22
#define TILE_TRIANGLE_COVA_L 23
#define TILE_TRIANGLE_COVA_R 24
#define TILE_SOLID_CAVE1     25
#define TILE_SOLID_CAVE2     26
#define TILE_ESTALAGMITA_L   27
#define TILE_ESTALAGMITA_R   28
#define TILE_BREAK_CAVE      29
#define TILE_PUNTITA_L      30
#define TILE_PUNTITA_R      31
#define TILE_BLOC_LAVA      32
#define TILE_LAVA       33

#define TILE_SIZE 80

#define BLAU  CLITERAL(Color){8, 9, 250}

#define LEVEL_SHOP 10   // índice especial para la tienda

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
// ===== TEXTURAS (sección global de texturas) =====
// NUEVO: Añadir después de las texturas de MonsterBird
Texture2D EscorpiR;
Texture2D EscorpiL;
Texture2D CastanyaR;   // o CastanyaL si tienes sprite mirando izquierda
Texture2D CastanyaL;
Texture2D Lava;

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

// Texturas de la cueva
Texture2D terraCova;
Texture2D terraCovaL;
Texture2D terraCovaR;
Texture2D triangleCovaL;
Texture2D triangleCovaR;
Texture2D bloqueSolidoCueva1;
Texture2D bloqueSolidoCueva2;
Texture2D estalagmitaL;
Texture2D estalagmitaR;
Texture2D bloqueRompibleCueva;
Texture2D puntitaL;
Texture2D puntitaR;
Texture2D bloqueLava;

Texture2D MIAU;
Texture2D negro;

Texture2D pedra;
Texture2D TendaFora;
Texture2D FondoTenda;

Texture2D mapImage;

Texture2D MenuFondo;
Texture2D MenuTítol;
Texture2D Menu1;
Texture2D Menu2;
Texture2D Menu3;
Texture2D Menu4;
Texture2D Menu5;
Texture2D Menu6;

Texture2D Inventari;
Texture2D PowerBracelet;

Texture2D GameOver;
// AUDIO
Music titleMusic;
Music gameMusic;
Sound jumpSound;
Sound levelStartSound;
Sound coinSound;
Sound punchSound;
Sound coinBlockSound;
Sound blockBreakSound;
Sound lifeTakenSound;
Music gameOverMusic;

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

typedef enum {
    ITEM_NONE,
    ITEM_RING
} ItemType;

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

    ItemType inventory[5];
    int inventoryCount;
    bool ringActive;
} Player;

typedef enum {
    BLOCK_SOLID,
    BLOCK_BREAKABLE
} BlockType;

typedef enum {
    DROP_NONE,
    DROP_COIN,
    DROP_STAR,
    DROP_RING
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

typedef struct WorldItem {
    Vector2 position;
    ItemType type;
    bool active;
    float bobTimer;
} WorldItem;

typedef struct enemic {
    bool vida;
    int velocitat;
    bool siToca;
    Vector3 posicio;
} pterodactil, escorpi, castaña;

typedef enum {
    STATE_CREDITS,
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
void UpdateCameraHorizontalOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void PterodactilMoviment(enemic* pterodactil, EnvItem* envItems, int envItemsLength, float delta);
void PlayerBreakBlock(Player* player, EnvItem* envItems, int envItemsLength, int LeftOrRight, std::vector<WorldItem>& worldItems, bool& ringDropped);
void PlayerHitEnemy(Player* player, enemic* pterodactil, int LeftOrRight);
void EnemyHitPlayer(Player* player, enemic* pterodactil);
void PlayerAttackEnemy(Player* player, enemic* ptero, int LeftOrRight);

std::vector<EnvItem> BuildEnvItemsFromMap(int* map, int rows, int cols,
    Texture2D blockSolidTerra, Texture2D blockBreak,
    Texture2D blockTerraR, Texture2D blockTerraL,
    Texture2D blockHerbaR, Texture2D blockHerbaL,
    Texture2D blockSolidHerba, Texture2D blockInterrogant,
    Texture2D blockEstrella, Texture2D blockCalaveraGroc,
    Texture2D blockCalaveraRosa, Texture2D blockBossaCollons,
    Texture2D blockBossaCollonsPetit, Texture2D blockPorta,
    Texture2D negro, Texture2D pedra)
{
    std::vector<EnvItem> BuildEnvItemsFromMap(int* map, int rows, int cols,
        Texture2D blockSolidTerra, Texture2D blockBreak,
        Texture2D blockTerraR, Texture2D blockTerraL,
        Texture2D blockHerbaR, Texture2D blockHerbaL,
        Texture2D blockSolidHerba, Texture2D blockInterrogant,
        Texture2D blockEstrella, Texture2D blockCalaveraGroc,
        Texture2D blockCalaveraRosa, Texture2D blockBossaCollons,
        Texture2D blockBossaCollonsPetit, Texture2D blockPorta,
        Texture2D negro, Texture2D pedra);
    
        std::vector<EnvItem> items;
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                int tile = map[y * cols + x];   // <-- acceso plano
                if (tile == 0) continue;
                if (tile == TILE_WARP) {
                    EnvItem warp = { 0 };
                    warp.rect = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                    warp.blocking = 0;
                    warp.active = true;
                    warp.tileID = TILE_WARP;
                    warp.texture = { 0 };
                    items.push_back(warp);
                    continue;
                }
                EnvItem item = { 0 };
                item.rect = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                item.blocking = 1;
                item.active = true;
                item.lifetime = 0.0f;
                if (tile == TILE_SOLID) { item.texture = blockSolidTerra;   item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_BREAK) { item.texture = blockBreak;        item.type = BLOCK_BREAKABLE; item.drop = DROP_COIN; }
                else if (tile == TILE_NEGRO) { item.texture = negro;             item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_PEDRA) { item.texture = pedra;             item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_PORTA) { item.texture = blockPorta;        item.type = BLOCK_SOLID;     item.blocking = 0; item.collectible = false; item.drop = DROP_NONE; }
                else if (tile == TILE_SOLID_HERBA) { item.texture = blockSolidHerba;  item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_HERBA_R) { item.texture = blockHerbaR;       item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_HERBA_L) { item.texture = blockHerbaL;       item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_TERRA_R) { item.texture = blockTerraR;       item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_TERRA_L) { item.texture = blockTerraL;       item.type = BLOCK_SOLID;     item.drop = DROP_NONE; }
                else if (tile == TILE_INTERROGANT) { item.texture = blockInterrogant;  item.type = BLOCK_BREAKABLE; item.drop = DROP_STAR; }
                else if (tile == TILE_ESTRELLA) { item.texture = blockEstrella;     item.type = BLOCK_BREAKABLE; item.drop = DROP_STAR; }
                else if (tile == TILE_EMOTICONOCALAVERAGROC) { item.texture = blockCalaveraGroc; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_EMOTICONOCALAVERAROSA) { item.texture = blockCalaveraRosa; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_BOSSACOLLONS) { item.texture = blockBossaCollons; item.type = BLOCK_SOLID; item.blocking = 0; item.collectible = true; item.drop = DROP_NONE; }
                else if (tile == TILE_SHOP_ENTER) { item.type = BLOCK_SOLID; item.blocking = 0; item.collectible = false; item.drop = DROP_NONE; }
                else if (tile == TILE_SHOP_EXIT) { item.texture = blockPorta; item.type = BLOCK_SOLID; item.blocking = 0; item.collectible = false; item.drop = DROP_NONE; }
                else if (tile == TILE_TERRA_COVA) { item.texture = terraCova;           item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_TERRA_COVA_L) { item.texture = terraCovaL;          item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_TERRA_COVA_R) { item.texture = terraCovaR;          item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_TRIANGLE_COVA_L) { item.texture = triangleCovaL;       item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_TRIANGLE_COVA_R) { item.texture = triangleCovaR;       item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_SOLID_CAVE1) { item.texture = bloqueSolidoCueva1;  item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_SOLID_CAVE2) { item.texture = bloqueSolidoCueva2;  item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_ESTALAGMITA_L) { item.texture = estalagmitaL;        item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_ESTALAGMITA_R) { item.texture = estalagmitaR;        item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_PUNTITA_L) { item.texture = puntitaL;        item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_PUNTITA_R) { item.texture = puntitaR;        item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_BLOC_LAVA) { item.texture = bloqueLava;        item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_LAVA) { item.texture = Lava;        item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                else if (tile == TILE_BREAK_CAVE) { item.texture = bloqueRompibleCueva; item.type = BLOCK_BREAKABLE; item.drop = DROP_COIN; }
                else { item.texture = blockSolidTerra; item.type = BLOCK_SOLID; item.drop = DROP_NONE; }
                item.tileID = tile;
                items.push_back(item);
            }
        }
        return items;
    }

//------------------------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------------------------
int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 900;

    float creditsTimer = 0.0f;

    GameState gameState = STATE_MENU;
    int currentLevel = 1;
    int previousLevel = 1;  // para recordar desde qué nivel se entró a la tienda

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

    // ===== CARGA DE TEXTURAS (dentro de main, tras cargar MonsterBird) =====
// NUEVO:
    EscorpiR = LoadTexture("resources/escorpiR.png");
    EscorpiL = LoadTexture("resources/escorpiL.png");
    CastanyaR = LoadTexture("resources/CastanyaR.png");
    CastanyaL = LoadTexture("resources/CastanyaL.png");
    Lava = LoadTexture("resources/Lava.png");

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

    terraCova = LoadTexture("resources/terraCova.png");
    terraCovaL = LoadTexture("resources/terraCovaL.png");
    terraCovaR = LoadTexture("resources/terraCovaR.png");
    triangleCovaL = LoadTexture("resources/triangleCovaL.png");
    triangleCovaR = LoadTexture("resources/triangleCovaR.png");
    bloqueSolidoCueva1 = LoadTexture("resources/bloqueSolidoCueva1.png");
    bloqueSolidoCueva2 = LoadTexture("resources/bloqueSolidoCueva2.png");
    estalagmitaL = LoadTexture("resources/estalagmitaL.png");
    estalagmitaR = LoadTexture("resources/estalagmitaR.png");
    bloqueRompibleCueva = LoadTexture("resources/bloqueRompibleCueva.png");
	puntitaL = LoadTexture("resources/puntitaL.png");
	puntitaR = LoadTexture("resources/puntitaR.png");
    bloqueLava = LoadTexture("resources/BlocLava.png");

    MIAU = LoadTexture("resources/Patricio.png");
    negro = LoadTexture("resources/Negro.png");

    pedra = LoadTexture("resources/Tenda/Piedra.png");
    TendaFora = LoadTexture("resources/Tenda/Tenda2.png");

    mapImage = LoadTexture("resources/mapa.png");



    MenuFondo = LoadTexture("resources/Fondo.png");
    MenuTítol = LoadTexture("resources/0.png");
    Menu1 = LoadTexture("resources/1.png");
    Menu2 = LoadTexture("resources/2.png");
    Menu3 = LoadTexture("resources/3.png");
    Menu4 = LoadTexture("resources/4.png");
    Menu5 = LoadTexture("resources/5.png");
    Menu6 = LoadTexture("resources/6.png");

    Inventari = LoadTexture("resources/Inventari.png");
    PowerBracelet = LoadTexture("resources/PowerBracelet.png");

    GameOver = LoadTexture("resources/GameOver.png");



    // AUDIO
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
    SetSoundVolume(coinBlockSound, 1.0f);

    blockBreakSound = LoadSound("resources/BlockBreak.wav");
    SetSoundVolume(blockBreakSound, 1.0f);

    lifeTakenSound = LoadSound("resources/LifeTaken.wav");
    SetSoundVolume(lifeTakenSound, 0.8f);

    gameOverMusic = LoadMusicStream("resources/GameOver.wav");
    gameOverMusic.looping = false;
    SetMusicVolume(gameOverMusic, 0.6f);




    Rectangle frameRecR = { 0.0f, 0.0f, ((float)AlexKiddWalkR.width / 4), ((float)AlexKiddWalkR.height) };
    Rectangle frameRecL = { 0.0f, 0.0f, ((float)AlexKiddWalkL.width / 4), ((float)AlexKiddWalkL.height) };
    Rectangle frameRecJump = { 0.0f, 0.0f, ((float)AlexKiddJumpR.width), ((float)AlexKiddJumpR.height) };
    Rectangle frameRecPuny = { 0.0f, 0.0f, ((float)AlexKiddPunyR.width), ((float)AlexKiddPunyR.height) };
    Rectangle frameRecCrouch = { 0.0f, 0.0f, ((float)AlexKiddPunyR.width), ((float)AlexKiddPunyR.height) };
    Rectangle framePterodactil = { 0.0f, 0.0f, ((float)MonsterBirdR.width / 2), ((float)MonsterBirdR.height) };

    Rectangle frameEscorpi = { 0.0f, 0.0f, ((float)EscorpiR.width / 2), (float)EscorpiR.height };
    Rectangle frameCastanya = { 0.0f, 0.0f, ((float)CastanyaR.width / 2), (float)CastanyaR.height };
    Rectangle frameLava = { 0.0f, 0.0f, ((float)Lava.width / 4), (float)Lava.height };

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
    std::vector<enemic> escorpins;
    std::vector<enemic> castanyes;

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

    // Escorpins — solo map4 (nivel 3 jugable)
    enemic e1 = { 0 }; e1.posicio = Vector3{ 2000, 1080 }; e1.velocitat = 2; e1.vida = true; escorpins.push_back(e1);
    enemic e2 = { 0 }; e2.posicio = Vector3{ 4000, 1080 }; e2.velocitat = 2; e2.vida = true; escorpins.push_back(e2);
    enemic e3 = { 0 }; e3.posicio = Vector3{ 6000, 1080 }; e3.velocitat = 2; e3.vida = true; escorpins.push_back(e3);
    enemic e4 = { 0 }; e4.posicio = Vector3{ 8000, 1080 }; e4.velocitat = 3; e4.vida = true; escorpins.push_back(e4);
    enemic e5 = { 0 }; e5.posicio = Vector3{ 10000,1080 }; e5.velocitat = 3; e5.vida = true; escorpins.push_back(e5);

    // Castanyes — solo map4 (nivel 3 jugable)
    enemic c1 = { 0 }; c1.posicio = Vector3{ 3000, 1080 }; c1.velocitat = 2; c1.vida = true; castanyes.push_back(c1);
    enemic c2 = { 0 }; c2.posicio = Vector3{ 5000, 1080 }; c2.velocitat = 2; c2.vida = true; castanyes.push_back(c2);
    enemic c3 = { 0 }; c3.posicio = Vector3{ 7000, 1080 }; c3.velocitat = 3; c3.vida = true; castanyes.push_back(c3);
    enemic c4 = { 0 }; c4.posicio = Vector3{ 9000, 1080 }; c4.velocitat = 3; c4.vida = true; castanyes.push_back(c4);

    // NUEVO: guardar originales para reset
    std::vector<enemic> originalPterodactilos = pterodactilos;
    std::vector<enemic> originalEscorpins = escorpins;
    std::vector<enemic> originalCastanyes = castanyes;

    player.position = Vector2{ 550, 200 };
    player.spawn = player.position;
    player.alive = true;
    player.respawnTimer = 2.0f;

    std::vector<WorldItem> worldItems;
    bool ringDropped = false;

    int map[105][24] = {
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
{3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,15,10,0,0,3,3,3,3},
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

    int map2[105][24] = {
 {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
 {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
 {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
 {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
 {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
 {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
 {3,3,3,3,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,0,3,3,3,3},
 {3,3,3,3,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,15,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    // MAPA DE LA TIENDA (level especial)
    int map3[12][24] = {
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,3,3,3,3},
         {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
         {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3}
    };

    int map4[24][180] = {
    {3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,0,0,30,25,25,25,26,25,26,25,26,25,26,26,26,25,25,26,25,26/*Castaña*/,25,26,25,25,25,25,26,26,25,26,26,25,26,26,26,25,25,26,25,26,25,25,25,25,26,26,25,26,26,25,25,26,25,25,26,26,26,26,25,25,25,26,25,25,25,26,26,26,25,26,25,26,25,26,25,25,25,25,25,25,25,25,25,25,26,26,26,26,26,25,26,25,26,25,25,26,25,26,25,26,26,26,26,25,26,25,26,25,26,25,25,25,25,25,25,25,25,25,25,26,26,25,25,26,25,26,25,26,25,25,25,25,25,25,25,25,25,25,26,26,26,26,25,25,26,25,26,25,26,25,26,25,25,26,26,25,25,25,26,25,26,25,25,26,26,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,0,0,23,26,25,26,27,26,25,26,28,28,26,25,25,26,25,25,26,0/*Castaña*/,28,25,28,25,26,25,26,25,26,25,27,25,27,26,25,26,25,26,25,25,25,28,25,26,25,26,25,28,28,26,0,25,0,25,26,26,28,26,25,27,0,0,26,27,26,25,0,27,25,26,0,26,25,28,26,25,0,26,25,0,0,27,25,0,0,0,0,25,26,9,0,0,0,0,25,26,25,26,25,10,0,0,0,10,26,25,26,26,26,26,25,26,0,28,0,0,27,28,25,0,27,28,27,0,26,25,26,25,28,27,26,28,0,28,26,28,25,26,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,0,30,26,25,26,27,0,0,27,0,0,0,0,28,28,25,26,25,26,0/*Castaña*/,0,27,0,0,0,0,13,13,13,0,0,0,0,27,28,27,25,26,26,25,26,0,0,26,25,27,0,0,0,29,29,29,0,27,25,0,0,28,0,0,0,0,28,0,0,0,0,0,28,0,0,28,0,0,29,29,29,0,26,0,0,0,28,0,0,25,26,27,25,13,13,13,13,0,28,25,26,25,26,29,0,0,0,29,25,26,0,28,27,26,28,0,0,0,0,0,0,0,28,0,0,0,0,0,27,26,28,25,0,0,0,0,0,0,28,0,26,25,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,0,23,25,27,27,0,0,0,29,29,29,29,29,0,0,26,26,27,28,0/*Castaña*/,0,0,0,0,0,0,29,29,29,0,0,0,0,0,0,0,28,28,25,25,28,0,0,27,0,0,11,0,0,29,10,29,0,0,27,0,0,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,29,29,29,0,28,0,0,0,0,0,0,0,0,0,27,26,25,26,25,0,0,0,28,0,0,0,0,29,0,0,26,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,30,26,27,0,0,0,0,0,10,0,0,0,10,0,0,28,0,0,0,0/*Castaña*/,0,0,0,29,29,29,29,29,29,29,29,0,0,0,0,0,0,0,27,0,0,0,0,0,0,29,29,0,0,0,29,0,0,0,0,29,10,29,0,13,13,13,0,0,0,0,0,0,0,0,0,0,29,29,29,29,29,0,0,0,0,0,0,29,29,29,0,0,0,0,0,0,0,26,0,0,0,0,0,0,0,0,0,0,26,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,0,23,25,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,0/*Castaña*/,0,29,0,10,29,0,0,0,0,0,29,29,29,0,0,0,0,0,0,0,0,29,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,29,29,0,29,29,0,0,29,29,0,0,0,29,29,10,10,29,0,0,29,29,0,0,29,9,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,25,0,0,0,29,29,29,29,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,30,26,27,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,29,29/*Castaña*/,10,29,0,0,0,0,0,0,0,0,29,29,29,0,0,0,0,0,0,29,9,10,29,0,0,0,0,0,0/*Hueco lava*/,21,22,0,0,0,0,0,0,0,0,29,29,29,0,29,29,0,29,29,0,0,0,0,29,29,13,13,29,0,29,29,29,0,0,29,29,29,0,0,0,0,10/*Estrella rosa*/,26,26,26,26,0,0,21,20,20,20,22,0,0,25,26,0,0,0,10,13,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,23,25,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,0,29,29/*Castaña*/,29,0,0,0,0,0,13,13,0,0,0,10,10,29,0,0,29,29,0,29,29,29,13,0,0,21,20,22,0/*Hueco lava*/,20,20,20,22,0,0,0,0,0,0,29,29,29,0,0,0,0,29,0,29,0,0,0,29,29,29,29,29,0,29,29,29,0,0,0,0,0,29,26,26,26,0/*Estrella rosa*/,0,0,0,0,0,0,20,20,20,20,20,0,0,28,27,0,0,29,11,0,0,0,0,29,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,25,24,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,26,27,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,0,29,29/*Castaña*/,29,0,0,0,0,0,0,0,0,0,0,0,0,29,0,0,29,29,0,0,0,0,0,21,20,20,20,20,0/*Hueco lava*/,20,20,20,20,20,20,22,0,0,0,0,0,0,0,0,0,0,29,29,29,0,0,0,0,0,0,0,0,0,0,0,0,0,26,26,26,26,0,0,0,0,0/*Estrella rosa*/,0,0,0,0,0,21,20,20,20,20,20,0/*Calavera rosa*/,0,0,0,0,29,29,10,0,0,0,0,29,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,28,26,3,3,3,3},
    {3,3,3,3,3,3,3,3,0,0,25,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,21,20,20,20/*Castaña*/,20,20,22,0,0,0,0,0,0,0,0,0,0,0,21,20,20,20,20,20,20,20,20,20,20,20,20,20,0/*Hueco lava*/,0,0,0,0,0,0,20,20,20,22,0,0,0,0,0,0,0,0,0,0,0,0,21,20,0,20,0,20,20,0,20,22,0,0,0,0,0,0,0,0,0,0/*Estrella rosa*/,29,29,29,0,21,20,20,20,20,20,20,0/*Calavera rosa*/,0,0,0,29,29,10,0,0,0,0/*Calavera rosa*/,0,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,25,3,3,3,3},
    {3,3,3,3,1,1,1,1,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,22,33,33,33,21,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,0/*Hueco lava*/,0,10,13,0,0,0,20,20,20,20,0,0,0,0,21,20,20,20,20,20,20,20,20,20,0,20,0,20,20,0,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,12/*Calavera rosa*/,20,20,20,20,20,20,20,20,20,12/*Calavera rosa*/,20,20,20,20,22,0,0,21,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,3,3,3,3},
    {3,3,3,3,1,1,1,1,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,32,32,32,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,0/*Hueco lava*/,21,20,20,20,22,0,20,20,20,20,0,0,0,0,20,20,20,20,20,20,20,20,20,20,0,20,0,20,20,0,20,20,20,20,20,20,20,20,20,20,20,20/*Estrella rosa*/,20,20,20,20,20,20,20,20,20,20,20,20/*Calavera rosa*/,20,20,20,20,20,20,20,20,20,20/*Calavera rosa*/,20,20,20,20,20,0,0,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    };

    std::vector<EnvItem> envItems = BuildEnvItemsFromMap((int*)map, 105, 24,
        blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
        blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
        blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
        blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);

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
        UpdateCameraDownOnly,
        UpdateCameraHorizontalOnly

    };

    int cameraOption = 4;

    SetTargetFPS(60);

    static float x = 400, y = 400;

    bool gameOver = false;
    float gameOverTimer = 0.0f;
    // Variables animación menú
    float menuImageTimer = 0.0f;
    int menuImageIndex = 0;
    bool mostrarInventari = false;

    int inventariSeleccionat = 0;

    while (!WindowShouldClose())
    {
        if (gameState == STATE_CREDITS) {
            creditsTimer += GetFrameTime();

            if (creditsTimer >= 3.0f) {
                StopMusicStream(gameMusic);
                PlaySound(levelStartSound);
                levelStarting = true;
                levelStartTimer = 0.0f;
                gameState = STATE_MAP;
            }
        }

        // AUDIO
        if (gameState == STATE_MENU)
        {
            UpdateMusicStream(titleMusic);
            if (menuImageIndex < 6) {
                menuImageTimer += GetFrameTime();
                if (menuImageTimer >= 0.5f) {
                    menuImageTimer = 0.0f;
                    menuImageIndex++;
                }
            }

            DrawTexturePro(MenuFondo,
                Rectangle{ 0, 0, (float)MenuFondo.width, (float)MenuFondo.height },
                Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                Vector2{ 0, 0 }, 0.0f, WHITE);

            DrawTexturePro(MenuTítol,
                Rectangle{ 0, 0, (float)MenuTítol.width, (float)MenuTítol.height },
                Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                Vector2{ 0, 0 }, 0.0f, WHITE);

            Texture2D menuImages[] = { Menu1, Menu2, Menu3, Menu4, Menu5, Menu6 };
            for (int i = 0; i < menuImageIndex; i++) {
                DrawTexturePro(menuImages[i],
                    Rectangle{ 0, 0, (float)menuImages[i].width, (float)menuImages[i].height },
                    Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0, 0 }, 0.0f, WHITE);
            }
        }
        else if (gameState == STATE_PLAYING) {
            UpdateMusicStream(gameMusic);
        }

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        if (gameState == STATE_MENU && IsKeyPressed(KEY_ENTER)) {
            StopMusicStream(titleMusic);
            gameState = STATE_CREDITS;
            creditsTimer = 0.0f;
        }

        if (gameOver)
        {
            UpdateMusicStream(gameOverMusic);
        }

        // Selección de cámara según nivel (el nivel 3 usa la cámara centrada, etc.)
        if (currentLevel == 3) cameraOption = 6;
        else cameraOption = 5;

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
            frameEscorpi.x = (float)pteroFrame * (float)EscorpiR.width / 2;
            frameCastanya.x = (float)pteroFrame * (float)CastanyaR.width / 2;
        }

        // UPDATE
        if (gameState == STATE_PLAYING) {
            float deltaTime = GetFrameTime();
            UpdatePlayer(&player, envItems.data(), envItems.size(), deltaTime);

            if (IsKeyPressed(KEY_P)) {
                mostrarInventari = !mostrarInventari;
                if (!mostrarInventari) inventariSeleccionat = 0;
            }

            if (mostrarInventari) {
                if (IsKeyPressed(KEY_D) && inventariSeleccionat < player.inventoryCount - 1)
                    inventariSeleccionat++;
                if (IsKeyPressed(KEY_A) && inventariSeleccionat > 0)
                    inventariSeleccionat--;
                if (IsKeyPressed(KEY_ENTER) && player.inventoryCount > 0) {
                    ItemType selectedItem = player.inventory[inventariSeleccionat];
                    if (selectedItem == ITEM_RING) {
                        player.ringActive = true;
                        for (int i = inventariSeleccionat; i < player.inventoryCount - 1; i++)
                            player.inventory[i] = player.inventory[i + 1];
                        player.inventory[player.inventoryCount - 1] = ITEM_NONE;
                        player.inventoryCount--;
                        if (inventariSeleccionat >= player.inventoryCount && inventariSeleccionat > 0)
                            inventariSeleccionat--;
                        mostrarInventari = false;
                    }
                }
            }
            else {
                Rectangle playerRect = { player.position.x - 20, player.position.y - 80, 40, 80 };
                for (int i = 0; i < (int)worldItems.size(); i++) {
                    WorldItem* wi = &worldItems[i];
                    if (!wi->active) continue;
                    wi->bobTimer += deltaTime;
                    Rectangle itemRect = { wi->position.x - 20, wi->position.y - 20, 40, 40 };
                    if (CheckCollisionRecs(playerRect, itemRect)) {
                        if (player.inventoryCount < 5) {
                            player.inventory[player.inventoryCount] = wi->type;
                            player.inventoryCount++;
                            PlaySound(coinSound);
                        }
                        wi->active = false;
                    }
                }
            }
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

                StopMusicStream(gameMusic);

                if (player.lives > 0) {
                    PlaySound(lifeTakenSound);
                }
                else {
                    gameOver = true;
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

                    PlayMusicStream(gameMusic);

                    Vector2 spawnPos = { camera.target.x + 100, camera.target.y - 250 };
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

            if (currentLevel == 3) {
                for (int i = 0; i < (int)escorpins.size(); i++)
                    PterodactilMoviment(&escorpins[i], envItems.data(), envItems.size(), deltaTime);
                for (int i = 0; i < (int)castanyes.size(); i++)
                    PterodactilMoviment(&castanyes[i], envItems.data(), envItems.size(), deltaTime);
            }

            cameraUpdaters[cameraOption](&camera, &player, envItems.data(), envItems.size(), deltaTime, screenWidth, screenHeight);

            Rectangle playerRect = { player.position.x - 20, player.position.y - 80, 40, 80 };

            // ----- TRANSICIONES ENTRE NIVELES Y TIENDA -----
            for (int i = 0; i < (int)envItems.size(); i++)
            {
                EnvItem* ei = &envItems[i];
                if (!ei->active) continue;

                // --- PUERTA NORMAL (avanza de nivel) ---
                if (ei->tileID == TILE_PORTA && CheckCollisionRecs(playerRect, ei->rect))
                {
                    // Solo avanzamos si no estamos en la tienda (la tienda tiene su propia salida)
                    if (currentLevel != LEVEL_SHOP)
                    {
                        // Incrementar nivel normal (1->2->3->...)
                        currentLevel++;

                        // Si superamos el nivel 3 (demo) mostramos créditos y volvemos al menú
                        if (currentLevel > 3)
                        {
                            StopMusicStream(gameMusic);
                            PlayMusicStream(titleMusic);
                            gameState = STATE_MENU;
                            gameOver = false;
                            player.lives = 3;
                            menuImageTimer = 0.0f;
                            menuImageIndex = 0;
                            currentLevel = 1;
                            player.alive = true;
                            player.speedY = 0;
                            player.velX = 0;
                            player.coins = 0;
                            player.deathAnim = false;
                            player.position = Vector2{ 550, 200 };
                            camera.target = Vector2{ 550, 200 };
                            camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
                            // Recargar nivel 1
                            envItems = BuildEnvItemsFromMap((int*)map, 105, 24,
                                blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
                                blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
                                blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
                                blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);
                            originalEnvItems = envItems;
                            pterodactilos = originalPterodactilos;
                            escorpins = originalEscorpins;
                            castanyes = originalCastanyes;
                        }

                        // Cargar el siguiente nivel según el número
                        StopMusicStream(gameMusic);
                        PlaySound(levelStartSound);
                        levelStarting = true;
                        levelStartTimer = 0.0f;
                        gameState = STATE_MAP;

                        player.alive = true;
                        player.speedY = 0;
                        player.velX = 0;
                        player.deathAnim = false;
                        // Posiciones iniciales de cada nivel
                        if (currentLevel == 2) {
                            player.position = Vector2{ 550, 200 };
                            camera.target = Vector2{ 550, 200 };
                        }
                        else if (currentLevel == 3) {
                            player.position = Vector2{ 1000, 1000 };
                            camera.target = Vector2{ 1000, 1000 };
                        }
                        camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };

                        // Construir el mapa correspondiente
                        if (currentLevel == 2) {
                            envItems = BuildEnvItemsFromMap((int*)map2, 20, 24,
                                blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
                                blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
                                blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
                                blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);
                        }
                        else if (currentLevel == 3) {
                            envItems = BuildEnvItemsFromMap((int*)map4, 24, 180,
                                blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
                                blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
                                blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
                                blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);
                        }
                        originalEnvItems = envItems;
                        pterodactilos = originalPterodactilos;
                    }
                }

                // --- ENTRAR A LA TIENDA (solo disponible en nivel 2, por ejemplo) ---
                if (ei->tileID == TILE_SHOP_ENTER && ei->active && CheckCollisionRecs(playerRect, ei->rect) && IsKeyPressed(KEY_W))
                {
                    if (currentLevel != LEVEL_SHOP)
                    {
                        previousLevel = currentLevel;   // guardamos desde dónde venimos
                        currentLevel = LEVEL_SHOP;

                        StopMusicStream(gameMusic);
                        PlaySound(levelStartSound);
                        levelStarting = true;
                        levelStartTimer = 0.0f;
                        gameState = STATE_MAP;

                        player.alive = true;
                        player.speedY = 0;
                        player.velX = 0;
                        player.deathAnim = false;
                        player.position = Vector2{ 200, 200 };   // punto de spawn dentro de la tienda
                        camera.target = Vector2{ 200, 200 };
                        camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };

                        // Cargar el mapa de la tienda (map3)
                        envItems = BuildEnvItemsFromMap((int*)map3, 12, 24,
                            blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
                            blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
                            blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
                            blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);
                        originalEnvItems = envItems;
                        pterodactilos = originalPterodactilos;
                    }
                }

                // --- SALIR DE LA TIENDA (volver al nivel anterior) ---
                if (ei->tileID == TILE_SHOP_EXIT && ei->active && CheckCollisionRecs(playerRect, ei->rect) && IsKeyPressed(KEY_W))
                {
                    if (currentLevel == LEVEL_SHOP)
                    {
                        currentLevel = previousLevel;   // restauramos el nivel desde el que entramos

                        StopMusicStream(gameMusic);
                        PlaySound(levelStartSound);
                        levelStarting = true;
                        levelStartTimer = 0.0f;
                        gameState = STATE_MAP;

                        player.alive = true;
                        player.speedY = 0;
                        player.velX = 0;
                        player.deathAnim = false;
                        // Posición de salida (cerca de la entrada a la tienda)
                        if (currentLevel == 2) {
                            player.position = Vector2{ 1000, 400 };
                            camera.target = Vector2{ 1000, 400 };
                        }
                        else {
                            player.position = Vector2{ 550, 200 };
                            camera.target = Vector2{ 550, 200 };
                        }
                        camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };

                        // Recargar el mapa del nivel correspondiente
                        if (currentLevel == 2) {
                            envItems = BuildEnvItemsFromMap((int*)map2, 20, 24,
                                blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
                                blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
                                blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
                                blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);
                        }
                        else if (currentLevel == 1) {
                            envItems = BuildEnvItemsFromMap((int*)map, 105, 24,
                                blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
                                blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
                                blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
                                blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);
                        }
                        originalEnvItems = envItems;
                        pterodactilos = originalPterodactilos;
                    }
                }

                // --- COLECTABLES (bolsas) ---
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

        if (gameState == STATE_CREDITS)
        {
            ClearBackground(BLACK);

            DrawText("CREDITS", screenWidth / 2 - 150, 200, 60, WHITE);

            DrawText("Alan del Tio", screenWidth / 2 - 200, 350, 40, LIGHTGRAY);
            DrawText("Ian Leon", screenWidth / 2 - 200, 420, 40, LIGHTGRAY);
            DrawText("Yarley Tituana", screenWidth / 2 - 200, 490, 40, LIGHTGRAY);
            DrawText("Lluc Torner", screenWidth / 2 - 200, 560, 40, LIGHTGRAY);
        }
        if (currentLevel == 3)
            ClearBackground(CLITERAL(Color) { 85, 0, 0 });  // morado oscuro cueva
        else
            ClearBackground(BLAU);

        if (gameState == STATE_PLAYING)
        {
            BeginMode2D(camera);

            DrawTextureEx(nuvol, Vector2{ 200, -100 }, 0, 0.2f, WHITE);
            DrawTextureEx(nuvol, Vector2{ 700, 150 }, 0, 0.2f, WHITE);
            DrawTextureEx(nuvol, Vector2{ 950, 0 }, 0, 0.2f, WHITE);
            if (currentLevel == 2)
            {
                DrawTexturePro(
                    TendaFora,
                    Rectangle{ 0, 0, (float)TendaFora.width, (float)TendaFora.height },
                    Rectangle{ 1000, 450, 400, 350 },
                    Vector2{ 300, 150 }, 0.0f, WHITE
                );
            }
            DrawTextureEx(MIAU, Vector2{ petPosition.x - 40, petPosition.y - 85 }, 0, 1.0f, WHITE);

            for (int i = 0; i < (int)envItems.size(); i++)
            {
                if (!envItems[i].active) continue;
                if (envItems[i].tileID == TILE_WARP) continue;
                DrawTexturePro(
                    envItems[i].texture,
                    Rectangle{ 0, 0, (float)envItems[i].texture.width, (float)envItems[i].texture.height },
                    envItems[i].rect,
                    Vector2{ 0, 0 }, 0.0f, WHITE
                );
            }

            for (int i = 0; i < (int)worldItems.size(); i++) {
                WorldItem* wi = &worldItems[i];
                if (!wi->active) continue;
                if (wi->type == ITEM_RING) {
                    DrawTexturePro(
                        PowerBracelet,
                        Rectangle{ 0, 0, (float)PowerBracelet.width, (float)PowerBracelet.height },
                        Rectangle{ wi->position.x - 40, wi->position.y - 40, 80, 80 },
                        Vector2{ 0, 0 }, 0.0f, WHITE);
                }
            }

            for (int i = 0; i < (int)pterodactilos.size(); i++)
                EnemyHitPlayer(&player, &pterodactilos[i]);

            if (currentLevel == 3) {
                for (int i = 0; i < (int)escorpins.size(); i++)
                    EnemyHitPlayer(&player, &escorpins[i]);
                for (int i = 0; i < (int)castanyes.size(); i++)
                    EnemyHitPlayer(&player, &castanyes[i]);
            }

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

                if (IsKeyPressed(KEY_ENTER) && !attacking && !mostrarInventari)
                {
                    attacking = true;
                    attackTimer = 20;

                    PlaySound(punchSound);

                    PlayerBreakBlock(&player, envItems.data(), envItems.size(), LeftOrRight, worldItems, ringDropped);

                    for (int i = 0; i < (int)pterodactilos.size(); i++)
                        PlayerAttackEnemy(&player, &pterodactilos[i], LeftOrRight);

                    if (currentLevel == 3) {
                        for (int i = 0; i < (int)escorpins.size(); i++)
                            PlayerAttackEnemy(&player, &escorpins[i], LeftOrRight);
                        for (int i = 0; i < (int)castanyes.size(); i++)
                            PlayerAttackEnemy(&player, &castanyes[i], LeftOrRight);
                    }
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

            if (currentLevel == 3) {
                for (int i = 0; i < (int)escorpins.size(); i++) {
                    enemic* e = &escorpins[i];
                    if (!e->vida) continue;
                    if (e->velocitat > 0)
                        DrawTextureRec(EscorpiR, frameEscorpi, Vector2{ e->posicio.x, e->posicio.y }, WHITE);
                    else
                        DrawTextureRec(EscorpiL, frameEscorpi, Vector2{ e->posicio.x, e->posicio.y }, WHITE);
                }
                for (int i = 0; i < (int)castanyes.size(); i++) {
                    enemic* c = &castanyes[i];
                    if (!c->vida) continue;
                    if (c->velocitat > 0)
                        DrawTextureRec(CastanyaR, frameCastanya, Vector2{ c->posicio.x, c->posicio.y }, WHITE);
                    else
                        DrawTextureRec(CastanyaL, frameCastanya, Vector2{ c->posicio.x, c->posicio.y }, WHITE);
                }
            }

            EndMode2D();

            // HUD: vidas
            for (int i = 0; i < player.lives; i++)
                DrawTextureEx(AlexKiddIdleR, Vector2{ 20.0f + i * 45.0f, 60.0f }, 0, 0.35f, WHITE);

            DrawText(TextFormat("Coins: %d", player.coins), 20, 20, 30, YELLOW);

            if (player.inventoryCount > 0) {
                DrawText("P = Inventari", screenWidth - 220, 20, 22, WHITE);
                for (int i = 0; i < player.inventoryCount; i++) {
                    if (player.inventory[i] == ITEM_RING) {
                        DrawTexturePro(PowerBracelet,
                            Rectangle{ 0, 0, (float)PowerBracelet.width, (float)PowerBracelet.height },
                            Rectangle{ (float)(screenWidth - 220 + i * 50), 50, 40, 40 },
                            Vector2{ 0, 0 }, 0.0f, WHITE);
                    }
                }
            }
            if (player.ringActive) {
                DrawText("ANELL ACTIU!", screenWidth / 2 - 80, 20, 28, GOLD);
            }

            // GAME OVER
            if (gameOver && !player.deathAnim)
            {
                DrawTexturePro(
                    GameOver,
                    Rectangle{ 0, 0, (float)GameOver.width, (float)GameOver.height },
                    Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0, 0 }, 0.0f, WHITE
                );

                if (IsKeyPressed(KEY_R))
                {
                    StopMusicStream(gameMusic);
                    StopMusicStream(gameOverMusic);
                    PlayMusicStream(titleMusic);

                    gameOver = false;
                    gameOverTimer = 0.0f;
                    gameState = STATE_MENU;

                    player.lives = 3;
                    player.alive = true;
                    menuImageTimer = 0.0f;
                    menuImageIndex = 0;
                    player.speedY = 0;
                    player.velX = 0;
                    player.coins = 0;
                    player.deathAnim = false;
                    player.position = Vector2{ 550, 200 };
                    camera.target = Vector2{ 550, 200 };
                    camera.offset = Vector2{ screenWidth / 3.3f, screenHeight / 1.5f };

                    envItems = originalEnvItems;
                    for (int i = 0; i < (int)pterodactilos.size(); i++)
                        pterodactilos = originalPterodactilos;

                    currentLevel = 1;
                    envItems = BuildEnvItemsFromMap((int*)map, 105, 24,
                        blockSolidTerra, blockBreak, blockTerraR, blockTerraL,
                        blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant,
                        blockEstrella, blockCalaveraGroc, blockCalaveraRosa,
                        blockBossaCollons, blockBossaCollonsPetit, blockPorta, negro, pedra);
                    originalEnvItems = envItems;
                }
            }
        }

        if (gameState == STATE_MAP)
        {
            levelStartTimer += GetFrameTime();

            ClearBackground(BLACK);

            DrawTexturePro(
                mapImage,
                Rectangle{ 0, 0, (float)mapImage.width, (float)mapImage.height },
                Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                Vector2{ 0, 0 }, 0.0f, WHITE
            );

            if (!IsSoundPlaying(levelStartSound))
            {
                PlayMusicStream(gameMusic);
                gameState = STATE_PLAYING;
            }
        }
        if (mostrarInventari)
        {
            DrawTexturePro(
                Inventari,
                Rectangle{ 0, 0, (float)Inventari.width, (float)Inventari.height },
                Rectangle{ 0, 0, (float)screenWidth, (float)screenHeight },
                Vector2{ 0, 0 }, 0.0f, WHITE);

            float slotSize = 80.0f;
            float startX = screenWidth / 2.0f - (5 * slotSize + 4 * 10) / 2.0f;
            float slotY = screenHeight / 2.0f - slotSize / 2.0f;

            for (int i = 0; i < 5; i++) {
                float sx = startX + i * (slotSize + 10);
                Color slotColor = (i == inventariSeleccionat) ? YELLOW : WHITE;
                DrawRectangleLinesEx(Rectangle{ sx - 3, slotY - 3, slotSize + 6, slotSize + 6 }, 3, slotColor);
                if (i < player.inventoryCount && player.inventory[i] == ITEM_RING) {
                    DrawTexturePro(PowerBracelet,
                        Rectangle{ 0, 0, (float)PowerBracelet.width, (float)PowerBracelet.height },
                        Rectangle{ sx, slotY, slotSize, slotSize },
                        Vector2{ 0, 0 }, 0.0f, WHITE);
                }
            }

            DrawText("A/D: seleccionar   ENTER: usar   P: tancar", screenWidth / 2 - 280, (int)slotY + 100, 26, WHITE);

            if (player.inventoryCount > 0 && inventariSeleccionat < player.inventoryCount) {
                if (player.inventory[inventariSeleccionat] == ITEM_RING) {
                    DrawText("Anell del Poder", screenWidth / 2 - 100, (int)slotY - 50, 28, GOLD);
                }
            }
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
    UnloadSound(blockBreakSound);
    UnloadSound(lifeTakenSound);
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
        PlaySound(jumpSound);
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

    // Movimiento horizontal
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

void PlayerBreakBlock(Player* player, EnvItem* envItems, int envItemsLength, int LeftOrRight, std::vector<WorldItem>& worldItems, bool& ringDropped)
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

            if (ei->tileID == TILE_INTERROGANT) {
                PlaySound(coinBlockSound);
                ei->active = false;
                if (!ringDropped) {
                    ringDropped = true;
                    WorldItem wi;
                    wi.position = { ei->rect.x + TILE_SIZE / 2.0f, ei->rect.y + TILE_SIZE / 2.0f };
                    wi.type = ITEM_RING;
                    wi.active = true;
                    wi.bobTimer = 0.0f;
                    worldItems.push_back(wi);
                }
                continue;
            }

            if (ei->type == BLOCK_BREAKABLE) {

                if (ei->tileID == TILE_BREAK) {
                    PlaySound(blockBreakSound);
                }

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
    float punchWidth = 70, punchHeight = 40;
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

void UpdateCameraHorizontalOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height)
{
    static float furthestX = -2.0f;
    static float fixedY = -2.0f;   // <-- nuevo: altura fija

    // Reset al entrar al nivel
    if (player->position.x < 600.0f) {
        furthestX = -2.0f;
        fixedY = -2.0f;
    }

    // Inicializar ambos en el primer frame
    if (furthestX < 0.0f) {
        furthestX = player->position.x;
        fixedY = player->position.y;   // capturamos la Y del spawn, nunca más cambia
    }

    // Solo avanza hacia la derecha
    if (player->position.x > furthestX) {
        furthestX = player->position.x;
    }

    camera->offset = Vector2{ width / 3.5f, height / 1.35f };
    camera->target.x = furthestX;
    camera->target.y = fixedY;     // Y completamente fija
}