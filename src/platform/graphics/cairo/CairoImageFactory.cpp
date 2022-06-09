#include "cru/platform/graphics/cairo/CairoImageFactory.h"
#include "cru/common/Exception.h"
#include "cru/platform/Check.h"
#include "cru/platform/graphics/cairo/CairoImage.h"
#include "cru/platform/graphics/cairo/CairoResource.h"

#include <png.h>
#include <memory>

namespace cru::platform::graphics::cairo {

namespace {
bool IsPngHeader(const std::byte* buffer, int size) {
  return png_sig_cmp(reinterpret_cast<png_const_bytep>(buffer), 0, size) == 0;
}

std::uint32_t Mul(std::uint32_t v, float a) {
  if (a == 0.f) return v;
  return static_cast<std::uint32_t>(v * a);
}

std::uint32_t ConvertPngPixelToCairo(std::uint32_t source) {
  unsigned char* source_bytes = reinterpret_cast<unsigned char*>(&source);

  std::uint32_t result = 0;

  std::uint32_t alpha = source_bytes[3];

  float a = source_bytes[3] / 255.f;

  result |= alpha << 24;

  result |= Mul(static_cast<std::uint32_t>(source_bytes[0]), a) << 16;
  result |= Mul(static_cast<std::uint32_t>(source_bytes[1]), a) << 8;
  result |= Mul(static_cast<std::uint32_t>(source_bytes[2]), a);

  return result;
}

std::unique_ptr<CairoImage> DecodePng(CairoGraphicsFactory* factory,
                                      io::Stream* stream) {
  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace_type = 0;
  int compression_type = 0;
  int filter_type = 0;
  png_bytepp row_pointers = nullptr;

  png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    return nullptr;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return nullptr;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return nullptr;
  }

  png_set_read_fn(png_ptr, stream,
                  [](png_structp png_ptr, png_bytep data, png_size_t length) {
                    auto stream =
                        static_cast<io::Stream*>(png_get_io_ptr(png_ptr));
                    stream->Read(reinterpret_cast<std::byte*>(data), length);
                  });

  png_read_png(png_ptr, info_ptr,
               PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_SWAP_ALPHA |
                   PNG_TRANSFORM_GRAY_TO_RGB,
               nullptr);

  width = png_get_image_width(png_ptr, info_ptr);
  height = png_get_image_height(png_ptr, info_ptr);

  row_pointers = png_get_rows(png_ptr, info_ptr);

  std::unique_ptr<CairoImage> cairo_image(
      new CairoImage(factory, width, height));
  auto cairo_surface = cairo_image->GetCairoSurface();

  auto cairo_surface_stride = cairo_image_surface_get_stride(cairo_surface);
  auto cairo_surface_data = cairo_image_surface_get_data(cairo_surface);

  for (int row = 0; row < height; row++) {
    auto row_pointer = reinterpret_cast<std::uint32_t*>(row_pointers[row]);
    auto cairo_row_data = reinterpret_cast<std::uint32_t*>(
        cairo_surface_data + row * cairo_surface_stride);

    for (int col = 0; col < width; col++) {
      cairo_row_data[col] = ConvertPngPixelToCairo(row_pointer[col]);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

  return cairo_image;
}

std::uint8_t Div(std::uint32_t v, float a) {
  if (a == 0.f) return v;
  std::uint32_t temp = static_cast<std::uint32_t>(v / a);
  if (temp > 255) temp = 255;
  return temp;
}

std::uint32_t ConvertCairoPixelToPng(std::uint32_t source) {
  std::uint32_t result = 0;
  unsigned char* result_bytes = reinterpret_cast<unsigned char*>(&result);

  result_bytes[3] = (source & 0xFF000000) >> 24;
  float a = result_bytes[3] / 255.f;

  result_bytes[0] = Div((source & 0x00FF0000) >> 16, a);
  result_bytes[1] = Div((source & 0x0000FF00) >> 8, a);
  result_bytes[2] = Div(source & 0x000000FF, a);

  return result;
}

void EncodePng(cairo_surface_t* cairo_surface, io::Stream* stream) {
  auto width = cairo_image_surface_get_width(cairo_surface);
  auto height = cairo_image_surface_get_height(cairo_surface);
  auto cairo_surface_stride = cairo_image_surface_get_stride(cairo_surface);
  auto cairo_surface_data = cairo_image_surface_get_data(cairo_surface);

  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;

  png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    return;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return;
  }

  png_set_write_fn(
      png_ptr, stream,
      [](png_structp png_ptr, png_bytep data, png_size_t length) {
        auto stream = static_cast<io::Stream*>(png_get_io_ptr(png_ptr));
        stream->Write(reinterpret_cast<std::byte*>(data), length);
      },
      nullptr);

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  std::vector<std::byte> buffer;
  buffer.resize(width * height * 4);

  for (int row = 0; row < height; row++) {
    auto buffer_row =
        reinterpret_cast<std::uint32_t*>(buffer.data() + row * width * 4);
    auto cairo_row_data = reinterpret_cast<std::uint32_t*>(
        cairo_surface_data + row * cairo_surface_stride);
    for (int col = 0; col < width; col++) {
      buffer_row[col] = ConvertCairoPixelToPng(cairo_row_data[col]);
    }
  }

  std::vector<png_bytep> row_pointer;
  row_pointer.resize(height);
  for (int row = 0; row < height; row++) {
    row_pointer[row] =
        reinterpret_cast<png_bytep>(buffer.data() + row * width * 4);
  }

  png_set_rows(png_ptr, info_ptr, row_pointer.data());

  png_write_png(png_ptr, info_ptr, 0, nullptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
}
}  // namespace

CairoImageFactory::CairoImageFactory(CairoGraphicsFactory* factory)
    : CairoResource(factory) {}

CairoImageFactory::~CairoImageFactory() {}

std::unique_ptr<IImage> CairoImageFactory::DecodeFromStream(
    io::Stream* stream) {
  std::byte buffer[8];
  stream->Read(buffer, 8);
  if (IsPngHeader(buffer, 8)) {
    stream->Seek(0, io::Stream::SeekOrigin::Begin);
    return DecodePng(GetCairoGraphicsFactory(), stream);
  }

  throw Exception(u"Image format unknown. Currently only support png.");
}

void CairoImageFactory::EncodeToStream(IImage* image, io::Stream* stream,
                                       ImageFormat format, float quality) {
  auto cairo_image = CheckPlatform<CairoImage>(image, GetPlatformId());

  if (format == ImageFormat::Png) {
    EncodePng(cairo_image->GetCairoSurface(), stream);
    return;
  }

  throw Exception(u"Not implemented. Currently only support png.");
}

std::unique_ptr<IImage> CairoImageFactory::CreateBitmap(int width, int height) {
  return std::make_unique<CairoImage>(GetCairoGraphicsFactory(), width, height);
}

}  // namespace cru::platform::graphics::cairo
