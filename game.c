#include "render.c"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_RATIO_X (1)
#define SCREEN_RATIO_Y (1)

#define SCREEN_FACTOR (1000)
#define SCREEN_WIDTH  (SCREEN_RATIO_X*SCREEN_FACTOR) 
#define SCREEN_HEIGHT (SCREEN_RATIO_Y*SCREEN_FACTOR)

static uint32_t screen_pixels[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};

#define LOG_INFO(...) do { printf("[%s:%d] INFO: %s\n", __FILE__, __LINE__, __VA_ARGS__); } while(0);
#define LOG_ERROR(...) do { printf("ERROR [%s:%d]: %s\n", __FILE__, __LINE__, __VA_ARGS__); } while(0);

char* shift(int* argc, char*** argv) {
    if (*argc <= 0) {
        LOG_ERROR("no arguments left");
        return 0;
    }
    --(*argc);
    return ++*(*argv);
}

void render_balls(Screen* screen, size_t r) {
    int count = screen->width/r;
    for (int y = 0; y < count; y++) {
        for (int x = 0; x < count; x++) {
            screen_draw_circle(screen, r+y*r*2, r+x*r*2, r, 0xFF0000FF);
        }
    }
}

void screen_print(const Screen* s) {
    for (int y = 0; y < s->height; y++) {
        printf("[ ");
        for (int x = 0; x < s->width; x++) {
            printf("%x ", screen_get_color_at(s, x, y));
        }
        printf("\b]\n");
    }
}

void screen_clear(Screen* s, uint32_t color) {
    screen_draw_rect(s, 0, 0, s->width, s->height, color);
}

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    Vec3 verts[8];
} Cube;

typedef struct {
    Cube c;
    float theta;

    float dt;

    int frame_count;
} Game_Struct;


Vec2 screen_project(const Screen* s, Vec3 p) {
    float _x = (((float)p.x/p.z) + 1)/2;
    float _y = (((float)p.y/p.z) + 1)/2;
    return (Vec2) {
        .x = _x * s->width,
        .y = _y * s->height,
    };
}
#define FOREGROUND_COLOR 0xff00ff00

void screen_draw_cube(Screen* s, Cube c) {
    #define CUBE_VERT_CNT 8
    for (int i = 0; i < CUBE_VERT_CNT/2; i++) {
        Vec2 p1 = screen_project(s, c.verts[i]);
        Vec2 p2 = screen_project(s, c.verts[(i+1) % (CUBE_VERT_CNT/2)]);
        screen_draw_line(s, p1.x, p1.y, p2.x, p2.y, FOREGROUND_COLOR);
    }
    for (int i = CUBE_VERT_CNT/2; i < CUBE_VERT_CNT; i++) {
        Vec2 p1 = screen_project(s, c.verts[i]);
        Vec2 p2 = screen_project(s, c.verts[CUBE_VERT_CNT/2 + (i+1) % (CUBE_VERT_CNT/2)]);
        screen_draw_line(s, p1.x, p1.y, p2.x, p2.y, FOREGROUND_COLOR);
    }
    for (int i = 0; i < CUBE_VERT_CNT/2; i++) {
        Vec2 p1 = screen_project(s, c.verts[i]);
        Vec2 p2 = screen_project(s, c.verts[i+4]);
        screen_draw_line(s, p1.x, p1.y, p2.x, p2.y, FOREGROUND_COLOR);
    }
}

#define MATH_PI (22.0f/7.0f)

void game_init(Screen* s, Game_Struct* gs) {
    gs->c = (Cube){
        .verts =  {
            {-0.5, -0.5, 2.5},
            {0.5, -0.5, 2.5},
            {0.5, 0.5, 2.5},
            {-0.5, 0.5, 2.5},

            {-0.5, -0.5, 1.5},
            {0.5, -0.5, 1.5},
            {0.5, 0.5, 1.5},
            {-0.5, 0.5, 1.5},
        }
    };
    gs->theta = MATH_PI/6;

    for (int i = 0; i < CUBE_VERT_CNT; i++) {
        
        // rotate
        float x = gs->c.verts[i].x;
        float z = gs->c.verts[i].z;
        float sin_t = sin(gs->theta);
        float cos_t = cos(gs->theta);
        printf("sin_t %f, cos_t %f\n", sin_t, cos_t);

        gs->c.verts[i].x = x*cos_t + z*sin_t;
        gs->c.verts[i].z = x*-sin_t + z*cos_t;

        printf("vert.x %f, vert.z %f\n", gs->c.verts[i].x, gs->c.verts[i].z);
    }
}

void game_update(Screen* s, Game_Struct* gs) {
    screen_clear(s, 0);

    for (int i = 0; i < CUBE_VERT_CNT; i++)
        gs->c.verts[i].z += gs->dt;

    screen_draw_cube(s, gs->c);

    // char temp_buf[128] = {0};
    // sprintf(temp_buf, "./frames/cube/frame%i.ppm", gs->frame_count++);
    // screen_to_ppm(s, temp_buf);
}



