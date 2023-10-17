#include <stdio.h>
#include <raylib.h>

#define screenWidth 1200
#define screenHeight 600
#define tileSize 50

//650
//450

short int sceneWidth = tileSize * 13;
short int sceneHeight = tileSize * 9;


typedef struct {
    int maxBombs;
    int bombLength;
} playerProps;

typedef struct {
    float x;
    float y;
    float size;
    float velX;
    float velY;
    float acc;
    float fric;
    playerProps properties;
} objectPlayer;

typedef struct {
    short int is_active;
    float x;
    float y;
    float bombTimer;
    float spawnTime;
    short int exploding;
    short int exploded;
} objectBomb;

void updateMove(objectPlayer *player) {
    short int borderX = (screenWidth - sceneWidth) / 2;
    short int borderY = (screenHeight - sceneHeight) / 2;

    if (IsKeyDown(KEY_W) && player->y > borderY)
        player->velY -= player->acc;
    else if (player->y <= borderY)
        player->y = borderY;

    if (IsKeyDown(KEY_S) && player->y < sceneHeight + borderY - player->size)
        player->velY += player->acc;
    else if (player->y >= sceneHeight + borderY - player->size)
        player->y = sceneHeight + borderY - player->size;

    if (IsKeyDown(KEY_A) && player->x >= borderX)
        player->velX -= player->acc;
    else if (player->x < borderX)
        player->x = 0 + borderX;

    if (IsKeyDown(KEY_D) && player->x <= sceneWidth + borderX - player->size)
        player->velX += player->acc;
    else if (player->x >= sceneWidth + borderX - player->size)
        player->x = sceneWidth + borderX - player->size;

    player->x += player->velX;
    player->y += player->velY;

    player->velX *= player->fric;
    player->velY *= player->fric;
    
}


void explodeBombs(objectPlayer *player, objectBomb *bombs[5], Sound explosionSound) {
    for(int i = 0; i < 5; i++) {
    if (GetTime() - bombs[i]->spawnTime >= bombs[i]->bombTimer && bombs[i]->is_active) {
        bombs[i]->exploding = 1;
    }
    if (bombs[i]->exploding) {
        bombs[i]->exploded = GetTime();
        bombs[i]->exploding = 0;
        PlaySound(explosionSound);
    }
    if (bombs[i]->exploded) {
        if (GetTime() - bombs[i]->exploded > 0.9) {
            bombs[i]->exploded = 0;
            bombs[i]->is_active = 0;
            player->properties.maxBombs += 1;
        }
    }
    }
}

objectBomb bombs[5] = {{0, 0, 0, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}};


void spawnBombs(objectPlayer *player, objectBomb *bombs[5]) {
    for(int i = 0; i<5; i++) {
        if (player->properties.maxBombs > 0 && !bombs[i]->is_active) {
            player->properties.maxBombs -= 1;
            bombs[i]->is_active = 1;

            int coordX = (((int)(player->x + tileSize/2) / tileSize) * tileSize) - tileSize/2;
            coordX += ((int)(player->x + tileSize/2) % tileSize > tileSize / 2) ? tileSize : 0;

            int coordY = (((int)(player->y + tileSize/2) / tileSize) * tileSize) - tileSize/2;
            coordY += ((int)(player->y + tileSize/2) % tileSize > tileSize / 2) ? tileSize : 0;

            bombs[i]->x = coordX;
            bombs[i]->y = coordY;

            bombs[i]->spawnTime = GetTime();
            break;
        }
    }
}



short int mapa[9][13] = {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                       };


void debug(objectPlayer *player);






