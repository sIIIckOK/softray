#include "render.c"
#include <stdint.h>
#include <stdio.h>
#include "common.c"

#define SCREEN_RATIO_Y (1)
#define SCREEN_RATIO_X (1)

#define SCREEN_FACTOR (500)
#define SCREEN_WIDTH  (SCREEN_RATIO_X*SCREEN_FACTOR) 
#define SCREEN_HEIGHT (SCREEN_RATIO_Y*SCREEN_FACTOR)

static uint32_t screen_pixels[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};
static uint32_t screen_pixels_depth[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};

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
    Cam camera;
    float dt;
} Core_Data;

typedef struct {
    Vec3 pos;
    Vec3 vel;
    Vec3 acc;

    float damp;
} Entity;

typedef struct {
    Object obj;
    Object new_obj;
} Data_Struct;

#define OUTFILE "./frames/"

void game_init(Core_Data* core, Data_Struct* ds) {
    float depth = 0.6;
    core->camera.pos.z = -5;

    bool ok = obj_load_file("./objects/cube.obj", &ds->new_obj);
    if (!ok) return;

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
            .z = depth + 1,
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
            .z = depth + 2,
        },
        .color = {
            0x00,
            0x00,
            0xff,
            .a = 0xff,
        }
    };
    da_append(&ds->obj.vertices, v1);
    da_append(&ds->obj.vertices, v2);
    da_append(&ds->obj.vertices, v3);

    da_append(&ds->obj.face_idx, 1);
    da_append(&ds->obj.face_idx, 2);
    da_append(&ds->obj.face_idx, 3);
}

void game_update(Screen* s, Core_Data* core, Data_Struct* ds) {
    screen_clear(s, 0xff181818);

    #define CAM_WALK_SPEED (3)
    #define CAM_RUN_SPEED  (CAM_WALK_SPEED*3)
    float cam_speed = CAM_WALK_SPEED;

    if (core->keyboard[' '].held) {
        cam_speed *= CAM_RUN_SPEED;
    }

    if (core->keyboard['W'].held) {
        core->camera.pos.z += core->dt*cam_speed;
    } else if (core->keyboard['S'].held) {
        core->camera.pos.z -= core->dt*cam_speed;
    }

    if (core->keyboard['A'].held) {
        core->camera.pos.x -= core->dt*cam_speed;
    } else if (core->keyboard['D'].held) {
        core->camera.pos.x += core->dt*cam_speed;
    }

    screen_draw_obj_igbetter(s, &core->camera, &ds->new_obj);
}



