#include "cru/platform/gui/sdl/OpenGLRenderer.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/gui/sdl/Window.h"

namespace cru::platform::gui::sdl {
SdlOpenGLRenderer::SdlOpenGLRenderer(SdlWindow* window) {
  assert(window);
  assert(window->GetSdlWindow());

  window_ = window;
  sdl_window_ = window->GetSdlWindow();
  sdl_window_id_ = window->GetSdlWindowId();

  sdl_gl_context_ = SDL_GL_CreateContext(sdl_window_);

  if (!sdl_gl_context_) {
    throw SdlException("Failed to create sdl gl context.");
  }

  CheckSdlReturn(SDL_GL_MakeCurrent(sdl_window_, sdl_gl_context_));

  auto version = gladLoadGLContext(&glad_gl_context_, SDL_GL_GetProcAddress);
  CRU_LOG_TAG_DEBUG("SDL window id {}, openGL version: {}.", sdl_window_id_,
                    version);
}

SdlOpenGLRenderer::~SdlOpenGLRenderer() {
  CheckSdlReturn(SDL_GL_DestroyContext(sdl_gl_context_));
}

}  // namespace cru::platform::gui::sdl
