#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
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

int nbr_digits = 8;

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
    j = (int)((float)WINDOW_Y_SIZE / 2) * (1 - (y / max_y));
    if (y <= 0) {
      j = ((float)WINDOW_Y_SIZE / 2) +
          (((float)WINDOW_Y_SIZE / 2) * (y / min_y));
    }
    assert(j >= 0 && j <= WINDOW_Y_SIZE); // really necessary?
    putPixelRGB(renderer, i, j, c.r, c.g, c.b);
  }
}

// Draw the digits showing the scale of the current window
void drawScaleDigits(SDL_Surface *surfaces[nbr_digits],
                     SDL_Rect dests[nbr_digits], SDL_Texture *texs[nbr_digits],
                     SDL_Renderer *rend) {
  float shifter = 10000000;
  float remaining = (max_x - min_x) / 2;
  int rest;
  for (int i = 0; i < nbr_digits; i++) {
    if ((remaining < 0.99) && (i == 0)) {
      shifter = 0.1;
      surfaces[i] =
          IMG_Load("../Plotter/images/d.png"); // This does not seem to work yet
    } else {
      rest = remaining / shifter;
      remaining -= (float)rest * shifter;
      shifter /= 10;
      switch (rest) {
      case 0:
        surfaces[i] = IMG_Load("../Plotter/images/0.png");
        break;
      case 1:
        surfaces[i] = IMG_Load("../Plotter/images/1.png");
        break;
      case 2:
        surfaces[i] = IMG_Load("../Plotter/images/2.png");
        break;
      case 3:
        surfaces[i] = IMG_Load("../Plotter/images/3.png");
        break;
      case 4:
        surfaces[i] = IMG_Load("../Plotter/images/4.png");
        break;
      case 5:
        surfaces[i] = IMG_Load("../Plotter/images/5.png");
        break;
      case 6:
        surfaces[i] = IMG_Load("../Plotter/images/6.png");
        break;
      case 7:
        surfaces[i] = IMG_Load("../Plotter/images/7.png");
        break;
      case 8:
        surfaces[i] = IMG_Load("../Plotter/images/8.png");
        break;
      case 9:
      default:
        surfaces[i] = IMG_Load("../Plotter/images/9.png");
      }
    }
    texs[i] = SDL_CreateTextureFromSurface(rend, surfaces[i]);
    SDL_QueryTexture(texs[i], NULL, NULL, &dests[i].w, &dests[i].h);
    dests[i].w /= 6;
    dests[i].h /= 6;
    dests[i].x = (50 - dests[i].w) + 15 * i;
    dests[i].y = (50 - dests[i].h);
    SDL_FreeSurface(surfaces[i]);
  }
  SDL_RenderClear(rend);
  for (int i = 0; i < nbr_digits; i++) {
    SDL_RenderCopy(rend, texs[i], NULL, &dests[i]);
  }
}
// This function has to be called after drawScaleDigits!
void removeDigits(SDL_Texture *texs[nbr_digits]) {
  for (int i = 0; i < nbr_digits; i++) {
    SDL_DestroyTexture(texs[i]);
  }
}
// -------------------------- Test Functions -------------------------------
float basicFunc(float x) { return x * x; }
float basicFunc2(float x) { return 1.1 * x + 2; }
float basicFunc3(float x) { return 1000 * sin(x / 500); }
float basicFunc4(float x) { return x != 0 ? 1 / x : 0; }

// ------------------------ Window Manipulation -------------------------
void changeScale(float scale) {
  if (DEBUG_PRINT) {
    printf("Changing Scale to: %.2f\n", scale);
  }
  min_x = -scale;
  max_x = scale;
  min_y = -scale;
  max_y = scale;
}
void incrementScale(int incr) {
  if (DEBUG_PRINT) {
    printf("Incrementing Scale\n");
  }
  min_x = min_x + incr * min_x / 10;
  max_x = max_x + incr * max_x / 10;
  min_y = min_y + incr * min_y / 10;
  max_y = max_y + incr * max_y / 10;
}

int main(int argc, char *argv[]) {
  // returns zero on success else non-zero
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
  }
  SDL_Window *win =
      SDL_CreateWindow("PLOTTER", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_X_SIZE, WINDOW_Y_SIZE, 0);

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);

  // Create scale digits:
  SDL_Surface *surfaces[nbr_digits];
  SDL_Rect dests[nbr_digits];
  SDL_Texture *texs[nbr_digits];

  int close = 0;

  while (!close) {
    SDL_Event event;
    drawScaleDigits(surfaces, dests, texs, rend);
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
          changeScale(0.1);
          break;
        case SDL_SCANCODE_2:
          changeScale(1);
          break;
        case SDL_SCANCODE_3:
          changeScale(100);
          break;
        case SDL_SCANCODE_4:
          incrementScale(-1);
          break;
        case SDL_SCANCODE_5:
          incrementScale(1);
          break;
        default:
          break;
        }
      }
    }
    SDL_RenderPresent(rend);
    SDL_Delay(1000 / 30);
  }

  removeDigits(texs);
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
