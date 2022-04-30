#pragma once

#define WIDTH  800
#define HEIGHT 800
#define SPACING 5

#define ROW_SQ (WIDTH/SPACING)
#define COLUMN_SQ (HEIGHT/SPACING)
#define TOTAL_SQUARES (ROW_SQ * COLUMN_SQ )
#define GRID_LINES ( ((ROW_SQ + 1) * 2) + ((COLUMN_SQ + 1) * 2))
