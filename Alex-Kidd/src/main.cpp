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
#define PLAYER_HOR_SPD 500.0f
#define JUMP_HOLD_FORCE 1200.0f   // fuerza extra mientras mantienes salto
#define MAX_JUMP_TIME 0.2f        // tiempo máximo que afecta (segundos)

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
Texture2D MonsterBirdR;
Texture2D MonsterBirdL;

Texture2D blockSolid;
Texture2D blockBreak;


//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;

    bool alive;          // 👈 nuevo
    float respawnTimer;  // 👈 nuevo
    Vector2 spawn;    // 👈 nuevo
    bool isJumping;
    float jumpTime;
} Player;

//typedef struct EnvItem {
//    Rectangle rect;
//    int blocking;
//    Color color;
//    Texture2D nuvol;
//} EnvItem;

typedef enum {
    BLOCK_SOLID,
    BLOCK_BREAKABLE
} BlockType;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Texture2D texture;   // en lloc de Color
    BlockType type;
    bool active;
} EnvItem;

typedef struct enemic {
    bool vida;
    int velocitat;
    bool siToca;
    Vector3 posicio;
} pterodactil, escorpi, mono;



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
   

    // INICIALITZACIÓ
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera platformer");

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
    MonsterBirdR = LoadTexture("resources/MonsterBirdR.png");
    MonsterBirdL = LoadTexture("resources/MonsterBirdL.png");

    blockSolid = LoadTexture("resources/BlocSolid.png");
    blockBreak = LoadTexture("resources/BlocBreakable.png");
    


    Rectangle frameRecR = { 0.0f, 0.0f, ((float)AlexKiddWalkR.width / 4), ((float)AlexKiddWalkR.height) };
    Rectangle frameRecL = { 0.0f, 0.0f, ((float)AlexKiddWalkL.width / 4), ((float)AlexKiddWalkL.height) };
    Rectangle frameRecJump = { 0.0f, 0.0f, ((float)AlexKiddJumpR.width), ((float)AlexKiddJumpR.height) };
    Rectangle frameRecPuny = { 0.0f, 0.0f, ((float)AlexKiddPunyR.width), ((float)AlexKiddPunyR.height) };
    Rectangle frameRecCrouch = { 0.0f, 0.0f, ((float)AlexKiddPunyR.width), ((float)AlexKiddPunyR.height) };
    Rectangle framePterodactil = { 0.0f, 0.0f, ((float)MonsterBirdR.width / 2), ((float)MonsterBirdR.height) };


    int playerFrame = 0;
    int playerCounter = 0;

    int pteroFrame = 0;
    int pteroCounter = 0;

    int framesSpeed = 8;

    int LeftOrRight = NULL;
    int var = 0;

    bool attacking = false;
    int attackTimer = 0;
    
    
    int FramesPuny = 0;

    

    Player player = { 0 };
    player.position = Vector2{ 600, 200 };
    player.speed = 0;
    player.canJump = false;
    player.isJumping = false;
    player.jumpTime = 0;

    enemic pterodactil = { 0 };
    pterodactil.posicio = Vector3{ 600, 200 };
    pterodactil.velocitat = 2;
    pterodactil.siToca = false;
    pterodactil.vida = true;
    
    player.position = Vector2{ 600, 200 };
    player.spawn = player.position;   // 👈 guardar spawn
    player.alive = true;
    player.respawnTimer = 0;
  

    EnvItem envItems[] = {
    {{ 0, 400, 1000, 200 }, 1, blockSolid, BLOCK_SOLID, true},
    {{ 600, 200, 80, 80 }, 1, blockBreak, BLOCK_BREAKABLE, true},
    {{ 450, 100, 80, 80 }, 1, blockBreak, BLOCK_BREAKABLE, true},
    {{ 1000, 200, 80, 80 }, 1, blockBreak, BLOCK_BREAKABLE, true},
    {{ -200, 200, 80, 80 }, 1, blockBreak, BLOCK_BREAKABLE, true},
    {{ 250, 500, 80, 80 }, 1, blockSolid, BLOCK_SOLID, true},
    {{ 330, 500, 80, 80 }, 1, blockSolid, BLOCK_SOLID, true},
    {{ 410, 500, 80, 80 }, 1, blockSolid, BLOCK_SOLID, true },
    {{ 490, 500, 80, 80 }, 1, blockSolid, BLOCK_SOLID, true },
    {{ 330, 580, 80, 80 }, 1, blockSolid, BLOCK_SOLID, true },
    {{ 410, 580, 80, 80 }, 1, blockSolid, BLOCK_SOLID, true },
    {{ 490, 580, 80, 80 }, 1, blockSolid, BLOCK_SOLID, true },
};

    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Store pointers to the multiple update camera functions
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
    //--------------------------------------------------------------------------------------

    // Main game loop

    static float x = 400, y = 400;
    
    while (!WindowShouldClose())
    {

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }
        
        int cameraOption = 5; // ahora es la cámara “solo baja”

        /*cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);*/
        

        // Animacions---------------------------------------------------------------------------------------------------------------
        if (attacking)
        {
            attackTimer--;

            if (attackTimer <= 0)
            {
                attacking = false;
            }
        }
        
        playerCounter++;

        if (IsKeyDown(KEY_D)) {


            if (playerCounter >= (60 / framesSpeed))
            {
                playerCounter = 0;
                playerFrame++;

                if (playerFrame > 3) playerFrame = 0;

                frameRecR.x = (float)playerFrame * (float)AlexKiddWalkR.width / 4;
            }

        }
        if (IsKeyDown(KEY_A)) {

            if (playerCounter >= (60 / framesSpeed))
            {
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

        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);

        if (!player.alive)
        {
            player.respawnTimer -= deltaTime;

            if (player.respawnTimer <= 0)
            {
                player.alive = true;
                player.position = player.spawn;
                player.speed = 0;
            }
        }

        PterodactilMoviment(&pterodactil, envItems, envItemsLength, deltaTime);


        cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        


        ClearBackground(BLAU);
        


        

        BeginMode2D(camera);
        DrawTextureEx(nuvol, Vector2{ 100, 100 }, 0, 0.2f, WHITE);
        DrawTextureEx(nuvol, Vector2{ 900, 300 }, 0, 0.2f, WHITE);

        for (int i = 0; i < envItemsLength; i++)
        {
            if (!envItems[i].active) continue; // ignorar bloques desactivados

            DrawTexturePro(
                envItems[i].texture,
                Rectangle{ 0, 0, (float)envItems[i].texture.width, (float)envItems[i].texture.height },
                envItems[i].rect,
                Vector2{ 0, 0 },
                0.0f,
                WHITE
            );
        }

        EnemyHitPlayer(&player, &pterodactil);
        /*Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 35.0f, 40.0f };
        DrawRectangleRec(playerRect, WHITE);*/
        if (player.alive)
        {
            if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D)) LeftOrRight = 0;
            else if (IsKeyPressed(KEY_A) || IsKeyDown(KEY_A)) LeftOrRight = 1;

            if (attacking && LeftOrRight == 0)DrawTextureRec(AlexKiddPunyR, frameRecPuny, Vector2{ player.position.x - 35, player.position.y - 129 }, WHITE);
            else if (attacking && LeftOrRight == 1)DrawTextureRec(AlexKiddPunyL, frameRecPuny, Vector2{ player.position.x - 75, player.position.y - 129 }, WHITE);
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
                attackTimer = 20; // duración del golpe

                // Romper bloques
                PlayerBreakBlock(&player, envItems, envItemsLength, LeftOrRight);

                // Golpear pterodáctilo
                PlayerAttackEnemy(&player, &pterodactil, LeftOrRight);
            }

      
        }

        if (pterodactil.vida)
        {
            if (pterodactil.velocitat > 0)
                DrawTextureRec(MonsterBirdR, framePterodactil, Vector2{ pterodactil.posicio.x, pterodactil.posicio.y }, WHITE);
            else
                DrawTextureRec(MonsterBirdL, framePterodactil, Vector2{ pterodactil.posicio.x, pterodactil.posicio.y }, WHITE);
        }
        

    
        


    
        //DrawTextureEx(AlexKiddWalkR, Vector2{ player.position.x - 30, player.position.y - 76}, 0, 2.0f, WHITE);   //Textura Alex Kidd
        
        
        EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    UnloadTexture(background);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta)
{
    if (!player->alive) return;
    // ---------------------------
    // HITBOX DEL JUGADOR
    // ---------------------------
    Rectangle playerRect = {
        player->position.x - 20,
        player->position.y - 80,
        40,
        80
    };

    // ---------------------------
    // INPUT SALTO
    // ---------------------------
    if (IsKeyPressed(KEY_SPACE) && player->canJump && !IsKeyDown(KEY_S))
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;

        player->isJumping = true;
        player->jumpTime = 0;
    }
    if (IsKeyReleased(KEY_SPACE) && player->speed < 0)
    {
        player->speed *= 0.4f; // 👈 corta la subida
    }
    if (IsKeyDown(KEY_SPACE) && player->isJumping)
    {
        player->jumpTime += delta;

        if (player->jumpTime < MAX_JUMP_TIME)
        {
            player->speed -= JUMP_HOLD_FORCE * delta;
        }
    }
    if (IsKeyReleased(KEY_SPACE))
    {
        player->isJumping = false;
    }

    // ---------------------------
    // MOVIMIENTO HORIZONTAL
    // ---------------------------
    float moveX = 0;
    if (IsKeyDown(KEY_A) && !IsKeyDown(KEY_S)) moveX = -PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_D) && !IsKeyDown(KEY_S)) moveX = PLAYER_HOR_SPD * delta;

    playerRect.x += moveX;

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = &envItems[i];

        if (ei->blocking && ei->active)
        {
            if (CheckCollisionRecs(playerRect, ei->rect))
            {
                if (moveX > 0) // derecha
                    playerRect.x = ei->rect.x - playerRect.width;
                else if (moveX < 0) // izquierda
                    playerRect.x = ei->rect.x + ei->rect.width;
            }
        }
    }

    // ---------------------------
    // GRAVEDAD
    // ---------------------------
    player->speed += G * delta;
    float moveY = player->speed * delta;

    // ---------------------------
    // TUNNELING FIX: mover en pasos pequeños
    // ---------------------------
    int steps = (int)fabs(moveY / 5.0f) + 1;
    float stepSize = moveY / steps;
    player->canJump = false;

    for (int s = 0; s < steps; s++)
    {
        playerRect.y += stepSize;

        for (int i = 0; i < envItemsLength; i++)
        {
            EnvItem* ei = &envItems[i];

            if (ei->blocking && ei->active)
            {
                if (CheckCollisionRecs(playerRect, ei->rect))
                {
                    if (stepSize > 0) // cayendo (suelo)
                    {
                        playerRect.y = ei->rect.y - playerRect.height;
                        player->speed = 0;
                        player->canJump = true;
                        player->isJumping = false;
                    }
                    if (stepSize < 0) // subiendo (techo)
                    {
                        playerRect.y = ei->rect.y + ei->rect.height;
                        player->speed = 0;

                        // No romper bloques al saltar
                        // if (ei->type == BLOCK_BREAKABLE) ei->active = false; // eliminar
                    }

                    // salir del bucle de pasos si colisionamos
                    s = steps;
                    break;
                }
            }
        }
    }

    // ---------------------------
    // ACTUALIZAR POSICIÓN FINAL
    // ---------------------------
    player->position.x = playerRect.x + playerRect.width / 2;
    player->position.y = playerRect.y + playerRect.height;
}
void PlayerBreakBlock(Player* player, EnvItem* envItems, int envItemsLength, int LeftOrRight)
{
    // Rectángulo de acción delante del jugador
    float width = 50;
    float height = 40;
    float offsetX = (LeftOrRight == 0) ? 20 : -width; // derecha o izquierda
    float offsetY = -60; // altura del puño desde la base del jugador

    Rectangle actionRect = {
        player->position.x + offsetX,
        player->position.y + offsetY,
        width,
        height
    };

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = &envItems[i];

        if (ei->active && ei->type == BLOCK_BREAKABLE)
        {
            if (CheckCollisionRecs(actionRect, ei->rect))
            {
                // Desactivar bloque
                ei->active = false;
            }
        }
    }
}

