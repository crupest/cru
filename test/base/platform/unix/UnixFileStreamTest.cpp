#include "cru/base/platform/unix/UnixFileStream.h"

#include <catch2/catch_test_macros.hpp>

#include <fcntl.h>
#include <filesystem>

TEST_CASE("UnixFileStream Work", "[stream]") {
  using namespace cru;
  using namespace cru::io;
  using namespace cru::platform::unix;

  auto temp_file_path =
      (std::filesystem::temp_directory_path() / "cru_test_temp.XXXXXX")
          .generic_string();
  mkstemp(temp_file_path.data());

  UnixFileStream file(temp_file_path.c_str(), O_WRONLY | O_CREAT);
  file.Write("abc", 3);
  file.Close();

  UnixFileStream file2(temp_file_path.c_str(), O_RDONLY);
  auto buffer = std::make_unique<std::byte[]>(3);
  file2.Read(buffer.get(), 3);
  REQUIRE(std::string_view(reinterpret_cast<const char*>(buffer.get()), 3) ==
          "abc");
  file2.Close();

  std::filesystem::remove(temp_file_path);
}
