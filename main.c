#include "render.c"
#include <stdio.h>

#define SCREEN_RATIO_X (16)
#define SCREEN_RATIO_Y (9)

#define SCREEN_FACTOR (100)
#define SCREEN_WIDTH  (SCREEN_RATIO_X*SCREEN_FACTOR) 
#define SCREEN_HEIGHT (SCREEN_RATIO_Y*SCREEN_FACTOR)

static uint32_t screen_pixels[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};

void render_balls(Screen* screen, size_t r) {
    int count = screen->width/r;
    for (int y = 0; y < count; y++) {
        for (int x = 0; x < count; x++) {
            screen_draw_circle(screen, r+y*r*2, r+x*r*2, r, 0xFF00000);
        }
    }
}

int main() {
    Screen s = {
        .pixels = screen_pixels,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
    };
    #define OUT_DIR "frames/"
    render_balls(&s, 40);
    char* outpath = OUT_DIR"balls.ppm";
    screen_to_ppm(&s, outpath);
}


