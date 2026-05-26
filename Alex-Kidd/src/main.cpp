/*
    Alex Kidd – Versión definitiva combinada (optimizada)
    (tienda + enemigos + anillo + inventario)
    Compra automática al tocar los objetos de la tienda (si hay monedas suficientes)
    Mejora: persistencia de objetos comprados + mensajes con fondo negro
    MODIFICACIÓN: Hitbox del jugador personalizable y más pequeña al agacharse
*/
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

#define G 2000
#define PLAYER_JUMP_SPD 1000.0f
#define PLAYER_ACC 1000.0f
#define PLAYER_FRICTION 1000.0f
#define PLAYER_MAX_SPEED 400.0f
#define JUMP_HOLD_FORCE 1200.0f
#define MAX_JUMP_TIME 0.2f
#define RING_DURATION 10.0f

//HITBOX
#define PLAYER_HITBOX_WIDTH 70.0f            // ancho de la hitbox
#define PLAYER_HITBOX_HEIGHT 126.0f          // alto de pie
#define PLAYER_CROUCH_HITBOX_HEIGHT 60.0f    // alto al agacharse

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
#define TILE_SHOP_ENTER 18
#define TILE_SHOP_EXIT  19
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
#define TILE_PUNTITA_L       30
#define TILE_PUNTITA_R       31
#define TILE_BLOC_LAVA       32
#define TILE_LAVA            33

#define TILE_SHOP_VIDA     40
#define TILE_SHOP_ANILLO   41
#define TILE_SHOP_RANDOM   42

#define TILE_SIZE 80
#define BLAU  Color{8, 9, 250}
#define LEVEL_SHOP 10

// ---------------------------------------------------------------------
// Textures
// ---------------------------------------------------------------------
Texture2D background, nuvol;
Texture2D AlexKiddIdleR, AlexKiddIdleL, AlexKiddWalkR, AlexKiddWalkL;
Texture2D AlexKiddJumpR, AlexKiddJumpL, AlexKiddPunyR, AlexKiddPunyL;
Texture2D AlexKiddCrouchR, AlexKiddCrouchL, AlexKiddDeath;
Texture2D MonsterBirdR, MonsterBirdL;
Texture2D EscorpiR, EscorpiL, CastanyaR, CastanyaL, Lava;
Texture2D blockSolidTerra, blockBreak, blockTerraR, blockTerraL;
Texture2D blockHerbaR, blockHerbaL, blockSolidHerba, blockInterrogant;
Texture2D blockEstrella, blockPorta;
Texture2D blockCalaveraGroc, blockCalaveraRosa;
Texture2D blockBossaCollons, blockBossaCollonsPetit;
Texture2D terraCova, terraCovaL, terraCovaR, triangleCovaL, triangleCovaR;
Texture2D bloqueSolidoCueva1, bloqueSolidoCueva2, estalagmitaL, estalagmitaR;
Texture2D bloqueRompibleCueva, puntitaL, puntitaR, bloqueLava;
Texture2D MIAU, negro, pedra, TendaFora, TendaBackground, ItemRandom, VidaExtra;
Texture2D mapImage, MenuFondo, MenuTítol, Menu1, Menu2, Menu3, Menu4, Menu5, Menu6;
Texture2D Inventari, PowerBracelet, GameOver;
Texture2D texMapChar[2];

// ---------------------------------------------------------------------
// Audio
// ---------------------------------------------------------------------
Music titleMusic, gameMusic, gameOverMusic;
Sound jumpSound, levelStartSound, coinSound, punchSound;
Sound coinBlockSound, blockBreakSound, lifeTakenSound;

// ---------------------------------------------------------------------
// Font
// ---------------------------------------------------------------------
Font fontBm;

// ---------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------
typedef enum { ITEM_NONE, ITEM_RING } ItemType;

typedef struct Player {
    Vector2 position;
    float speedY, velX;
    bool canJump, alive, isJumping, deathAnim;
    float respawnTimer, jumpTime;
    int coins, lives, deathFrame, deathFrameCounter;
    float deathY, deathX;
    Vector2 spawn;
    ItemType inventory[5];
    int inventoryCount;
    bool ringActive;
    float ringTimer;
    float acc;
    float friction;
    float maxSpeed;
} Player;

typedef enum { BLOCK_SOLID, BLOCK_BREAKABLE } BlockType;
typedef enum { DROP_NONE, DROP_COIN, DROP_STAR, DROP_RING } DropType;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Texture2D texture;
    BlockType type;
    bool active, collectible;
    DropType drop;
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
    Vector3 posicio;
} pterodactil, escorpi, castaña;

typedef enum { STATE_CREDITS, STATE_MENU, STATE_MAP, STATE_PLAYING } GameState;

// ---------------------------------------------------------------------
// Function prototypes
// ---------------------------------------------------------------------
void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraDownOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraHorizontalOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void PterodactilMoviment(enemic* p, EnvItem* envItems, int len, float delta);
void PlayerBreakBlock(Player* player, EnvItem* envItems, int len, int LeftOrRight, std::vector<WorldItem>& worldItems, bool& ringDropped);
void PlayerAttackEnemy(Player* player, enemic* p, int dir);
void EnemyHitPlayer(Player* player, enemic* p);
void EnemyGroundMoviment(enemic* p, EnvItem* envItems, int len, float delta);

// --- NUEVA FUNCIÓN AUXILIAR PARA OBTENER LA HITBOX DEL JUGADOR ---
Rectangle GetPlayerBodyRect(Player* player) {
    float width = PLAYER_HITBOX_WIDTH;
    float height = (IsKeyDown(KEY_S) && player->canJump) ? PLAYER_CROUCH_HITBOX_HEIGHT : PLAYER_HITBOX_HEIGHT;
    return { player->position.x - width / 2.0f, player->position.y - height, width, height };
}

std::vector<EnvItem> BuildEnvItemsFromMap(const int* map, int rows, int cols);

// ---------------------------------------------------------------------
// Tile definition lookup table (initialized after texture loading)
// ---------------------------------------------------------------------
struct TileDef {
    Texture2D texture;
    BlockType type;
    int blocking;
    bool collectible;
    DropType drop;
};

static TileDef tileDefs[45];

void InitTileDefs() {
    for (int i = 0; i < 45; ++i)
        tileDefs[i] = { blockSolidTerra, BLOCK_SOLID, 1, false, DROP_NONE };

    tileDefs[TILE_EMPTY] = { {0}, BLOCK_SOLID, 0, false, DROP_NONE };
    tileDefs[TILE_SOLID] = { blockSolidTerra, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_BREAK] = { blockBreak, BLOCK_BREAKABLE, 1, false, DROP_COIN };
    tileDefs[TILE_NEGRO] = { negro, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_PEDRA] = { pedra, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_PORTA] = { blockPorta, BLOCK_SOLID, 0, false, DROP_NONE };
    tileDefs[TILE_SOLID_HERBA] = { blockSolidHerba, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_HERBA_R] = { blockHerbaR, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_HERBA_L] = { blockHerbaL, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_TERRA_R] = { blockTerraR, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_TERRA_L] = { blockTerraL, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_INTERROGANT] = { blockInterrogant, BLOCK_BREAKABLE, 1, false, DROP_STAR };
    tileDefs[TILE_ESTRELLA] = { blockEstrella, BLOCK_BREAKABLE, 1, false, DROP_STAR };
    tileDefs[TILE_EMOTICONOCALAVERAGROC] = { blockCalaveraGroc, BLOCK_BREAKABLE, 1, false, DROP_NONE };
    tileDefs[TILE_EMOTICONOCALAVERAROSA] = { blockCalaveraRosa, BLOCK_BREAKABLE, 1, false, DROP_NONE };
    tileDefs[TILE_BOSSACOLLONS] = { blockBossaCollons, BLOCK_SOLID, 0, true, DROP_NONE };
    tileDefs[TILE_BOSSACOLLONSPETIT] = { blockBossaCollonsPetit, BLOCK_SOLID, 0, true, DROP_NONE };
    tileDefs[TILE_SHOP_ENTER] = { {0}, BLOCK_SOLID, 0, false, DROP_NONE };
    tileDefs[TILE_SHOP_EXIT] = { {0}, BLOCK_SOLID, 0, false, DROP_NONE };
    tileDefs[TILE_TERRA_COVA] = { terraCova, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_TERRA_COVA_L] = { terraCovaL, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_TERRA_COVA_R] = { terraCovaR, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_TRIANGLE_COVA_L] = { triangleCovaL, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_TRIANGLE_COVA_R] = { triangleCovaR, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_SOLID_CAVE1] = { bloqueSolidoCueva1, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_SOLID_CAVE2] = { bloqueSolidoCueva2, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_ESTALAGMITA_L] = { estalagmitaL, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_ESTALAGMITA_R] = { estalagmitaR, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_PUNTITA_L] = { puntitaL, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_PUNTITA_R] = { puntitaR, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_BLOC_LAVA] = { bloqueLava, BLOCK_SOLID, 1, false, DROP_NONE };
    tileDefs[TILE_LAVA] = { {0}, BLOCK_SOLID, 0, false, DROP_NONE };
    tileDefs[TILE_BREAK_CAVE] = { bloqueRompibleCueva, BLOCK_BREAKABLE, 1, false, DROP_COIN };
    tileDefs[TILE_SHOP_VIDA] = { VidaExtra,   BLOCK_SOLID, 0, false, DROP_NONE };
    tileDefs[TILE_SHOP_ANILLO] = { PowerBracelet, BLOCK_SOLID, 0, false, DROP_NONE };
    tileDefs[TILE_SHOP_RANDOM] = { ItemRandom,  BLOCK_SOLID, 0, false, DROP_NONE };
}

// ---------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------
void LoadMapLevel(int level, std::vector<EnvItem>& envItems,
    const int* map1, const int* map2, const int* map3, const int* map4,
    int rows1, int cols1, int rows2, int cols2, int rows3, int cols3, int rows4, int cols4) {
    switch (level) {
    case 1:  envItems = BuildEnvItemsFromMap(map1, rows1, cols1); break;
    case 2:  envItems = BuildEnvItemsFromMap(map2, rows2, cols2); break;
    case 3:  envItems = BuildEnvItemsFromMap(map4, rows4, cols4); break;
    case LEVEL_SHOP: envItems = BuildEnvItemsFromMap(map3, rows3, cols3); break;
    default: break;
    }
}

static float cam_furthestX = -2.0f;
static float cam_fixedY = -2.0f;
static float cam_lowestY = 0.0f;
static float cam_fixedX = 0.0f;
static bool  cam_init = false;

