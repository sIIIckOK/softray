#include "render.c"
#include <stdint.h>
#include <stdio.h>
#include "common.c"

#define SCREEN_RATIO_X (1)
#define SCREEN_RATIO_Y (1)

#define SCREEN_FACTOR (1000)
#define SCREEN_WIDTH  (SCREEN_RATIO_X*SCREEN_FACTOR) 
#define SCREEN_HEIGHT (SCREEN_RATIO_Y*SCREEN_FACTOR)

static uint32_t screen_pixels[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};

char* shift(int* argc, char*** argv) {
    if (*argc <= 0) {
        printf("[ERROR] no arguments left to shift\n");
        return 0;
    }
    --(*argc);
    return ++*(*argv);
}

typedef struct {
    bool  is_pressed;
    bool  was_pressed;
    float held_time;
} Button;

#define KEY_CHAR_END (257)
typedef enum {
    KEY_CODE_NULL = 0,

    // ..chars..
    
    KEY_CODE_CTRL_L = 257,
    KEY_CODE_CTRL_R,

    KEY_CODE_SHIFT_L,
    KEY_CODE_SHIFT_R,

    KEY_CODE_ALT_L,
    KEY_CODE_ALT_R,

    KEY_CODE_ENTER,
    KEY_CODE_ESCAPE,
    KEY_CODE_BACKSPACE,
    KEY_CODE_TAB,
    KEY_CODE_UP,
    KEY_CODE_DOWN,
    KEY_CODE_LEFT,
    KEY_CODE_RIGHT,

    KEY_CODE_COUNT,
} KEY_CODE;

typedef Button Keyboard_Keys[KEY_CODE_COUNT];

typedef enum {
    KEY_MOUSE_LEFT,
    KEY_MOUSE_RIGHT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_4,
    KEY_MOUSE_5,
    KEY_MOUSE_COUNT,
} Key_Mouse;

typedef struct {
    Vec2 pos;
    Key_Mouse button[KEY_MOUSE_COUNT];
} Mouse;

typedef struct {
    Keyboard_Keys keyboard;
    Mouse mouse;
} Core_Data;

typedef struct {
    float dt;
} Data_Struct;

static int counter = 0;
char temp_buffer[128] = {0};

#define OUTFILE "./frames/cube/"

void game_init(Screen* s, Core_Data* cs, Data_Struct* ds) {
}

void game_update(Screen* s, Core_Data* cs, Data_Struct* ds) {
    screen_clear(s, 0);
    if (cs->keyboard['W'].is_pressed) {
        printf("balls from inside the game\n");
    }
}



