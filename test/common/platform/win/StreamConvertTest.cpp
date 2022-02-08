#include "cru/common/io/OpenFileFlag.h"
#include "cru/common/platform/win/Exception.h"
#include "cru/common/platform/win/StreamConvert.h"
#include "cru/common/platform/win/Win32FileStream.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>

TEST(StreamConvert, FileStreamWork) {
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
  ASSERT_EQ(std::string_view(buffer.get(), 3), "abc");
  com_stream->Release();
  file2.Close();

  std::filesystem::remove(temp_file_path);
}
