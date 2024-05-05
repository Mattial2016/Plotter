#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define WINDOW_X_SIZE 1000
#define MIDDLE_X WINDOW_X_SIZE / 2
#define WINDOW_Y_SIZE 1000
#define MIDDLE_Y WINDOW_Y_SIZE / 2
#define BAR_LENGTH WINDOW_X_SIZE / 100

#define DEBUG_PRINT true

// These variables hold the min/max x/y value currently
// shown in the window.
float min_x = -10;
float max_x = 10;
float min_y = -10;
float max_y = 10;

typedef struct my_color {
  int r;
  int g;
  int b;
} my_color;

// ------------------ Draw fundamental functions -------------------------
// Helper function for drawing a single pixel
void putPixelRGB(SDL_Renderer *renderer, int x, int y, unsigned char r,
                 unsigned char g, unsigned char b) {
  SDL_SetRenderDrawColor(renderer, (Uint8)r, (Uint8)g, (Uint8)b, 255);
  if (SDL_RenderDrawPoint(renderer, x, y) != 0) {
    printf("Failed RenderDrawPoint!\n");
    exit(1);
  }
  SDL_SetRenderDrawColor(renderer, (Uint8)0, (Uint8)0, (Uint8)0, 255);
}

// Helper function for drawing a single line
void putLineRGB(SDL_Renderer *renderer, int x1, int y1, int x2, int y2,
                unsigned char r, unsigned char g, unsigned char b) {
  SDL_SetRenderDrawColor(renderer, (Uint8)r, (Uint8)g, (Uint8)b, 255);
  if (SDL_RenderDrawLine(renderer, x1, y1, x2, y2) != 0) {
    printf("Failed RenderDrawPoint!\n");
    exit(1);
  }
  SDL_SetRenderDrawColor(renderer, (Uint8)0, (Uint8)0, (Uint8)0, 255);
}

// ------------------------ Draw functions -------------------------------
void drawCoordinateWindow(SDL_Renderer *renderer) {
  // Draw the coordinate axis:
  putLineRGB(renderer, WINDOW_X_SIZE / 2, 0, WINDOW_X_SIZE / 2, WINDOW_Y_SIZE,
             0, 255, 255);
  putLineRGB(renderer, 0, WINDOW_Y_SIZE / 2, WINDOW_X_SIZE, WINDOW_Y_SIZE / 2,
             0, 255, 255);
  // Draw the bars:
  for (int y = 0; y < WINDOW_Y_SIZE; y += WINDOW_Y_SIZE / 10) {
    putLineRGB(renderer, MIDDLE_X - BAR_LENGTH, y, MIDDLE_X + BAR_LENGTH, y, 0,
               255, 255);
  }
  for (int x = 0; x < WINDOW_X_SIZE; x += WINDOW_X_SIZE / 10) {
    putLineRGB(renderer, x, MIDDLE_Y - BAR_LENGTH, x, MIDDLE_Y + BAR_LENGTH, 0,
               255, 255);
  }
  // Make arrows for axis:
  putLineRGB(renderer, WINDOW_X_SIZE - 3 * BAR_LENGTH, MIDDLE_Y - BAR_LENGTH,
             WINDOW_X_SIZE, MIDDLE_Y, 0, 255, 255);
  putLineRGB(renderer, WINDOW_X_SIZE - 3 * BAR_LENGTH, MIDDLE_Y + BAR_LENGTH,
             WINDOW_X_SIZE, MIDDLE_Y, 0, 255, 255);
  putLineRGB(renderer, MIDDLE_X - BAR_LENGTH, 3 * BAR_LENGTH, MIDDLE_X, 0, 0,
             255, 255);
  putLineRGB(renderer, MIDDLE_X + BAR_LENGTH, 3 * BAR_LENGTH, MIDDLE_X, 0, 0,
             255, 255);
}

void drawFunction(SDL_Renderer *renderer, float func(float), my_color c) {
  float y;
  int i = -1;
  int j;
  for (float x = min_x; x < max_x; x += (max_x - min_x) / WINDOW_X_SIZE) {
    i++;
    y = func(x);
    if (y > max_y || y < min_y) {
      continue;
    }
    if (y > 0) {
      j = (int)((float)WINDOW_Y_SIZE / 2) * (1 - (y / max_y));
    } else {
      j = ((float)WINDOW_Y_SIZE / 2) +
          (((float)WINDOW_Y_SIZE / 2) * (y / min_y));
    }
    assert(j >= 0 && j <= WINDOW_Y_SIZE);
    putPixelRGB(renderer, i, j, c.r, c.g, c.b);
  }
}
// -------------------------- Test Functions -------------------------------
float basicFunc(float x) { return x * x; }
float basicFunc2(float x) { return 1.1 * x + 2; }
float basicFunc3(float x) { return sin(x); }

// ------------------------ Window Manipulation -------------------------
void changeScale(int scale) {
  if (DEBUG_PRINT) {
    printf("Changing Scale to: %d\n", scale);
  }
  min_x = -scale;
  max_x = scale;
  min_y = -scale;
  max_y = scale;
}

int main(int argc, char *argv[]) {
  // returns zero on success else non-zero
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
  }
  SDL_Window *win =
      SDL_CreateWindow("PLOTTER", // creates a window
                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_X_SIZE, WINDOW_Y_SIZE, 0);

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);

  int close = 0;

  while (!close) {
    SDL_Event event;
    SDL_RenderClear(rend);
    drawCoordinateWindow(rend);
    my_color f1 = {255, 0, 0};
    my_color f2 = {255, 0, 255};
    my_color f3 = {0, 255, 0};
    drawFunction(rend, basicFunc, f1);
    drawFunction(rend, basicFunc2, f2);
    drawFunction(rend, basicFunc3, f3);
    SDL_RenderPresent(rend);

    // Events management
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_QUIT:
        close = 1;
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_Q:
          exit(0);
        case SDL_SCANCODE_1:
          changeScale(10);
          break;
        case SDL_SCANCODE_2:
          changeScale(100);
          break;
        case SDL_SCANCODE_3:
          changeScale(1000);
          break;
        case SDL_SCANCODE_4:
          changeScale(1);
          break;
        default:
          break;
        }
      }
    }

    // SDL_RenderClear(rend);
    SDL_RenderPresent(rend);
    SDL_Delay(1000 / 30);
  }

  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
