#include "cru/base/StringUtil.h"
#include "cru/base/platform/win/Stream.h"

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

  std::string path = string::ToUtf8String(temp_file_path);

  ComStream file(path, OpenFileFlags::Write | OpenFileFlags::Create);
  file.Write("abc", 3);
  file.Close();

  ComStream file2(path, OpenFileFlags::Read);
  IStream* com_stream = ToComStream(&file2);
  LARGE_INTEGER position;
  position.QuadPart = 0;
  CheckHResult(com_stream->Seek(position, SEEK_SET, nullptr));
  auto buffer = std::make_unique<char[]>(3);
  CheckHResult(com_stream->Read(buffer.get(), 3, nullptr));
  REQUIRE(std::string_view(buffer.get(), 3) == "abc");
  com_stream->Release();
  file2.Close();

  std::filesystem::remove(temp_file_path);
}

TEST_CASE("ComStream Work", "[stream]") {
  using namespace cru;
  using namespace cru::io;
  using namespace cru::platform::win;

  auto temp_file_path =
      (std::filesystem::temp_directory_path() / "cru_test_temp.XXXXXX")
          .native();
  _wmktemp(temp_file_path.data());

  std::string path = string::ToUtf8String(temp_file_path);

  ComStream file(path, OpenFileFlags::Write | OpenFileFlags::Create);
  auto write_count = file.Write("abc", 3);
  REQUIRE(write_count == 3);
  file.Close();

  REQUIRE(std::filesystem::file_size(path) == 3);

  ComStream file2(path, OpenFileFlags::Read);
  auto buffer = std::make_unique<std::byte[]>(3);
  auto read_count = file2.Read(buffer.get(), 3);
  REQUIRE(read_count == 3);
  REQUIRE(std::string_view(reinterpret_cast<const char*>(buffer.get()), 3) ==
          "abc");
  file2.Close();

  std::filesystem::remove(temp_file_path);
}
