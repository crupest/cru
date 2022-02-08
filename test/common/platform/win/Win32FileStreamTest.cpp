#include "cru/common/io/OpenFileFlag.h"
#include "cru/common/platform/win/Win32FileStream.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>

TEST(Win32FileStream, Work) {
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
  auto buffer = std::make_unique<std::byte[]>(3);
  file2.Read(buffer.get(), 3);
  ASSERT_EQ(std::string_view(reinterpret_cast<const char*>(buffer.get()), 3),
            "abc");
  file2.Close();

  std::filesystem::remove(temp_file_path);
}
