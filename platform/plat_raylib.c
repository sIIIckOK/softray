#include "../game.c"
#include "../include/raylib.h"
#include <stdio.h>
#include <stdlib.h>

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


int key_translate_table[] = {
    [KEY_NULL] = KEY_CODE_NULL,
    [KEY_A] = 'A',
    [KEY_B] = 'B',
    [KEY_C] = 'C',
    [KEY_D] = 'D',
    [KEY_E] = 'E',
    [KEY_F] = 'F',
    [KEY_G] = 'G',
    [KEY_H] = 'H',
    [KEY_I] = 'I',
    [KEY_J] = 'J',
    [KEY_K] = 'K',
    [KEY_L] = 'L',
    [KEY_M] = 'M',
    [KEY_N] = 'N',
    [KEY_O] = 'O',
    [KEY_P] = 'P',
    [KEY_Q] = 'Q',
    [KEY_R] = 'R',
    [KEY_S] = 'S',
    [KEY_T] = 'T',
    [KEY_U] = 'U',
    [KEY_V] = 'V',
    [KEY_W] = 'W',
    [KEY_X] = 'X',
    [KEY_Y] = 'Y',
    [KEY_Z] = 'Z',

    [KEY_ZERO]  = '0',
    [KEY_ONE]   = '1',
    [KEY_TWO]   = '2',
    [KEY_THREE] = '3',
    [KEY_FOUR]  = '4',
    [KEY_FIVE]  = '5',
    [KEY_SIX]   = '6',
    [KEY_SEVEN] = '7',
    [KEY_EIGHT] = '8',
    [KEY_NINE]  = '9',

    [KEY_SPACE]         = ' ',
    [KEY_COMMA]         = ',',
    [KEY_PERIOD]        = '.',
    [KEY_SLASH]         = '/',
    [KEY_SEMICOLON]     = ';',
    [KEY_APOSTROPHE]    = '\'',
    [KEY_LEFT_BRACKET]  = '[',
    [KEY_RIGHT_BRACKET] = ']',
    [KEY_BACKSLASH]     = '\\',
    [KEY_MINUS]         = '-',
    [KEY_EQUAL]         = '=',

    [KEY_LEFT_SHIFT]    = KEY_CODE_SHIFT_L,
    [KEY_RIGHT_SHIFT]   = KEY_CODE_SHIFT_R,

    [KEY_LEFT_CONTROL]  = KEY_CODE_CTRL_L,
    [KEY_RIGHT_CONTROL] = KEY_CODE_CTRL_R,

    [KEY_LEFT_ALT]      = KEY_CODE_ALT_L,
    [KEY_RIGHT_ALT]     = KEY_CODE_ALT_R,

    [KEY_ENTER]         = KEY_CODE_ENTER,
    [KEY_ESCAPE]        = KEY_CODE_ESCAPE,

    [KEY_BACKSPACE]     = KEY_CODE_BACKSPACE,
    [KEY_TAB]           = KEY_CODE_TAB,
    [KEY_UP]            = KEY_CODE_UP,
    [KEY_DOWN]          = KEY_CODE_DOWN,
    [KEY_LEFT]          = KEY_CODE_LEFT,
    [KEY_RIGHT]         = KEY_CODE_RIGHT,
};

int mouse_key_translate_translation_table[] = {
    [MOUSE_BUTTON_LEFT]    = KEY_MOUSE_LEFT,
    [MOUSE_BUTTON_RIGHT]   = KEY_MOUSE_RIGHT,
    [MOUSE_BUTTON_FORWARD] = KEY_MOUSE_4,
    [MOUSE_BUTTON_BACK]    = KEY_MOUSE_5,
    [MOUSE_BUTTON_MIDDLE]  = KEY_MOUSE_WHEEL,
};

int key_translate(int key) {
    if (key > ARRAY_SIZE(key_translate_table) || key < 0) {
        return 0;
    }

    return key_translate_table[key];
}

void keyboard_key_poll(Core_Data* cd) {
    for (int i = 0; i < ARRAY_SIZE(cd->keyboard); i++) {
        cd->keyboard[i].was_pressed = cd->keyboard[i].is_pressed;
        cd->keyboard[i].is_pressed = false;
    }

    int key = GetKeyPressed();
    while (key != 0) {
        int my_key = key_translate(key);
        cd->keyboard[my_key].is_pressed = true;
        key = GetCharPressed();
    }
}

void mouse_key_poll(Core_Data* cd) {
    Vector2 pos = GetMousePosition();
    Vector2 del = GetMouseDelta();

    cd->mouse.pos.x = pos.x;
    cd->mouse.pos.y = pos.y;

    cd->mouse.delta.x = del.x;
    cd->mouse.delta.y = del.y;

    cd->mouse.button[KEY_MOUSE_LEFT].was_pressed = cd->mouse.button[KEY_MOUSE_LEFT].is_pressed;
    cd->mouse.button[KEY_MOUSE_LEFT].is_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ? true : false;

    cd->mouse.button[KEY_MOUSE_RIGHT].was_pressed = cd->mouse.button[KEY_MOUSE_RIGHT].is_pressed;
    cd->mouse.button[KEY_MOUSE_RIGHT].is_pressed = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ? true : false;

    cd->mouse.button[KEY_MOUSE_MIDDLE].was_pressed = cd->mouse.button[KEY_MOUSE_MIDDLE].is_pressed;
    cd->mouse.button[KEY_MOUSE_MIDDLE].is_pressed = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) ? true : false;

    cd->mouse.button[KEY_MOUSE_4].was_pressed = cd->mouse.button[KEY_MOUSE_4].is_pressed;
    cd->mouse.button[KEY_MOUSE_4].is_pressed = IsMouseButtonPressed(MOUSE_BUTTON_FORWARD) ? true : false;

    cd->mouse.button[KEY_MOUSE_5].was_pressed = cd->mouse.button[KEY_MOUSE_5].is_pressed;
    cd->mouse.button[KEY_MOUSE_5].is_pressed = IsMouseButtonPressed(MOUSE_BUTTON_BACK) ? true : false;
}


int main(int argc, char** argv) {
    char* program_name = shift(&argc, &argv);
    Screen s = {
        .pixels = screen_pixels,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
    };
    Data_Struct ds = {0};
    Core_Data cd = {0};
    
    InitWindow(s.width, s.height, "Hello");
    Image img = {
        s.pixels,
        s.width, s.height,
        1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    Texture2D tex = LoadTextureFromImage(img);
    game_init(&s, &cd, &ds);

    #define FPS 60
    SetTargetFPS(FPS);
    while(!WindowShouldClose()) {
        ds.dt = GetFrameTime();
        keyboard_key_poll(&cd);
        mouse_key_poll(&cd);

        BeginDrawing();
        game_update(&s, &cd, &ds);

        UpdateTexture(tex, s.pixels);
        DrawTexture(tex, 0, 0, WHITE);

        ClearBackground(BLACK);
        EndDrawing();
    }
}

