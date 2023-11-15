#include <stdio.h>
#include <raylib.h>

#define screenWidth 800
#define screenHeight 600
#define tileSize 40
#define NUMTILES_H 15
#define NUMTILES_W 15

short int sceneWidth = tileSize * NUMTILES_W;
short int sceneHeight = tileSize * NUMTILES_H;

typedef struct {
    int maxBombs;
    int bombLength;
    int bombDelay;
} playerProps;

typedef struct {
    Vector2 pos;
    float size;
    float velX;
    float velY;
    float acc;
    float fric;
    playerProps properties;
} PlayerObj;

typedef struct {
    short int is_active;
    Vector2 pos;
    float bombTimer;
    float spawnTime;
    short int exploding;
    short int exploded;
} BombObj;

typedef struct {
    int x;
    int y;
} Vector2Int;

//Obtém uma coordenada com base em dois índices no mapa;
Vector2 getCoords(int x, int y) {
    short int sceneX = (screenWidth - sceneWidth) / 2;
    short int sceneY = (screenHeight - sceneHeight) / 2;
    float coordX = x * tileSize + sceneX;
    float coordY = y * tileSize + sceneY;
    Vector2 coords = {coordX, coordY};
    return coords;
}

//Obtém dois índices no mapa com base em uma coordenada
Vector2Int getIndex(float x, float y) {
    short int sceneX = (screenWidth - sceneWidth) / 2;
    short int sceneY = (screenHeight - sceneHeight) / 2;
    int indexX = ((x + tileSize / 2) - sceneX) / tileSize;
    int indexY = ((y + tileSize / 2) - sceneY) / tileSize;
    Vector2Int index = {indexX, indexY};
    return index;
}



//Verifica qual foi a útima tecla pressionada
KeyboardKey lastKey = KEY_NULL;

void getLastKey() {
        if (IsKeyPressed(KEY_W)) lastKey = KEY_W;
        if (IsKeyPressed(KEY_A)) lastKey = KEY_A;
        if (IsKeyPressed(KEY_S)) lastKey = KEY_S;
        if (IsKeyPressed(KEY_D)) lastKey = KEY_D;

        // Se alguma das quatro teclas for solta, verifica qual foi a última tecla pressionada
        if (IsKeyReleased(KEY_W) || IsKeyReleased(KEY_A) || IsKeyReleased(KEY_S) || IsKeyReleased(KEY_D))
        {
            // Percorre as quatro teclas em ordem inversa e atribui a primeira que estiver pressionada
            if (IsKeyDown(KEY_D)) lastKey = KEY_D;
            else if (IsKeyDown(KEY_S)) lastKey = KEY_S;
            else if (IsKeyDown(KEY_A)) lastKey = KEY_A;
            else if (IsKeyDown(KEY_W)) lastKey = KEY_W;
            else lastKey = KEY_NULL;
        }

}

//Encontra os blocos próximos ao player e verifica se há colisões
void prevCollision(PlayerObj *player, short int mapa[][NUMTILES_W]) {
    Vector2Int playerIndex = getIndex(player->pos.x, player->pos.y);
    
    for (int y = playerIndex.y - 1; y <= playerIndex.y + 1; y++) {
        for (int x = playerIndex.x - 1; x <= playerIndex.x + 1; x++) {
            if (mapa[y][x] >= 1 && x >= 0) {
                Vector2 coords = getCoords(x, y);

                DrawCircle(coords.x + tileSize/2, coords.y + tileSize/2, 4, RED);
                DrawRectangleLines(coords.x, coords.y, tileSize, tileSize, RED);
                
                int playerLeft = player->pos.x + 2;
                int playerRight = player->pos.x + player->size - 2;
                int playerTop = player->pos.y + 2;
                int playerBottom = player->pos.y + player->size - 2;

                int tileLeft = coords.x;
                int tileRight = coords.x + tileSize;
                int tileTop = coords.y;
                int tileBottom = coords.y + tileSize;

                Rectangle playerRect = { player->pos.x, player->pos.y, player->size, player->size };
                Rectangle tileRect = { coords.x, coords.y, tileSize, tileSize };
                

                //Resolve a colisão movendo o player para fora do bloco
                if (CheckCollisionRecs(playerRect, tileRect)) {
                    int tolerance = tileSize - 10;
                    int move_on_collision = 2.8;

                    int top_dist = tileTop - playerTop;
                    if(top_dist >= tolerance) {
                        player->pos.y = tileTop - player->size;
                    } else if(top_dist < tolerance && top_dist > 1) {
                        player->pos.y -= move_on_collision;
                    }

                    int left_dist = tileLeft - playerLeft;
                    if(left_dist >= tolerance) {
                        player->pos.x = tileLeft - player->size;
                    } else if(left_dist < tolerance && left_dist  > 1) {
                        player->pos.x -= move_on_collision;
                    }

                    int bottom_dist = playerBottom - tileBottom;
                    if(bottom_dist >= tolerance) {
                        player->pos.y = tileBottom;
                    } else if(bottom_dist < tolerance && bottom_dist > 1) {
                        player->pos.y += move_on_collision;
                    }

                    int right_dist = playerRight - tileRight;
                    if(right_dist >= tolerance) {
                        player->pos.x = tileRight;
                    } else if(right_dist < tolerance && right_dist > 1) {
                        player->pos.x += move_on_collision;
                    }

                }
            }
        }
    }
}

