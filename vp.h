#ifndef VP_H_
#define VP_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <raylib.h>
#include <raymath.h>

#define DA_INIT_CAP 256

#define da_append(da, item)                                             \
    do {                                                                \
        if ((da)->count >= (da)->capacity) {                            \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity*2; \
            (da)->items = realloc((da)->items, sizeof(*(da)->items)*(da)->capacity); \
            assert((da)->items != NULL);                                \
        }                                                               \
        (da)->items[(da)->count++] = (item);                            \
    } while (0)

#define da_append_many(da, new_items, new_items_count)                  \
    do {                                                                \
        if ((da)->count + new_items >= (da)->capacity) {                \
            if ((da)->capacity == 0) {                                  \
                (da)->capacity = DA_INIT_CAP;                           \
            }                                                           \
                                                                        \
            while ((da)->count + new_items_count >= (da)->capacity) {   \
                (da)->capacity *= 2;                                    \
            }                                                           \
            (da)->items = realloc((da)->items, sizeof(*(da)->items)*(da)->capacity); \
            assert((da)->items != NULL);                                \
        }                                                               \
                                                                        \
        memcpy((da)->items + (da)->count, new_items, new_items_count*sizeof(*(da)->items)); \
        (da)->count += new_items_count;                                 \
    } while (0)

#define da_reset(da)                            \
    do {                                        \
        if ((da)->items != NULL) {              \
            free((da)->items);                  \
            (da)->items = NULL;                 \
        }                                       \
        (da)->capacity = 0;                     \
        (da)->count = 0;                        \
    } while (0)

typedef struct {
    Vector2 *items;
    size_t count;
    size_t capacity;
} Vector2s;

typedef struct {
    Vector2 vec;
    Vector2s dps;
    Color color;
    char name;
} Vp_Vector2;

typedef struct {
    Vp_Vector2 *items;
    size_t count;
    size_t capacity;
} Vp_Vector2s;

typedef struct {
    Vector2 vec;
    bool created;
} Dp;

typedef struct {
    Vp_Vector2s vecs;
    Rectangle plane_rect;
    size_t unit_count;
    float step_size;
} Vp;

#ifndef DEFAULT_MARKER_SIZE
#define DEFAULT_MARKER_SIZE make_vec2(2, 18)
#endif
#ifndef DEFAULT_FONT_GAP
#define DEFAULT_FONT_GAP 10
#endif
#ifndef DEFAULT_CROSS_COLOR
#define DEFAULT_CROSS_COLOR WHITE
#endif
#ifndef DEFAULT_STEP_COLOR
#define DEFAULT_STEP_COLOR WHITE
#endif

Vp vp_alloc(Rectangle plane_rect, size_t unit_count);

// `geolib_add_vecs` returns the index of the newly added vec
size_t vp_add_vec(Vp *vp, Vector2 v);
size_t vp_add_vec_dps(Vp *vp, Vector2 v, ...);

void vp_add_dp(Vp *vp, size_t idx, Vector2 dp);
void vp_add_dps(Vp *vp, size_t idx, ...);

void vp_draw_plane(Vp *vp, Vector2 unit_marker_size, Font font, float font_gap, Color cross_color, Color step_color);
void vp_clean(Vp *vp);
void vp_draw_vec(Vector2 v, Vector2 start, Vector2 origin, float scalar, Color color);
void vp_plot_unwrapped(Vp *vp, Vector2 unit_marker_size, Font font, float font_gap, Color cross_color, Color step_color);
void vp_plot_vec(Vp *vp, Vp_Vector2 v);
void vp_plot_vecs(Vp *vp);
void vp_draw_info(Vp *vp, Vector2 pos, Font font);
void vp_update_plane_rect(Vp *vp, Rectangle new_plane_rect);

Vp_Vector2 *vp_get_vec(Vp *vp, size_t idx);

Vector2 make_vec2(float x, float y);
Dp make_dp(float x, float y);
Dp vec2dp(Vector2 v);

void draw_cross(Rectangle rect, float thick, Color color);
void draw_arrow(Vector2 start_point, Vector2 end_point, float thick, float tip_len, float tip_width, Color color);
void draw_vec_info(Vp_Vector2 v, Vector2 pos, Font font);

Vector2 to_cartesian_system(Vector2 v, Vector2 start, float scalar);
float radians2degrees(float radians);
float degrees2radians(float degrees);
Vector2 polar_to_coords(float len, float angle);

float get_vec_angle(Vector2 v);

#endif  // VP_H_

#ifdef VP_IMPLEMENTATION

const int vec_name_pool[] = {
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
};

const Color color_pool[] = {
    RED,
    BLUE,
    GREEN,
    MAGENTA,
    PURPLE,
    ORANGE,
};

#ifndef ARRAY_LEN
#define ARRAY_LEN(arr) sizeof(arr)/sizeof(arr[0])
#endif

