#ifndef GEOLIB_H_
#define GEOLIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <raylib.h>
#include <raymath.h>

#ifndef ARRAY_LEN
#define ARRAY_LEN(arr) sizeof(arr)/sizeof(arr[0])
#endif

#ifndef UNIMPLEMENTED
#define UNIMPLEMENTED(...)                                              \
    do {                                                                \
        printf("%s:%d: UNIMPLEMENTED: %s\n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                        \
    } while (0)
#endif

#ifndef UNUSED
#define UNUSED(v) (void) (v)
#endif

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
    Vector2 vector;
    Vector2 start;
    Color color;
    char name;
} Geolib_Vector2;

typedef struct {
    Geolib_Vector2 *items;
    size_t count;
    size_t capacity;
} Geolib_Vector2s;

typedef struct {
    Geolib_Vector2s vectors;
    Rectangle plane_rect;
    size_t unit_count;
    float step_size;
} Geolib;

Geolib geolib_alloc(Rectangle plane_rect, size_t unit_count);
void geolib_add_vector(Geolib *gl, Vector2 v, Vector2 start);
void geolib_draw_plane(Geolib *gl, Vector2 unit_marker_size, Font font, float font_gap, Color cross_color, Color step_color);
void geolib_clean(Geolib *gl);
void geolib_draw_vector(Vector2 v, Vector2 start, Vector2 origin, float scalar, Color color);
void geolib_plot_vec(Geolib *gl, Geolib_Vector2 v);
void geolib_plot_vecs(Geolib *gl);
void geolib_draw_vecs_info(Geolib *gl, Vector2 pos, Font font);
void geolib_update_plane_rect(Geolib *gl, Rectangle new_plane_rect);

Vector2 make_vector2(float x, float y);

void draw_cross(Rectangle rect, float thick, Color color);
void draw_arrow(Vector2 start_point, Vector2 end_point, float thick, float tip_len, float tip_width, Color color);
void draw_vector_info(Geolib_Vector2 v, Vector2 pos, Font font);

Vector2 to_cartesian_system(Vector2 v, Vector2 start, float scalar);
float radians2degrees(float radians);
float degrees2radians(float degrees);
Vector2 polar_to_coords(float len, float angle);

float get_vec_angle(Vector2 v);

#endif  // GEOLIB_H_

#ifdef GEOLIB_IMPLEMENTATION

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

static_assert(ARRAY_LEN(vec_name_pool) == ARRAY_LEN(color_pool), "The pools are not equal in size, please make sure they are");

Geolib geolib_alloc(Rectangle plane_rect, size_t unit_count)
{
    Geolib gl = {0};
    gl.unit_count = unit_count;
    gl.plane_rect = plane_rect;
    gl.step_size = plane_rect.width/2 / unit_count;
    return gl;
}

void geolib_clean(Geolib *gl)
{
    da_reset(&gl->vectors);
}

void geolib_add_vector(Geolib *gl, Vector2 v, Vector2 start)
{
    size_t pool_index = fmodf(gl->vectors.count, ARRAY_LEN(color_pool));

    da_append(&gl->vectors, ((Geolib_Vector2) {
                .vector = v,
                .start = start,
                .color = color_pool[pool_index],
                .name = vec_name_pool[pool_index]
            }));
}

void geolib_plot_vec(Geolib *gl, Geolib_Vector2 v)
{
    Vector2 plane_center = make_vector2(gl->plane_rect.x + gl->plane_rect.width/2,
                                        gl->plane_rect.y + gl->plane_rect.height/2);

    geolib_draw_vector(v.vector, v.start, plane_center, gl->step_size, v.color);
}

void geolib_plot_vecs(Geolib *gl)
{
    for (size_t i = 0; i < gl->vectors.count; ++i) {
        geolib_plot_vec(gl, gl->vectors.items[i]);
    }
}

// TODO: Make this function accept a rectangle in which it should fit
void geolib_draw_vecs_info(Geolib *gl, Vector2 pos, Font font)
{
    for (size_t i = 0; i < gl->vectors.count; ++i) {
        draw_vector_info(gl->vectors.items[i], pos, font);
        pos.y += font.baseSize;
    }
}

void geolib_update_plane_rect(Geolib *gl, Rectangle new_plane_rect)
{
    if (gl->plane_rect.x == new_plane_rect.x &&
        gl->plane_rect.y == new_plane_rect.y &&
        gl->plane_rect.width == new_plane_rect.width &&
        gl->plane_rect.height == new_plane_rect.height) return;

    gl->plane_rect = new_plane_rect;
    gl->step_size = gl->plane_rect.width/2 / gl->unit_count;
}

Vector2 make_vector2(float x, float y)
{
    return (Vector2) {x, y};
}

void draw_cross(Rectangle rect, float thick, Color color)
{
    DrawLineEx(make_vector2(rect.x, rect.y + rect.height/2),
               make_vector2(rect.x + rect.width, rect.y + rect.height/2), thick, color);
    DrawLineEx(make_vector2(rect.x + rect.width/2, rect.y),
               make_vector2(rect.x + rect.width/2, rect.y + rect.height), thick, color);
}

void geolib_draw_plane(Geolib *gl, Vector2 unit_marker_size, Font font, float font_gap, Color cross_color, Color step_color)
{
    draw_cross(gl->plane_rect, 2, cross_color);

    int directions[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1},
    };

    Vector2 line_start = make_vector2(gl->plane_rect.x + gl->plane_rect.width/2, gl->plane_rect.y + gl->plane_rect.height/2);
    for (size_t i = 0;  i < ARRAY_LEN(directions); ++i) {
        for (size_t j = 1; j < gl->unit_count+1; ++j) {
            int xdir = directions[i][0];
            int ydir = directions[i][1];

            Vector2 curr_pos = make_vector2(
                line_start.x + (j*(gl->plane_rect.width/2 / gl->unit_count))*xdir,
                line_start.y + (j*(gl->plane_rect.height/2 / gl->unit_count))*ydir);

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
                       make_vector2(step_rect.x + (text_size.x/2 * -abs(xdir) + (text_size.x + font_gap) * -abs(ydir)),
                                    step_rect.y + (text_size.y/2 * -abs(ydir) + (text_size.y/2 + font_gap) * abs(xdir))), // TODO: document this
                       font.baseSize, 1, step_color);
        }
    }
}

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

void geolib_draw_vector(Vector2 v, Vector2 start, Vector2 origin, float scalar, Color color)
{
    Vector2 cartesian_start = to_cartesian_system(start, origin, scalar);
    Vector2 cartesian_v = to_cartesian_system(v, cartesian_start, scalar);
    draw_arrow(cartesian_start, cartesian_v, 3, 20, 10, color);
}

void draw_vector_info(Geolib_Vector2 v, Vector2 pos, Font font)
{
    DrawTextEx(font, TextFormat("%c = (%.2f, %.2f)", v.name, v.vector.x, v.vector.y), pos, font.baseSize, 1, v.color);
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
    return make_vector2(len * cos(angle), len * sin(angle));
}

float get_vec_angle(Vector2 v)
{
    return radians2degrees(atan2f(v.y, v.x));
}

#endif // GEOLIB_IMPLEMENTATION

// TODO: make this library library agnostic: This library should work with other libraries such as sdl2
// TODO: find a better way to let the user modify the vectors
// TODO: add functions to help with drawing the same vector at multiple positions (the user should be able to define multiple starting positions for each vector)