void PterodactilMoviment(enemic* pterodactil, EnvItem* envItems, int envItemsLength, float delta)
{
    // ---------------------------
    // HITBOX DEL PTERODÁCTILO
    // ---------------------------
    Rectangle pteroRect = {
        pterodactil->posicio.x,
        pterodactil->posicio.y,
        80,   // ajusta al tamaño real del sprite
        40
    };

    // ---------------------------
    // MOVIMIENTO HORIZONTAL
    // ---------------------------
    float moveX = pterodactil->velocitat * delta * 100; // escalado con delta
    pteroRect.x += moveX;

    // ---------------------------
    // COLISIONES CON BLOQUES
    // ---------------------------
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = &envItems[i];

        if (ei->blocking && ei->active)
        {
            if (CheckCollisionRecs(pteroRect, ei->rect))
            {
                // 🔁 CAMBIAR DIRECCIÓN
                pterodactil->velocitat *= -1;

                // Ajustar posición para no quedarse dentro del bloque
                if (moveX > 0) // iba a la derecha
                {
                    pteroRect.x = ei->rect.x - pteroRect.width;
                }
                else // iba a la izquierda
                {
                    pteroRect.x = ei->rect.x + ei->rect.width;
                }

                break;
            }
        }
    }

    // ---------------------------
    // ACTUALIZAR POSICIÓN FINAL
    // ---------------------------
    pterodactil->posicio.x = pteroRect.x;
}