//Função para mover o player
void updateMove(PlayerObj *player) {
    short int borderX = (screenWidth - sceneWidth) / 2;
    short int borderY = (screenHeight - sceneHeight) / 2;
    getLastKey();
    if (lastKey == KEY_W && player->pos.y > borderY)
        player->velY -= player->acc;
    else if (player->pos.y <= borderY)
        player->pos.y = borderY;

    if (lastKey == KEY_S && player->pos.y < sceneHeight + borderY - player->size)
        player->velY += player->acc;
    else if (player->pos.y >= sceneHeight + borderY - player->size)
        player->pos.y = sceneHeight + borderY - player->size;

    if (lastKey == KEY_A && player->pos.x >= borderX)
        player->velX -= player->acc;
    else if (player->pos.x < borderX)
        player->pos.x = 0 + borderX;

    if (lastKey == KEY_D && player->pos.x <= sceneWidth + borderX - player->size)
        player->velX += player->acc;
    else if (player->pos.x >= sceneWidth + borderX - player->size)
        player->pos.x = sceneWidth + borderX - player->size;

    player->pos.x += player->velX;
    player->pos.y += player->velY;

    player->velX *= player->fric;
    player->velY *= player->fric;
}

void spawnBombs(PlayerObj *player, BombObj bombs[]) {
    for (int i = 0; i < 5; i++) {
        if (player->properties.maxBombs > 0 && !bombs[i].is_active) { //Spawna no máximo x bombas por vez

            player->properties.maxBombs -= 1;
            bombs[i].is_active = 1;

            Vector2Int index = getIndex(player->pos.x, player->pos.y);
            bombs[i].pos = getCoords(index.x, index.y);

            bombs[i].spawnTime = GetTime();
            break;
        }
    }
}

void explodeBombs(PlayerObj *player, BombObj bombs[]) {
    for (int i = 0; i < 5; i++) {
        if (GetTime() - bombs[i].spawnTime >= bombs[i].bombTimer && bombs[i].is_active) {
            bombs[i].exploding = 1;
        }
        if (bombs[i].exploding) {
            bombs[i].exploded = GetTime();
            bombs[i].exploding = 0;
        }
        if (bombs[i].exploded) {
            if (GetTime() - bombs[i].exploded > 0.9) {
                bombs[i].exploded = 0;
                bombs[i].is_active = 0;
                player->properties.maxBombs += 1;
            }
        }
    }
}


short int mapa[NUMTILES_H][NUMTILES_W];

void initMapa() {

    for (int i = 0; i < NUMTILES_H; i++) {
        for (int j = 0; j < NUMTILES_W; j++) {
            if(i%2 && j%2) {
                mapa[i][j] = 1;
            } else {
                mapa[i][j] = 0;
            }
        }
    }
}

//Assinatura de funções
void debug(PlayerObj *player, BombObj bombs[], Vector2 scene);


