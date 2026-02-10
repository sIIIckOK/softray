#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "common.c"

#include <stdio.h>
#include <sys/types.h>


#define NEAR_CLIP_PLANE  (0.25)
#define FOREGROUND_COLOR (0xff00ff00)
#define MY_RED           (0xff0000ff)

typedef struct {
    float* depth;
    uint32_t* pixels;
    size_t width;
    size_t height;
} Screen;

typedef struct {
    Vec3 pos;
    float fov;
} Cam;

typedef struct {
    Vec3 pos;
    Col color;
} Vertex;

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
    // {v1, v2, v3, v4, v5....}
    // { 0, 1, 2, 1, 2, 3}
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

typedef struct {
    Vec3 v;
    bool ok;
} Vec3_Result;

Vec2 vec3_to_vec2(Vec3 v3) {
    return (Vec2) {
        .x = v3.x,
        .y = v3.y,
    };
}

Vec3_Result screen_project(const Screen* s, const Cam* c, Vec3 p) {
    if (p.z <= NEAR_CLIP_PLANE) {
        return (Vec3_Result) {
            .v = {0},
            .ok = false,
        };
    }
    float aspect = 16.0f/10;
    float fov_rad = c->fov * MATH_PI/180;
    float fov_x = (tan(fov_rad/2) * 2) * aspect;
    float fov_y = (tan(fov_rad/2) * 2);
    float x = p.x/(-p.z * fov_x);
    float y = p.y/(-p.z * fov_y);
    return (Vec3_Result) {
        .v = {
            .x = ((x + 1)/2)*s->width,
            .y = ((y + 1)/2)*s->height,
            .z = p.z
        },
        .ok = true,
    };
}

bool edge_function(Vec2 v1, Vec2 v2, Vec2 p) {
    return (cross2d(v1.x - v2.x, v1.y - v2.y, v1.x - p.x, v1.y - p.y) > 0);
}

bool is_point_in_triangle(Vec2 p, Vec2 v1, Vec2 v2, Vec2 v3) {
    return (edge_function(v1, v2, p) && edge_function(v2, v3, p) && edge_function(v3, v1, p));
}

bool is_triangle_front(Vec3 v1, Vec3 v2, Vec3 v3) {
    Vec3 va = vec3_sub(v2, v1);
    Vec3 vb = vec3_sub(v3, v1);

    return cross2d(va.x, va.y, vb.x, vb.y) > 0;
}

static inline float edge_coeff(float x0, float y0, float x1, float y1, float px, float py) {
    return (px - x0)*(y1 - y0) - (py - y0)*(x1 - x0);
}