void PlayerHitEnemy(Player* player, enemic* pterodactil, int LeftOrRight)
{
    float width = 50;
    float height = 40;
    float offsetX = (LeftOrRight == 0) ? 20 : -width;
    float offsetY = -60;

    Rectangle attackRect = {
        player->position.x + offsetX,
        player->position.y + offsetY,
        width,
        height
    };

    Rectangle pteroRect = {
        pterodactil->posicio.x,
        pterodactil->posicio.y,
        80,
        40
    };

    if (pterodactil->vida && CheckCollisionRecs(attackRect, pteroRect))
    {
        pterodactil->vida = false; // 💀 muere
    }
}

void EnemyHitPlayer(Player* player, enemic* pterodactil)
{
    if (!player->alive || !pterodactil->vida) return;

    Rectangle playerRect = {
        player->position.x - 20,
        player->position.y - 80,
        40,
        80
    };

    Rectangle pteroRect = {
        pterodactil->posicio.x,
        pterodactil->posicio.y,
        80,
        40
    };

    if (CheckCollisionRecs(playerRect, pteroRect))
    {
        player->alive = false;
        player->respawnTimer = 2.0f; // ⏱ 2 segundos
    }
}
void PlayerAttackEnemy(Player* player, enemic* ptero, int LeftOrRight)
{
    if (!ptero->vida) return; // ya está muerto

    // Tamaño del golpe
    float punchWidth = 50;
    float punchHeight = 40;

    // Offset según dirección
    float offsetX = (LeftOrRight == 0) ? 20 : -punchWidth; // derecha o izquierda
    float offsetY = -60; // ajusta según altura del puño

    Rectangle punchHitbox = {
        player->position.x + offsetX,
        player->position.y + offsetY,
        punchWidth,
        punchHeight
    };

    Rectangle pteroHitbox = {
        ptero->posicio.x,
        ptero->posicio.y,
        MonsterBirdR.width / 2.0f, // ancho del sprite
        MonsterBirdR.height       // altura del sprite
    };

    // Colisión
    if (CheckCollisionRecs(punchHitbox, pteroHitbox))
    {
        ptero->vida = false; // lo matamos definitivamente
    }
}
//CAMERA----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

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

    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = envItems + i;
        minX = fminf(ei->rect.x, minX);
        maxX = fmaxf(ei->rect.x + ei->rect.width, maxX);
        minY = fminf(ei->rect.y, minY);
        maxY = fmaxf(ei->rect.y + ei->rect.height, maxY);
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
    static float minSpeed = 30;
    static float minEffectLength = 10;
    static float fractionSpeed = 0.8f;

    camera->offset = Vector2{ width / 2.0f, height / 2.0f };
    Vector2 diff = Vector2Subtract(player->position, camera->target);
    float length = Vector2Length(diff);

    if (length > minEffectLength)
    {
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

    if (eveningOut)
    {
        if (evenOutTarget > camera->target.y)
        {
            camera->target.y += evenOutSpeed * delta;

            if (camera->target.y > evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
        else
        {
            camera->target.y -= evenOutSpeed * delta;

            if (camera->target.y < evenOutTarget)
            {
                camera->target.y = evenOutTarget;
                eveningOut = 0;
            }
        }
    }
    else
    {
        if (player->canJump && (player->speed == 0) && (player->position.y != camera->target.y))
        {
            eveningOut = 1;
            evenOutTarget = player->position.y;
        }
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
    camera->offset = Vector2{ width / 2.0f, height / 2.0f };

    if (lowestY == 0) lowestY = player->position.y;

    if (player->position.y > lowestY)
        lowestY = player->position.y;

    camera->target.x = player->position.x;
    camera->target.y = lowestY;
}




