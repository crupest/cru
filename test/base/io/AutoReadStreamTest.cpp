#include "cru/base/io/AutoReadStream.h"
#include "cru/base/io/MemoryStream.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <cstddef>
#include <ranges>

TEST_CASE("AutoReadStream should work.", "[io][stream]") {
  using namespace cru::io;

  const int size = 100;
  std::vector<std::byte> buffer(size);
  buffer[1] = std::byte(0xf0);
  buffer[2] = std::byte(0x0f);

  MemoryStream underlying_stream(buffer.data(), buffer.size(), true);
  AutoReadStream stream(&underlying_stream, true, false);

  REQUIRE(stream.CanRead());
  REQUIRE(!stream.CanWrite());

  std::vector<std::byte> buffer2(size * 2);

  auto read = stream.Read(buffer2.data(), buffer2.size());
  auto read2 = stream.Read(buffer2.data(), size, 1);

  REQUIRE(read == size);
  REQUIRE(std::ranges::equal(buffer, buffer2 | std::views::take(size)));
  REQUIRE(read2 == Stream::kEOF);
}
