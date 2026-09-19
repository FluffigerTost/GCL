#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>


typedef struct {
    int x, y;
}GCL_Point;
typedef struct {
    float r;
    float g;
    float b;
    float a;
}GCL_RGBA;
#define GCL_RED     (GCL_RGBA){1.0f, 0.0f, 0.0f, 1.0}
#define GCL_GREEN   (GCL_RGBA){0.0f, 1.0f, 0.0f, 1.0}
#define GCL_BLUE    (GCL_RGBA){0.0f, 0.0f, 1.0f, 1.0}
int g_rows = 0;
int g_collums = 0;
GCL_RGBA g_backgroundColor;

void GCL_HideCursor() {
    printf("\033[?25l");
}
void GCL_ShowCursor() {
    printf("\033[?25h");
}
void GCL_InitConsole(int rows, int collums) {
    g_rows = rows;
    g_collums = collums;
    g_backgroundColor = (GCL_RGBA){1.0f, 1.0f, 1.0f, 1.0f};

    printf("\033[8;%d;%dt", collums, rows);
}
void GCL_BackgroundColor(GCL_RGBA color) {
    g_backgroundColor = color;
}
void GCL_SetColor(GCL_RGBA color){
    color.r = color.r * color.a + (g_backgroundColor.r * (1.0f - color.a));
    color.g = color.g * color.a + (g_backgroundColor.g * (1.0f - color.a));
    color.b = color.b * color.a + (g_backgroundColor.b * (1.0f - color.a));
    printf("\033[48;2;%d;%d;%dm", (int)roundf(color.r * 255), (int)roundf(color.g * 255), (int)roundf(color.b * 255));
}
void GCL_GotoXY(int x, int y) {
    printf("\033[%d;%dH", y, x);
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
        GCL_SetColor(g_backgroundColor);
        printf("\033[2J");
    }
}
void GCL_DrawPixel(int x, int y) {
    GCL_GotoXY(x, y);
    fwrite(" ", 1, 1, stdout);
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

static void GCL_DrawLineInArray(int x1, int y1, int x2, int y2, bool** screenBuffer) {
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
            screenBuffer[y - 1][x - 1] = 1;
            if (x == x2) continue;

            y_next = (int)roundf(k * ((float)i + 1.0f) + (float)y1);
            int y_fix_diff = y_next - y;
            if (y_fix_diff > 1) {
                for (int j = 1; j < y_fix_diff; j++) {
                    screenBuffer[y + j - 1][x - 1] = 1;
                }
            }
            else if (y_fix_diff < -1) {
                for (int j = -1; j > y_fix_diff; j--) {
                    screenBuffer[y + j - 1][x - 1] = 1;
                }
            }
        }
    }
    else if (xDiff < 0) {
        for (int i = 0; i >= xDiff; i--) {
            x = x1 + i;
            y = (int)roundf(k * (float)i + (float)y1);
            screenBuffer[y - 1][x - 1] = 1;

            if (x == x2) continue;

            y_next = (int)roundf(k * ((float)i - 1.0f) + (float)y1);
            int y_fix_diff = y_next - y;
            if (y_fix_diff > 1) {
                for (int j = 1; j < y_fix_diff; j++) {
                    screenBuffer[y + j - 1][x - 1] = 1;
                }
            }
            else if (y_fix_diff < -1) {
                for (int j = -1; j > y_fix_diff; j--) {
                    screenBuffer[y + j - 1][x - 1] = 1;
                }
            }
        }
    }
    else if (xDiff == 0) {
        if (yDiff > 0) {
            for (int i = 0; i <= yDiff; i++) {
                screenBuffer[y1 + i - 1][x1 - 1] = 1;
            }
        }
        else if (yDiff < 0) {
            for (int i = 0; i >= yDiff; i--) {
                screenBuffer[y2 - i - 1][x1 - 1] = 1;
            }
        }
    }
}
bool** screenBuffer = nullptr;
void GCL_DrawTriangle(GCL_Point p1, GCL_Point p2, GCL_Point p3) {
    static bool initScreenBuffer = true;
    if (initScreenBuffer) {
        initScreenBuffer = false;
        screenBuffer = malloc((g_collums) * sizeof(bool*));
        for (int y = 0; y < g_collums; y++) {
            screenBuffer[y] = malloc((g_rows) * sizeof(bool));
        }
    }

    for (int y = 0; y < g_collums; y++) {
        memset(screenBuffer[y], 0, g_rows);
    }

    GCL_DrawLineInArray(p1.x, p1.y, p2.x, p2.y, screenBuffer);
    GCL_DrawLineInArray(p2.x, p2.y, p3.x, p3.y, screenBuffer);
    GCL_DrawLineInArray(p3.x, p3.y, p1.x, p1.y, screenBuffer);

    GCL_DrawLine(p1.x, p1.y, p2.x, p2.y);
    GCL_DrawLine(p2.x, p2.y, p3.x, p3.y);
    GCL_DrawLine(p3.x, p3.y, p1.x, p1.y);

    int min_x = min(p1.x, min(p2.x, p3.x)) - 1;
    int max_x = max(p1.x, max(p2.x, p3.x));

    int min_y = min(p1.y, min(p2.y, p3.y)) - 1;
    int max_y = max(p1.y, max(p2.y, p3.y));

    int screenBuffer_x = 0;
    int screenBuffer_y = 0;
    for (int y = 0; y < max_y - min_y; y++) {
        for (int x = 0; x < max_x - min_x; x++) {
            screenBuffer_x = min_x + x;
            screenBuffer_y = min_y + y;
            if (screenBuffer[screenBuffer_y][screenBuffer_x] == true) {
                GCL_DrawPixel(screenBuffer_x + 1, screenBuffer_y + 1);

                for (int scan_x = screenBuffer_x + 2; scan_x < max_x; scan_x++) {
                    if (screenBuffer[screenBuffer_y][scan_x] == 1 && screenBuffer[screenBuffer_y][scan_x - 1] == 0) {
                        int len = scan_x - (screenBuffer_x) + 1;
                        char array[len];
                        memset(array, ' ', len * sizeof(bool));
                        GCL_GotoXY(screenBuffer_x + 1, screenBuffer_y + 1);
                        fwrite(array, sizeof(array), 1, stdout);
                        goto xFinish;
                    }
                }
            }
        }
        xFinish:
    }
}
void GCL_DrawCircle(int x, int y, int r, float x_scale) {
    for (int i = -r; i <= r; i++) {
        int x_calc = roundf(sqrtf(r * r - i * i) * x_scale);
        int x_left  = x - x_calc;
        int x_right = x + x_calc;
        int x_diff = x_right - x_left;

        char line[x_diff + 1];
        memset(line,  ' ', sizeof(line));
        GCL_GotoXY(x_left, y + i);
        fwrite(line, 1, sizeof(line), stdout);

    }
}
void GCL_DrawCircleLines(int x, int y, int r, float x_scale) {
    for (int i = -r; i <= 0; i++) {
        int x_calc = roundf(sqrtf(r * r - i * i) * x_scale);

        if (i == r) continue;
        int x_next =  roundf(sqrtf(r * r - (i + 1) * (i + 1)) * x_scale);
        int x_diff = abs(x_next - x_calc);
        if (1) {
            char line[x_diff];
            memset(line, ' ', sizeof(line));

            if (x_diff > 0) {
                GCL_GotoXY(x + x_calc, y - i);
                fwrite(line, 1, sizeof(line), stdout);

                GCL_GotoXY(x - x_calc - x_diff, y - i);
                fwrite(line, 1, sizeof(line), stdout);



                GCL_GotoXY(x + x_calc, y + i);
                fwrite(line, 1, sizeof(line), stdout);

                GCL_GotoXY(x - x_calc - x_diff, y + i);
                fwrite(line, 1, sizeof(line), stdout);
            }
            else {
                GCL_GotoXY(x + x_calc, y - i);
                fwrite(" ", 1, 1, stdout);

                GCL_GotoXY(x - x_calc - 1, y - i);
                fwrite(" ", 1, 1, stdout);



                GCL_GotoXY(x + x_calc , y + i);
                fwrite(" ", 1, 1, stdout);

                GCL_GotoXY(x - x_calc - 1, y + i);
                fwrite(" ", 1, 1, stdout);
            }

        }
    }
}

void GCL_CleanProgramm() {
    for (int y = 0; y < g_collums; y++) {
        free(screenBuffer[y]);
    }
}
