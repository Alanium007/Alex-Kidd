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

#define BLAU  CLITERAL(Color){8, 9, 250}

//----------------------------------------------------------------------------------
// TEXTURES
//----------------------------------------------------------------------------------

Texture2D background;
Texture2D nuvol;
Texture2D AlexKidd;                                 
Texture2D AlexKiddWalkR;
Texture2D AlexKiddWalkL;


//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
    Texture2D AlexKiddWalkR;
    Texture2D AlexKiddWalkL;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
    Texture2D nuvol;
} EnvItem;



//----------------------------------------------------------------------------------
// FUNCIONS
//----------------------------------------------------------------------------------
void UpdatePlayer(Player* player, EnvItem* envItems, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D* camera, Player* player, EnvItem* envItems, int envItemsLength, float delta, int width, int height);

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
    AlexKidd = LoadTexture("resources/AlexKidd.png");
    AlexKiddWalkR = LoadTexture("resources/AlexKiddWalkR.png");
    AlexKiddWalkL = LoadTexture("resources/AlexKiddWalkL.png");

    Rectangle frameRecR = { 0.0f, 0.0f, ((float)AlexKiddWalkR.width / 4), ((float)AlexKiddWalkR.height) };
    Rectangle frameRecL = { 0.0f, 0.0f, ((float)AlexKiddWalkL.width / 4), ((float)AlexKiddWalkL.height) };

    int currentFrame = 0;

    int framesCounter = 0;
    int framesSpeed = 8;

    Player player = { 0 };
    player.position = Vector2{ 600, 200 };
    player.speed = 0;
    player.canJump = false;
    EnvItem envItems[] = {
        {{ 0, 400, 1000, 200 }, 1, BLACK },
        {{ 300, 200, 400, 10 }, 1, GRAY },
        {{ 250, 300, 100, 10 }, 1, GRAY },
        {{ 650, 300, 100, 10 }, 1, GRAY }
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
        UpdateCameraPlayerBoundsPush
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

        framesCounter++;

        
        if (IsKeyDown(KEY_D)) {
   

                if (framesCounter >= (60 / framesSpeed))
                {
                    framesCounter = 0;
                    currentFrame++;

                    if (currentFrame > 3) currentFrame = 0;

                    frameRecR.x = (float)currentFrame * (float)AlexKiddWalkR.width / 4;
                }
                
        }
        if (IsKeyDown(KEY_A)) {

            if (framesCounter >= (60 / framesSpeed))
            {
                framesCounter = 0;
                currentFrame++;

                if (currentFrame > 3) currentFrame = 0;

                frameRecL.x = (float)currentFrame * (float)AlexKiddWalkR.width / 4;
            }

        }
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);



 
        cameraUpdaters[cameraOption](&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLAU);

        DrawTextureEx(nuvol, Vector2{ 100, 100 }, 0, 0.2f, WHITE);

        DrawTextureEx(nuvol, Vector2{ 900, 1000 }, 0, 0.2f, WHITE);

        BeginMode2D(camera);

        for (int i = 0; i < envItemsLength; i++) DrawRectangleRec(envItems[i].rect, envItems[i].color);


        /*Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 35.0f, 40.0f };
        DrawRectangleRec(playerRect, WHITE);*/
        
        if (!IsKeyDown(KEY_D)&& !IsKeyDown(KEY_A)) DrawTextureRec(AlexKidd, frameRecR, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
        else if (IsKeyDown(KEY_D))DrawTextureRec(AlexKiddWalkR, frameRecR, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
        else if (IsKeyDown(KEY_A))DrawTextureRec(AlexKiddWalkL, frameRecL, Vector2{ player.position.x - 40, player.position.y - 129 }, WHITE);
        
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
    if (IsKeyDown(KEY_A)) player->position.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_D)) player->position.x += PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }

    bool hitObstacle = false;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem* ei = envItems + i;
        Vector2* p = &(player->position);
        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y <= p->y + player->speed * delta)
        {
            hitObstacle = true;
            player->speed = 0.0f;
            p->y = ei->rect.y;
            break;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed * delta;
        player->speed += G * delta;
        player->canJump = false;
    }
    else player->canJump = true;
}

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