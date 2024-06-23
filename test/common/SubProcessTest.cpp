#include "cru/common/String.h"
#include "cru/common/SubProcess.h"

#include <catch2/catch_test_macros.hpp>

using cru::String;
using cru::SubProcess;

TEST_CASE("SubProcess", "[subprocess]") {
  SECTION("echo should work.") {
    SubProcess process = SubProcess::Create(
        String::FromUtf8(CRU_TEST_HELPER_ECHO_LOCATION), {u"abc"});
    process.Wait();
    REQUIRE(process.GetExitResult().IsSuccess());
    auto output = process.GetStdoutStream()->ReadToEndAsUtf8String();
    REQUIRE(output == u"abc");
  }

  SECTION("tee should work.") {
    constexpr auto str = "abc";
    SubProcess process =
        SubProcess::Create(String::FromUtf8(CRU_TEST_HELPER_TEE_LOCATION));
    process.GetStdinStream()->Write(str, 3);
    process.GetStdinStream()->Close();
    process.Wait();
    REQUIRE(process.GetExitResult().IsSuccess());
    auto output = process.GetStdoutStream()->ReadToEndAsUtf8String();
    REQUIRE(output == u"abc");
  }
}
