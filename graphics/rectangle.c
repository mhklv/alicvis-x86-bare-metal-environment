// #include "types.h"
// #include "drivers/display.h"
// #include "graphics/rectangle.h"



// void draw_rect(struct Rectangle* rect) {
//     uint32 i, j;

//     if (rect->x >= screen_width()) {
//         rect->x = 0;
//     }

//     if (rect->y >= screen_height()) {
//         rect->y = 0;
//     }
    
//     for (i = rect->y; i < rect->y + rect->height; ++i) {
//         for (j = rect->x; j < rect->x + rect->width; ++j) {
//             plot_pixel(j, i);
//         }
//     }
//     // rect->active = 1;
// }


// void move_rect(struct Rectangle* rect, int32 delta_x,
//                int32 delta_y, int32 delta_width, int32 delta_height,
//                uint8 r, uint8 g, uint8 b) {
//     if (rect->active) {
//         set_pixel_col(0, 0, 0);
//         draw_rect(rect);
//         set_pixel_col(r, g, b);
//     }
//     rect->x += delta_x;
//     rect->y += delta_y;
//     rect->width += delta_width;
//     rect->height += delta_height;
//     if (rect->active) {
//         draw_rect(rect);
//     }
// }



