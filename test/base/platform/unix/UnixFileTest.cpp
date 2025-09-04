
#include "cru/base/platform/unix/UnixFile.h"

#include <catch2/catch_test_macros.hpp>

#include <fcntl.h>
#include <filesystem>

TEST_CASE("UnixFile Auto Close", "[unix]") {
  using namespace cru;
  using namespace cru::platform::unix;

  auto calledTimes = 0;
  auto mockClose = [&calledTimes](int _) {
    calledTimes += 1;
    return 0;
  };

  auto temp_file_path =
      (std::filesystem::temp_directory_path() / "cru_test_temp.XXXXXX")
          .generic_string();
  mkstemp(temp_file_path.data());

  auto fdNumber = ::open(temp_file_path.c_str(), O_WRONLY | O_CREAT);

  {
    UnixFileDescriptor fd(fdNumber, true, std::move(mockClose));
    REQUIRE(calledTimes == 0);
    UnixFileDescriptor fd2(std::move(fd));
    REQUIRE(calledTimes == 0);
    UnixFileDescriptor fd3;
    fd3 = std::move(fd2);
    REQUIRE(calledTimes == 0);
  }
  REQUIRE(calledTimes == 1);

  std::filesystem::remove(temp_file_path);
}

TEST_CASE("UnixFile NonBlock Read", "[unix]") {
  using namespace cru;
  using namespace cru::platform::unix;

  char buffer[1];

  auto pipe = OpenUniDirectionalPipe(UnixPipeFlags::NonBlock);
  REQUIRE(pipe.read.Read(buffer, 1) == -1);
}
