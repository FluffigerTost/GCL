#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>

int g_rows = 0;
int g_collums = 0;

void GCL_HideCursor() {
    printf("\033[?25l");
}
void GCL_ShowCursor() {
    printf("\033[?25h");
}
void GCL_InitConsole(int rows, int collums) {
    g_rows = rows;
    g_collums = collums;

    printf("\033[8;%d;%dt", collums, rows);
}
void GCL_SetColor(uint8_t r, uint8_t g, uint8_t b){
    printf("\033[48;2;%d;%d;%dm", r, g, b);
}
void GCL_GotoXY(int x, int y) {
    printf("\033[%d;%dH", y, x);
}
void GCL_DrawPixel(int x, int y) {
    GCL_GotoXY(x, y);
    fwrite(" ", 1, 1, stdout);
}
void GCL_ClearBackground(unsigned long timer_millis) {
    static unsigned long t = 0;
    bool clearNow = false;
    if (GetTickCount() - t > timer_millis) {
        t = GetTickCount();
        clearNow = true;
    }
    if (clearNow) {
        GCL_GotoXY(1, 1);
        printf("\033[2J");
    }
}

void GCL_DrawRectangle(int x, int y, int width, int height) {
    char recBuffer[width];
    memset(recBuffer, ' ', sizeof(recBuffer));

    for (int i = 0; i < height; i++) {
        GCL_GotoXY(x, y + i);
        fwrite(recBuffer, 1, sizeof(recBuffer), stdout);
    }
}
void GCL_DrawLine(int x1, int y1, int x2, int y2) {
    int xDiff = x2 - x1;
    int yDiff = y2 - y1;
    float k = 0;
    if (xDiff != 0) {
        k = (float)yDiff / (float)xDiff;
    }

    int y = 0;
    int x = 0;
    int y_next = 0;

    if (xDiff > 0) {
        for (int i = 0; i <= xDiff; i++) {
            x = x1 + i;
            y = (int)roundf(k * (float)i + (float)y1);
            GCL_DrawPixel(x, y);
            if (x == x2) continue;

            y_next = (int)roundf(k * ((float)i + 1.0f) + (float)y1);
            int y_fix_diff = y_next - y;
            if (y_fix_diff > 1) {
                for (int j = 1; j < y_fix_diff; j++) {
                    GCL_DrawPixel(x + 1, y + j);
                }
            }
            else if (y_fix_diff < -1) {
                for (int j = -1; j > y_fix_diff; j--) {
                    GCL_DrawPixel(x + 1, y + j);
                }
            }
        }
    }
    else if (xDiff < 0) {
        for (int i = 0; i >= xDiff; i--) {
            x = x1 + i;
            y = (int)roundf(k * (float)i + (float)y1);
            GCL_DrawPixel(x, y);

            if (x == x2) continue;

            y_next = (int)roundf(k * ((float)i - 1.0f) + (float)y1);
            int y_fix_diff = y_next - y;
            if (y_fix_diff > 1) {
                for (int j = 1; j < y_fix_diff; j++) {
                    GCL_DrawPixel(x, y + j);
                }
            }
            else if (y_fix_diff < -1) {
                for (int j = -1; j > y_fix_diff; j--) {
                    GCL_DrawPixel(x, y + j);
                }
            }
        }
    }
    else if (xDiff == 0) {
        if (yDiff > 0) {
            for (int i = 0; i <= yDiff; i++) {
                GCL_DrawPixel(x1, y1 + i);
            }
        }
        else if (yDiff < 0) {
            for (int i = 0; i >= yDiff; i--) {
                GCL_DrawPixel(x1, y2 - i);
            }
        }
    }
}
void GCLDrawLineBresenham(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    int x = x1;
    int y = y1;

    while (1) {
        GCL_DrawPixel(x, y);

        if (x == x2 && y == y2) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}
