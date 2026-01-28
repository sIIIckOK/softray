#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "common.c"

#include <stdio.h>
#include <sys/types.h>


#define NEAR_CLIP_PLANE  (0.5)
#define FOREGROUND_COLOR (0xff00ff00)
#define MY_RED           (0xff0000ff)

typedef struct {
    uint32_t* pixels;
    size_t width;
    size_t height;
} Screen;

typedef struct {
    Vec3 pos;
} Cam;

typedef struct {
    Vec3 pos;
    Col color;
} Vertex;
typedef struct {
    Vec2i pos;
    Col   color;
} Screen_Vertex;

typedef struct {
    Vertex* items;
    size_t count;
    size_t capacity;
} Vertices;

typedef struct {
    size_t* items;
    size_t count;
    size_t capacity;
} Indices;

typedef struct {
    Vertices vertices;
    Indices face_idx;
} Object;

uint32_t col_getint(Col c) {
    uint32_t result = 
        c.r << 8*0 |
        c.g << 8*1 |
        c.b << 8*2 |
        c.a << 8*3;
    return result;
}

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
    printf("[INFO] Generated %s\n", outpath);
    return true;
}

static inline void screen_put_pixel(Screen* screen, int x, int y, uint32_t color) {
    if (x >= 0 && y >= 0 && 
        x < screen->width && y < screen->height) {
        screen->pixels[x + y * screen->width] = color;
    }
}

static inline uint32_t screen_get_color(const Screen* screen, int x, int y) {
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
            screen_put_pixel(screen, dx, dy, color);
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
                screen_put_pixel(screen, dx, dy, color);
            }
        }
    }
}

// Uses Bresenham's Algorithm
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
        screen_put_pixel(screen, x, y, color);
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

typedef struct {
    Vec2i v;
    bool ok;
} Vec2i_Result;

Vec2i_Result screen_project(const Screen* s, Vec3 p) {
    if (p.z <= NEAR_CLIP_PLANE) {
        return (Vec2i_Result) {
            .v = {0},
            .ok = false,
        };
    }
    float x = ((p.x/p.z) + 1)/2;
    float y = ((p.y/p.z) + 1)/2;
    return (Vec2i_Result) {
        .v = {
            .x = x*s->width,
            .y = y*s->height,
        },
        .ok = true,
    };
}
float _sign(Vec2i p1, Vec2i p2, Vec2i p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
bool point_in_triangle (Vec2i pt, Vec2i v1, Vec2i v2, Vec2i v3)
{
    float d1, d2, d3;
    bool has_neg, has_pos;
    d1 = _sign(pt, v1, v2);
    d2 = _sign(pt, v2, v3);
    d3 = _sign(pt, v3, v1);
    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(has_neg && has_pos);
}

void screen_draw_triangle(Screen* s, Screen_Vertex v1, Screen_Vertex v2, Screen_Vertex v3) {
    Vec2i p1 = v1.pos;
    Vec2i p2 = v2.pos;
    Vec2i p3 = v3.pos;
    
    int big_tri_area = triangle2d_area(p1, p2, p3);

    int min_x = MIN(p1.x, MIN(p2.x, p3.x));
    int max_x = MAX(p1.x, MAX(p2.x, p3.x));

    int min_y = MIN(p1.y, MIN(p2.y, p3.y));
    int max_y = MAX(p1.y, MAX(p2.y, p3.y));

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (x < 0 || x > s->width || y < 0 || y > s->width) continue;
            Vec2i this_point = { x, y };
            if (!point_in_triangle(this_point, v1.pos, v2.pos, v3.pos)) continue;

            float u = (float)triangle2d_area(this_point, p2,         p3)        /big_tri_area;
            float v = (float)triangle2d_area(p1,         this_point, p3)        /big_tri_area;
            float w = (float)triangle2d_area(p1,         p2,         this_point)/big_tri_area;

            Col color = {0};
            color.r = v1.color.r*u + v2.color.r*v + v3.color.r*w;
            color.g = v1.color.g*u + v2.color.g*v + v3.color.g*w;
            color.b = v1.color.b*u + v2.color.b*v + v3.color.b*w;
            color.a = v1.color.a*u + v2.color.a*v + v3.color.a*w;

            uint32_t color_int = col_getint(color);
            screen_put_pixel(s, x, y, color_int);
        }
    }
}

typedef struct {
    Screen_Vertex v;
    bool ok;
} Screen_Vertex_Result;

static inline Screen_Vertex_Result screen_project_vertex(const Screen* s, Vertex v) {
    Vec2i_Result res = screen_project(s, v.pos);
    return (Screen_Vertex_Result) {
        .v = {
            .pos = res.v,
            .color = v.color,
        },
        .ok = res.ok,
    };
}

Vec3 camera_project(const Cam* c, Vec3 v) {
    return (Vec3) {
        .x = v.x - c->pos.x,
        .y = v.y - c->pos.y,
        .z = v.z - c->pos.z,
    };
}

void screen_draw_object(Screen* s, Cam* c, Object obj) {
    for (int i = 0; i < obj.vertices.count; i += 3) {
        #define VERTS_IN_TRIANGLE 3
        Vertex vs[3] = {0};
        Screen_Vertex_Result vs_res[3] = {0};
        for (int vi = 0; vi < VERTS_IN_TRIANGLE; vi++) {
            vs[vi] = obj.vertices.items[i+vi];
            vs[vi].pos = camera_project(c, vs[vi].pos); 
            vs_res[vi] = screen_project_vertex(s, vs[vi]);
        }
        if (vs_res[0].ok && vs_res[1].ok && vs_res[2].ok) {
            screen_draw_triangle(s, vs_res[0].v, vs_res[1].v, vs_res[2].v);
        }
    }
}


void screen_print(const Screen* s) {
    for (int y = 0; y < s->height; y++) {
        printf("[ ");
        for (int x = 0; x < s->width; x++) {
            printf("%x ", screen_get_color(s, x, y));
        }
        printf("\b]\n");
    }
}

void screen_clear(Screen* s, uint32_t color) {
    screen_draw_rect(s, 0, 0, s->width, s->height, color);
}


