#define GEOLIB_IMPLEMENTATION
#include "geolib.h"

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(900, 600, "geolib");
    SetExitKey(0);

    Font font = LoadFontEx("fonts/iosevka.ttf", 25, NULL, 0);

    float w = GetRenderWidth();
    float h = GetRenderHeight();
    float grid_size = w/3;
    Geolib gl = geolib_alloc((Rectangle) {
            .x = w/2 - grid_size/2,
            .y = h/2 - grid_size/2,
            .width = grid_size,
            .height = grid_size,
        }, 10);

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

            geolib_add_vector(&gl, make_vector2(2, 3));

            geolib_plot(&gl, make_vector2(2, 18), font, 10, WHITE, WHITE);
            geolib_draw_vecs_info(&gl, make_vector2(0, 0), font);

            geolib_clean(&gl);
        }
        EndDrawing();
    }

    UnloadFont(font);
    return 0;
}

// TODO: maybe add vectors every frame, and clean it afterwards
// TODO: force the user to call `geolib_clean`