void ResetGameToMenu(Player& player, Camera2D& camera, int& currentLevel,
    bool& gameOver, GameState& gameState, int& menuImageIndex,
    std::vector<EnvItem>& envItems,
    std::vector<enemic>& pterodactilos, std::vector<enemic>& escorpins, std::vector<enemic>& castanyes,
    const std::vector<enemic>& origP, const std::vector<enemic>& origE, const std::vector<enemic>& origC,
    const int* map1, int rows1, int cols1,
    std::vector<Vector2>& purchasedShopItems) {
    StopMusicStream(gameMusic);
    PlayMusicStream(titleMusic);
    gameOver = false;
    gameState = STATE_MENU;
    player.lives = 300;
    player.alive = true;
    player.coins = 0;
    player.deathAnim = false;
    player.speedY = 0.0f;
    player.velX = 0.0f;
    player.position = Vector2{ 550.0f, 200.0f };
    camera.target = Vector2{ 550.0f, 200.0f };
    currentLevel = 1;
    menuImageIndex = 0;
    player.ringActive = false;
    player.inventoryCount = 0;
    envItems = BuildEnvItemsFromMap(map1, rows1, cols1);
    pterodactilos = origP;
    escorpins = origE;
    castanyes = origC;
    purchasedShopItems.clear();
    cam_furthestX = -2.0f;
    cam_fixedY = -2.0f;
    cam_lowestY = 0.0f;
    cam_fixedX = 0.0f;
    cam_init = false;
}


// ---------------------------------------------------------------------
// DialogBox (Animacio Quadre de Text)
// ---------------------------------------------------------------------
static std::vector<float> BuildCharPositions(Font font, const std::string& line,
    float startX, float fontSize)
{
    std::vector<float> positions;
    float x = startX;
    for (char c : line) {
        char buf[2] = { c, '\0' };
        x += MeasureTextEx(font, buf, fontSize, 2.0f).x + 2.0f;
        positions.push_back(x);
    }
    return positions;
}

class DialogBox {
    enum State { BORDER, TEXT };

    Rectangle   rect;
    Font        font;
    float       fontSize = 30.0f;
    float       padding = 16.0f;
    float       textSpeed = 1000.0f;
    float       borderSpeed = 1500.0f;

    float borderProgress = 0.0f;
    float totalPerimeter = 0.0f;
    float animX = 0.0f;

    std::vector<std::string>         lines;        // pares: línea de texto + línea de retorno vacía
    std::vector<std::vector<float>>  charPositions;
    size_t currentLine = 0;
    State  state = BORDER;
    bool   done = false;

    static constexpr float BORDER_T = 6.0f;

    float TextLineH()   const { return fontSize + 4.0f; }
    float ReturnLineH() const { return fontSize * 0.33f; }

public:
    DialogBox(Rectangle pos, const std::vector<std::string>& dialogue, Font f)
        : rect(pos), font(f)
    {
        // Intercalar líneas reales con líneas de retorno vacías
        for (size_t i = 0; i < dialogue.size(); ++i) {
            lines.push_back(dialogue[i]);
            if (i + 1 < dialogue.size()) lines.push_back("");
        }

        // Pre-calcular posiciones de caracteres para cada línea de texto
        charPositions.resize(lines.size());
        for (size_t i = 0; i < lines.size(); i += 2)
            charPositions[i] = BuildCharPositions(font, lines[i], rect.x + padding, fontSize);

        totalPerimeter = (rect.width + rect.height) * 2.0f;
        animX = rect.x + padding;
    }

    void Update(float dt) {
        if (done) return;

        if (state == BORDER) {
            borderProgress += borderSpeed * dt;
            if (borderProgress >= totalPerimeter) {
                borderProgress = totalPerimeter;
                state = TEXT;
            }
            return;
        }

        // state == TEXT
        const float startX = rect.x + padding;
        const float endX = rect.x + rect.width - padding;
        const bool  isText = (currentLine % 2 == 0);
        const float target = isText ? endX : startX;
        const float dir = isText ? 1.0f : -1.0f;

        animX += dir * textSpeed * dt;
        if ((isText && animX >= target) || (!isText && animX <= target)) {
            animX = target;
            if (currentLine + 1 < lines.size()) {
                ++currentLine;
                animX = isText ? endX : startX;  // posición de inicio del siguiente segmento
            }
            else {
                done = true;
            }
        }
    }

    void Draw() const {
        DrawBorder();
        if (state == BORDER) return;
        DrawContent();
    }

    bool IsDone() const { return done; }

private:
    void DrawBorder() const {
        // Dibuja el borde recorre el perímetro en orden
        struct Side { float x, y, w, h; float length; };
        Side sides[4] = {
            { rect.x,                            rect.y,                             rect.width,  BORDER_T,  rect.width  },
            { rect.x + rect.width - BORDER_T,    rect.y,                             BORDER_T,    rect.height, rect.height },
            { rect.x,                            rect.y + rect.height - BORDER_T,    rect.width,  BORDER_T,  rect.width  },
            { rect.x,                            rect.y,                             BORDER_T,    rect.height, rect.height },
        };
        // Correcciones para que el borde izquierdo y derecho no se superpongan
        sides[1].y = rect.y;
        sides[2].x = rect.x + rect.width - sides[2].w;   // empieza por la derecha
        sides[3].y = rect.y + rect.height - BORDER_T;    // empieza por abajo

        float remaining = borderProgress;
        // Arriba: izq->der
        float d = (remaining > rect.width) ? rect.width : remaining;
        if (d > 0) DrawRectangle(rect.x, rect.y, d, BORDER_T, BLACK);
        remaining -= rect.width;
        // Derecha: arriba->abajo
        d = (remaining > rect.height) ? rect.height : remaining;
        if (remaining > 0 && d > 0)
            DrawRectangle(rect.x + rect.width - BORDER_T, rect.y, BORDER_T, d, BLACK);
        remaining -= rect.height;
        // Abajo: der->izq
        d = (remaining > rect.width) ? rect.width : remaining;
        if (remaining > 0 && d > 0)
            DrawRectangle(rect.x + rect.width - d, rect.y + rect.height - BORDER_T, d, BORDER_T, BLACK);
        remaining -= rect.width;
        // Izquierda: abajo->arriba
        d = (remaining > rect.height) ? rect.height : remaining;
        if (remaining > 0 && d > 0)
            DrawRectangle(rect.x, rect.y + rect.height - d, BORDER_T, d, BLACK);
    }

    void DrawContent() const {
        if (done)
            DrawRectangle(rect.x + BORDER_T, rect.y + BORDER_T,
                rect.width - BORDER_T * 2, rect.height - BORDER_T * 2, BLACK);

        const float startX = rect.x + padding;
        const float endX = rect.x + rect.width - padding;
        const float totalW = endX - startX;
        float y = rect.y + padding;

        for (size_t i = 0; i <= currentLine && i < lines.size(); ++i) {
            const bool isText = (i % 2 == 0);
            const float lineH = isText ? TextLineH() : ReturnLineH();
            const bool  isCurrent = (i == currentLine);

            if (!isCurrent) {
                // Línea ya completada
                DrawRectangle(startX, y, totalW, lineH, BLACK);
                if (isText)
                    DrawTextEx(font, lines[i].c_str(), { startX, y }, fontSize, 2.0f, WHITE);
            }
            else {
                // Línea en animación
                if (isText) {
                    float fillW = animX - startX;
                    DrawRectangle(startX, y, fillW, lineH, BLACK);
                    float drawX = startX;
                    for (size_t j = 0; j < lines[i].size(); ++j) {
                        if (animX >= charPositions[i][j]) {
                            char buf[2] = { lines[i][j], '\0' };
                            DrawTextEx(font, buf, { drawX, y }, fontSize, 2.0f, WHITE);
                        }
                        char buf[2] = { lines[i][j], '\0' };
                        drawX += MeasureTextEx(font, buf, fontSize, 2.0f).x + 2.0f;
                    }
                }
                else {
                    float fillW = endX - animX;
                    DrawRectangle(animX, y, fillW, lineH, BLACK);
                }
            }
            y += lineH;
        }
    }
};

