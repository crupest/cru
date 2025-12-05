#include "cru/platform/gui/sdl/OpenGLRenderer.h"
#include "cru/base/Base.h"
#include "cru/base/log/Logger.h"
#include "cru/platform/graphics/cairo/CairoPainter.h"
#include "cru/platform/gui/sdl/Base.h"
#include "cru/platform/gui/sdl/Window.h"

#include <SDL3/SDL_video.h>
#include <cairo.h>

namespace cru::platform::gui::sdl {
namespace {
constexpr float kVertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f,
                               0.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                               0.0f,  -1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
constexpr unsigned int kIndices[] = {0, 1, 2, 0, 2, 3};

constexpr auto kVertexShader = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
})";

constexpr auto kFragmentShader = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
})";
}  // namespace

SdlOpenGLRenderer::SdlOpenGLRenderer(SdlWindow* window, int width, int height) {
  assert(window);
  assert(window->GetSdlWindow());

  window_ = window;
  sdl_window_ = window->GetSdlWindow();
  sdl_window_id_ = window->GetSdlWindowId();

  sdl_gl_context_ = SDL_GL_CreateContext(sdl_window_);
  gl_texture_ = 0;
  cairo_surface_ = nullptr;
  cairo_ = nullptr;

  if (!sdl_gl_context_) {
    throw SdlException("Failed to create sdl gl context.");
  }

  auto _ = MakeContextCurrent();

  auto version = gladLoadGLContext(&glad_gl_context_, SDL_GL_GetProcAddress);
  CruLogDebug(kLogTag, "SDL window id {}, openGL version: {}.{}.",
              sdl_window_id_, GLAD_VERSION_MAJOR(version),
              GLAD_VERSION_MINOR(version));

  glad_gl_context_.Enable(GL_BLEND);
  glad_gl_context_.BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  gl_shader_program_ = CreateGLProgram();

  glad_gl_context_.GenVertexArrays(1, &gl_vertex_array_);
  glad_gl_context_.GenBuffers(1, &gl_vertex_buffer_);
  glad_gl_context_.GenBuffers(1, &gl_element_buffer_);

  glad_gl_context_.BindVertexArray(gl_vertex_array_);

  glad_gl_context_.BindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_);
  glad_gl_context_.BufferData(GL_ARRAY_BUFFER, sizeof(kVertices),
                              static_cast<const void*>(kVertices),
                              GL_STATIC_DRAW);

  glad_gl_context_.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_element_buffer_);
  glad_gl_context_.BufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices),
                              static_cast<const void*>(kIndices),
                              GL_STATIC_DRAW);

  glad_gl_context_.VertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(0));
  glad_gl_context_.EnableVertexAttribArray(0);
  glad_gl_context_.VertexAttribPointer(
      1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
      reinterpret_cast<void*>(3 * sizeof(float)));
  glad_gl_context_.EnableVertexAttribArray(1);

  Resize(width, height);
}

SdlOpenGLRenderer::~SdlOpenGLRenderer() {
  glad_gl_context_.DeleteBuffers(1, &gl_vertex_buffer_);
  glad_gl_context_.DeleteBuffers(1, &gl_element_buffer_);
  glad_gl_context_.DeleteTextures(1, &gl_texture_);

  if (cairo_) {
    cairo_destroy(cairo_);
    cairo_ = nullptr;
  }

  if (cairo_surface_) {
    cairo_surface_destroy(cairo_surface_);
    cairo_surface_ = nullptr;
  }

  CheckSdlReturn(SDL_GL_DestroyContext(sdl_gl_context_));
}

void SdlOpenGLRenderer::Resize(int width, int height) {
  CruLogDebug(kLogTag, "Resize to {} x {}", width, height);

  width_ = width;
  height_ = height;

  auto _ = MakeContextCurrent();

  if (cairo_) {
    cairo_destroy(cairo_);
    cairo_ = nullptr;
  }

  if (cairo_surface_) {
    cairo_surface_destroy(cairo_surface_);
    cairo_surface_ = nullptr;
  }

  if (gl_texture_) {
    glad_gl_context_.DeleteTextures(1, &gl_texture_);
    gl_texture_ = 0;
  }

  cairo_surface_ =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  auto status = cairo_surface_status(cairo_surface_);
  if (status != CAIRO_STATUS_SUCCESS) {
    throw Exception("Failed to create cairo surface.");
  }

  cairo_ = cairo_create(cairo_surface_);
  status = cairo_status(cairo_);
  if (status != CAIRO_STATUS_SUCCESS) {
    throw Exception("Failed to create cairo context.");
  }

  glad_gl_context_.Viewport(0, 0, width, height);

  glad_gl_context_.GenTextures(1, &gl_texture_);
  glad_gl_context_.BindTexture(GL_TEXTURE_2D, gl_texture_);
  glad_gl_context_.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                 GL_NEAREST);
  glad_gl_context_.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                 GL_NEAREST);
}

