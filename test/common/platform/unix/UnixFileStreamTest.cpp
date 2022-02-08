#include "cru/common/io/OpenFileFlag.h"
#include "cru/common/platform/unix/UnixFileStream.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>

TEST(UnixFileStream, Work) {
  using namespace cru;
  using namespace cru::io;
  using namespace cru::platform::unix;

  auto temp_file_path =
      (std::filesystem::temp_directory_path() / "cru_test_temp.XXXXXX")
          .generic_string();
  mktemp(temp_file_path.data());

  String path = String::FromUtf8(temp_file_path);

  UnixFileStream file(path, OpenFileFlags::Write | OpenFileFlags::Create);
  file.Write("abc", 3);
  file.Close();

  UnixFileStream file2(path, OpenFileFlags::Read);
  auto buffer = std::make_unique<std::byte[]>(3);
  file2.Read(buffer.get(), 3);
  ASSERT_EQ(std::string_view(reinterpret_cast<const char*>(buffer.get()), 3),
            "abc");
  file2.Close();

  std::filesystem::remove(temp_file_path);
}
