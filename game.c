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
    float dt;
} Data_Struct;

static int counter = 0;
char temp_buffer[128] = {0};

#define OUTFILE "./frames/cube/"

void game_init(Screen* s, Data_Struct* ds) {
}

void game_update(Screen* s, Data_Struct* ds) {
    screen_clear(s, 0);
}



