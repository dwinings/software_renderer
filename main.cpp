/*
 * Copyright (C) 2009 Josh A. Beam
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdio>
#include <cmath>
#include <SDL/SDL.h>
#include "Rasterizer.h"
#include "Model.h"
#include "TGAImage.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

Color black(0, 0, 0, 1);
Color white(1, 1, 1, 1);
Color red(1, 0, 0, 1);
Color green(0, 1, 0, 1);
Color blue(0, 0, 1, 1);
Color cyan(0, 1, 1, 1);
Color yellow(1, 1, 0, 1);
Color magenta(1, 0, 1, 1);

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

void tga_to_color(const TGAColor& tcolor, Color& color) {
  color.R = tcolor.bgra[2];
  color.G = tcolor.bgra[1];
  color.B = tcolor.bgra[0];
  color.A = tcolor.bgra[3];
}

Vector3f world2screen(Vector3f p, const Rasterizer& rasterizer) {
  return Vector3f(
      (p.x + 1.0f) * rasterizer.width()  / 2,
      (p.y + 1.0f) * rasterizer.height() / 2,
      p.z
  );
}

void drawTestLines(Rasterizer &rasterizer) {
  rasterizer.Line(175, 100, 0, 0, cyan);
  rasterizer.Line(0, 100, 175, 200, cyan);
  rasterizer.Line(0, WINDOW_HEIGHT, 50, 0, black, green);
  rasterizer.Line(0, 0, 175, 100, black, green);
}

void drawWireframeHead(Rasterizer &rasterizer) {
  Color colors[]{black, white, red, green, blue, cyan, yellow};

  Model model("african_head.obj");
  model.load_texture("african_head_diffuse.tga");


  for (uint32_t face_idx = 0; face_idx < model.face_count(); face_idx++) {
    for (uint32_t face_vertex = 0; face_vertex < 3; face_vertex++) {
      int32_t face_vertex_num = model.face(face_idx)[face_vertex][0];
      Vector3f vec_0 = model.vertex(model.face(face_idx)[face_vertex][0]);
      Vector3f vec_1 = model.vertex(model.face(face_idx)[(face_vertex + 1) % 3][0]);
      int start_x = (int)((vec_0.x + 1.0f) * rasterizer.width()  / 2.0f);
      int start_y = (int)((vec_0.y + 1.0f) * rasterizer.height() / 2.0f);
      int end_x   = (int)((vec_1.x + 1.0f) * rasterizer.width()  / 2.0f);
      int end_y   = (int)((vec_1.y + 1.0f) * rasterizer.height() / 2.0f);
      rasterizer.Line(start_x, start_y, end_x, end_y, colors[std::rand() % 7]);
    }
#ifdef DEBUG
    std::cerr << "-----------------------------------------------------------\n";
#endif
  }
}

void drawTriangles(Rasterizer &rasterizer) {
  Vector3f t0[] {Vector3f(10, 70, 0), Vector3f(50, 160, 0), Vector3f(70, 80, 0)};
  Vector3f t1[] {Vector3f(180, 50, 0),  Vector3f(150, 1, 0),   Vector3f(70, 180, 0)};
  Vector3f t2[] {Vector3f(180, 150, 0), Vector3f(120, 160, 0), Vector3f(130, 180, 0)};
  rasterizer.Triangle(t0, cyan);
  rasterizer.Triangle(t1, yellow);
  rasterizer.Triangle(t2, magenta);
}


void drawTriangleHead(Rasterizer &rasterizer) {
  Color colors[]{black, white, red, green, blue, cyan, yellow};

  Model model("african_head.obj");
  model.load_texture("african_head_diffuse.tga");
  TGAImage texture;
  Color theColor;

  Vector3f screen_coords[3];
  Vector3f world_coords[3];
  Vector2f tex_coords[3];
  Vector3f light_direction(0.0f, 0.0f, -1.0f);
  light_direction = light_direction.normalize();


  for (uint32_t face_idx = 0; face_idx < model.face_count(); face_idx++) {
    std::vector<Vector3i> face = model.face(face_idx);

    for (uint32_t face_vertex_idx = 0; face_vertex_idx < 3; face_vertex_idx++) {
      Vector3f vertex = model.vertex(face[face_vertex_idx][0]);
      Vector2f texture_uv = model.texture(face[face_vertex_idx][1]);
      Vector3f normal = model.normal(face[face_vertex_idx][2]);

      world_coords[face_vertex_idx] = vertex;
      screen_coords[face_vertex_idx] = world2screen(vertex, rasterizer);
      tex_coords[face_vertex_idx] = texture_uv;
    }

    // || AC x AB ||
    auto normal = ((world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0])).normalize();
    float intensity = (normal * light_direction);

    if (intensity > 1) intensity = 1;

    if (intensity > 0){
      rasterizer.Triangle(screen_coords[0], screen_coords[1], screen_coords[2], tex_coords, model, intensity);
    }
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

  drawTriangleHead(rasterizer);
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
    printf("Frames per second: %u    \r", fps);
  }

  SDL_Quit();
  return 0;
}