static_assert(ARRAY_LEN(vec_name_pool) == ARRAY_LEN(color_pool), "The pools are not equal in size, please make sure they are");

Vp vp_alloc(Rectangle plane_rect, size_t unit_count)
{
    Vp vp = {0};
    vp.unit_count = unit_count;
    vp.plane_rect = plane_rect;
    vp.step_size = plane_rect.width/2 / unit_count;
    return vp;
}

void vp_clean(Vp *vp)
{
    da_reset(&vp->vecs);
}

// TODO: return the pointer to the Vp_Vector instead of the index
size_t vp_add_vec(Vp *vp, Vector2 v)
{
    return vp_add_vec_dps(vp, v, make_dp(0, 0));
}

// TODO: expect NULL as the last argument instead of accepting the number of arguments
size_t vp_add_vec_dps(Vp *vp, Vector2 v, ...)
{
    size_t pool_index = fmodf(vp->vecs.count, ARRAY_LEN(color_pool));
    Vp_Vector2 vp_vec = (Vp_Vector2) {
        .vec = v,
        .dps = {0},
        .color = color_pool[pool_index],
        .name = vec_name_pool[pool_index]
    };

    va_list args;
    va_start(args, v);
    Dp curr_dp = va_arg(args, Dp);
    while (curr_dp.created == true) {
        da_append(&vp_vec.dps, curr_dp.vec);
        curr_dp = va_arg(args, Dp);
    }
    va_end(args);

    da_append(&vp->vecs, vp_vec);
    return vp->vecs.count-1;
}

void vp_add_dp(Vp *vp, size_t idx, Vector2 dp) // TODO: implement another function like this that accepts indexes instead of Vector2
{
    vp_add_dps(vp, idx, 1, dp);
}

void vp_add_dps(Vp *vp, size_t idx, ...)
{
    Vp_Vector2 *vpv = vp_get_vec(vp, idx);

    va_list args;
    va_start(args, idx);
    Dp curr_dp = va_arg(args, Dp);
    while (curr_dp.created) {
        da_append(&vpv->dps, curr_dp.vec);
        curr_dp = va_arg(args, Dp);
    }
    va_end(args);
}

void vp_plot_vec(Vp *vp, Vp_Vector2 v)
{
    Vector2 plane_center = make_vec2(vp->plane_rect.x + vp->plane_rect.width/2,
                                     vp->plane_rect.y + vp->plane_rect.height/2);

    for (size_t i = 0; i < v.dps.count; ++i) {
        vp_draw_vec(v.vec, v.dps.items[i], plane_center, vp->step_size, v.color);
    }
}

void vp_plot_vecs(Vp *vp)
{
    for (size_t i = 0; i < vp->vecs.count; ++i) {
        vp_plot_vec(vp, vp->vecs.items[i]);
    }
}

// TODO: Make this function accept a rectangle in which it should fit
void vp_draw_info(Vp *vp, Vector2 pos, Font font)
{
    for (size_t i = 0; i < vp->vecs.count; ++i) {
        draw_vec_info(vp->vecs.items[i], pos, font);
        pos.y += font.baseSize;
    }
}

void vp_update_plane_rect(Vp *vp, Rectangle new_plane_rect)
{
    vp->plane_rect = new_plane_rect;
    vp->step_size = vp->plane_rect.width/2 / vp->unit_count;
}

Vp_Vector2 *vp_get_vec(Vp *vp, size_t idx)
{
    assert(vp->vecs.count > idx);
    return &vp->vecs.items[idx];
}

Vector2 make_vec2(float x, float y)
{
    return (Vector2) {x, y};
}

Dp make_dp(float x, float y)
{
    return (Dp) {
        .vec = make_vec2(x, y),
        .created = true
    };
}

Dp vec2dp(Vector2 v)
{
    return (Dp) {
        .vec = v,
        .created = true,
    };
}

void draw_cross(Rectangle rect, float thick, Color color)
{
    DrawLineEx(make_vec2(rect.x, rect.y + rect.height/2),
               make_vec2(rect.x + rect.width, rect.y + rect.height/2), thick, color);
    DrawLineEx(make_vec2(rect.x + rect.width/2, rect.y),
               make_vec2(rect.x + rect.width/2, rect.y + rect.height), thick, color);
}

