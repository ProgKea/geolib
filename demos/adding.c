#define GEOLIB_IMPLEMENTATION
#include "../geolib.h"

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(900, 600, "Geolib scaling");
    SetExitKey(0);

    Font font = LoadFontEx("../fonts/iosevka.ttf", 30, NULL, 0);

    float w = GetRenderWidth();
    float h = GetRenderHeight();
    float grid_size = w/3;
    Geolib gl = geolib_alloc((Rectangle) {
            .x = w/2 - grid_size/2,
            .y = h/2 - grid_size/2,
            .width = grid_size,
            .height = grid_size,
        }, 5);

    Vector2 a = make_vector2(2, 1);
    Vector2 b = make_vector2(-2, 1);
    Vector2 c = Vector2Add(a, b);
    geolib_add_vector(&gl, a);
    geolib_add_vector(&gl, b);
    geolib_add_vector(&gl, c);

    while (!WindowShouldClose()) {
        w = GetRenderWidth();
        h = GetRenderHeight();
        grid_size = w/2;

        Rectangle plane_rect = (Rectangle) {
            .x = w/2 - grid_size/2,
            .y = h/2 - grid_size/2,
            .width = grid_size,
            .height = grid_size,
        };

        geolib_update_plane_rect(&gl, plane_rect);

        BeginDrawing();
        {
            ClearBackground(GetColor(0x181818FF));
            geolib_draw_plane(&gl, make_vector2(5, 20), font, 10, WHITE, WHITE);
            geolib_plot_vecs(&gl);
            geolib_draw_vecs_info(&gl, make_vector2(0, 0), font);
        }
        EndDrawing();
    }

    geolib_dealloc(&gl);
    UnloadFont(font);
    return 0;
}