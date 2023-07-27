#define GEOLIB_IMPLEMENTATION
#include "geolib.h"

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

// TODO: Automatically assign each new vector a color and a name similar to how tsoding did with his layout system. (He used a struct to keep track of the state.)
