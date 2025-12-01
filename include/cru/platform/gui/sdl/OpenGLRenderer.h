#pragma once

#include "gl.h"

#include <cru/base/Base.h>

#include <SDL3/SDL_video.h>

namespace cru::platform::gui::sdl {
class SdlWindow;

class SdlOpenGLRenderer {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::gui::sdl::SdlOpenGLRenderer")
 public:
  explicit SdlOpenGLRenderer(SdlWindow* window);
  ~SdlOpenGLRenderer();

 private:
  SdlWindow* window_;
  SDL_Window* sdl_window_;
  SDL_WindowID sdl_window_id_;
  SDL_GLContext sdl_gl_context_;
  GladGLContext glad_gl_context_;
};
}  // namespace cru::platform::gui::sdl
