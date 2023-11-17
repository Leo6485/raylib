#include <stdio.h>
#include <raylib.h>
#include <math.h>

#define SCREEN_W 1200
#define SCREEN_H 700
#define TILESIZE 40
#define NUMTILES_H 15
#define NUMTILES_W 15

const short int sceneWidth = TILESIZE * NUMTILES_W;
const short int sceneHeight = TILESIZE * NUMTILES_H;

const Vector2 scene = {(SCREEN_W - sceneWidth) / 2, (SCREEN_H - sceneHeight) / 2};

typedef struct {
    int maxBombs;
    int bombLength;
    int bombDelay;
} playerProps;

typedef struct Bomb{
    Rectangle pos;
    Rectangle explosion_right;
    Rectangle explosion_left;
    Rectangle explosion_up;
    Rectangle explosion_down;
    float maxTop;
    float maxRight;
    float maxBottom;
    float maxLeft;
    int isActive;
    int distance;
    int vel;
    int time;
}Bomb;

typedef struct {
    Vector2 pos;
    float size;
    float velX;
    float velY;
    float acc;
    float fric;
    int draw_bomb;
    int put_bomb;
    int num_bombs;
    Bomb bombs[10];
} PlayerObj;

typedef struct {
    int x;
    int y;
} Vector2Int;

typedef struct {
    PlayerObj player;
} game;

KeyboardKey lastKey = KEY_NULL;

// Funções relacionadas ao mapa
void initMapa();
Vector2 getCoords(int x, int y);
Vector2Int getIndex(float x, float y);

// Funções relacionadas ao jogador
void getLastKey();
void updateMove(game *g);
void prevCollision(game *g, short int mapa[][NUMTILES_W]);

// Funções do jogo
void updateGame(game *g);
void gameOver();
void mainMenu();
void debug(game *g);

short int mapa[NUMTILES_H][NUMTILES_W];

void initBombs(game *g, int n) {
    for(int i = 0; i < n; i++) {
            
        //g->player->bombs[i].pos = {0, 0, TILESIZE, TILESIZE};
        //g->player->bombs[i].explosion_right {0, 0, TILESIZE, TILESIZE};
        //g->player->bombs[i].explosion_left = {0, 0, TILESIZE, TILESIZE};
        //g->player->bombs[i].explosion_up = {0, 0, TILESIZE, TILESIZE};
        //g->player->bombs[i].explosion_down = {0, 0, TILESIZE, TILESIZE};
        g->player.bombs[i].maxTop = 0;
        g->player.bombs[i].maxRight = 0;
        g->player.bombs[i].maxBottom = 0;
        g->player.bombs[i].maxLeft = 0;
        g->player.bombs[i].isActive = 0;
        g->player.bombs[i].distance = 1;
        g->player.bombs[i].vel = TILESIZE / 4;
        g->player.bombs[i].time = 0;
    }
}

void draw_bomb(game *g){
    for(int i = 0; i < g->player.num_bombs; i++) { 
        if(g->player.bombs[i].isActive == 1) {
            DrawRectangleRec(g->player.bombs[i].pos, RED);
            DrawRectangleRec(g->player.bombs[i].explosion_right, RED);
            DrawRectangleRec(g->player.bombs[i].explosion_left, RED);
            DrawRectangleRec(g->player.bombs[i].explosion_up, RED);
            DrawRectangleRec(g->player.bombs[i].explosion_down, RED);
        }
    }
}

