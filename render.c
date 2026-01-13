#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <stdio.h>



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
    printf("INFO: Generated %s\n", outpath);
    return true;
}

static inline void screen_draw_pixel(Screen* screen, int x, int y, uint32_t color) {
    if (x < screen->width && y < screen->height) {
        screen->pixels[x + y * screen->width] = color;
    }
}

void screen_draw_rect(Screen* screen, 
                      int x, int y, 
                      int w, int h, 
                      uint32_t color) {
    for (int dy = y; dy < y + h; dy++) {
        for (int dx = x; dx < x + w; dx++) {
            screen_draw_pixel(screen, dx, dy, color);
        }
    }
}

void screen_draw_circle(Screen* screen, 
                      int x, int y, 
                      int r,
                      uint32_t color) {
    for (int dy = y - r; dy < y + r; dy++) {
        int _dy = y - dy;
        int _dy2 = _dy*_dy;
        for (int dx = x - r; dx < x + r; dx++) {
            int _dx = x - dx;
            if (_dx*_dx + _dy2 <= r*r) {
                screen_draw_pixel(screen, dx, dy, color);
            }
        }
    }
}


