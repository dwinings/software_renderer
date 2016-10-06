#include <SDL/SDL.h>
#undef main
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Definitions.h"
#include "Rasterizer.h"
#include "Model.h"
#include "TGAImage.h"
#include "AffineTransforms.h"
#include "OldMainRoutines.h"
#include "Shaders.h"



void drawShaderHead(Rasterizer &rasterizer) {
  std::cerr << "Loading model assets...";
  Model model("african_head.obj");
  std::cerr << "Loading texture... ";
  model.load_texture("african_head_diffuse.tga.gz");
  std::cerr << "Loading normals... ";
  model.load_normal_texture("african_head_nm.tga.gz");

  Vector3f screen_coords[3];
  NormalMapDiffuseShader shader(model);

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

void load_icon(const char* path) {
  SDL_Surface *Icon;														// The icon itself
  Uint8       *Pixels, *Mask;												// We need a mask and a pointer for the raw image
  int         i, mlen;													// Iterator and a store variable
  Icon = SDL_LoadBMP(path);												// We load the bmp file

  if (Icon == NULL) {
    return;
  }

  if ((Icon->w % 8) != 0) {
    SDL_FreeSurface(Icon);
    return;
  }

  if (!Icon->format->palette) {
    SDL_FreeSurface(Icon);
    return;
  }

  SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, *((Uint8 *)Icon->pixels));
  Pixels = (Uint8 *)Icon->pixels;
  mlen = Icon->w*Icon->h;
  if (!(Mask = (Uint8 *)malloc(mlen / 8))) {
    SDL_FreeSurface(Icon);
    return;
  }

  // Dark icon-alpha-ing magic.
  memset(Mask, 0, mlen / 8);
  for (i = 0; i<mlen;) {

    if (Pixels[i] != *Pixels) {
      Mask[i / 8] |= 0x01;
    }
    ++i;
    if ((i % 8) != 0) {
      Mask[i / 8] <<= 1;
    }
  }

  SDL_WM_SetIcon(Icon, Mask);
  SDL_FreeSurface(Icon);
  return;
}

int main(int argc, char *argv[])
{
  // initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init failed\n");
    return 1;
  }

  SDL_WM_SetCaption("Software Renderer", "triangle");
  load_icon((MODELS_DIR + "icon.bmp").c_str());

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


    // BEGIN MAIN LOOP CODE

    // END MAIN LOOP CODE

    // lock surface and clear framebuffer
    SDL_LockSurface(screen);
    SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    // calculate the number of seconds that
    // have passed since the last update
    int time_ms = SDL_GetTicks();
    int elapsed_ms = time_ms - lastTicks;
    uint32_t sleepTime = std::max(0, 16 - elapsed_ms);

    float time = elapsed_ms / 1000.0f;
    lastTicks = time_ms;

    SDL_Delay(sleepTime);

    /*
    if (elapsed_ms > 0) {
      int fps = 1000 / elapsed_ms;
      printf("Frames per second: %u    \r", fps);
    }
    */
  }

  SDL_Quit();
  return 0;
}