void update_bomb(game *g, short int mapa[][NUMTILES_W]) {
    if (IsKeyPressed(KEY_SPACE)) {
        g->player.put_bomb = 1;
    }
    for(int i = 0; i < 10; i++){
    char bombText[64];
    snprintf(bombText, sizeof(bombText), "Bomb[%d] maxBottom: %.2f", i, g->player.bombs[i].maxBottom);
    DrawText(bombText, 10, 90 + i*30, 18, DARKGRAY);
    }

    if (g->player.put_bomb == 1) {
        for (int i = 0; i < g->player.num_bombs; i++) {
            if (g->player.bombs[i].isActive == 0) {
                g->player.bombs[i].isActive = 1;
                g->player.bombs[i].pos = (Rectangle){g->player.pos.x, g->player.pos.y, TILESIZE, TILESIZE};
                g->player.bombs[i].explosion_right = g->player.bombs[i].pos;
                g->player.bombs[i].explosion_left = g->player.bombs[i].pos;
                g->player.bombs[i].explosion_down = g->player.bombs[i].pos;
                g->player.bombs[i].explosion_up = g->player.bombs[i].pos;

                g->player.bombs[i].time = GetTime();

                g->player.bombs[i].maxTop = 0;
                g->player.bombs[i].maxRight = 0;
                g->player.bombs[i].maxBottom = 0;
                g->player.bombs[i].maxLeft = 0;

                Vector2Int bombIndex = getIndex(g->player.bombs[i].pos.x, g->player.bombs[i].pos.y);

                for (int j = bombIndex.y; j <= bombIndex.y + g->player.bombs[i].distance; j++) {
                    if (mapa[j][bombIndex.x] == 0 && j > 0) {
                        g->player.bombs[i].maxBottom += TILESIZE;
                    } else if (mapa[j][bombIndex.x] == 2 && j > 0) {
                        g->player.bombs[i].maxBottom += TILESIZE;
                        break;
                    } else if (mapa[j][bombIndex.x] == 1 && j > 0) {
                        break;
                    }
                }

                for (int j = bombIndex.x; j <= bombIndex.x + g->player.bombs[i].distance; j++) {
                    if (mapa[bombIndex.y][j] == 0 && j > 0) {
                        g->player.bombs[i].maxRight += TILESIZE;
                    } else if (mapa[bombIndex.y][j] == 2 && j > 0) {
                        g->player.bombs[i].maxRight += TILESIZE;
                        break;
                    } else if (mapa[bombIndex.y][j] == 1 && j > 0) {
                        break;
                    }
                }

                for (int j = bombIndex.y; j >= bombIndex.y - g->player.bombs[i].distance; j--) {
                    if (mapa[j][bombIndex.x] == 0 && j > 0) {
                        g->player.bombs[i].maxTop += TILESIZE;
                    } else if (mapa[j][bombIndex.x] == 2 && j > 0) {
                        g->player.bombs[i].maxTop += TILESIZE;
                        break;
                    } else if (mapa[j][bombIndex.x] == 1 && j > 0) {
                        break;
                    }
                }

                for (int j = bombIndex.x; j >= bombIndex.x - g->player.bombs[i].distance; j--) {
                    if (mapa[bombIndex.y][j] == 0 && j > 0) {
                        g->player.bombs[i].maxLeft += TILESIZE;
                    } else if (mapa[bombIndex.y][j] == 2 && j > 0) {
                        g->player.bombs[i].maxLeft += TILESIZE;
                        break;
                    } else if (mapa[bombIndex.y][j] == 1 && j > 0) {
                        break;
                    }
                }

                break;
            }
        }
    }

    for (int i = 0; i < g->player.num_bombs; i++) {
        if (g->player.bombs[i].isActive == 1) {
            if (fabs(g->player.bombs[i].time - GetTime()) > 3 && fabs(g->player.bombs[i].time - GetTime()) < 5) {
                float grow_tax = g->player.bombs[i].vel;

                if (g->player.bombs[i].explosion_right.width < g->player.bombs[i].distance * TILESIZE) {
                    if (g->player.bombs[i].explosion_right.width + grow_tax <= g->player.bombs[i].maxRight) {
                        g->player.bombs[i].explosion_right.width += grow_tax;
                    }
                }

                if (g->player.bombs[i].explosion_left.width < g->player.bombs[i].distance * TILESIZE) {
                    if (g->player.bombs[i].explosion_left.width + grow_tax <= g->player.bombs[i].maxLeft) {
                        g->player.bombs[i].explosion_left.width += grow_tax;
                        g->player.bombs[i].explosion_left.x -= grow_tax;
                    }
                }

                if (g->player.bombs[i].explosion_up.height < g->player.bombs[i].distance * TILESIZE) {
                    if (g->player.bombs[i].explosion_up.height + grow_tax <= g->player.bombs[i].maxTop) {
                        g->player.bombs[i].explosion_up.height += grow_tax;
                    }
                }

                if (g->player.bombs[i].explosion_down.height < g->player.bombs[i].distance * TILESIZE) {
                    if (g->player.bombs[i].explosion_down.height + grow_tax <= g->player.bombs[i].maxBottom) {
                        g->player.bombs[i].explosion_down.height += grow_tax;
                        g->player.bombs[i].explosion_down.y -= grow_tax;
                    }
                }
            } else if (fabs(g->player.bombs[i].time - GetTime()) > 3) {
                g->player.bombs[i].isActive = 0;
            }
        }
    }

    g->player.put_bomb = 0;
}


