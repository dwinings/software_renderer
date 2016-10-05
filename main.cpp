#include <SDL/SDL.h>
#undef main
#include <iostream>
#include <cmath>
#include "Definitions.h"
#include "Rasterizer.h"
#include "Model.h"
#include "TGAImage.h"
#include "AffineTransforms.h"
#include "OldMainRoutines.h"
#include "Shaders.h"



void drawShaderHead(Rasterizer &rasterizer) {
  Model model("african_head.obj");
  model.load_texture("african_head_diffuse.tga");
  Vector3f screen_coords[3];
  GouraudShader shader(model);

  for (uint32_t face_idx = 0; face_idx < model.face_count(); face_idx++) {
    std::vector<Vector3i> face = model.face(face_idx);
    for (uint32_t face_vertex_idx = 0; face_vertex_idx < 3; face_vertex_idx++) {
      screen_coords[face_vertex_idx] = shader.vertex(face_idx, face_vertex_idx);
    }

    rasterizer.Triangle(screen_coords, shader);
  }
}


static bool g_Running = true;

static void
HandleKeyEvent(const SDL_Event &event)
{
  switch(event.key.keysym.sym) {
    default:
      break;
    case SDLK_ESCAPE:
      g_Running = false;
      break;
  }
}

static void
HandleEvent(const SDL_Event &event)
{
  switch(event.type) {
    default:
      break;
    case SDL_QUIT:
      g_Running = false;
      break;
    case SDL_KEYDOWN:
      HandleKeyEvent(event);
      break;
  }
}

int main(int argc, char *argv[])
{
  // initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init failed\n");
    return 1;
  }


  // create window for drawing
  SDL_Surface *screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_SWSURFACE);
  if(!screen) {
    SDL_Quit();
    fprintf(stderr, "SDL_SetVideoMode failed\n");
    return 1;
  }

  Rasterizer rasterizer;
  rasterizer.SetFrameBuffer((uint32_t*)screen->pixels, WINDOW_WIDTH, WINDOW_HEIGHT);

  drawShaderHead(rasterizer);
  unsigned int lastTicks = SDL_GetTicks();

  // loop until we're done running the program
  while(g_Running) {
    // handle events
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      HandleEvent(event);
    }


    /*
// BEGIN MAIN LOOP CODE
     */

    // END MAIN LOOP CODE

    // lock surface and clear framebuffer
    SDL_LockSurface(screen);
    SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    // calculate the number of seconds that
    // have passed since the last update
    unsigned int ticks = SDL_GetTicks();
    unsigned int ticksDiff = ticks - lastTicks;
    if(ticksDiff == 0)
      continue;
    float time = ticksDiff / 1000.0f;
    lastTicks = ticks;

    // display frames per second
    unsigned int fps = 1000 / ticksDiff;
    // printf("Frames per second: %u    \r", fps);
  }

  SDL_Quit();
  return 0;
}
