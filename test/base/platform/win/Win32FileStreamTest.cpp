#include "cru/base/StringUtil.h"
#include "cru/base/io/OpenFileFlag.h"
#include "cru/base/platform/win/Win32FileStream.h"

#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <filesystem>

TEST_CASE("Win32FileStream Work", "[stream]") {
  using namespace cru;
  using namespace cru::io;
  using namespace cru::platform::win;

  auto temp_file_path =
      (std::filesystem::temp_directory_path() / "cru_test_temp.XXXXXX")
          .native();
  _wmktemp(temp_file_path.data());

  std::string path = string::ToUtf8(temp_file_path);

  Win32FileStream file(path, OpenFileFlags::Write | OpenFileFlags::Create);
  auto write_count = file.Write("abc", 3);
  REQUIRE(write_count == 3);
  file.Close();

  REQUIRE(std::filesystem::file_size(path) == 3);

  Win32FileStream file2(path, OpenFileFlags::Read);
  auto buffer = std::make_unique<std::byte[]>(3);
  auto read_count = file2.Read(buffer.get(), 3);
  REQUIRE(read_count == 3);
  REQUIRE(std::string_view(reinterpret_cast<const char*>(buffer.get()), 3) ==
          "abc");
  file2.Close();

  std::filesystem::remove(temp_file_path);
}
