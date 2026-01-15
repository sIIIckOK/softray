#include "render.c"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_RATIO_X (16)
#define SCREEN_RATIO_Y (9)

#define SCREEN_FACTOR (10)
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

void game_update(Screen* s) {
    render_balls(s, 10);
}