int main() {
    game g = {
        .player = {{scene.x, scene.y}, TILESIZE, 0.0, 0.0, 2.5, 0.5, 0, 0, 5, {{},{},{},{},{},{},{},{},{},{}}}
    };
    
    initMapa();

    for (int i = 0; i < NUMTILES_H; i += 2) {
        int x;
        for (int j = 0; j < NUMTILES_W; j += 2) {
            x = GetRandomValue(0, 1);
            if (x && (i > 1 || j > 1)) {
                mapa[i][j] = 2;
            }
        }
    }

    InitWindow(SCREEN_W, SCREEN_H, "Projeto");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if(IsKeyDown(KEY_ENTER)){
            mainMenu();
        } else if(IsKeyDown(KEY_G)) {
            gameOver();
        } else {
            updateGame(&g);
            update_bomb(&g, mapa);
            draw_bomb(&g);
        }
    }

    CloseWindow();

    return 0;
}

void initMapa() {
    for (int i = 0; i < NUMTILES_H; i++) {
        for (int j = 0; j < NUMTILES_W; j++) {
            if (i % 2 && j % 2) {
                mapa[i][j] = 1;
            } else {
                mapa[i][j] = 0;
            }
        }
    }
}

Vector2 getCoords(int x, int y) {
    float coordX = x * TILESIZE + scene.x;
    float coordY = y * TILESIZE + scene.y;
    Vector2 coords = {coordX, coordY};
    return coords;
}

Vector2Int getIndex(float x, float y) {
    int indexX = ((x + TILESIZE / 2) - scene.x) / TILESIZE;
    int indexY = ((y + TILESIZE / 2) - scene.y) / TILESIZE;
    Vector2Int index = {indexX, indexY};
    return index;
}

void prevCollision(game *g, short int mapa[][NUMTILES_W]) {
    Vector2Int playerIndex = getIndex(g->player.pos.x, g->player.pos.y);

    for (int y = playerIndex.y - 1; y <= playerIndex.y + 1; y++) {
        for (int x = playerIndex.x - 1; x <= playerIndex.x + 1; x++) {
            if (mapa[y][x] >= 1 && x >= 0 && x <= NUMTILES_W) {
                Vector2 coords = getCoords(x, y);

                DrawCircle(coords.x + TILESIZE / 2, coords.y + TILESIZE / 2, 4, RED);
                DrawRectangleLines(coords.x, coords.y, TILESIZE, TILESIZE, RED);

                int playerLeft = g->player.pos.x + 2;
                int playerRight = g->player.pos.x + g->player.size - 2;
                int playerTop = g->player.pos.y + 2;
                int playerBottom = g->player.pos.y + g->player.size - 2;

                int tileLeft = coords.x;
                int tileRight = coords.x + TILESIZE;
                int tileTop = coords.y;
                int tileBottom = coords.y + TILESIZE;

                Rectangle playerRect = {g->player.pos.x, g->player.pos.y, g->player.size, g->player.size};
                Rectangle tileRect = {coords.x, coords.y, TILESIZE, TILESIZE};

                if (CheckCollisionRecs(playerRect, tileRect)) {
                    int tolerance = TILESIZE - 10;
                    int move_on_collision = 2.8;

                    int top_dist = tileTop - playerTop;
                    if (top_dist >= tolerance) {
                        g->player.pos.y = tileTop - g->player.size;
                    } else if (top_dist < tolerance && top_dist > 1) {
                        g->player.pos.y -= move_on_collision;
                    }

                    int left_dist = tileLeft - playerLeft;
                    if (left_dist >= tolerance) {
                        g->player.pos.x = tileLeft - g->player.size;
                    } else if (left_dist < tolerance && left_dist > 1) {
                        g->player.pos.x -= move_on_collision;
                    }

                    int bottom_dist = playerBottom - tileBottom;
                    if (bottom_dist >= tolerance) {
                        g->player.pos.y = tileBottom;
                    } else if (bottom_dist < tolerance && bottom_dist > 1) {
                        g->player.pos.y += move_on_collision;
                    }

                    int right_dist = playerRight - tileRight;
                    if (right_dist >= tolerance) {
                        g->player.pos.x = tileRight;
                    } else if (right_dist < tolerance && right_dist > 1) {
                        g->player.pos.x += move_on_collision;
                    }
                }
            }
        }
    }
}

