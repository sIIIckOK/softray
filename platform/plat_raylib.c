#include "../game.c"
#include "../include/raylib.h"

Texture render_init_screen(Screen* s) {
    Image image = {0};
    image.data = s->pixels;
    image.width = s->width;
    image.height = s->height;
    image.mipmaps = 1;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    return LoadTextureFromImage(image);   
}

void render_screen(Screen* s, RenderTexture rt, Texture t) {
    BeginTextureMode(rt);
    UpdateTexture(t, s->pixels);
    DrawTexture(rt.texture, 0, 0, WHITE); EndTextureMode();
}

int main(int argc, char** argv) {
    char* program_name = shift(&argc, &argv);
    Screen s = {
        .pixels = screen_pixels,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
    };
    
    InitWindow(s.width, s.height, "Hello");
    SetTargetFPS(60);
    Image img = {
        s.pixels,
        s.width, s.height,
        1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    Texture2D tex = LoadTextureFromImage(img);
    game_update(&s);

    while(!WindowShouldClose()) {
        BeginDrawing();
        game_update(&s);

        UpdateTexture(tex, s.pixels);
        DrawTexture(tex, 0, 0, WHITE);

        ClearBackground(BLACK);
        EndDrawing();
    }
}

