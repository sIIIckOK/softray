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
    FILE* f = fopen(outpath, "wb");
    if (!f) {
        return false;
        printf("ERROR: %s\n", strerror(errno));
    } 
    char temp[128] = {0};
    sprintf(temp, "P6\n%zu %zu\n255\n", screen->width, screen->height);
    fwrite(temp, strlen(temp), 1, f);
    for (int dy = 0; dy < screen->height; dy++) {
        for (int dx = 0; dx < screen->width; dx++) {
            uint32_t p = screen->pixels[dx + dy * screen->width];
            uint8_t r = p >> 8*0;
            uint8_t g = p >> 8*1;
            uint8_t b = p >> 8*2;
            fwrite(&r, sizeof(r), 1, f);
            fwrite(&g, sizeof(g), 1, f);
            fwrite(&b, sizeof(b), 1, f);
        }
    }
    fclose(f);
    printf("INFO: Generated %s\n", outpath);
    return true;
}

static inline void screen_draw_pixel(Screen* screen, int x, int y, uint32_t color) {
    if (x < screen->width && y < screen->height) {
        screen->pixels[x + y * screen->width] = color;
    }
}

static inline uint32_t screen_get_color_at(const Screen* screen, int x, int y) {
    if (x < screen->width && y < screen->height) {
        return screen->pixels[x + y * screen->width];
    }
    return 0;
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

#define MIN(a, b) ((a) < (b) ? (a) : (b))   
#define MAX(a, b) ((a) > (b) ? (a) : (b))   

void screen_draw_line_naive(Screen* screen, 
                      int start_x, int start_y, 
                      int end_x, int end_y,
                      uint32_t color) 
{
    int min_x = MIN(start_x, end_x);
    int max_x = MAX(start_x, end_x);
    int min_y = MIN(start_y, end_y);
    int max_y = MAX(start_y, end_y);

    int del_x = max_x - min_x;
    int del_y = max_y - min_y;

    float slope = 1;
    if (del_x != 0) slope = (float)del_y/del_x;
    float b = min_y - slope * min_x;

    for (int y = min_y; y < max_y; y++) {
        for (int x = min_x; x < max_x; x++) {
            float eqn = slope*x + b;
            if (y >= eqn - 1 && 
                y <= eqn + 1) {
                screen_draw_pixel(screen, x, y, color);
            }
        }
    }
}

void screen_draw_line(Screen* screen, 
                      int start_x, int start_y, 
                      int end_x, int end_y,
                      uint32_t color) 
{
    int x = start_x;
    int y = start_y;
    int dx = abs(end_x - start_x);
    int dy = abs(end_y - start_y);
    int sx = start_x < end_x ? 1 : -1;
    int sy = start_y < end_y ? 1 : -1;
    int err = dx - dy;
    while (true) {
        screen_draw_pixel(screen, x, y, color);
        if (x == end_x && y == end_y) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}
void screen_draw_line_thickness(Screen* screen, 
                      int start_x, int start_y, 
                      int end_x, int end_y,
                      int thickness,
                      uint32_t color) 
{
    int x = start_x;
    int y = start_y;
    int dx = abs(end_x - start_x);
    int dy = abs(end_y - start_y);
    int sx = start_x < end_x ? 1 : -1;
    int sy = start_y < end_y ? 1 : -1;
    int err = dx - dy;
    while (true) {
        screen_draw_rect(screen, x, y, thickness, thickness, color);
        if (x == end_x && y == end_y) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}




