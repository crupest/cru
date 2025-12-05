#pragma once

#include "gl.h"

#include <cru/base/Base.h>
#include <cru/base/Guard.h>
#include <cru/platform/GraphicsBase.h>
#include <cru/platform/graphics/Painter.h>
#include <cru/platform/gui/Base.h>

#include <SDL3/SDL_video.h>
#include <cairo.h>

namespace cru::platform::gui::sdl {
class SdlWindow;

class SdlOpenGLRenderer {
 private:
  constexpr static auto kLogTag = "cru::platform::gui::sdl::SdlOpenGLRenderer";

 public:
  SdlOpenGLRenderer(SdlWindow* window, int width, int height);
  ~SdlOpenGLRenderer();

  void Resize(int width, int height);
  std::unique_ptr<graphics::IPainter> BeginPaint();
  void Present();

 private:
  GLuint CreateGLProgram();
  Guard MakeContextCurrent();

 private:
  SdlWindow* window_;
  SDL_Window* sdl_window_;
  SDL_WindowID sdl_window_id_;

  int width_, height_;

  SDL_GLContext sdl_gl_context_;
  GladGLContext glad_gl_context_;

  GLuint gl_shader_program_;
  GLuint gl_vertex_array_;
  GLuint gl_vertex_buffer_;
  GLuint gl_element_buffer_;
  GLuint gl_texture_;

  cairo_surface_t* cairo_surface_;
  cairo_t* cairo_;
};
}  // namespace cru::platform::gui::sdl
