#ifndef __COMMON_C__
#define __COMMON_C__

#include <math.h>
#include <stdint.h>
#include <sys/types.h>


#define LOG_INFO(...) do { printf("[%s:%d] INFO: %s\n", __FILE__, __LINE__, __VA_ARGS__); } while(0);
#define LOG_ERROR(...) do { printf("ERROR [%s:%d]: %s\n", __FILE__, __LINE__, __VA_ARGS__); } while(0);

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define MATH_PI (22.0f/7.0f)
#define MIN(a, b) ((a) < (b) ? (a) : (b))   
#define MAX(a, b) ((a) > (b) ? (a) : (b))   

#define COMP_X (0)
#define COMP_Y (1)
#define COMP_Z (2)

typedef union {
    struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; };
    uint8_t v[4];
} Col;

typedef union {
    struct { float x; float y; };
    float v[2];
} Vec2;

typedef union {
    struct { float x; float y; float z; };
    float v[3];
} Vec3;

typedef union {
    struct { int x; int y; };
    int v[2];
} Vec2i;

typedef union {
    struct { int x; int y; int z; };
    int v[3];
} Vec3i;

Vec2 vec2_add(Vec2 v1, Vec2 v2) {
    return (Vec2){
        .x = v1.x + v2.x, 
        .y = v1.y + v2.y,
    };
}

Vec2 vec2_sub(Vec2 v1, Vec2 v2) {
    return (Vec2){
        .x = v1.x - v2.x, 
        .y = v1.y - v2.y,
    };
}

Vec2 vec2_hadamard(Vec2 v1, Vec2 v2) {
    return (Vec2){
        .x = v1.x*v2.x, 
        .y = v1.y*v2.y,
    };
}

Vec2 vec2_mult_scal(Vec2 v1, float s) {
    return (Vec2){
        .x = v1.x*s, 
        .y = v1.y*s,
    };
}

Vec2 vec2_div_scal(Vec2 v1, float s) {
    return (Vec2){
        .x = v1.x/s, 
        .y = v1.y/s,
    };
}

float vec2_len(Vec2 v) {
    return sqrtf(v.x*v.x + v.y*v.y);
}

float vec2_len_sqr(Vec2 v) {
    return v.x*v.x + v.y*v.y;
}

Vec2 vec2_norm(Vec2 v) {
    float len = vec2_len(v);
    return (Vec2) {
        .x = v.x/len,
        .y = v.y/len,
    };
}

//////////////////////

Vec3 vec3_add(Vec3 v1, Vec3 v2) {
    return (Vec3){
        .x = v1.x + v2.x, 
        .y = v1.y + v2.y,
        .z = v1.z + v2.z,
    };
}

Vec3 vec3_sub(Vec3 v1, Vec3 v2) {
    return (Vec3){
        .x = v1.x - v2.x, 
        .y = v1.y - v2.y,
        .z = v1.z - v2.z,
    };
}

Vec3 vec3_hadamard(Vec3 v1, Vec3 v2) {
    return (Vec3){
        .x = v1.x*v2.x, 
        .y = v1.y*v2.y,
        .z = v1.z*v2.z,
    };
}

Vec3 vec3_mult_scal(Vec3 v1, float s) {
    return (Vec3){
        .x = v1.x*s, 
        .y = v1.y*s,
        .z = v1.z*s,
    };
}

Vec3 vec3_div_scal(Vec3 v1, float s) {
    return (Vec3){
        .x = v1.x/s, 
        .y = v1.y/s,
        .z = v1.z/s,
    };
}

float vec3_len(Vec3 v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

float vec3_len_sqr(Vec3 v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

Vec3 vec3_norm(Vec3 v) {
    float len = vec3_len(v);
    return (Vec3) {
        .x = v.x/len,
        .y = v.y/len,
        .z = v.z/len,
    };
}

Vec3 vec3_cross(Vec3 v1, Vec3 v2) {
    return (Vec3) {
        .x = v1.y*v2.z - v1.z*v2.y,
        .y = v1.z*v2.x - v1.x*v2.z,
        .z = v1.x*v2.y - v1.y*v2.x,
    };
}

int cross2d(int p1_x, int p1_y, int p2_x, int p2_y) {
    return p1_x*p2_y - p1_y*p2_x;
}


Vec2i vec2i_distance_vec(Vec2i v1, Vec2i v2) {
    return (Vec2i){
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
    };
}

int triangle2d_area(Vec2i v1, Vec2i v2, Vec2i v3) {
    Vec2i dv1 = vec2i_distance_vec(v1, v2);
    Vec2i dv2 = vec2i_distance_vec(v1, v3);

    int result = cross2d(dv1.x, dv1.y, dv2.x, dv2.y)>>1;
    if (result < 0) result*=-1;
    return result;
}

#define da_append(xs, x)                                                             \
    do {                                                                             \
        if ((xs)->count >= (xs)->capacity) {                                         \
            if ((xs)->capacity == 0) (xs)->capacity = 256;                           \
            else (xs)->capacity *= 2;                                                \
            (xs)->items = realloc((xs)->items, (xs)->capacity*sizeof(*(xs)->items)); \
        }                                                                            \
                                                                                     \
        (xs)->items[(xs)->count++] = (x);                                            \
    } while (0)

#endif// __COMMON_C__

