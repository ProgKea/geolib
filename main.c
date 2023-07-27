#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <raylib.h>
#include <raymath.h>

#define ARRAY_LEN(arr) sizeof(arr)/sizeof(arr[0])
#define FONT_SIZE 30

#define UNIMPLEMENTED(...)                                              \
    do {                                                                \
        printf("%s:%d: UNIMPLEMENTED: %s\n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                        \
    } while (0)

#define UNUSED(v) (void) (v)

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

const float directions[4][2] = {
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1},
};

// TODO: add font gap
void draw_cartesian_plane(Rectangle rect, Vector2 step_size, int step_count, Font font, float font_gap, Color cross_color, Color step_color)
{
    draw_cross(rect, 2, cross_color);

    Vector2 line_start = make_vector2(rect.x + rect.width/2, rect.y + rect.height/2);
    for (int i = 0;  i < (int) (ARRAY_LEN(directions)); ++i) {
        for (int j = 1; j < step_count+1; ++j) {
            int xdir = directions[i][0];
            int ydir = directions[i][1];

            Vector2 curr_pos = make_vector2(
                line_start.x + (j*(rect.width/2 / step_count))*xdir,
                line_start.y + (j*(rect.height/2 / step_count))*ydir);

            Rectangle rect = (Rectangle) {
                .x = curr_pos.x,
                .y = curr_pos.y,
                .width = fabsf(step_size.x * xdir + step_size.y * ydir),
                .height = fabsf(step_size.y * xdir + step_size.x * ydir),
            };

            rect.x -= rect.width/2;
            rect.y -= rect.height/2;

            DrawRectangleRec(rect, step_color);

            const char *coordinate_text = TextFormat("%d", j*(xdir + -ydir));
            Vector2 text_size = MeasureTextEx(font, coordinate_text, FONT_SIZE, 1);
            DrawTextEx(font,
                       coordinate_text,
                       make_vector2(rect.x + (text_size.x/2 * -abs(xdir) + (text_size.x + font_gap) * -abs(ydir)),
                                    rect.y + (text_size.y/2 * -abs(ydir) + (text_size.y/2 + font_gap) * abs(xdir))), // TODO: document this
                       FONT_SIZE, 1, WHITE);
        }
    }
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

void draw_arrow(Vector2 start_point, Vector2 end_point, float thick, float tip_len, float tip_width, Color color)
{
    Vector2 diff = Vector2Subtract(end_point, start_point);
    float diff_len = Vector2Length(diff);
    diff = Vector2Scale(diff, (diff_len-tip_len)/diff_len);
    Vector2 tip_start = Vector2Add(diff, start_point);
    DrawLineEx(start_point, tip_start, thick, color);

    Vector2 left = Vector2Add(tip_start, polar_to_coords(tip_width/2, get_vec_angle(diff) - 90));
    Vector2 right = Vector2Add(tip_start, polar_to_coords(tip_width/2, get_vec_angle(diff) + 90));
    DrawTriangle(end_point, left, right, WHITE);
}

void draw_vector(Vector2 v, Vector2 start, float scalar)
{
    Vector2 cartesian_point = to_cartesian_system(v, start, scalar);
    draw_arrow(start, cartesian_point, 3, 20, 10, WHITE);
    DrawCircleV(cartesian_point, 3, RED);
}

// typedef struct {
//     Vector2 vector;
//     Color color;
//     const char *name;
// } GEOL_Vector2;

// typedef struct {
//     GEOL_Vector2 *items;
//     size_t count;
//     size_t capacity;
// } GEOL_Vector2s;

// typedef struct {
//     GEOL_Vector2s vectors;
//     Rectangle cartesian_plane_rect;
//     size_t unit_count;
// } Geo_Lib;

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(900, 600, "geolib");
    SetExitKey(0);
    Font font = LoadFontEx("fonts/iosevka.ttf", FONT_SIZE, NULL, 0);

    while (!WindowShouldClose()) {
        float w = GetRenderWidth();
        float h = GetRenderHeight();

        float grid_size = w/3;
        Rectangle grid_rect = (Rectangle) {
            .x = w/2 - grid_size/2,
            .y = h/2 - grid_size/2,
            .width = grid_size,
            .height = grid_size,
        };

        int step_count = 5;
        Vector2 grid_origin = make_vector2(grid_rect.x + grid_rect.width/2, grid_rect.y + grid_rect.height/2);
        float step_len = grid_size/2/step_count;

        BeginDrawing();
        {
            ClearBackground(GetColor(0x181818FF));
            draw_cartesian_plane(grid_rect, make_vector2(5, 20), step_count, font, 10.0, WHITE, WHITE);
            draw_vector(make_vector2(2, 1), grid_origin, step_len);
        }
        EndDrawing();
    }

    UnloadFont(font);
    return 0;
}

// TODO: create a function that allows me to put labels above, next to, below, inside rectangles
// TODO: Automatically assign each new vector a color and a name similar to how tsoding did with his layout system. (He used a struct to keep track of the state. That struct could be the cartesian coordinate plane itself)