void getLastKey() {
    if (IsKeyPressed(KEY_W)) lastKey = KEY_W;
    if (IsKeyPressed(KEY_A)) lastKey = KEY_A;
    if (IsKeyPressed(KEY_S)) lastKey = KEY_S;
    if (IsKeyPressed(KEY_D)) lastKey = KEY_D;

    if (IsKeyReleased(KEY_W) || IsKeyReleased(KEY_A) || IsKeyReleased(KEY_S) || IsKeyReleased(KEY_D)) {
        if (IsKeyDown(KEY_D))
            lastKey = KEY_D;
        else if (IsKeyDown(KEY_S))
            lastKey = KEY_S;
        else if (IsKeyDown(KEY_A))
            lastKey = KEY_A;
        else if (IsKeyDown(KEY_W))
            lastKey = KEY_W;
        else
            lastKey = KEY_NULL;
    }
}

void updateMove(game *g) {
    getLastKey();
    if (lastKey == KEY_W && g->player.pos.y > scene.y)
        g->player.velY -= g->player.acc;
    else if (g->player.pos.y <= scene.y)
        g->player.pos.y = scene.y;

    if (lastKey == KEY_S && g->player.pos.y < sceneHeight + scene.y - g->player.size)
        g->player.velY += g->player.acc;
    else if (g->player.pos.y >= sceneHeight + scene.y - g->player.size)
        g->player.pos.y = sceneHeight + scene.y - g->player.size;

    if (lastKey == KEY_A && g->player.pos.x >= scene.x)
        g->player.velX -= g->player.acc;
    else if (g->player.pos.x < scene.x)
        g->player.pos.x = 0 + scene.x;

    if (lastKey == KEY_D && g->player.pos.x <= sceneWidth + scene.x - g->player.size)
        g->player.velX += g->player.acc;
    else if (g->player.pos.x >= sceneWidth + scene.x - g->player.size)
        g->player.pos.x = sceneWidth + scene.x - g->player.size;

    g->player.pos.x += g->player.velX;
    g->player.pos.y += g->player.velY;

    g->player.velX *= g->player.fric;
    g->player.velY *= g->player.fric;
}

void updateGame(game *g) {
    updateMove(g);
    DrawRectangle(scene.x, scene.y, sceneWidth, sceneHeight, GREEN);

    for (int i = 0; i < NUMTILES_H; i++) {
        for (int j = 0; j < NUMTILES_W; j++) {
            if (mapa[i][j] == 1) {
                short int tileX = scene.x + j * TILESIZE;
                short int tileY = scene.y + i * TILESIZE;
                DrawRectangle(tileX, tileY, TILESIZE, TILESIZE, DARKGRAY);
            } else if (mapa[i][j] == 2) {
                short int tileX = scene.x + j * TILESIZE;
                short int tileY = scene.y + i * TILESIZE;
                DrawRectangle(tileX, tileY, TILESIZE, TILESIZE, WHITE);
            }
        }
    }

    debug(g);

    BeginDrawing();

    ClearBackground(BLACK);

    for (int x = 0; x <= sceneWidth; x += TILESIZE) {
        DrawLine(scene.x + x, scene.y, scene.x + x, SCREEN_H / 2 + sceneHeight / 2, DARKGRAY);
    }

    for (int y = 0; y <= sceneHeight; y += TILESIZE) {
        DrawLine(scene.x, scene.y + y, SCREEN_W / 2 + sceneWidth / 2, scene.y + y, DARKGRAY);
    }

    prevCollision(g, mapa);
    DrawRectangle(g->player.pos.x, g->player.pos.y, g->player.size, g->player.size, BLUE);

    EndDrawing();
}

void mainMenu() {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawText("Bomb SUS", SCREEN_W/2 - 125, 50, 50, BLUE);

    DrawRectangle(SCREEN_W/2 - 125, SCREEN_H/2 - 50, 250, 100, BLUE);
    EndDrawing();
}

void gameOver() {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Game Over!", SCREEN_W/2 - 75, 300, 25, WHITE);
    EndDrawing();
}

void debug(game *g) {
    char playerXText[64];
    char playerYText[64];

    snprintf(playerXText, sizeof(playerXText), "Player X: %.2f", g->player.pos.x);
    snprintf(playerYText, sizeof(playerYText), "Player Y: %.2f", g->player.pos.y);

    DrawText(playerXText, 10, 10, 18, DARKGRAY);
    DrawText(playerYText, 10, 30, 18, DARKGRAY);

    DrawCircle(g->player.pos.x, g->player.pos.y, 5, RED);
    DrawCircle(scene.x, scene.y, 10, RED);
}