std::unique_ptr<graphics::IPainter> SdlOpenGLRenderer::BeginPaint() {
  assert(cairo_surface_);
  assert(gl_texture_);

  auto painter = std::make_unique<graphics::cairo::CairoPainter>(
      nullptr, cairo_, false, cairo_surface_);
  painter->SetEndDrawCallback([this] { Present(); });
  return painter;
}

void SdlOpenGLRenderer::Present() {
  assert(cairo_surface_);
  assert(gl_texture_);

  auto _ = MakeContextCurrent();

  auto data = cairo_image_surface_get_data(cairo_surface_);
  glad_gl_context_.TexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, width_, height_, 0,
                              GL_BGRA, GL_UNSIGNED_BYTE, data);

  glad_gl_context_.UseProgram(gl_shader_program_);
  glad_gl_context_.ActiveTexture(GL_TEXTURE0);
  glad_gl_context_.BindTexture(GL_TEXTURE_2D, gl_texture_);
  glad_gl_context_.BindVertexArray(gl_vertex_array_);
  glad_gl_context_.BindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_);
  glad_gl_context_.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_element_buffer_);

  glad_gl_context_.DrawElements(
      GL_TRIANGLES, sizeof(kIndices) / sizeof(*kIndices), GL_UNSIGNED_INT, 0);
  CheckSdlReturn(SDL_GL_SwapWindow(sdl_window_));
}

GLuint SdlOpenGLRenderer::CreateGLProgram() {
  auto check_shader = [this](std::string_view name, GLuint shader) {
    int success;
    char infoLog[512];
    glad_gl_context_.GetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glad_gl_context_.GetShaderInfoLog(shader, 512, nullptr, infoLog);
      CruLogError(kLogTag, "Failed to compile OpenGL {} shader: {}", name,
                  infoLog);
    }
  };

  auto check_program = [this](std::string_view name, GLuint program) {
    int success;
    char infoLog[512];
    glad_gl_context_.GetProgramiv(program, GL_COMPILE_STATUS, &success);
    if (!success) {
      glad_gl_context_.GetProgramInfoLog(program, 512, nullptr, infoLog);
      CruLogError(kLogTag, "Failed to link OpenGL {} program: {}", name,
                  infoLog);
    }
  };

  auto vertex_shader = glad_gl_context_.CreateShader(GL_VERTEX_SHADER);
  glad_gl_context_.ShaderSource(vertex_shader, 1, &kVertexShader, nullptr);
  glad_gl_context_.CompileShader(vertex_shader);
  check_shader("vertex", vertex_shader);

  auto fragment_shader = glad_gl_context_.CreateShader(GL_FRAGMENT_SHADER);
  glad_gl_context_.ShaderSource(fragment_shader, 1, &kFragmentShader, nullptr);
  glad_gl_context_.CompileShader(fragment_shader);
  check_shader("fragment", fragment_shader);

  auto shader_program = glad_gl_context_.CreateProgram();
  glad_gl_context_.AttachShader(shader_program, vertex_shader);
  glad_gl_context_.AttachShader(shader_program, fragment_shader);
  glad_gl_context_.LinkProgram(shader_program);
  check_program("main", shader_program);

  glad_gl_context_.DeleteShader(fragment_shader);
  glad_gl_context_.DeleteShader(vertex_shader);

  return shader_program;
}

Guard SdlOpenGLRenderer::MakeContextCurrent() {
  auto old_context = SDL_GL_GetCurrentContext();
  auto old_window = SDL_GL_GetCurrentWindow();

  CheckSdlReturn(SDL_GL_MakeCurrent(sdl_window_, sdl_gl_context_));

  return Guard([old_context, old_window] {
    CheckSdlReturn(SDL_GL_MakeCurrent(old_window, old_context));
  });
}

}  // namespace cru::platform::gui::sdl
