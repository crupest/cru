#include "cru/common/SubProcess.h"

#include <catch2/catch_test_macros.hpp>

using cru::SubProcess;

TEST_CASE("SubProcess", "[subprocess]") {
  SECTION("should work.") {
    SubProcess process = SubProcess::Create(u"echo", {u"abc"});
    process.Wait();
    REQUIRE(process.GetExitResult().IsSuccess());
    auto output = process.GetStdoutStream()->ReadToEndAsUtf8String();
    REQUIRE(output == u"abc\n");
  }
}
