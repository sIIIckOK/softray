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
    bool  pressed;
    bool  held;
    bool  released;
    bool  off;
    float held_time;
} Button;

static bool btn_is_pressed(Button btn) { return btn.pressed; }
static bool btn_is_held(Button btn) { return btn.held; }

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
    KEY_MOUSE_NULL,
    KEY_MOUSE_LEFT,
    KEY_MOUSE_RIGHT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_4,
    KEY_MOUSE_5,
    KEY_MOUSE_WHEEL,
    KEY_MOUSE_COUNT,
} Key_Mouse;

typedef struct {
    Vec2 pos;
    Vec2 delta;
    Button button[KEY_MOUSE_COUNT];
} Mouse;

typedef struct {
    Screen screen;
    Keyboard_Keys keyboard;
    Mouse mouse;
    float dt;
} Core_Data;

typedef struct {
    Vec3 pos;
    Vec3 vel;
    Vec3 acc;

    float damp;
} Entity;

typedef struct {

} Data_Struct;

#define OUTFILE "./frames/"

void game_init(Core_Data* core, Data_Struct* ds) {
}

void game_update(Screen* s, Core_Data* core, Data_Struct* ds) {
    screen_clear(s, 0);

    float depth = 1;
    Vertex v1 = {
        .pos = {
            .x = -0.5,
            .y = -0.5,
            .z = depth,
        },
        .color = {
            0xff,
            0x00,
            0x00,
            .a = 0xff,
        }
    };
    Vertex v2 = {
        .pos = {
            .x = 0.5,
            .y = -0.5,
            .z = depth,
        },
        .color = {
            0x00,
            0xff,
            0x00,
            .a = 0xff,
        }
    };
    Vertex v3 = {
        .pos = {
            .x = 0,
            .y = 0.5,
            .z = depth,
        },
        .color = {
            0x00,
            0x00,
            0xff,
            .a = 0xff,
        }
    };

    screen_draw_triangle(s, v1, v2, v3);
}



