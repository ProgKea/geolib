#define VP_IMPLEMENTATION
#define USE_DEFAULTS
#include "vp.h"

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(900, 600, "geolib");
    SetExitKey(0);

    Font font = LoadFontEx("../fonts/iosevka.ttf", 25, NULL, 0);

    float w = GetRenderWidth();
    float h = GetRenderHeight();
    float grid_size = w/3;
    Vp vp = vp_alloc((Rectangle) {
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
            vp_update_plane_rect(&vp, plane_rect);
        }

        BeginDrawing();
        {
            ClearBackground(GetColor(0x181818FF));

#if defined(GEOLIB_ADDING)
            Vector2 a_vec = make_vec2(2, 3);
            Vector2 b_vec = make_vec2(5, -2.25);
            size_t a_idx = vp_add_vec(&vp, a_vec);
            size_t b_idx = vp_add_vec(&vp, b_vec);
            vp_add_dp(&vp, a_idx, b_vec);
            vp_add_dp(&vp, b_idx, a_vec);
            vp_add_vec(&vp, Vector2Add(a_vec, b_vec));
#elif defined(GEOLIB_SUBTRACTING)
            Vector2 a_vec = make_vec2(5, -3);
            Vector2 b_vec = make_vec2(2, 4.5);
            vp_add_vec(&vp, a_vec);
            vp_add_vec(&vp, b_vec);
            vp_add_vec_dps(&vp, Vector2Subtract(a_vec, b_vec), make_dp(0, 0), vec2dp(b_vec));
#elif defined(GEOLIB_ROTATING)
            size_t a_idx = vp_add_vec(&vp, make_vec2(0, 5));
            Vp_Vector2 *a_vpv = vp_get_vec(&vp, a_idx);
            a_vpv->vec = Vector2Rotate(a_vpv->vec, degrees2radians(90)*sin(GetTime()));
#elif defined(GEOLIB_SCALING)
            size_t a_idx = vp_add_vec(&vp, make_vec2(10, 10));
            Vp_Vector2 *a_vpvec = vp_get_vec(&vp, a_idx);
            a_vpvec->vec = Vector2Scale(a_vpvec->vec, sin(GetTime()));
#endif

            vp_plot(&vp, font);
            vp_draw_info(&vp, make_vec2(0, 0), font);
            vp_clean(&vp);
        }
        EndDrawing();
    }

    UnloadFont(font);
    return 0;
}