void vp_draw_plane(Vp *vp, Vector2 unit_marker_size, Font font, float font_gap, Color cross_color, Color step_color)
{
    draw_cross(vp->plane_rect, 2, cross_color);

    int directions[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1},
    };

    Vector2 line_start = make_vec2(vp->plane_rect.x + vp->plane_rect.width/2, vp->plane_rect.y + vp->plane_rect.height/2);
    for (size_t i = 0;  i < ARRAY_LEN(directions); ++i) {
        for (size_t j = 1; j < vp->unit_count+1; ++j) {
            int xdir = directions[i][0];
            int ydir = directions[i][1];

            Vector2 curr_pos = make_vec2(
                line_start.x + (j*(vp->plane_rect.width/2 / vp->unit_count))*xdir,
                line_start.y + (j*(vp->plane_rect.height/2 / vp->unit_count))*ydir);

            Rectangle step_rect = (Rectangle) {
                .x = curr_pos.x,
                .y = curr_pos.y,
                .width = fabsf(unit_marker_size.x * xdir + unit_marker_size.y * ydir),
                .height = fabsf(unit_marker_size.y * xdir + unit_marker_size.x * ydir),
            };

            step_rect.x -= step_rect.width/2;
            step_rect.y -= step_rect.height/2;

            DrawRectangleRec(step_rect, step_color);

            const char *coordinate_text = TextFormat("%d", j*(xdir + -ydir));
            Vector2 text_size = MeasureTextEx(font, coordinate_text, font.baseSize, 1);
            DrawTextEx(font,
                       coordinate_text,
                       make_vec2(step_rect.x + (text_size.x/2 * -abs(xdir) + (text_size.x + font_gap) * -abs(ydir)),
                                 step_rect.y + (text_size.y/2 * -abs(ydir) + (text_size.y/2 + font_gap) * abs(xdir))), // TODO: document this
                       font.baseSize, 1, step_color);
        }
    }
}

void vp_plot_unwrapped(Vp *vp, Vector2 unit_marker_size, Font font, float font_gap, Color cross_color, Color step_color)
{
    vp_draw_plane(vp, unit_marker_size, font, font_gap, cross_color, step_color);
    vp_plot_vecs(vp);
}
#if defined(USE_DEFAULTS)
void vp_plot(Vp *vp, Font font)
{

    vp_plot_unwrapped(vp, DEFAULT_MARKER_SIZE, font, DEFAULT_FONT_GAP, DEFAULT_CROSS_COLOR, DEFAULT_STEP_COLOR);
}
#else
void vp_plot(Vp *vp, Vector2 unit_marker_size, Font font, float font_gap, Color cross_color, Color step_color)
{
    vp_plot_unwrapped(vp, unit_marker_size, font, font_gap, cross_color, step_color);
}
#endif

void draw_arrow(Vector2 start_point, Vector2 end_point, float thick, float tip_len, float tip_width, Color color)
{
    Vector2 diff = Vector2Subtract(end_point, start_point);
    float diff_len = Vector2Length(diff);
    diff = Vector2Scale(diff, (diff_len-tip_len)/diff_len);
    Vector2 tip_start = Vector2Add(diff, start_point);
    DrawLineEx(start_point, tip_start, thick, color);

    Vector2 left = Vector2Add(tip_start, polar_to_coords(tip_width/2, get_vec_angle(diff) - 90));
    Vector2 right = Vector2Add(tip_start, polar_to_coords(tip_width/2, get_vec_angle(diff) + 90));
    DrawTriangle(end_point, left, right, color);
}

void vp_draw_vec(Vector2 v, Vector2 start, Vector2 origin, float scalar, Color color)
{
    Vector2 cartesian_start = to_cartesian_system(start, origin, scalar);
    Vector2 cartesian_v = to_cartesian_system(v, cartesian_start, scalar);
    draw_arrow(cartesian_start, cartesian_v, 3, 20, 10, color);
}

void draw_vec_info(Vp_Vector2 v, Vector2 pos, Font font)
{
    DrawTextEx(font, TextFormat("%c = (%.2f, %.2f)", v.name, v.vec.x, v.vec.y), pos, font.baseSize, 1, v.color);
}

Vector2 to_cartesian_system(Vector2 v, Vector2 start, float scalar)
{
    v = Vector2Scale(v, scalar);
    v.y *= -1;
    v = Vector2Add(v, start);
    return v;
}

float radians2degrees(float radians)
{
    return radians * 180 / PI;
}

float degrees2radians(float degrees)
{
    return degrees * PI / 180;
}

Vector2 polar_to_coords(float len, float angle)
{
    angle = degrees2radians(angle);
    return make_vec2(len * cos(angle), len * sin(angle));
}

float get_vec_angle(Vector2 v)
{
    return radians2degrees(atan2f(v.y, v.x));
}

#endif // VP_IMPLEMENTATION

// TODO: make this library library agnostic: This library should work with other libraries such as sdl2
// TODO: add functions to help with drawing the same vector at multiple positions (the user should be able to define multiple starting positions for each vector)
// TODO: rename functions that are a too long

// TODO: Make this kind of api easier maybe by providing a struct that stores the vector and the index
/*
  Vector2 a_vec = make_vec2(2, 3);
  Vector2 b_vec = make_vec2(5, -2.25);
  size_t a_idx = vp_add_vec(&vp, a_vec);
  size_t b_idx = vp_add_vec(&vp, b_vec);
  vp_add_dp(&vp, a_idx, b_vec);
  vp_add_dp(&vp, b_idx, a_vec);
  vp_add_vec(&vp, Vector2Add(a_vec, b_vec));
*/


// TODO: Seperate drawing functions from the rest