void screen_draw_triangle(Screen* s, Vertex v1, Vertex v2, Vertex v3) {
    Vec2 p1 = vec3_to_vec2(v1.pos);
    Vec2 p2 = vec3_to_vec2(v2.pos);
    Vec2 p3 = vec3_to_vec2(v3.pos);

    int min_x = ceilf(MIN(p1.x, MIN(p2.x, p3.x)));
    int max_x = floorf(MAX(p1.x, MAX(p2.x, p3.x)));
    int min_y = ceilf(MIN(p1.y, MIN(p2.y, p3.y)));
    int max_y = floorf(MAX(p1.y, MAX(p2.y, p3.y)));

    float tri_area = edge_coeff(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
    if (tri_area == 0.0f) return;
    float inv_area = 1.0f / tri_area;

    float e1_dx =  (p2.y - p1.y);
    float e1_dy = -(p2.x - p1.x);

    float e2_dx =  (p3.y - p2.y);
    float e2_dy = -(p3.x - p2.x);

    float e3_dx =  (p1.y - p3.y);
    float e3_dy = -(p1.x - p3.x);

    float start_x = min_x;
    float start_y = min_y;

    float e1_row = edge_coeff(p1.x, p1.y, p2.x, p2.y, start_x, start_y);
    float e2_row = edge_coeff(p2.x, p2.y, p3.x, p3.y, start_x, start_y);
    float e3_row = edge_coeff(p3.x, p3.y, p1.x, p1.y, start_x, start_y);

    for (float y = min_y; y <= max_y; y++) {
        float e1 = e1_row;
        float e2 = e2_row;
        float e3 = e3_row;

        for (float x = min_x; x <= max_x; x++) {
            if ((e1 <= 0 && e2 <= 0 && e3 <= 0) || (e1 >= 0 && e2 >= 0 && e3 >= 0)) 
                if (x >= 0 && y >= 0 && x < s->width && y < s->height) {
                float w1 = e1 * inv_area;
                float w2 = e2 * inv_area;
                float w3 = e3 * inv_area;

                    float new_pixel_depth = v1.pos.z * w1 + v2.pos.z * w2 + v3.pos.z * w3;
                    int idx = (int)x + (s->width * (int)y);
                    float old_pixel_depth = s->depth[idx];

                    if (old_pixel_depth == 0.0f || old_pixel_depth > new_pixel_depth) {
                        //draw new_pixel
                        s->depth[idx] = new_pixel_depth;
                        Col color;

                        color.r = v1.color.r * w1 + v2.color.r * w2 + v3.color.r * w3;
                        color.g = v1.color.g * w1 + v2.color.g * w2 + v3.color.g * w3;
                        color.b = v1.color.b * w1 + v2.color.b * w2 + v3.color.b * w3;
                        color.a = v1.color.a * w1 + v2.color.a * w2 + v3.color.a * w3;

                        screen_put_pixel(s, x, y, col_getint(color));
                    }
                }

            e1 += e1_dx;
            e2 += e2_dx;
            e3 += e3_dx;
        }

        e1_row += e1_dy;
        e2_row += e2_dy;
        e3_row += e3_dy;
    }

}

void screen_draw_triangle_deprecate(Screen* s, Vertex v1, Vertex v2, Vertex v3) {
    Vec2 p1 = vec3_to_vec2(v1.pos);
    Vec2 p2 = vec3_to_vec2(v2.pos);
    Vec2 p3 = vec3_to_vec2(v3.pos);
    
    float big_tri_area = triangle2d_area(p1, p2, p3);
    float inv_big_tri_area = 1.0/big_tri_area;

    float min_x = ceilf(MIN(p1.x, MIN(p2.x, p3.x)));
    float max_x = floorf(MAX(p1.x, MAX(p2.x, p3.x)));

    float min_y = ceilf(MIN(p1.y, MIN(p2.y, p3.y)));
    float max_y = floorf(MAX(p1.y, MAX(p2.y, p3.y)));

    for (float y = min_y + 0.5; y <= max_y; y++) {
        for (float x = min_x + 0.5; x <= max_x; x++) {
            Vec2 this_point = { x, y };
            if (!is_point_in_triangle(this_point, p1, p2, p3)) 
                continue;

            float u = triangle2d_area(this_point, p2,         p3)*inv_big_tri_area;
            float v = triangle2d_area(p1,         this_point, p3)*inv_big_tri_area;
            float w = triangle2d_area(p1,         p2,         this_point)*inv_big_tri_area;

            Col color = {0};
            color.r = v1.color.r*u + v2.color.r*v + v3.color.r*w;
            color.g = v1.color.g*u + v2.color.g*v + v3.color.g*w;
            color.b = v1.color.b*u + v2.color.b*v + v3.color.b*w;
            color.a = v1.color.a*u + v2.color.a*v + v3.color.a*w;

            screen_put_pixel(s, x, y, col_getint(color));
        }
    }
}

typedef struct {
    Vertex v;
    bool ok;
} Vertex_Result;

static inline Vertex_Result screen_vertex_project(const Screen* s, const Cam* c, Vertex v) {
    Vec3_Result res = screen_project(s, c, v.pos);
    return (Vertex_Result) {
        .v = {
            .pos = res.v,
            .color = v.color,
        },
        .ok = res.ok,
    };
}

Vertex camera_project(const Cam* c, Vertex v) {
    return (Vertex) {
        .color = v.color,
        .pos = {
            .x = v.pos.x - c->pos.x,
            .y = v.pos.y - c->pos.y,
            .z = v.pos.z - c->pos.z,
        }
    };
}

void scale_vec3(Vec3* v, float scale_factor) {
    v->x *= scale_factor;
    v->y *= scale_factor;
    v->z *= scale_factor;
}

void screen_draw_obj(Screen* s, Cam* c, Object* obj) {
    static int balls = 0;
    for (int i = 0; i < obj->face_idx.count - 2; i += 3) {
        Vertex v1 = obj->vertices.items[obj->face_idx.items[i+2]];
        Vertex v2 = obj->vertices.items[obj->face_idx.items[i+1]];
        Vertex v3 = obj->vertices.items[obj->face_idx.items[i+0]];

        float scale_factor = 1;
        scale_vec3(&v1.pos, scale_factor);
        scale_vec3(&v2.pos, scale_factor);
        scale_vec3(&v3.pos, scale_factor);
        Vertex_Result sv1 = screen_vertex_project(s, c, camera_project(c, v1));
        Vertex_Result sv2 = screen_vertex_project(s, c, camera_project(c, v2));
        Vertex_Result sv3 = screen_vertex_project(s, c, camera_project(c, v3));
    
        if (!sv1.ok || !sv2.ok || !sv3.ok) continue;


        sv1.v.color = (Col) {255, 0, 0, 255};
        sv2.v.color = (Col) {155, 0, 0, 255};
        sv3.v.color = (Col) {055, 0, 0, 255};
        if (is_triangle_front(sv1.v.pos, sv2.v.pos, sv3.v.pos)) {
            screen_draw_triangle(s, sv1.v, sv2.v, sv3.v);
        }
    }
}

void screen_draw_obj_deprecate(Screen* s, Cam* c, Object* obj) {
    for (int i = 0; i < obj->face_idx.count; i += 3) {
        size_t id1 = obj->face_idx.items[i+0];
        size_t id2 = obj->face_idx.items[i+1];
        size_t id3 = obj->face_idx.items[i+2];
        Vertex sv1 = screen_vertex_project(s, c, camera_project(c, obj->vertices.items[id1])).v;
        sv1.color = (Col) {255, 100, 0, 255};
        Vertex sv2 = screen_vertex_project(s, c, camera_project(c, obj->vertices.items[id2])).v;
        sv2.color = (Col) {255, 50, 0, 255};
        Vertex sv3 = screen_vertex_project(s, c, camera_project(c, obj->vertices.items[id3])).v;
        sv3.color = (Col) {255, 0, 0, 255};
        if (is_triangle_front(sv1.pos, sv2.pos, sv3.pos)) {
            screen_draw_triangle_deprecate(s, sv1, sv2, sv3);
        }
    }
}

void screen_draw_obj_points(Screen* s, const Cam* c, Object* obj) {
    for (int i = 0; i < obj->vertices.count; i++) {
        Vertex v = obj->vertices.items[i];
        v = camera_project(c, v);

        Vertex_Result rv = screen_vertex_project(s, c, (Vertex){.pos = v.pos, .color = v.color});
        #define PT_SIZE 10
        if (rv.ok) screen_draw_rect(s, rv.v.pos.x, rv.v.pos.y, PT_SIZE, PT_SIZE, 0xffffffff);
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

// obj parser
//
//


typedef struct {
    char* data;
    size_t len;
} String_View;

#define SV_FORMAT "%.*s"
#define SV_ARGS(sv) ((int)(sv).len), (sv).data

typedef struct {
    String_View content;
    size_t ptr;
} Parser;

Parser parser_new_from_cstr(char* content) {
    return (Parser){
        .content = (String_View) {
            .len = strlen(content),
            .data = content,
        },
        .ptr = 0,
    };
}

void sv_skip_spaces(String_View* sv) {
    while (sv->len < 0 && isspace(*sv->data)) {
        sv->data++; sv->len--;
    }
}

bool sv_cmp_cstr(String_View sv, char* cstr) {
    size_t cstr_len = strlen(cstr);
    if (sv.len != cstr_len) return false;
    for (int i = 0; i < sv.len; i++) if (sv.data[i] != cstr[i]) return false;
    return true;
}

bool sv_starts_with(String_View sv, char* prefix) {
    size_t prefix_len = strlen(prefix);
    if (sv.len < prefix_len) return false;
    sv.len = prefix_len;
    return sv_cmp_cstr(sv, prefix);
}

void parser_skip_spaces(Parser* p) {
    while (p->ptr < p->content.len && 
        isspace(p->content.data[p->ptr])) p->ptr++;
}

String_View parser_get_line(Parser* p) {
    char* data = &p->content.data[p->ptr];
    size_t len = 0;
    for (char* c = data; *c && *c != '\n'; c++) len++;
    p->ptr += len;

    return (String_View) {
        .data = data,
        .len = len,
    };
}

void parser_skip_to_next_line(Parser* p) {
    char* data = &p->content.data[p->ptr];
    size_t len = 0;
    for (char* c = data; *c && *c != '\n'; c++) len++;
    p->ptr += len;
}

char parser_next_char(Parser* p) {
    if (p->ptr + 1 >= p->content.len) return 0;
    return p->content.data[p->ptr++];
}

String_View parser_next_token(Parser* p) {
    char* data = &p->content.data[p->ptr];
    size_t len = 0;
    for (char* c = data; *c && !isspace(*c); c++) { len++; }
    p->ptr += len + 1;

    return (String_View) {
        .data = data,
        .len = len,
    };
}

String_View parser_next_token_chars(Parser* p, char* str) {
    char* data = &p->content.data[p->ptr];
    size_t len = 0;
    for (char* c = data; *c && !strchr(str, *c); c++) { len++; }
    p->ptr += len + 1;

    return (String_View) {
        .data = data,
        .len = len,
    };
}

String_View parser_seek_until_chars(Parser* const p, char* str) { // NOTE(@siiick): DOES NOT CHANGE THE PARSER STATE
    char* data = &p->content.data[p->ptr];
    size_t len = 0;
    for (char* c = data; *c && !strchr(str, *c); c++) { len++; }
    size_t ptr = p->ptr + len + 1;

    return (String_View) {
        .data = data,
        .len = len,
    };
}


bool obj_load_file(char* obj_path, Object* obj) {
    Vertices vertices = {0};
    Indices face_idx = {0};

    char* content = read_file_as_string(obj_path);
    if (!content) { 
        printf("ERROR: Could not find obj file `%s` to read\n", obj_path);
        return false; 
    }

    Parser p = parser_new_from_cstr(content);

    size_t face_count = 0;
    
    String_View tok = parser_next_token(&p);
    while (p.ptr < p.content.len) {
        tok = parser_next_token(&p);
        parser_skip_spaces(&p);
        if (sv_starts_with(tok, "#")) {
            parser_skip_to_next_line(&p);
        }
        if (sv_cmp_cstr(tok, "v")) {
            float x = strtof(parser_next_token(&p).data, NULL);
            float y = strtof(parser_next_token(&p).data, NULL);
            float z = strtof(parser_next_token(&p).data, NULL);
            
            Vertex v = (Vertex) {
                .pos.x = x,
                .pos.y = y,
                .pos.z = z,
            };
            da_append(&vertices, v);
        } else if (sv_cmp_cstr(tok, "f")) {

            size_t a = strtol(parser_next_token_chars(&p, "/ ").data, NULL, 10) - 1;
            parser_next_token(&p);
            da_append(&face_idx, a);

            size_t b = strtol(parser_next_token_chars(&p, "/ ").data, NULL, 10) - 1;
            parser_next_token(&p);
            da_append(&face_idx, b);

            size_t c = strtol(parser_next_token_chars(&p, "/ ").data, NULL, 10) - 1;
            parser_next_token(&p);
            da_append(&face_idx, c);

            String_View last_ele = parser_seek_until_chars(&p, "/ \n");
            printf("|"SV_FORMAT"|\n", SV_ARGS(last_ele));
            char* endptr = NULL;
            size_t d = strtol(last_ele.data, &endptr, 10) - 1;
            if (!(endptr == last_ele.data) && d != 0) {
                da_append(&face_idx, a);
                da_append(&face_idx, c);
                da_append(&face_idx, d);
                parser_skip_to_next_line(&p);
            }
            face_count++;
        }
    }
    printf(":::face count %zu\n", face_count);

    obj->vertices = vertices;
    obj->face_idx = face_idx;
    return true;
}

