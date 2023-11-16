#include <stdio.h>
#include <raylib.h>

#define screenWidth 1000
#define screenHeight 600
#define tileSize 40
#define NUMTILES_H 15
#define NUMTILES_W 15

const short int sceneWidth = tileSize * NUMTILES_W;
const short int sceneHeight = tileSize * NUMTILES_H;

const Vector2 scene = {(screenWidth - sceneWidth) / 2, (screenHeight - sceneHeight) / 2};

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

int main() {
    game g = {
        .player = {{scene.x, scene.y}, tileSize, 0.0, 0.0, 2.5, 0.5, {5, 1, 0}}
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

    InitWindow(screenWidth, screenHeight, "Projeto");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if(IsKeyDown(KEY_ENTER)){
            mainMenu();
        } else if(IsKeyDown(KEY_G)) {
            gameOver();
        } else {
            updateGame(&g);
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
    float coordX = x * tileSize + scene.x;
    float coordY = y * tileSize + scene.y;
    Vector2 coords = {coordX, coordY};
    return coords;
}

Vector2Int getIndex(float x, float y) {
    int indexX = ((x + tileSize / 2) - scene.x) / tileSize;
    int indexY = ((y + tileSize / 2) - scene.y) / tileSize;
    Vector2Int index = {indexX, indexY};
    return index;
}

void prevCollision(game *g, short int mapa[][NUMTILES_W]) {
    Vector2Int playerIndex = getIndex(g->player.pos.x, g->player.pos.y);

    for (int y = playerIndex.y - 1; y <= playerIndex.y + 1; y++) {
        for (int x = playerIndex.x - 1; x <= playerIndex.x + 1; x++) {
            if (mapa[y][x] >= 1 && x >= 0) {
                Vector2 coords = getCoords(x, y);

                DrawCircle(coords.x + tileSize / 2, coords.y + tileSize / 2, 4, RED);
                DrawRectangleLines(coords.x, coords.y, tileSize, tileSize, RED);

                int playerLeft = g->player.pos.x + 2;
                int playerRight = g->player.pos.x + g->player.size - 2;
                int playerTop = g->player.pos.y + 2;
                int playerBottom = g->player.pos.y + g->player.size - 2;

                int tileLeft = coords.x;
                int tileRight = coords.x + tileSize;
                int tileTop = coords.y;
                int tileBottom = coords.y + tileSize;

                Rectangle playerRect = {g->player.pos.x, g->player.pos.y, g->player.size, g->player.size};
                Rectangle tileRect = {coords.x, coords.y, tileSize, tileSize};

                if (CheckCollisionRecs(playerRect, tileRect)) {
                    int tolerance = tileSize - 10;
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
                short int tileX = scene.x + j * tileSize;
                short int tileY = scene.y + i * tileSize;
                DrawRectangle(tileX, tileY, tileSize, tileSize, DARKGRAY);
            } else if (mapa[i][j] == 2) {
                short int tileX = scene.x + j * tileSize;
                short int tileY = scene.y + i * tileSize;
                DrawRectangle(tileX, tileY, tileSize, tileSize, WHITE);
            }
        }
    }

    debug(g);

    BeginDrawing();

    ClearBackground(BLACK);

    for (int x = 0; x <= sceneWidth; x += tileSize) {
        DrawLine(scene.x + x, scene.y, scene.x + x, screenHeight / 2 + sceneHeight / 2, DARKGRAY);
    }

    for (int y = 0; y <= sceneHeight; y += tileSize) {
        DrawLine(scene.x, scene.y + y, screenWidth / 2 + sceneWidth / 2, scene.y + y, DARKGRAY);
    }

    prevCollision(g, mapa);
    DrawRectangle(g->player.pos.x, g->player.pos.y, g->player.size, g->player.size, BLUE);

    EndDrawing();
}

void mainMenu() {
    BeginDrawing();
    ClearBackground(WHITE);
    DrawText("Bomb SUS", screenWidth/2 - 125, 50, 50, BLUE);

    DrawRectangle(screenWidth/2 - 125, screenHeight/2 - 50, 250, 100, BLUE);
    EndDrawing();
}

void gameOver() {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Game Over!", screenWidth/2 - 75, 300, 25, WHITE);
    EndDrawing();
}

void debug(game *g) {
    char playerXText[64];
    char playerYText[64];
    char playerBombs[64];

    snprintf(playerXText, sizeof(playerXText), "Player X: %.2f", g->player.pos.x);
    snprintf(playerYText, sizeof(playerYText), "Player Y: %.2f", g->player.pos.y);
    snprintf(playerBombs, sizeof(playerBombs), "Player Bombs: %d", g->player.properties.maxBombs);

    DrawText(playerXText, 10, 10, 18, DARKGRAY);
    DrawText(playerYText, 10, 30, 18, DARKGRAY);
    DrawText(playerBombs, 10, 60, 18, DARKGRAY);

    DrawCircle(g->player.pos.x, g->player.pos.y, 5, RED);
    DrawCircle(scene.x, scene.y, 10, RED);
}
