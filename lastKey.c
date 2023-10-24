#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib - Última tecla pressionada");

    KeyboardKey lastKey = KEY_NULL;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Se alguma das quatro teclas for pressionada, atualiza a variável lastKey
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

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(TextFormat("Última tecla pressionada: %c", lastKey), 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
