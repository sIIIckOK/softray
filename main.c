#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <stdio.h>


#define SCREEN_RATIO_X (16)
#define SCREEN_RATIO_Y (9)

#define SCREEN_FACTOR (100)
#define SCREEN_WIDTH  (SCREEN_RATIO_X*SCREEN_FACTOR) 
#define SCREEN_HEIGHT (SCREEN_RATIO_Y*SCREEN_FACTOR)

static uint32_t screen_pixels[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};

typedef struct {
    uint32_t* pixels;
    size_t width;
    size_t height;
} Screen;

bool screen_to_ppm(const Screen* screen, char* outpath) {
    FILE* f = fopen(outpath, "w");
    if (!f) {
        return false;
        printf("ERROR: %s\n", strerror(errno));
    } 
    char temp[128] = {0};
    sprintf(temp, "P6 %zu %zu 255\n", screen->width, screen->height);
    fwrite(temp, strlen(temp), 1, f);
    for (int dy = 0; dy < screen->height; dy++) {
        for (int dx = 0; dx < screen->width; dx++) {
            uint32_t p = screen->pixels[dx + dy * screen->width];
            uint8_t r = p >> 8*2;
            uint8_t g = p >> 8*1;
            uint8_t b = p >> 8*0;
            fwrite(&r, sizeof(r), 1, f);
            fwrite(&g, sizeof(g), 1, f);
            fwrite(&b, sizeof(b), 1, f);
        }
    }
    return true;
}

void screen_draw_pixel(Screen* screen, int x, int y, uint32_t color) {
    screen->pixels[x + y * screen->width] = color;
}

void screen_draw_rect(Screen* screen, 
                      int x, int y, 
                      int w, int h, 
                      uint32_t color) {
    for (int dy = y; dy < y + h; dy++) {
        for (int dx = x; dx < x + w; dx++) {
            screen->pixels[dx + dy * screen->width] = color;
        }
    }
}


int main() {
    // px = x/z
    // py = y/z
    
    
    Screen s = {
        .pixels = screen_pixels,
        .width = SCREEN_WIDTH,
        .height = SCREEN_HEIGHT,
    };
    #define OUT_FILE "data/"
    char* outpath = OUT_FILE"balls.ppm";
    int i = 0;
    screen_draw_rect(&s, 0, 0, 100, 100, 0xFF0000);
    printf("%x\n", s.pixels[0]);
    screen_to_ppm(&s, outpath);
}