int main() {
    short int sceneX = (screenWidth - sceneWidth) / 2;
    short int sceneY = (screenHeight - sceneHeight) / 2;
    int bombStart = 0;

    objectPlayer player = {sceneX, sceneY, tileSize, 0.0, 0.0, 2.5, 0.5,
                            {5, 1}
                          };
    
    //Cria uma matriz de ponteiros para o objeto bombs
    objectBomb *bombs_ptr[5];
    for(int i = 0; i < 5; i++) {
        bombs_ptr[i] = &bombs[i];
    }


     
    for (int i = 0; i < 9; i += 2) {
        int x;
        for (int j = 0; j < 13; j += 2) {
            x = GetRandomValue(0, 1);
            if(x && (i > 1 || j > 1)) {
                mapa[i][j] = 2;
            }

        }
    }


    InitWindow(screenWidth, screenHeight, "Projeto");
    InitAudioDevice();
    Sound explosionSound = LoadSound("explosion.wav");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        DrawRectangle(sceneX, sceneY, sceneWidth, sceneHeight, GREEN);

        //Depuração da cena
        DrawCircle(sceneX, sceneY, 10, RED);

        char bombsActives[64];
        snprintf(bombsActives, sizeof(bombsActives), "Bombs : %d %d %d %d %d", bombs[0].is_active, bombs[1].is_active, bombs[2].is_active, bombs[3].is_active, bombs[4].is_active);
        DrawText(bombsActives, 10, 90, 18, DARKGRAY);

        updateMove(&player);
        if (IsKeyDown(KEY_ENTER) && GetTime() - bombStart > 1) {
            bombStart = GetTime();
            spawnBombs(&player, bombs_ptr);
        }
        
        //Colisões com o mapa
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 13; j++) {
                //Desenha os blocos indestrutíveis
                if (mapa[i][j] == 1) {
                    short int tileX = sceneX + j * tileSize;
                    short int tileY = sceneY + i * tileSize;
                    DrawRectangle(tileX, tileY, tileSize, tileSize, DARKGRAY);

                    int playerLeft = player.x + 2;
                    int playerRight = player.x + tileSize - 2;
                    int playerTop = player.y + 2;
                    int playerBottom = player.y + tileSize - 2;

                    int tileLeft = tileX;
                    int tileRight = tileX + tileSize;
                    int tileTop = tileY;
                    int tileBottom = tileY + tileSize;

            Rectangle playerRect = { player.x, player.y, tileSize, tileSize };
            Rectangle tileRect = { tileX, tileY, tileSize, tileSize };

            if (CheckCollisionRecs(playerRect, tileRect)) {
                        //Colisões laterais
                        if (playerRight > tileLeft && playerLeft < tileLeft) { //D
                            player.x = tileX - tileSize;
                        } else if (playerRight > tileRight && playerLeft < tileRight) { //A
                            player.x = tileX + tileSize;
                        }
                        
                        //Colisões frontais
                        if (playerTop > tileTop && playerBottom < tileTop) { //S
                            player.y = tileY - tileSize;
                        } else if (playerTop > tileBottom && playerBottom < tileBottom) { //W
                            player.y = tileY + tileSize;
                        }
                    }
                //Desenha os blocos destrutíveis
                } else if (mapa[i][j] == 2) {
                    short int tileX = sceneX + j * tileSize;
                    short int tileY = sceneY + i * tileSize;
                    DrawRectangle(tileX, tileY, tileSize, tileSize, WHITE);
                }
            }
        }

        debug(&player);
        BeginDrawing();

        ClearBackground(BLACK);
        DrawRectangle(player.x, player.y, player.size, player.size, BLUE);

        for (int i = 0; i < 5; i++) {
            if (bombs[i].is_active && !bombs[i].exploded) {
                DrawRectangle(bombs[i].x, bombs[i].y, tileSize, tileSize, RED);
            }
        }

        explodeBombs(&player, bombs_ptr, explosionSound);

        //Pseudo explosão
        for (int i = 0; i < 5; i++) {
            if (bombs[i].exploded) {
                DrawRectangle(bombs[i].x, bombs[i].y, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].x - tileSize, bombs[i].y, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].x + tileSize, bombs[i].y, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].x, bombs[i].y - tileSize, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].x, bombs[i].y + tileSize, tileSize, tileSize, YELLOW);
            }
        }
        
        //Grade quadriculada
        for (int x = 0; x <= sceneWidth; x += tileSize) {
            DrawLine(screenWidth / 2 - sceneWidth / 2 + x, screenHeight / 2 - sceneHeight / 2, screenWidth / 2 - sceneWidth / 2 + x, screenHeight / 2 + sceneHeight / 2, DARKGRAY);
        }

        for (int y = 0; y <= sceneHeight; y += tileSize) {
            DrawLine(screenWidth / 2 - sceneWidth / 2, screenHeight / 2 - sceneHeight / 2 + y, screenWidth / 2 + sceneWidth / 2, screenHeight / 2 - sceneHeight / 2 + y, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadSound(explosionSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}



void debug(objectPlayer *player) {
    char playerXText[64];
    char playerYText[64];
    char playerBombs[64];

    snprintf(playerXText, sizeof(playerXText), "Player X: %.2f", player->x);
    snprintf(playerYText, sizeof(playerYText), "Player Y: %.2f", player->y);
    snprintf(playerBombs, sizeof(playerYText), "Player Bombs: %d", player->properties.maxBombs);

    DrawText(playerXText, 10, 10, 18, DARKGRAY);
    DrawText(playerYText, 10, 30, 18, DARKGRAY);
    DrawText(playerBombs, 10, 60, 18, DARKGRAY);
    DrawCircle(player->x, player->y, 5, RED);
}