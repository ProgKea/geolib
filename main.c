#define GEOLIB_IMPLEMENTATION
#include "geolib.h"

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(900, 600, "geolib");
    SetExitKey(0);

    Font font = LoadFontEx("../fonts/iosevka.ttf", 25, NULL, 0);

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

        if (IsWindowResized()) {
            geolib_update_plane_rect(&gl, plane_rect);
        }

        BeginDrawing();
        {
            ClearBackground(GetColor(0x181818FF));

#if defined(GEOLIB_ADDING)
            Vector2 a_vec = make_vector2(2, 3);
            Vector2 b_vec = make_vector2(5, -2.25);
            size_t a_idx = geolib_add_vector(&gl, a_vec);
            size_t b_idx = geolib_add_vector(&gl, b_vec);
            geolib_add_drawing_point(&gl, a_idx, b_vec);
            geolib_add_drawing_point(&gl, b_idx, a_vec);
            geolib_add_vector(&gl, Vector2Add(a_vec, b_vec));
#elif defined(GEOLIB_SUBTRACTING)
            Vector2 a_vec = make_vector2(5, -3);
            Vector2 b_vec = make_vector2(2, 4.5);
            geolib_add_vector(&gl, a_vec);
            geolib_add_vector(&gl, b_vec);
            geolib_add_vector_dps(&gl, Vector2Subtract(a_vec, b_vec), make_dp(0, 0), vec2dp(b_vec));
#elif defined(GEOLIB_ROTATING)
            size_t a_idx = geolib_add_vector(&gl, make_vector2(0, 5));
            Geolib_Vector2 *a_glv = geolib_get_vector(&gl, a_idx);
            a_glv->vector = Vector2Rotate(a_glv->vector, degrees2radians(90)*sin(GetTime()));
#elif defined(GEOLIB_SCALING)
            size_t a_idx = geolib_add_vector(&gl, make_vector2(10, 10));
            Geolib_Vector2 *a_glvec = geolib_get_vector(&gl, a_idx);
            a_glvec->vector = Vector2Scale(a_glvec->vector, sin(GetTime()));
#endif

            geolib_plot(&gl, make_vector2(2, 18), font, 10, WHITE, WHITE);
            geolib_draw_vecs_info(&gl, make_vector2(0, 0), font);

            geolib_clean(&gl);
        }
        EndDrawing();
    }

    UnloadFont(font);
    return 0;
}

// TODO: force the user to call `geolib_clean`
