#include "cru/base/io/BufferStream.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <thread>

TEST_CASE("BufferStream should work.", "[io][stream]") {
  using namespace cru::io;

  const int size = 100;
  std::vector<std::byte> buffer(size);
  buffer[1] = std::byte(0xf0);
  buffer[2] = std::byte(0x0f);

  BufferStream stream;

  std::vector<std::byte> buffer2(size * 2);
  cru::Index read, read2;

  std::thread read_thread([&] {
    read = stream.Read(buffer2.data(), buffer2.size());
    read2 = stream.Read(buffer2.data(), size, 1);
  });

  std::thread write_thread([&] {
    stream.Write(buffer.data(), buffer.size());
    stream.WriteEof();
  });

  read_thread.join();
  write_thread.join();

  REQUIRE(read == size);
  REQUIRE(std::ranges::equal(buffer, buffer2 | std::views::take(size)));
  REQUIRE(read2 == Stream::kEOF);
}