// ---------------------------------------------------------------------
// MAIN
// ---------------------------------------------------------------------
int main(void) {
    const int screenWidth = 1920, screenHeight = 900;
    float creditsTimer = 0.0f;
    GameState gameState = STATE_MENU;
    int currentLevel = 1, previousLevel = 1;
    bool gameOver = false, levelStarting = false, mostrarInventari = false;
    int inventariSeleccionat = 0;
    float levelStartTimer = 0.0f, menuImageTimer = 0.0f;
    int menuImageIndex = 0;

    // ---- Variables de animación STATE_MAP ----
    const float MAP_REVEAL_X_START = 496.0f;
    const float MAP_REVEAL_X_END = 1359.0f;
    const float MAP_REVEAL_WIDTH = MAP_REVEAL_X_END - MAP_REVEAL_X_START;
    const Vector2 MAP_CHAR_POS = { 1360.0f * (screenWidth / 1900.0f),
                                        622.0f * (screenHeight / 900.0f) };
    float mapRevealProgress = 0.0f;   // 0.0 → 1.0
    float mapCharTimer = 0.0f;
    int   mapCharFrame = 0;

    // Persistencia de objetos comprados en la tienda
    std::vector<Vector2> purchasedShopItems;

    // ---- DialogBox state ----
    DialogBox* activeDialog = nullptr;
    float      dialogHoldTimer = 0.0f;     // espera tras completar la animación
    bool       dialogFreeze = false;    // congela al jugador mientras hay diálogo
    bool       shopWelcomeDone = false;    // evita repetir el diálogo de bienvenida
    float      shopDialogCooldown = 0.0f; // cooldown tras cerrar un diálogo de tienda (evita repetición)

    InitWindow(screenWidth, screenHeight, "Alex Kidd - Merged");
    InitAudioDevice();

    // ------------------- Load Textures -------------------
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
    TendaBackground = LoadTexture("resources/Tenda/Tenda.png");
    ItemRandom = LoadTexture("resources/Tenda/ItemRandom.png");
    VidaExtra = LoadTexture("resources/Tenda/VidaExtra.png");
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

    // Personaje comiendo en STATE_MAP: recortar zona y hacer negro→transparente
    // Zona del personaje dentro del mapa: x=1360-1503, y=622-831 (coordenadas imagen 1900x900)
    {
        const int CHAR_X1 = 1360, CHAR_Y1 = 622;
        const int CHAR_X2 = 1503, CHAR_Y2 = 831;
        Rectangle charCrop = { (float)CHAR_X1, (float)CHAR_Y1,
                                (float)(CHAR_X2 - CHAR_X1), (float)(CHAR_Y2 - CHAR_Y1) };
        const char* charPaths[2] = { "resources/Comiendo1.png", "resources/Comiendo2.png" };
        for (int i = 0; i < 2; i++) {
            Image img = LoadImage(charPaths[i]);
            ImageColorReplace(&img, BLACK, BLANK);
            ImageCrop(&img, charCrop);
            texMapChar[i] = LoadTextureFromImage(img);
            UnloadImage(img);
        }
    }

    fontBm = LoadFont("resources/alex-kidd-in-miracle-world-sms.otf");

    // ------------------- Audio -------------------
    titleMusic = LoadMusicStream("resources/TitleScreen.wav");
    titleMusic.looping = false;
    SetMusicVolume(titleMusic, 0.4f);
    gameMusic = LoadMusicStream("resources/MainTheme.wav");
    gameMusic.looping = true;
    SetMusicVolume(gameMusic, 0.3f);
    gameOverMusic = LoadMusicStream("resources/GameOver.wav");
    gameOverMusic.looping = false;
    SetMusicVolume(gameOverMusic, 0.6f);
    jumpSound = LoadSound("resources/Jump.wav");
    levelStartSound = LoadSound("resources/LevelStart.wav");
    // Calcular la duración real de LevelStart.wav para sincronizar la animación del mapa
    Wave lvlWave = LoadWave("resources/LevelStart.wav");
    const float MAP_ANIM_DURATION = (lvlWave.frameCount > 0 && lvlWave.sampleRate > 0) ? (float)lvlWave.frameCount / (float)lvlWave.sampleRate: 2.5f;
    UnloadWave(lvlWave);
    const float MAP_CHAR_FRAME_TIME = MAP_ANIM_DURATION / (3.0f * 2.0f);
    coinSound = LoadSound("resources/CoinCollection.wav");
    punchSound = LoadSound("resources/Punch.wav");
    coinBlockSound = LoadSound("resources/CoinBlock.wav");
    blockBreakSound = LoadSound("resources/BlockBreak.wav");
    lifeTakenSound = LoadSound("resources/LifeTaken.wav");
    SetSoundVolume(jumpSound, 0.5f);
    SetSoundVolume(levelStartSound, 0.6f);
    SetSoundVolume(coinSound, 0.7f);
    SetSoundVolume(punchSound, 0.6f);
    SetSoundVolume(coinBlockSound, 1.0f);
    SetSoundVolume(blockBreakSound, 1.0f);
    SetSoundVolume(lifeTakenSound, 0.8f);

    // Initialize tile definitions (must be after textures)
    InitTileDefs();

    // ------------------- Animation frames -------------------
    Rectangle frameRecR = { 0.0f, 0.0f, AlexKiddWalkR.width / 4.0f, (float)AlexKiddWalkR.height };
    Rectangle frameRecL = { 0.0f, 0.0f, AlexKiddWalkL.width / 4.0f, (float)AlexKiddWalkL.height };
    Rectangle frameRecJump = { 0.0f, 0.0f, (float)AlexKiddJumpR.width, (float)AlexKiddJumpR.height };
    Rectangle frameRecPuny = { 0.0f, 0.0f, (float)AlexKiddPunyR.width, (float)AlexKiddPunyR.height };
    Rectangle frameRecCrouchR = { 0.0f, 0.0f, (float)AlexKiddCrouchR.width, (float)AlexKiddCrouchR.height };
    Rectangle frameRecCrouchL = { 0.0f, 0.0f, (float)AlexKiddCrouchL.width, (float)AlexKiddCrouchL.height };
    Rectangle framePterodactil = { 0.0f, 0.0f, MonsterBirdR.width / 2.0f, (float)MonsterBirdR.height };
    Rectangle frameEscorpi = { 0.0f, 0.0f, EscorpiR.width / 2.0f, (float)EscorpiR.height };
    Rectangle frameCastanya = { 0.0f, 0.0f, CastanyaR.width / 2.0f, (float)CastanyaR.height };
    Rectangle frameRecDeath = { 0.0f, 0.0f, AlexKiddDeath.width / 3.0f, (float)AlexKiddDeath.height };
    int playerFrame = 0, playerCounter = 0, pteroFrame = 0, pteroCounter = 0;
    int framesSpeed = 8;
    int LeftOrRight = 0;
    bool attacking = false;
    int attackTimer = 0;

    int lavaFrame = 0;
    int lavaCounter = 0;
    int lavaFrameCount = 4;

    // ------------------- Player init -------------------
    Player player = { 0 };
    player.position = Vector2{ 550.0f, 200.0f };
    player.spawn = player.position;
    player.speedY = 0.0f; player.velX = 0.0f;
    player.canJump = false; player.isJumping = false;
    player.alive = true; player.deathAnim = false;
    player.lives = 300; player.coins = 0;
    player.inventoryCount = 0;
    player.ringActive = false;
    player.ringTimer = 0.0f;
    player.acc = PLAYER_ACC;
    player.friction = PLAYER_FRICTION;
    player.maxSpeed = PLAYER_MAX_SPEED;
    for (int i = 0; i < 5; i++) player.inventory[i] = ITEM_NONE;

    // ------------------- Enemies -------------------
    std::vector<enemic> pterodactilos, escorpins, castanyes;
    pterodactilos.reserve(10);
    pterodactilos.push_back({ true, 2, {600.0f, 1600.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {900.0f, 2160.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {600.0f, 2800.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {600.0f, 3040.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {800.0f, 3780.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {600.0f, 4100.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {800.0f, 4480.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {600.0f, 5210.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {600.0f, 6000.0f, 0.0f} });
    pterodactilos.push_back({ true, 2, {500.0f, 6580.0f, 0.0f} });
    escorpins.reserve(5);
    escorpins.push_back({ true, 2, {2000.0f, 1046.0f, 0.0f} });
    escorpins.push_back({ true, 2, {4000.0f, 970.0f, 0.0f} });
    escorpins.push_back({ true, 2, {6000.0f, 1080.0f, 0.0f} });
    escorpins.push_back({ true, 3, {8000.0f, 1080.0f, 0.0f} });
    escorpins.push_back({ true, 3, {10000.0f,1080.0f, 0.0f} });
    castanyes.reserve(3);
    castanyes.push_back({ true, 2, {3100.0f, 1000.0f, 0.0f} });
    castanyes.push_back({ true, 2, {5000.0f, 1080.0f, 0.0f} });
    castanyes.push_back({ true, 3, {9000.0f, 1080.0f, 0.0f} });

    const std::vector<enemic> originalPterodactilos = pterodactilos;
    const std::vector<enemic> originalEscorpins = escorpins;
    const std::vector<enemic> originalCastanyes = castanyes;

    Vector2 petPosition = player.position;

    // ------------------- Map level 1 (105 rows, 24 cols) -------------------
    int map1[105][24] = {
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
{3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3}
    };

    // ------------------- Map level 2 (20 rows, 24 cols) -------------------
    int map2[20][24] = {
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

    // ------------------- Map level shop (12 rows, 24 cols) -------------------
    int map3[12][24] = {
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,TILE_SHOP_VIDA,0,TILE_SHOP_ANILLO,0,TILE_SHOP_RANDOM,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,19,19,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,0,0,19,19,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3},
         {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3},
         {3,3,3,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,3,3,3,3}
    };

    // ------------------- Map level 4 (cave) (24 rows, 180 cols) -------------------
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
    {3,3,3,3,3,3,3,3,0,0,25,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,21,20,20,20/*Castaña*/,20,20,22,0,0,0,0,0,0,0,0,0,0,0,21,20,20,20,20,20,20,20,20,20,20,20,20,20,0/*Hueco lava*/,0,0,0,0,0,0,20,20,20,22,0,0,0,0,0,0,0,0,0,0,0,0,21,20,33,20,33,20,20,33,20,22,0,0,0,0,0,0,0,0,0,0/*Estrella rosa*/,29,29,29,0,21,20,20,20,20,20,20,0/*Calavera rosa*/,0,0,0,29,29,10,0,0,0,0/*Calavera rosa*/,0,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,25,3,3,3,3},
    {3,3,3,3,1,1,1,1,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,22,33,33,33,21,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,0/*Hueco lava*/,0,10,13,0,0,0,20,20,20,20,33,33,33,33,21,20,20,20,20,20,20,20,20,20,32,20,32,20,20,32,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,12/*Calavera rosa*/,20,20,20,20,20,20,20,20,20,12/*Calavera rosa*/,20,20,20,20,22,33,33,21,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,3,3,3,3},
    {3,3,3,3,1,1,1,1,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,32,32,32,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,33/*Hueco lava*/,21,20,20,20,22,33,20,20,20,20,32,32,32,32,20,20,20,20,20,20,20,20,20,20,32,20,32,20,20,32,20,20,20,20,20,20,20,20,20,20,20,20/*Estrella rosa*/,20,20,20,20,20,20,20,20,20,20,20,20/*Calavera rosa*/,20,20,20,20,20,20,20,20,20,20/*Calavera rosa*/,20,20,20,20,20,32,32,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    {3,3,3,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3},
    };
    // ------------------- Build environment -------------------
    std::vector<EnvItem> envItems;
    LoadMapLevel(currentLevel, envItems,
        (const int*)map1, (const int*)map2, (const int*)map3, (const int*)map4,
        105, 24, 20, 24, 12, 24, 24, 180);

    std::vector<WorldItem> worldItems;
    bool ringDropped = false;

    // ------------------- Camera -------------------
    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    void (*cameraUpdaters[])(Camera2D*, Player*, EnvItem*, int, float, int, int) = {
        UpdateCameraCenter, UpdateCameraCenterInsideMap, UpdateCameraCenterSmoothFollow,
        UpdateCameraEvenOutOnLanding, UpdateCameraPlayerBoundsPush,
        UpdateCameraDownOnly, UpdateCameraHorizontalOnly
    };
    int cameraOption = 4;

    SetTargetFPS(60);
    PlayMusicStream(titleMusic);

    //Resetear la animación cada vez que se entra en STATE_MAP
    auto ResetMapAnim = [&]() {
        mapRevealProgress = 0.0f;
        mapCharTimer = 0.0f;
        mapCharFrame = 0;
        levelStartTimer = 0.0f;
        };

    // ------------------- Main loop -------------------
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // ---- State transitions ----
        if (gameState == STATE_CREDITS) {
            creditsTimer += deltaTime;
            if (creditsTimer >= 3.0f) {
                StopMusicStream(gameMusic);
                PlaySound(levelStartSound);
                levelStarting = true;
                gameState = STATE_MAP;
                ResetMapAnim();
            }
        }

        if (gameState == STATE_MENU) {
            UpdateMusicStream(titleMusic);
            if (menuImageIndex < 6) {
                menuImageTimer += deltaTime;
                if (menuImageTimer >= 0.5f) {
                    menuImageTimer = 0.0f;
                    menuImageIndex++;
                }
            }
        }
        else if (gameState == STATE_PLAYING) {
            UpdateMusicStream(gameMusic);
        }
        if (gameOver) UpdateMusicStream(gameOverMusic);

        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
        if (gameState == STATE_MENU && IsKeyPressed(KEY_ENTER)) {
            StopMusicStream(titleMusic);
            gameState = STATE_CREDITS;
            creditsTimer = 0.0f;
        }

        // ---- Camera selection ----
        cameraOption = (currentLevel == 3) ? 6 : 5;

        // ---- Attack timer ----
        if (attacking) {
            attackTimer--;
            if (attackTimer <= 0) attacking = false;
        }

        // ---- Animation counters ----
        playerCounter++;
        if (IsKeyDown(KEY_D) && playerCounter >= (60 / framesSpeed)) {
            playerCounter = 0;
            playerFrame = (playerFrame + 1) % 4;
            frameRecR.x = playerFrame * (AlexKiddWalkR.width / 4.0f);
        }
        if (IsKeyDown(KEY_A) && playerCounter >= (60 / framesSpeed)) {
            playerCounter = 0;
            playerFrame = (playerFrame + 1) % 4;
            frameRecL.x = playerFrame * (AlexKiddWalkR.width / 4.0f);
        }
        pteroCounter++;
        if (pteroCounter >= (150 / framesSpeed)) {
            pteroCounter = 0;
            pteroFrame = (pteroFrame + 1) % 2;
            framePterodactil.x = pteroFrame * (MonsterBirdR.width / 2.0f);
            frameEscorpi.x = pteroFrame * (EscorpiR.width / 2.0f);
            frameCastanya.x = pteroFrame * (CastanyaR.width / 2.0f);
        }
        lavaCounter++;
        if (lavaCounter >= (60 / 6)) {
            lavaCounter = 0;
            lavaFrame = (lavaFrame + 1) % lavaFrameCount;
        }

        // ---- UPDATE (PLAYING) ----
        if (gameState == STATE_PLAYING) {
            // ---- Dialog update (freeze gameplay while active) ----
            if (activeDialog) {
                bool wasDone = activeDialog->IsDone();
                activeDialog->Update(deltaTime);
                if (activeDialog->IsDone()) {
                    if (!wasDone) dialogHoldTimer = 0.7f;
                    dialogHoldTimer -= deltaTime;
                    if (dialogHoldTimer <= 0.0f) {
                        delete activeDialog;
                        activeDialog = nullptr;
                        dialogFreeze = false;
                        dialogHoldTimer = 0.0f;
                        shopDialogCooldown = 1.5f;
                    }
                }
            }
            if (shopDialogCooldown > 0.0f) shopDialogCooldown -= deltaTime;

            if (!dialogFreeze)
                UpdatePlayer(&player, envItems.data(), envItems.size(), deltaTime);

            // Inventory input
            if (IsKeyPressed(KEY_P)) {
                mostrarInventari = !mostrarInventari;
                if (!mostrarInventari) inventariSeleccionat = 0;
            }
            if (mostrarInventari) {
                if (IsKeyPressed(KEY_D) && inventariSeleccionat < player.inventoryCount - 1) inventariSeleccionat++;
                if (IsKeyPressed(KEY_A) && inventariSeleccionat > 0) inventariSeleccionat--;
                if (IsKeyPressed(KEY_ENTER) && player.inventoryCount > 0) {
                    ItemType item = player.inventory[inventariSeleccionat];
                    if (item == ITEM_RING && !player.ringActive) {
                        player.ringActive = true;
                        player.ringTimer = RING_DURATION;
                        player.acc = 2000.0f;
                        player.friction = 1000.0f;
                        player.maxSpeed = 900.0f;
                        for (int i = inventariSeleccionat; i < player.inventoryCount - 1; i++)
                            player.inventory[i] = player.inventory[i + 1];
                        player.inventory[--player.inventoryCount] = ITEM_NONE;
                        if (inventariSeleccionat >= player.inventoryCount && inventariSeleccionat > 0)
                            inventariSeleccionat--;
                        mostrarInventari = false;
                    }
                }
            }
            else {
                // Pick up world items (rings) – usar la misma hitbox
                Rectangle playerRect = GetPlayerBodyRect(&player);
                for (auto& wi : worldItems) {
                    if (!wi.active) continue;
                    wi.bobTimer += deltaTime;
                    Rectangle itemRect = { wi.position.x - 20.0f, wi.position.y - 20.0f, 40.0f, 40.0f };
                    if (CheckCollisionRecs(playerRect, itemRect) && player.inventoryCount < 5) {
                        player.inventory[player.inventoryCount++] = wi.type;
                        wi.active = false;
                        PlaySound(coinSound);
                    }
                }
            }

            // Pet logic (MIAU)
            static float teleportCooldown = 0.0f;
            if (teleportCooldown > 0.0f) teleportCooldown -= deltaTime;
            float side = (player.velX >= 0.0f) ? -80.0f : 80.0f;
            Vector2 targetPos = Vector2Add(player.position, Vector2{ side, -30.0f });
            float dist = Vector2Length(Vector2Subtract(player.position, petPosition));
            if (dist > 1000.0f && teleportCooldown <= 0.0f && player.alive) {
                petPosition = targetPos;
                teleportCooldown = 1.2f;
            }
            else {
                Vector2 dir = Vector2Subtract(targetPos, petPosition);
                Vector2 vel = { dir.x * 5.0f * deltaTime, dir.y * 5.0f * deltaTime };
                Rectangle petRect = { petPosition.x - 20.0f, petPosition.y - 20.0f, 40.0f, 40.0f };
                petRect.x += vel.x;
                for (auto& ei : envItems)
                    if (ei.blocking && ei.active && CheckCollisionRecs(petRect, ei.rect))
                        petRect.x = (vel.x > 0.0f) ? ei.rect.x - petRect.width : ei.rect.x + ei.rect.width;
                petRect.y += vel.y;
                for (auto& ei : envItems)
                    if (ei.blocking && ei.active && CheckCollisionRecs(petRect, ei.rect))
                        petRect.y = (vel.y > 0.0f) ? ei.rect.y - petRect.height : ei.rect.y + ei.rect.height;
                petPosition = { petRect.x + 20.0f, petRect.y + 20.0f };
            }

            // Ring timer
            if (player.ringActive) {
                player.ringTimer -= deltaTime;
                if (player.ringTimer <= 0.0f) {
                    player.ringActive = false;
                    player.acc = PLAYER_ACC;
                    player.friction = PLAYER_FRICTION;
                    player.maxSpeed = PLAYER_MAX_SPEED;
                }
            }

            // Death & respawn
            if (!player.alive && !player.deathAnim && !gameOver) {
                player.deathAnim = true;
                player.deathY = player.position.y - 80.0f;
                player.deathX = player.position.x - 40.0f;
                player.deathFrame = 0;
                player.deathFrameCounter = 0;
                player.lives--;
                StopMusicStream(gameMusic);
                if (player.lives > 0) PlaySound(lifeTakenSound);
                else { gameOver = true; PlayMusicStream(gameOverMusic); }
            }
            if (player.deathAnim) {
                player.deathY -= 300.0f * deltaTime;
                if (++player.deathFrameCounter >= 8) {
                    player.deathFrameCounter = 0;
                    if (++player.deathFrame > 2) player.deathFrame = 2;
                }
                float topY = camera.target.y - screenHeight / camera.zoom;
                if (player.deathY < topY && !gameOver) {
                    player.deathAnim = false;
                    player.alive = true;
                    player.speedY = 0.0f; player.velX = 0.0f;
                    PlayMusicStream(gameMusic);
                    Vector2 spawnPos = { camera.target.x + 100.0f, camera.target.y - 250.0f };
                    bool safe = false;
                    for (int attempt = 0; attempt < 5 && !safe; attempt++) {
                        spawnPos.x = camera.target.x + 100.0f + attempt * 120.0f;
                        spawnPos.y = camera.target.y - 250.0f;
                        for (float y = spawnPos.y; y < spawnPos.y + 600.0f; y += 40.0f) {
                            Rectangle test = { spawnPos.x - 20.0f, y - 80.0f, 40.0f, 160.0f };
                            bool coll = false;
                            for (auto& ei : envItems)
                                if (ei.active && ei.blocking && CheckCollisionRecs(test, ei.rect)) { coll = true; break; }
                            if (!coll) {
                                Rectangle feet = { spawnPos.x - 20.0f, y + 75.0f, 40.0f, 20.0f };
                                bool lavaFloor = false;
                                for (auto& ei : envItems) {
                                    if (ei.active && ei.tileID == TILE_LAVA && CheckCollisionRecs(feet, ei.rect)) {
                                        lavaFloor = true;
                                        break;
                                    }
                                }
                                if (!lavaFloor) {
                                    for (auto& ei : envItems)
                                        if (ei.active && ei.blocking && CheckCollisionRecs(feet, ei.rect)) { safe = true; break; }
                                    if (safe) { spawnPos.y = y; break; }
                                }
                            }
                        }
                    }
                    player.position = spawnPos;
                }
                else if (player.deathY < topY && gameOver) player.deathAnim = false;
            }

            // Move enemies
            for (auto& p : pterodactilos) PterodactilMoviment(&p, envItems.data(), envItems.size(), deltaTime);
            if (currentLevel == 3) {
                for (auto& e : escorpins) EnemyGroundMoviment(&e, envItems.data(), envItems.size(), deltaTime);
                for (auto& c : castanyes) EnemyGroundMoviment(&c, envItems.data(), envItems.size(), deltaTime);
            }

            // Camera update
            cameraUpdaters[cameraOption](&camera, &player, envItems.data(), envItems.size(), deltaTime, screenWidth, screenHeight);
            if (currentLevel == LEVEL_SHOP) {
                camera.target.y = 580.0f;
                if (!shopWelcomeDone && !activeDialog) {
                    shopWelcomeDone = true;
                    delete activeDialog;
                    activeDialog = new DialogBox(
                        { screenWidth / 2 - 500, 180, 840, 110 },
                        { "Welcome! Please buy", "the things that you like." },
                        fontBm
                    );
                    dialogFreeze = true;
                }
            }

            // ---- Collisions with special tiles (including shop) ----
            Rectangle playerRect = GetPlayerBodyRect(&player);
            for (auto& ei : envItems) {
                if (!ei.active) continue;

                // Level complete (door)
                if (ei.tileID == TILE_PORTA && CheckCollisionRecs(playerRect, ei.rect) && currentLevel != LEVEL_SHOP) {
                    currentLevel++;
                    if (currentLevel > 3) {
                        ResetGameToMenu(player, camera, currentLevel, gameOver, gameState, menuImageIndex,
                            envItems, pterodactilos, escorpins, castanyes,
                            originalPterodactilos, originalEscorpins, originalCastanyes,
                            (const int*)map1, 105, 24, purchasedShopItems);
                        worldItems.clear();
                        ringDropped = false;
                        continue;
                    }
                    StopMusicStream(gameMusic);
                    PlaySound(levelStartSound);
                    gameState = STATE_MAP;
                    ResetMapAnim();
                    player.alive = true; player.speedY = 0.0f; player.velX = 0.0f; player.deathAnim = false;
                    if (currentLevel == 2) player.position = Vector2{ 550.0f, 200.0f };
                    else if (currentLevel == 3) player.position = Vector2{ 1000.0f, 1000.0f };
                    camera.target = player.position;
                    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
                    LoadMapLevel(currentLevel, envItems,
                        (const int*)map1, (const int*)map2, (const int*)map3, (const int*)map4,
                        105, 24, 20, 24, 12, 24, 24, 180);

                    // Al cargar la tienda, desactivar objetos ya comprados
                    if (currentLevel == LEVEL_SHOP) {
                        for (auto& item : envItems) {
                            if (item.tileID == TILE_SHOP_VIDA || item.tileID == TILE_SHOP_ANILLO || item.tileID == TILE_SHOP_RANDOM) {
                                Vector2 grid = { item.rect.x / TILE_SIZE, item.rect.y / TILE_SIZE };
                                if (std::find(purchasedShopItems.begin(), purchasedShopItems.end(), grid) != purchasedShopItems.end()) {
                                    item.active = false;
                                }
                            }
                        }
                    }
                    pterodactilos = originalPterodactilos;
                    escorpins = originalEscorpins;
                    castanyes = originalCastanyes;
                    worldItems.clear();
                    ringDropped = false;
                    player.ringActive = false;
                }

                // Shop enter
                if (ei.tileID == TILE_SHOP_ENTER && CheckCollisionRecs(playerRect, ei.rect) && IsKeyPressed(KEY_W) && currentLevel != LEVEL_SHOP) {
                    previousLevel = currentLevel;
                    currentLevel = LEVEL_SHOP;
                    StopMusicStream(gameMusic);
                    PlaySound(levelStartSound);
                    gameState = STATE_MAP;
                    ResetMapAnim();
                    player.alive = true; player.speedY = 0.0f; player.velX = 0.0f; player.deathAnim = false;
                    player.position = Vector2{ 360.0f, 800.0f };
                    camera.target = Vector2{ 360.0f, 800.0f };
                    LoadMapLevel(LEVEL_SHOP, envItems,
                        (const int*)map1, (const int*)map2, (const int*)map3, (const int*)map4,
                        105, 24, 20, 24, 12, 24, 24, 180);
                    for (auto& item : envItems) {
                        if (item.tileID == TILE_SHOP_VIDA || item.tileID == TILE_SHOP_ANILLO || item.tileID == TILE_SHOP_RANDOM) {
                            Vector2 grid = { item.rect.x / TILE_SIZE, item.rect.y / TILE_SIZE };
                            if (std::find(purchasedShopItems.begin(), purchasedShopItems.end(), grid) != purchasedShopItems.end()) {
                                item.active = false;
                            }
                        }
                    }
                    pterodactilos.clear();
                    escorpins.clear();
                    castanyes.clear();
                    worldItems.clear();
                    ringDropped = false;
                    shopWelcomeDone = false;
                }

                // Shop exit
                if (ei.tileID == TILE_SHOP_EXIT && CheckCollisionRecs(playerRect, ei.rect) && IsKeyPressed(KEY_W) && currentLevel == LEVEL_SHOP) {
                    currentLevel = previousLevel;
                    StopMusicStream(gameMusic);
                    PlaySound(levelStartSound);
                    gameState = STATE_MAP;
                    ResetMapAnim();
                    player.alive = true; player.speedY = 0.0f; player.velX = 0.0f; player.deathAnim = false;
                    if (currentLevel == 2) player.position = Vector2{ 1000.0f, 400.0f };
                    else player.position = Vector2{ 550.0f, 200.0f };
                    camera.target = player.position;
                    LoadMapLevel(currentLevel, envItems,
                        (const int*)map1, (const int*)map2, (const int*)map3, (const int*)map4,
                        105, 24, 20, 24, 12, 24, 24, 180);
                    pterodactilos = originalPterodactilos;
                    escorpins = originalEscorpins;
                    castanyes = originalCastanyes;
                    worldItems.clear();
                    ringDropped = false;
                }

                // Collectible bags (money)
                if (ei.collectible && CheckCollisionRecs(playerRect, ei.rect)) {
                    PlaySound(coinSound);
                    if (ei.texture.id == blockBossaCollons.id) player.coins += 100;
                    else if (ei.texture.id == blockBossaCollonsPetit.id) player.coins += 50;
                    ei.active = false;
                }

                // ---------- SHOP ITEMS: compra con mensajes ----------
                if (dialogFreeze || shopDialogCooldown > 0.0f) continue;

                if (ei.tileID == TILE_SHOP_VIDA && CheckCollisionRecs(playerRect, ei.rect)) {
                    if (player.coins >= 100) {
                        player.coins -= 100;
                        player.lives++;
                        PlaySound(coinBlockSound);
                        ei.active = false;
                        purchasedShopItems.push_back({ ei.rect.x / TILE_SIZE, ei.rect.y / TILE_SIZE });
                        delete activeDialog;
                        activeDialog = new DialogBox(
                            { screenWidth / 2 - 500, 180, 360, 70 },
                            { "Thank you." },
                            fontBm
                        );
                        dialogFreeze = true;
                    }
                    else {
                        delete activeDialog;
                        activeDialog = new DialogBox(
                            { screenWidth / 2 - 500, 180, 580, 70 },
                            { "Not enough coins." },
                            fontBm
                        );
                        dialogFreeze = true;
                    }
                }
                else if (ei.tileID == TILE_SHOP_ANILLO && CheckCollisionRecs(playerRect, ei.rect)) {
                    if (player.coins >= 200 && player.inventoryCount < 5) {
                        player.coins -= 200;
                        player.inventory[player.inventoryCount++] = ITEM_RING;
                        PlaySound(coinSound);
                        ei.active = false;
                        purchasedShopItems.push_back({ ei.rect.x / TILE_SIZE, ei.rect.y / TILE_SIZE });
                        delete activeDialog;
                        activeDialog = new DialogBox(
                            { screenWidth / 2 - 500, 180, 360, 70 },
                            { "Thank you." },
                            fontBm
                        );
                        dialogFreeze = true;
                    }
                    else {
                        delete activeDialog;
                        activeDialog = new DialogBox(
                            { screenWidth / 2 - 500, 180, 580, 70 },
                            { "Not enough coins." },
                            fontBm
                        );
                        dialogFreeze = true;
                    }
                }
                else if (ei.tileID == TILE_SHOP_RANDOM && CheckCollisionRecs(playerRect, ei.rect)) {
                    if (player.coins >= 150) {
                        player.coins -= 150;
                        int r = GetRandomValue(0, 1);
                        if (r == 0) {
                            player.lives++;
                            PlaySound(coinBlockSound);
                        }
                        else {
                            if (player.inventoryCount < 5) {
                                player.inventory[player.inventoryCount++] = ITEM_RING;
                                PlaySound(coinSound);
                            }
                            else {
                                player.lives++;
                                PlaySound(coinBlockSound);
                            }
                        }
                        ei.active = false;
                        purchasedShopItems.push_back({ ei.rect.x / TILE_SIZE, ei.rect.y / TILE_SIZE });
                        delete activeDialog;
                        activeDialog = new DialogBox(
                            { screenWidth / 2 - 500, 180, 360, 70 },
                            { "Thank you." },
                            fontBm
                        );
                        dialogFreeze = true;
                    }
                    else {
                        delete activeDialog;
                        activeDialog = new DialogBox(
                            { screenWidth / 2 - 500, 180, 580, 70 },
                            { "Not enough coins." },
                            fontBm
                        );
                        dialogFreeze = true;
                    }
                }
            }

            // Enemies hit player – usando GetPlayerBodyRect
            auto hitByEnemy = [&](enemic* e) {
                if (!player.alive || !e->vida) return;
                Rectangle pRect = GetPlayerBodyRect(&player);
                Rectangle eRect = { e->posicio.x, e->posicio.y, 80.0f, 40.0f };
                if (CheckCollisionRecs(pRect, eRect) && !player.ringActive) player.alive = false;
                };
            for (auto& p : pterodactilos) hitByEnemy(&p);
            if (currentLevel == 3) {
                for (auto& e : escorpins) hitByEnemy(&e);
                for (auto& c : castanyes) hitByEnemy(&c);
            }

            // Lava kills player
            if (player.alive && !player.ringActive) {
                Rectangle lavaPlayerRect = GetPlayerBodyRect(&player);
                for (auto& ei : envItems) {
                    if (!ei.active || ei.tileID != TILE_LAVA) continue;
                    if (CheckCollisionRecs(lavaPlayerRect, ei.rect)) {
                        player.alive = false;
                        break;
                    }
                }
            }
        } // fin STATE_PLAYING

        // ---- DRAW ----
        BeginDrawing();
        if (gameState == STATE_CREDITS) {
            ClearBackground(BLACK);
            DrawTextEx(fontBm, "Alex Kidd In Miracle World", Vector2{ screenWidth / 2 - 800, 100 }, 60, 2, WHITE);
            DrawTextEx(fontBm, "Alpha", Vector2{ screenWidth / 2 - 200, 200 }, 60, 2, WHITE);

            DrawTextEx(fontBm, "Los Macacos", Vector2{ screenWidth / 2 - 740, 340 }, 40, 2, WHITE);
            DrawTextEx(fontBm, "Alan del Tio", Vector2{ screenWidth / 2 - 710, 420 }, 30, 2, WHITE);
            DrawTextEx(fontBm, "Ian Leon", Vector2{ screenWidth / 2 - 715, 460 }, 30, 2, WHITE);
            DrawTextEx(fontBm, "Yarley Tituana", Vector2{ screenWidth / 2 - 715, 500 }, 30, 2, WHITE);
            DrawTextEx(fontBm, "Lluc Torner", Vector2{ screenWidth / 2 - 707, 540 }, 30, 2, WHITE);

            DrawTextEx(fontBm, "Tutor", Vector2{ screenWidth / 2 + 220, 410 }, 40, 2, WHITE);
            DrawTextEx(fontBm, "Alejandro Paris Gomez", Vector2{ screenWidth / 2 + 5, 480 }, 30, 2, WHITE);

            DrawTextEx(fontBm, "Projecte I", Vector2{ screenWidth / 2 - 245, 700 }, 40, 2, WHITE);
            DrawTextEx(fontBm, "CITM - Disseny i Desenvolupament de Videojocs", Vector2{ screenWidth / 2 - 900, 750 }, 40, 0, WHITE);
        }
        else if (currentLevel == 3) ClearBackground(Color{ 85,0,0 });
        else ClearBackground(BLAU);

        if (gameState == STATE_PLAYING) {
            BeginMode2D(camera);
            if (currentLevel == LEVEL_SHOP) {
                float ix = 4.0f * TILE_SIZE, iy = 0.0f, iw = 16.0f * TILE_SIZE, ih = 12.0f * TILE_SIZE;
                DrawTexturePro(TendaBackground,
                    Rectangle{ 0.0f, 0.0f, (float)TendaBackground.width, (float)TendaBackground.height },
                    Rectangle{ ix, iy, iw, ih },
                    Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
            else {
                DrawTextureEx(nuvol, Vector2{ 200.0f, -100.0f }, 0.0f, 0.2f, WHITE);
                DrawTextureEx(nuvol, Vector2{ 700.0f, 150.0f }, 0.0f, 0.2f, WHITE);
                DrawTextureEx(nuvol, Vector2{ 950.0f, 0.0f }, 0.0f, 0.2f, WHITE);
                if (currentLevel == 2) {
                    DrawTexturePro(TendaFora,
                        Rectangle{ 0.0f, 0.0f, (float)TendaFora.width, (float)TendaFora.height },
                        Rectangle{ 1000.0f, 450.0f, 400.0f, 350.0f },
                        Vector2{ 300.0f, 150.0f }, 0.0f, WHITE);
                }
            }
            DrawTextureEx(MIAU, Vector2{ petPosition.x - 40.0f, petPosition.y - 85.0f }, 0.0f, 1.0f, WHITE);
            for (auto& ei : envItems) {
                if (!ei.active || ei.tileID == TILE_WARP) continue;
                DrawTexturePro(ei.texture,
                    Rectangle{ 0.0f, 0.0f, (float)ei.texture.width, (float)ei.texture.height },
                    ei.rect, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
            for (auto& wi : worldItems) {
                if (!wi.active) continue;
                float bob = sinf(wi.bobTimer * 5.0f) * 5.0f;
                DrawTexturePro(PowerBracelet,
                    Rectangle{ 0.0f, 0.0f, (float)PowerBracelet.width, (float)PowerBracelet.height },
                    Rectangle{ wi.position.x - 40.0f, wi.position.y - 40.0f + bob, 80.0f, 80.0f },
                    Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
            for (auto& ei : envItems) {
                if (!ei.active || ei.tileID == TILE_WARP) continue;
                if (ei.tileID == TILE_LAVA) {
                    float frameW = (float)Lava.width / lavaFrameCount;
                    Rectangle src = { lavaFrame * frameW, 0.0f, frameW, (float)Lava.height };
                    DrawTexturePro(Lava, src, ei.rect, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
                }
                else {
                    DrawTexturePro(ei.texture,
                        Rectangle{ 0.0f, 0.0f, (float)ei.texture.width, (float)ei.texture.height },
                        ei.rect, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
                }
            }
            // Etiquetas de precio sobre los ítems de la tienda
            if (currentLevel == LEVEL_SHOP) {
                for (auto& ei : envItems) {
                    if (!ei.active) continue;
                    const char* label = nullptr;
                    if (ei.tileID == TILE_SHOP_VIDA)   label = "100";
                    else if (ei.tileID == TILE_SHOP_ANILLO) label = "200";
                    else if (ei.tileID == TILE_SHOP_RANDOM) label = "150";
                    if (!label) continue;
                    Vector2 textSize = MeasureTextEx(fontBm, label, 20.0f, 2.0f);
                    float tx = ei.rect.x + (ei.rect.width - textSize.x) / 2.0f;
                    float ty = ei.rect.y - 20.0f - textSize.y;
                    DrawTextEx(fontBm, label, { tx, ty }, 20.0f, 2.0f, YELLOW);
                }
            }

            for (auto& p : pterodactilos) {
                if (!p.vida) continue;
                Texture2D tex = (p.velocitat > 0) ? MonsterBirdR : MonsterBirdL;
                DrawTextureRec(tex, framePterodactil, Vector2{ p.posicio.x, p.posicio.y }, WHITE);
            }
            if (currentLevel == 3) {
                for (auto& e : escorpins) {
                    if (!e.vida) continue;
                    Texture2D tex = (e.velocitat > 0) ? EscorpiR : EscorpiL;
                    DrawTextureRec(tex, frameEscorpi, Vector2{ e.posicio.x, e.posicio.y }, WHITE);
                }
                for (auto& c : castanyes) {
                    if (!c.vida) continue;
                    Texture2D tex = (c.velocitat > 0) ? CastanyaR : CastanyaL;
                    DrawTextureRec(tex, frameCastanya, Vector2{ c.posicio.x, c.posicio.y }, WHITE);
                }
            }
            if (player.deathAnim) {
                Rectangle drec = { player.deathFrame * (AlexKiddDeath.width / 3.0f), 0.0f,
                                   AlexKiddDeath.width / 3.0f, (float)AlexKiddDeath.height };
                DrawTextureRec(AlexKiddDeath, drec, Vector2{ player.deathX, player.deathY }, WHITE);
            }

            if (player.alive) {
                if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D)) LeftOrRight = 0;
                else if (IsKeyPressed(KEY_A) || IsKeyDown(KEY_A)) LeftOrRight = 1;
                Vector2 pos;
                if (IsKeyDown(KEY_S) && player.canJump)
                    pos = { player.position.x - 40.0f, player.position.y - 98.0f };  // Ajusta según la altura real de la textura de crouch
                else
                    pos = { player.position.x - 40.0f, player.position.y - 129.0f };
                if (attacking) {
                    if (LeftOrRight == 0) DrawTextureRec(AlexKiddPunyR, frameRecPuny, pos, WHITE);
                    else DrawTextureRec(AlexKiddPunyL, frameRecPuny, Vector2{ player.position.x - 75.0f, player.position.y - 129.0f }, WHITE);
                }
                else if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A) && player.canJump && !IsKeyDown(KEY_S)) {
                    if (LeftOrRight == 0) DrawTextureRec(AlexKiddIdleR, frameRecR, pos, WHITE);
                    else DrawTextureRec(AlexKiddIdleL, frameRecL, pos, WHITE);
                }
                else if (IsKeyDown(KEY_D) && player.canJump && !IsKeyDown(KEY_S)) {
                    DrawTextureRec(AlexKiddWalkR, frameRecR, pos, WHITE);
                }
                else if (IsKeyDown(KEY_A) && player.canJump && !IsKeyDown(KEY_S)) {
                    DrawTextureRec(AlexKiddWalkL, frameRecL, pos, WHITE);
                }
                else if (!player.canJump) {
                    if (LeftOrRight == 0) DrawTextureRec(AlexKiddJumpR, frameRecJump, pos, WHITE);
                    else DrawTextureRec(AlexKiddJumpL, frameRecJump, pos, WHITE);
                }
                else if (IsKeyDown(KEY_S) && player.canJump) {
                    if (LeftOrRight == 0) DrawTextureRec(AlexKiddCrouchR, frameRecCrouchR, pos, WHITE);
                    else DrawTextureRec(AlexKiddCrouchL, frameRecCrouchL, pos, WHITE);
                }
                if (IsKeyPressed(KEY_ENTER) && !attacking && !mostrarInventari) {
                    attacking = true;
                    attackTimer = 20;
                    PlaySound(punchSound);
                    PlayerBreakBlock(&player, envItems.data(), envItems.size(), LeftOrRight, worldItems, ringDropped);
                    for (auto& p : pterodactilos) PlayerAttackEnemy(&player, &p, LeftOrRight);
                    if (currentLevel == 3) {
                        for (auto& e : escorpins) PlayerAttackEnemy(&player, &e, LeftOrRight);
                        for (auto& c : castanyes) PlayerAttackEnemy(&player, &c, LeftOrRight);
                    }
                }
            }
            EndMode2D();

            // HUD
            for (int i = 0; i < player.lives; i++)
                DrawTextureEx(AlexKiddIdleR, Vector2{ 20.0f + i * 45.0f, 60.0f }, 0.0f, 0.35f, WHITE);
            DrawTextEx(fontBm, TextFormat("Coins: %d", player.coins), Vector2{ 20, 20 }, 20, 0, YELLOW);
            if (player.ringActive) {
                DrawTextEx(fontBm, TextFormat("RING ACTIVE (%.1f)", player.ringTimer), Vector2{ screenWidth / 2 - 130, 20 }, 20, 0, GOLD);
            }
            if (player.inventoryCount > 0) {
                DrawTextEx(fontBm, "P = Inventari", Vector2{ screenWidth - 220, 20 }, 14, 0, WHITE);
                for (int i = 0; i < player.inventoryCount; i++) {
                    if (player.inventory[i] == ITEM_RING) {
                        DrawTexturePro(PowerBracelet,
                            Rectangle{ 0.0f, 0.0f, (float)PowerBracelet.width, (float)PowerBracelet.height },
                            Rectangle{ (float)(screenWidth - 220 + i * 50), 50.0f, 40.0f, 40.0f },
                            Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
                    }
                }
            }
            // Diálogo animado de la tienda
            if (activeDialog) {
                activeDialog->Draw();
            }
            if (gameOver && !player.deathAnim) {
                DrawTexturePro(GameOver,
                    Rectangle{ 0.0f, 0.0f, (float)GameOver.width, (float)GameOver.height },
                    Rectangle{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
                if (IsKeyPressed(KEY_R)) {
                    ResetGameToMenu(player, camera, currentLevel, gameOver, gameState, menuImageIndex,
                        envItems, pterodactilos, escorpins, castanyes,
                        originalPterodactilos, originalEscorpins, originalCastanyes,
                        (const int*)map1, 105, 24, purchasedShopItems);
                    worldItems.clear();
                    ringDropped = false;
                }
            }
        }
        else if (gameState == STATE_MAP) {
            levelStartTimer += deltaTime;

            // ---- Animación del mapa ----
            if (mapRevealProgress < 1.0f) {
                mapRevealProgress = levelStartTimer / MAP_ANIM_DURATION;
                if (mapRevealProgress > 1.0f) mapRevealProgress = 1.0f;
            }
            mapCharTimer += deltaTime;
            if (mapCharTimer >= MAP_CHAR_FRAME_TIME) {
                mapCharTimer -= MAP_CHAR_FRAME_TIME;
                mapCharFrame = (mapCharFrame + 1) % 2;
            }

            // ---- Draw animación del mapa ----
            ClearBackground(BLACK);
            float scaleX = (float)screenWidth / 1900.0f;
            float scaleY = (float)screenHeight / 900.0f;

            //Parte del fondo ya revelada (de izquierda a derecha)
            float revealedW = MAP_REVEAL_WIDTH * mapRevealProgress;
            if (revealedW > 0.0f) {
                // Dibujar la franja revelada del mapa (todo el alto, solo la columna revelada)
                // También dibujamos la parte izquierda del mapa (antes del inicio del reveal)
                Rectangle mapSrc = { MAP_REVEAL_X_START, 0.0f, revealedW, 900.0f };
                Rectangle mapDst = { MAP_REVEAL_X_START * scaleX, 0.0f,
                                     revealedW * scaleX, (float)screenHeight };
                DrawTexturePro(mapImage, mapSrc, mapDst, { 0.0f, 0.0f }, 0.0f, WHITE);
            }

            //Zona estática del mapa a la izquierda del reveal
            {
                Rectangle mapSrc = { 0.0f, 0.0f, MAP_REVEAL_X_START, 900.0f };
                Rectangle mapDst = { 0.0f, 0.0f, MAP_REVEAL_X_START * scaleX, (float)screenHeight };
                DrawTexturePro(mapImage, mapSrc, mapDst, { 0.0f, 0.0f }, 0.0f, WHITE);
            }

            //Personaje comiendo
            {
                float charW = (float)texMapChar[0].width * scaleX;
                float charH = (float)texMapChar[0].height * scaleY;
                DrawTexturePro(texMapChar[mapCharFrame],
                    Rectangle{ 0.0f, 0.0f,
                                (float)texMapChar[mapCharFrame].width,
                                (float)texMapChar[mapCharFrame].height },
                    Rectangle{ MAP_CHAR_POS.x, MAP_CHAR_POS.y, charW, charH },
                    Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }

            // ---- Transición a STATE_PLAYING cuando termina el sonido ----
            if (!IsSoundPlaying(levelStartSound)) {
                PlayMusicStream(gameMusic);
                gameState = STATE_PLAYING;
            }
        }
        else if (gameState == STATE_MENU) {
            DrawTexturePro(MenuFondo,
                Rectangle{ 0.0f, 0.0f, (float)MenuFondo.width, (float)MenuFondo.height },
                Rectangle{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            DrawTexturePro(MenuTítol,
                Rectangle{ 0.0f, 0.0f, (float)MenuTítol.width, (float)MenuTítol.height },
                Rectangle{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            Texture2D menuImgs[] = { Menu1,Menu2,Menu3,Menu4,Menu5,Menu6 };
            for (int i = 0; i < menuImageIndex; i++)
                DrawTexturePro(menuImgs[i],
                    Rectangle{ 0.0f, 0.0f, (float)menuImgs[i].width, (float)menuImgs[i].height },
                    Rectangle{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                    Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        if (mostrarInventari) {
            DrawTexturePro(Inventari,
                Rectangle{ 0.0f, 0.0f, (float)Inventari.width, (float)Inventari.height },
                Rectangle{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            float slotSize = 80.0f;
            float startX = screenWidth / 2.0f - (5.0f * slotSize + 40.0f) / 2.0f + 250.0f;
            float slotY = screenHeight / 2.0f - slotSize / 2.0f + 300.0f;
            for (int i = 0; i < 5; i++) {
                float x = startX + i * (slotSize + 10.0f);
                Color col = (i == inventariSeleccionat) ? YELLOW : WHITE;
                DrawRectangleLinesEx(Rectangle{ x - 3.0f, slotY - 3.0f, slotSize + 6.0f, slotSize + 6.0f }, 3.0f, col);
                if (i < player.inventoryCount && player.inventory[i] == ITEM_RING) {
                    DrawTexturePro(PowerBracelet,
                        Rectangle{ 0.0f, 0.0f, (float)PowerBracelet.width, (float)PowerBracelet.height },
                        Rectangle{ x, slotY, slotSize, slotSize },
                        Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
                }
            }
            DrawTextEx(fontBm, "A/D: seleccionar   ENTER: usar   P: tancar", Vector2{ screenWidth / 2 - 550, (float)slotY - 50 }, 14, 0, WHITE);
            if (player.inventoryCount > 0 && inventariSeleccionat < player.inventoryCount &&
                player.inventory[inventariSeleccionat] == ITEM_RING)
                DrawTextEx(fontBm, "Anell del Poder", Vector2{ screenWidth / 2 + 135 , (float)slotY - 40 }, 16, 0, GOLD);
        }
        EndDrawing();
    }

    // Cleanup dialog if still active
    delete activeDialog;
    activeDialog = nullptr;

    // Unload everything
    UnloadTexture(background); UnloadTexture(nuvol);
    UnloadTexture(AlexKiddIdleR); UnloadTexture(AlexKiddIdleL);
    UnloadTexture(AlexKiddWalkR); UnloadTexture(AlexKiddWalkL);
    UnloadTexture(AlexKiddJumpR); UnloadTexture(AlexKiddJumpL);
    UnloadTexture(AlexKiddPunyR); UnloadTexture(AlexKiddPunyL);
    UnloadTexture(AlexKiddCrouchR); UnloadTexture(AlexKiddCrouchL);
    UnloadTexture(AlexKiddDeath);
    UnloadTexture(MonsterBirdR); UnloadTexture(MonsterBirdL);
    UnloadTexture(EscorpiR); UnloadTexture(EscorpiL);
    UnloadTexture(CastanyaR); UnloadTexture(CastanyaL);
    UnloadTexture(Lava);
    UnloadTexture(blockSolidTerra); UnloadTexture(blockBreak);
    UnloadTexture(blockTerraR); UnloadTexture(blockTerraL);
    UnloadTexture(blockHerbaR); UnloadTexture(blockHerbaL);
    UnloadTexture(blockSolidHerba); UnloadTexture(blockInterrogant);
    UnloadTexture(blockEstrella); UnloadTexture(blockPorta);
    UnloadTexture(blockCalaveraGroc); UnloadTexture(blockCalaveraRosa);
    UnloadTexture(blockBossaCollons); UnloadTexture(blockBossaCollonsPetit);
    UnloadTexture(terraCova); UnloadTexture(terraCovaL); UnloadTexture(terraCovaR);
    UnloadTexture(triangleCovaL); UnloadTexture(triangleCovaR);
    UnloadTexture(bloqueSolidoCueva1); UnloadTexture(bloqueSolidoCueva2);
    UnloadTexture(estalagmitaL); UnloadTexture(estalagmitaR);
    UnloadTexture(bloqueRompibleCueva); UnloadTexture(puntitaL); UnloadTexture(puntitaR);
    UnloadTexture(bloqueLava);
    UnloadTexture(MIAU); UnloadTexture(negro); UnloadTexture(pedra);
    UnloadTexture(TendaFora); UnloadTexture(TendaBackground);
    UnloadTexture(mapImage);
    UnloadTexture(MenuFondo); UnloadTexture(MenuTítol);
    UnloadTexture(Menu1); UnloadTexture(Menu2); UnloadTexture(Menu3);
    UnloadTexture(Menu4); UnloadTexture(Menu5); UnloadTexture(Menu6);
    UnloadTexture(Inventari); UnloadTexture(PowerBracelet); UnloadTexture(GameOver);
    UnloadTexture(texMapChar[0]); UnloadTexture(texMapChar[1]);
    UnloadSound(jumpSound); UnloadSound(levelStartSound);
    UnloadSound(coinSound); UnloadSound(punchSound);
    UnloadSound(coinBlockSound); UnloadSound(blockBreakSound); UnloadSound(lifeTakenSound);
    UnloadMusicStream(titleMusic); UnloadMusicStream(gameMusic); UnloadMusicStream(gameOverMusic);
    UnloadFont(fontBm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

// ---------------------------------------------------------------------
// Function implementations
// ---------------------------------------------------------------------
void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta) {
    if (!player->alive) return;
    Rectangle playerRect = GetPlayerBodyRect(player);   // <-- USA LA NUEVA FUNCIÓN
    if (IsKeyPressed(KEY_SPACE) && player->canJump && !IsKeyDown(KEY_S)) {
        player->speedY = -PLAYER_JUMP_SPD;
        PlaySound(jumpSound);
        player->canJump = false;
        player->isJumping = true;
        player->jumpTime = 0.0f;
    }
    if (IsKeyReleased(KEY_SPACE) && player->speedY < 0.0f)
        player->speedY *= 0.4f;
    if (IsKeyDown(KEY_SPACE) && player->isJumping) {
        player->jumpTime += delta;
        if (player->jumpTime < MAX_JUMP_TIME)
            player->speedY -= JUMP_HOLD_FORCE * delta;
    }
    if (IsKeyReleased(KEY_SPACE)) player->isJumping = false;
    float acc = player->acc, friction = player->friction;
    if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_S)) {
        if (player->velX < 0.0f) player->velX += friction * delta;
        else player->velX += acc * delta;
    }
    else if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_S)) {
        if (player->velX > 0.0f) player->velX -= friction * delta;
        else player->velX -= acc * delta;
    }
    else {
        if (player->velX > 0.0f) { player->velX -= friction * delta; if (player->velX < 0.0f) player->velX = 0.0f; }
        else if (player->velX < 0.0f) { player->velX += friction * delta; if (player->velX > 0.0f) player->velX = 0.0f; }
    }
    if (player->velX > player->maxSpeed) player->velX = player->maxSpeed;
    if (player->velX < -player->maxSpeed) player->velX = -player->maxSpeed;
    float moveX = player->velX * delta;
    playerRect.x += moveX;
    for (int i = 0; i < envItemsLength; i++) {
        EnvItem* ei = &envItems[i];
        if (ei->blocking && ei->active && CheckCollisionRecs(playerRect, ei->rect)) {
            if (moveX > 0.0f) playerRect.x = ei->rect.x - playerRect.width;
            else if (moveX < 0.0f) playerRect.x = ei->rect.x + ei->rect.width;
        }
    }
    player->speedY += G * delta;
    float moveY = player->speedY * delta;
    int steps = (int)fabsf(moveY / 5.0f) + 1;
    float stepSize = moveY / steps;
    player->canJump = false;
    for (int s = 0; s < steps; s++) {
        playerRect.y += stepSize;
        for (int i = 0; i < envItemsLength; i++) {
            EnvItem* ei = &envItems[i];
            if (ei->blocking && ei->active && CheckCollisionRecs(playerRect, ei->rect)) {
                if (stepSize > 0.0f) {
                    playerRect.y = ei->rect.y - playerRect.height;
                    player->speedY = 0.0f;
                    player->canJump = true;
                    player->isJumping = false;
                }
                else if (stepSize < 0.0f) {
                    playerRect.y = ei->rect.y + ei->rect.height;
                    player->speedY = 0.0f;
                }
                s = steps;
                break;
            }
        }
    }
    player->position.x = playerRect.x + playerRect.width / 2.0f;
    player->position.y = playerRect.y + playerRect.height;
}

// Las demás funciones permanecen exactamente igual que en el código original
// ... (PlayerBreakBlock, EnemyGroundMoviment, etc.)

void PlayerBreakBlock(Player* player, EnvItem* envItems, int len, int LeftOrRight, std::vector<WorldItem>& worldItems, bool& ringDropped) {
    float w = 50.0f, h = 40.0f;
    float ox = (LeftOrRight == 0) ? 20.0f : -w;
    float oy = -60.0f;
    Rectangle actionRect = { player->position.x + ox, player->position.y + oy, w, h };
    for (int i = 0; i < len; i++) {
        EnvItem* ei = &envItems[i];
        if (!ei->active) continue;
        if (CheckCollisionRecs(actionRect, ei->rect)) {
            if (ei->tileID == TILE_INTERROGANT) {
                PlaySound(coinBlockSound);
                ei->active = false;
                if (!ringDropped) {
                    ringDropped = true;
                    WorldItem wi;
                    wi.position = Vector2{ ei->rect.x + TILE_SIZE / 2.0f, ei->rect.y + TILE_SIZE / 2.0f };
                    wi.type = ITEM_RING;
                    wi.active = true;
                    wi.bobTimer = 0.0f;
                    worldItems.push_back(wi);
                }
                continue;
            }
            if (ei->drop == DROP_STAR && ei->texture.id == blockEstrella.id) {
                PlaySound(coinBlockSound);
                bool big = GetRandomValue(0, 1) == 0;
                ei->texture = big ? blockBossaCollons : blockBossaCollonsPetit;
                ei->type = BLOCK_SOLID;
                ei->blocking = 0;
                ei->collectible = true;
                ei->drop = DROP_NONE;
                ei->lifetime = 8.0f;
                continue;
            }
            if (ei->type == BLOCK_BREAKABLE) {
                if (ei->tileID == TILE_BREAK || ei->tileID == TILE_BREAK_CAVE)
                    PlaySound(blockBreakSound);
                ei->active = false;
            }
        }
    }
}

void EnemyGroundMoviment(enemic* p, EnvItem* envItems, int len, float delta) {
    p->posicio.z += 2000.0f * delta;
    Rectangle r = { p->posicio.x, p->posicio.y, 80.0f, 76.0f };
    float moveX = p->velocitat * delta * 100.0f;
    r.x += moveX;
    for (int i = 0; i < len; i++) {
        EnvItem* ei = &envItems[i];
        if (!ei->blocking || !ei->active) continue;
        if (CheckCollisionRecs(r, ei->rect)) {
            p->velocitat *= -1;
            r.x = (moveX > 0.0f) ? ei->rect.x - r.width : ei->rect.x + ei->rect.width;
            break;
        }
    }
    float checkX = (p->velocitat > 0) ? r.x + r.width + 2.0f : r.x - 2.0f;
    Rectangle lavaCheck = { checkX, r.y + r.height - 5.0f, 10.0f, 20.0f };
    for (int i = 0; i < len; i++) {
        EnvItem* ei = &envItems[i];
        if (!ei->active || ei->tileID != TILE_LAVA) continue;
        if (CheckCollisionRecs(lavaCheck, ei->rect)) {
            p->velocitat *= -1;
            r.x -= moveX * 2.0f;
            break;
        }
    }
    float moveY = p->posicio.z * delta;
    r.y += moveY;
    for (int i = 0; i < len; i++) {
        EnvItem* ei = &envItems[i];
        if (!ei->blocking || !ei->active) continue;
        if (CheckCollisionRecs(r, ei->rect)) {
            if (moveY > 0.0f) {
                r.y = ei->rect.y - r.height;
                p->posicio.z = 0.0f;
            }
            else if (moveY < 0.0f) {
                r.y = ei->rect.y + ei->rect.height;
                p->posicio.z = 0.0f;
            }
            break;
        }
    }
    p->posicio.x = r.x;
    p->posicio.y = r.y;
}

void PterodactilMoviment(enemic* p, EnvItem* envItems, int len, float delta) {
    Rectangle r = { p->posicio.x, p->posicio.y, 80.0f, 40.0f };
    float moveX = p->velocitat * delta * 100.0f;
    r.x += moveX;
    for (int i = 0; i < len; i++) {
        EnvItem* ei = &envItems[i];
        if (ei->blocking && ei->active && CheckCollisionRecs(r, ei->rect)) {
            p->velocitat *= -1;
            if (moveX > 0.0f) r.x = ei->rect.x - r.width;
            else r.x = ei->rect.x + ei->rect.width;
            break;
        }
    }
    p->posicio.x = r.x;
}

void PlayerAttackEnemy(Player* player, enemic* p, int dir) {
    if (!p->vida) return;
    float pw = 70.0f, ph = 40.0f;
    float ox = (dir == 0) ? 20.0f : -pw;
    float oy = -60.0f;
    Rectangle punch = { player->position.x + ox, player->position.y + oy, pw, ph };
    Rectangle enemy = { p->posicio.x, p->posicio.y, 80.0f, 40.0f };
    if (CheckCollisionRecs(punch, enemy)) p->vida = false;
}

void EnemyHitPlayer(Player* player, enemic* p) {
    if (!player->alive || !p->vida) return;
    Rectangle playerRect = { player->position.x - 20.0f, player->position.y - 80.0f, 40.0f, 80.0f };
    Rectangle enemyRect = { p->posicio.x, p->posicio.y, 80.0f, 40.0f };
    if (CheckCollisionRecs(playerRect, enemyRect)) player->alive = false;
}

std::vector<EnvItem> BuildEnvItemsFromMap(const int* map, int rows, int cols) {
    std::vector<EnvItem> items;
    items.reserve(rows * cols);
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int tile = map[y * cols + x];
            if (tile == TILE_EMPTY) continue;
            if (tile == TILE_WARP) {
                EnvItem warp = { 0 };
                warp.rect = Rectangle{ (float)x * TILE_SIZE, (float)y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
                warp.blocking = 0;
                warp.active = true;
                warp.tileID = TILE_WARP;
                items.push_back(warp);
                continue;
            }
            EnvItem item = { 0 };
            item.rect = Rectangle{ (float)x * TILE_SIZE, (float)y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
            const TileDef& def = tileDefs[tile];
            item.texture = def.texture;
            item.type = def.type;
            item.blocking = def.blocking;
            item.collectible = def.collectible;
            item.drop = def.drop;
            item.active = true;
            item.lifetime = 0.0f;
            item.tileID = tile;
            items.push_back(item);
        }
    }
    return items;
}

// ---------------------------------------------------------------------
// Camera functions (sin cambios)
// ---------------------------------------------------------------------
void UpdateCameraCenter(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height) {
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    camera->target = player->position;
}
void UpdateCameraCenterInsideMap(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height) {
    camera->target = player->position;
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    float minX = 1000.0f, minY = 1000.0f, maxX = -1000.0f, maxY = -1000.0f;
    for (int i = 0; i < envItemsLength; i++) {
        EnvItem* e = &envItems[i];
        minX = fminf(e->rect.x, minX); maxX = fmaxf(e->rect.x + e->rect.width, maxX);
        minY = fminf(e->rect.y, minY); maxY = fmaxf(e->rect.y + e->rect.height, maxY);
    }
    Vector2 max = GetWorldToScreen2D(Vector2{ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D(Vector2{ minX, minY }, *camera);
    if (max.x < (float)width) camera->offset.x = (float)width - (max.x - (float)width / 2.0f);
    if (max.y < (float)height) camera->offset.y = (float)height - (max.y - (float)height / 2.0f);
    if (min.x > 0.0f) camera->offset.x = (float)width / 2.0f - min.x;
    if (min.y > 0.0f) camera->offset.y = (float)height / 2.0f - min.y;
}
void UpdateCameraCenterSmoothFollow(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height) {
    static float minSpeed = 30.0f, minEffectLength = 10.0f, fractionSpeed = 0.8f;
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float len = Vector2Length(diff);
    if (len > minEffectLength) {
        float speed = fmaxf(fractionSpeed * len, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed * delta / len));
    }
}
void UpdateCameraEvenOutOnLanding(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height) {
    static float evenOutSpeed = 700.0f;
    static int eveningOut = 0;
    static float evenOutTarget;
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    camera->target.x = player->position.x;
    if (eveningOut) {
        if (evenOutTarget > camera->target.y) {
            camera->target.y += evenOutSpeed * delta;
            if (camera->target.y > evenOutTarget) { camera->target.y = evenOutTarget; eveningOut = 0; }
        }
        else {
            camera->target.y -= evenOutSpeed * delta;
            if (camera->target.y < evenOutTarget) { camera->target.y = evenOutTarget; eveningOut = 0; }
        }
    }
    else if (player->canJump && player->speedY == 0.0f && player->position.y != camera->target.y) {
        eveningOut = 1;
        evenOutTarget = player->position.y;
    }
}
void UpdateCameraPlayerBoundsPush(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height) {
    static Vector2 bbox = { 0.2f,0.2f };
    Vector2 min = GetScreenToWorld2D(Vector2{ (1 - bbox.x) * 0.5f * (float)width, (1 - bbox.y) * 0.5f * (float)height }, *camera);
    Vector2 max = GetScreenToWorld2D(Vector2{ (1 + bbox.x) * 0.5f * (float)width, (1 + bbox.y) * 0.5f * (float)height }, *camera);
    camera->offset = Vector2{ (1 - bbox.x) * 0.5f * (float)width, (1 - bbox.y) * 0.5f * (float)height };
    if (player->position.x < min.x) camera->target.x = player->position.x;
    if (player->position.y < min.y) camera->target.y = player->position.y;
    if (player->position.x > max.x) camera->target.x = min.x + (player->position.x - max.x);
    if (player->position.y > max.y) camera->target.y = min.y + (player->position.y - max.y);
}
void UpdateCameraDownOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height) {
    camera->offset = Vector2{ (float)width / 3.3f, (float)height / 1.5f };
    if (player->position.y < 300.0f && player->position.x < 600.0f) { cam_lowestY = 0.0f; cam_init = false; }
    if (!cam_init || cam_lowestY == 0.0f) { cam_lowestY = player->position.y + 400.0f; cam_fixedX = 582.0f; cam_init = true; }
    if (player->position.y > cam_lowestY) cam_lowestY = player->position.y;
    camera->target.x = cam_fixedX;
    camera->target.y = cam_lowestY;
}
void UpdateCameraHorizontalOnly(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height) {
    if (player->position.x < 600.0f) { cam_furthestX = -2.0f; cam_fixedY = -2.0f; }
    if (cam_furthestX < 0.0f) { cam_furthestX = player->position.x; cam_fixedY = player->position.y; }
    if (player->position.x > cam_furthestX) cam_furthestX = player->position.x;
    camera->offset = Vector2{ (float)width / 3.5f, (float)height / 1.35f };
    camera->target.x = cam_furthestX;
    camera->target.y = cam_fixedY;
}