// Se alguma das quatro teclas for solta, verifica qual foi a última tecla pressionada
if (IsKeyReleased(KEY_W) || IsKeyReleased(KEY_A) || IsKeyReleased(KEY_S) || IsKeyReleased(KEY_D))
{
    // Percorre as quatro teclas em ordem inversa e atribui a primeira que estiver pressionada
    if (IsKeyDown(KEY_D)) lastKey = KEY_D;
    else if (IsKeyDown(KEY_S)) lastKey = KEY_S;
    else if (IsKeyDown(KEY_A)) lastKey = KEY_A;
    else if (IsKeyDown(KEY_W)) lastKey = KEY_W;
    else lastKey = KEY_NULL; // Nenhuma das quatro teclas está pressionada
}

// Desenha na tela a última tecla pressionada
BeginDrawing();
ClearBackground(RAYWHITE);
DrawText(TextFormat("Última tecla pressionada: %c", lastKey), 10, 10, 20, DARKGRAY);
EndDrawing();
