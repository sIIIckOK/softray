#include "render.c"
#include <stdint.h>
#include <stdio.h>
#include <float.h>
#include "common.c"

#define SCREEN_RATIO_X (4)
#define SCREEN_RATIO_Y (3)

#define SCREEN_FACTOR (200)
#define SCREEN_WIDTH  (SCREEN_RATIO_X*SCREEN_FACTOR) 
#define SCREEN_HEIGHT (SCREEN_RATIO_Y*SCREEN_FACTOR)

static uint32_t screen_pixels[SCREEN_WIDTH*SCREEN_HEIGHT]    = {0};
static float screen_pixels_depth[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};

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

static bool btn_is_pressed(Button btn) { 
    return btn.pressed; 
}
static bool btn_is_held(Button btn) { 
    return btn.held; 
}


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
    Object teapot;
    Object sword;
    Object monkey;
} Data_Struct;

#define OUTFILE "./frames/"

typedef struct {
    Vec3 pos;
    Vec3 vel;
    Vec3 acc;

    Model_Mesh* mesh;
    bool occupied;
} Entity;

typedef struct {
    size_t len;
    Entity* entities;
} Entity_Pool;

Entity_Pool entity_pool_new(size_t init_size) {
    Entity_Pool pool;
    pool.entities = malloc(init_size * sizeof(Entity));
    pool.len = init_size;
    return pool;
}

Entity_Pool entity_pool = {0};

bool entity_allocate(Entity_Pool pool, Entity entity) {
    for (int i = 0; i < pool.len; i++) {
        if (pool.entities[i].occupied == false) {
            pool.entities[i] = entity;
            pool.entities[i].occupied = true;
        }
    }
}

Model_Mesh teapot = {0};
Model_Mesh sword = {0};
Model_Mesh monkey = {0};
void game_init(Core_Data* core, Data_Struct* ds) {
    #define MAX_ENTITY_COUNT (1024)
    entity_pool = entity_pool_new(MAX_ENTITY_COUNT);

    core->camera.pos.z = -5;
    core->camera.fov = 90;
    core->camera.rot.y = 0;

    bool ok = mesh_load_from_objfile("./objects/teapot.obj", &teapot);
    if (!ok) return;
    Entity e = (Entity){
        .mesh = &teapot,
    };
    entity_allocate(entity_pool, e);

    ok = mesh_load_from_objfile("./objects/sword.obj", &sword);
    if (!ok) return;
    e = (Entity){
        .mesh = &sword,
    };
    entity_allocate(entity_pool, e);

    ok = mesh_load_from_objfile("./objects/monkey.obj", &monkey);
    if (!ok) return;
    e = (Entity){
        .mesh = &monkey,
    };
    entity_allocate(entity_pool, e);

    ds->teapot.mesh = &teapot;
    ds->teapot.transform.scale = (Vec3) {1, 1, 1};
    ds->teapot.transform.pos   = (Vec3) {0, -2, 4};
    ds->teapot.transform.rot.y += MATH_PI/2;

    ds->sword.mesh = &sword;
    ds->sword.transform.scale = (Vec3) {0.1, 0.1, 0.1};
    ds->sword.transform.pos   = (Vec3) {0, 4, 9};

    ds->monkey.mesh = &monkey;
    ds->monkey.transform.scale = (Vec3) {1, 1, 1};
    ds->monkey.transform.pos   = (Vec3) {10, 10, 10};
}

void depth_buffer_clear(Screen* s) {
    for (int i = 0; i < s->height*s->width; i++) {
        s->depth[i] = 0;
    }
} 


void game_update(Screen* s, Core_Data* core, Data_Struct* ds) {
    screen_clear(s, 0xff181818);
    depth_buffer_clear(s);


    #define CAM_WALK_SPEED (3)
    #define CAM_RUN_SPEED  (CAM_WALK_SPEED*3)
    
    #define SPIN_SPEED (MATH_PI)

    // ds->teapot.transform.rot.y += SPIN_SPEED * core->dt;
    ds->sword.transform.rot.y += SPIN_SPEED * core->dt;

    float cam_speed = CAM_WALK_SPEED;

    
    // Camera stuff
    #define CAMERA_SENS (MATH_PI/12)
    #define ZOOM_SPEED (10.0f)
    #define FOV_MIN    (30.0f) 
    #define FOV_MAX    (120.0f)


    core->camera.fov = CLAMP(core->camera.fov, FOV_MIN, FOV_MAX);

    core->camera.rot.y -= core->mouse.delta.x * CAMERA_SENS*core->dt;
    core->camera.rot.x += core->mouse.delta.y * CAMERA_SENS*core->dt;
    core->camera.rot.x = CLAMP(core->camera.rot.x, DEG_TO_RAD(-69), DEG_TO_RAD(69));

    if (core->keyboard[' '].held) {
        cam_speed = CAM_RUN_SPEED;
    }

    if (core->keyboard['W'].held) {
        core->camera.pos.z += core->dt*cam_speed;
    } else if (core->keyboard['S'].held) {
        core->camera.pos.z -= core->dt*cam_speed;
    }

    if (core->keyboard['A'].held) {
        core->camera.pos.x += core->dt*cam_speed;
    } else if (core->keyboard['D'].held) {
        core->camera.pos.x -= core->dt*cam_speed;
    }

    if (core->keyboard['J'].held) {
        core->camera.pos.y -= core->dt*cam_speed;
    } else if (core->keyboard['K'].held) {
        core->camera.pos.y += core->dt*cam_speed;
    }

    if (core->keyboard['I'].held) {
        core->camera.fov -= core->dt * ZOOM_SPEED;
    } else if (core->keyboard['O'].held) {
        core->camera.fov += core->dt * ZOOM_SPEED;
    }

    screen_draw_obj(s, &core->camera, &ds->teapot);
    screen_draw_obj(s, &core->camera, &ds->sword);
    screen_draw_obj(s, &core->camera, &ds->monkey);
}



