#include "cru/base/io/OpenFileFlag.h"
#include "cru/base/platform/win/Exception.h"
#include "cru/base/platform/win/StreamConvert.h"
#include "cru/base/platform/win/Win32FileStream.h"

#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <filesystem>

TEST_CASE("StreamConvert FileStreamWork", "[stream]") {
  using namespace cru;
  using namespace cru::io;
  using namespace cru::platform::win;

  auto temp_file_path =
      (std::filesystem::temp_directory_path() / "cru_test_temp.XXXXXX")
          .native();
  _wmktemp(temp_file_path.data());

  String path = temp_file_path;

  Win32FileStream file(path, OpenFileFlags::Write | OpenFileFlags::Create);
  file.Write("abc", 3);
  file.Close();

  Win32FileStream file2(path, OpenFileFlags::Read);
  IStream* com_stream = ConvertStreamToComStream(&file2);
  LARGE_INTEGER position;
  position.QuadPart = 0;
  ThrowIfFailed(com_stream->Seek(position, SEEK_SET, nullptr));
  auto buffer = std::make_unique<char[]>(3);
  ThrowIfFailed(com_stream->Read(buffer.get(), 3, nullptr));
  REQUIRE(std::string_view(buffer.get(), 3) == "abc");
  com_stream->Release();
  file2.Close();

  std::filesystem::remove(temp_file_path);
}