//Janela principal do game
void updateGame(PlayerObj *player, BombObj bombs[], Vector2 scene) {
        DrawRectangle(scene.x, scene.y, sceneWidth, sceneHeight, GREEN);


        updateMove(player);
        if (IsKeyDown(KEY_SPACE) && GetTime() - player->properties.bombDelay > 1) {
            player->properties.bombDelay = GetTime();
            spawnBombs(player, bombs);
        }

        //Desenha o mapa
        for (int i = 0; i < NUMTILES_H; i++) {
            for (int j = 0; j < NUMTILES_W; j++) {
                // Desenha os blocos indestrutíveis
                if (mapa[i][j] == 1) {
                    short int tileX = scene.x + j * tileSize;
                    short int tileY = scene.y + i * tileSize;
                    DrawRectangle(tileX, tileY, tileSize, tileSize, DARKGRAY);

                // Desenha os blocos destrutíveis
                } else if (mapa[i][j] == 2) {
                    short int tileX = scene.x + j * tileSize;
                    short int tileY = scene.y + i * tileSize;
                    DrawRectangle(tileX, tileY, tileSize, tileSize, WHITE);
                }
            }
        }
        
        debug(player, bombs, scene);

        BeginDrawing();

        ClearBackground(BLACK);

        for (int i = 0; i < 5; i++) {
            if (bombs[i].is_active && !bombs[i].exploded) {
                DrawRectangle(bombs[i].pos.x, bombs[i].pos.y, tileSize, tileSize, RED);
            }
        }

        explodeBombs(player, bombs);

        // Pseudo explosão
        for (int i = 0; i < 5; i++) {
            if (bombs[i].exploded) {
                DrawRectangle(bombs[i].pos.x, bombs[i].pos.y, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].pos.x - tileSize, bombs[i].pos.y, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].pos.x + tileSize, bombs[i].pos.y, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].pos.x, bombs[i].pos.y - tileSize, tileSize, tileSize, YELLOW);
                DrawRectangle(bombs[i].pos.x, bombs[i].pos.y + tileSize, tileSize, tileSize, YELLOW);
            }
        }

        // Grade quadriculada
        for (int x = 0; x <= sceneWidth; x += tileSize) {
            DrawLine(screenWidth / 2 - sceneWidth / 2 + x, screenHeight / 2 - sceneHeight / 2, screenWidth / 2 - sceneWidth / 2 + x, screenHeight / 2 + sceneHeight / 2, DARKGRAY);
        }

        for (int y = 0; y <= sceneHeight; y += tileSize) {
            DrawLine(screenWidth / 2 - sceneWidth / 2, screenHeight / 2 - sceneHeight / 2 + y, screenWidth / 2 + sceneWidth / 2, screenHeight / 2 - sceneHeight / 2 + y, DARKGRAY);
        }

        //Função que detecta os blocos próximos ao player e verifica a colisão
        prevCollision(player, mapa);
        DrawRectangle(player->pos.x, player->pos.y, player->size, player->size, BLUE);

        EndDrawing();
}

int main() {
    //Coordenadas de origem da tela verde
    Vector2 scene = {(screenWidth - sceneWidth) / 2, (screenHeight - sceneHeight) / 2};
    initMapa();

    PlayerObj player = {{scene.x, scene.y}, tileSize, 0.0, 0.0, 2.5, 0.5, {5, 1, 0}};
    BombObj bombs[5] = {{0, {0, 0}, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}, {0, 0, 0, 4.0, 0, 0, 0}};

    //Adiciona os blocos destrútiveis de forma aletória no mapa
    for (int i = 0; i < NUMTILES_H; i += 2) {
        int x;
        for (int j = 0; j < NUMTILES_W; j += 2) {
            x = GetRandomValue(0, 1);
            if (x && (i > 1 || j > 1)) {
                mapa[i][j] = 2;
            }
        }
    }

    InitWindow(screenWidth, screenHeight, "Projeto");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        //Janela principal do game
        updateGame(&player, bombs, scene);
    }

    CloseWindow();

    return 0;
}




//Debuga o player
void debug(PlayerObj *player, BombObj bombs[], Vector2 scene) {
    char playerXText[64];
    char playerYText[64];
    char playerBombs[64];

    snprintf(playerXText, sizeof(playerXText), "Player X: %.2f", player->pos.x);
    snprintf(playerYText, sizeof(playerYText), "Player Y: %.2f", player->pos.y);
    snprintf(playerBombs, sizeof(playerYText), "Player Bombs: %d", player->properties.maxBombs);

    DrawText(playerXText, 10, 10, 18, DARKGRAY);
    DrawText(playerYText, 10, 30, 18, DARKGRAY);
    DrawText(playerBombs, 10, 60, 18, DARKGRAY);

    //Desenha u círculo vermelho na origem do player
    DrawCircle(player->pos.x, player->pos.y, 5, RED);

    //Desenha um círculo vermelho na origem da tela verde
    DrawCircle(scene.x, scene.y, 10, RED);

    char bombsActives[64];
    snprintf(bombsActives, sizeof(bombsActives), "Bombs : %d %d %d %d %d", bombs[0].is_active, bombs[1].is_active, bombs[2].is_active, bombs[3].is_active, bombs[4].is_active);
    DrawText(bombsActives, 10, 90, 18, DARKGRAY);

}
