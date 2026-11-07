#include "cru/base/SubProcess.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>
#include <vector>

using cru::ParseCommandLineArguments;
using cru::SubProcess;

namespace {

// The helper writes '\n' after every argument. When stdout is a pipe, the C
// runtime may translate it into "\r\n", so CRLF is normalized to LF.
std::string NormalizeLineEndings(std::string text) {
  std::string result;
  result.reserve(text.size());
  for (size_t i = 0; i < text.size(); ++i) {
    if (text[i] == '\r' && i + 1 < text.size() && text[i + 1] == '\n') {
      continue;
    }
    result.push_back(text[i]);
  }
  return result;
}

// Runs the command line and returns what the program printed. Throws if the
// process can't be started or exits with a failure.
std::string RunCommandLine(std::string_view command_line) {
  auto process = SubProcess::CreateFromCommandLine(command_line);
  process.Wait();
  if (!process.GetExitResult().IsSuccess()) {
    throw cru::Exception("The helper process failed to run.");
  }
  return NormalizeLineEndings(
      process.GetStdoutStream()->ReadToEndAsUtf8String());
}

// The program path is single quoted: that handles paths containing spaces and
// keeps the backslashes of a Windows path literal, as nothing is escaped
// inside single quotes.
std::string EchoCommandLine(std::string_view arguments) {
  return std::string("'") + CRU_TEST_HELPER_ECHO_LOCATION + "' " +
         std::string(arguments);
}

}  // namespace

TEST_CASE("SubProcess", "[subprocess]") {
  SECTION("echo should work.") {
    SubProcess process =
        SubProcess::Create(CRU_TEST_HELPER_ECHO_LOCATION, {"abc"});
    process.Wait();
    REQUIRE(process.GetExitResult().IsSuccess());
    auto output = NormalizeLineEndings(
        process.GetStdoutStream()->ReadToEndAsUtf8String());
    REQUIRE(output == "abc\n");
  }

  SECTION("tee should work.") {
    constexpr auto str = "abc";
    SubProcess process = SubProcess::Create(CRU_TEST_HELPER_TEE_LOCATION);
    process.GetStdinStream()->Write(str, 3);
    process.GetStdinStream()->Close();
    process.Wait();
    REQUIRE(process.GetExitResult().IsSuccess());
    auto output = process.GetStdoutStream()->ReadToEndAsUtf8String();
    REQUIRE(output == "abc");
  }
}

TEST_CASE("SubProcess::CallFromCommandLine", "[subprocess]") {
  SECTION("A command line runs the program it names.") {
    REQUIRE(
        SubProcess::CallFromCommandLine(EchoCommandLine("abc")).IsSuccess());
  }

  SECTION("Quoting and escaping produce the intended argument.") {
    REQUIRE(RunCommandLine(EchoCommandLine("abc")) == "abc\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"("a b")")) == "a b\n");
    REQUIRE(RunCommandLine(EchoCommandLine("'a b'")) == "a b\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"(a\ b)")) == "a b\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"("a\"b")")) == "a\"b\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"('a"b')")) == "a\"b\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"(a\\b)")) == "a\\b\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"('a\b')")) == "a\\b\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"("C:\\dir name\\x")")) ==
            "C:\\dir name\\x\n");
    // A trailing backslash must survive being quoted for the child process.
    REQUIRE(RunCommandLine(EchoCommandLine(R"("a b\\")")) == "a b\\\n");
  }

  SECTION("Argument boundaries are preserved.") {
    REQUIRE(RunCommandLine(EchoCommandLine(R"("a b" 'c d' e)")) ==
            "a b\nc d\ne\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"( "" "" )")) == "\n\n");
  }

  SECTION("An empty argument reaches the program as an empty string.") {
    REQUIRE(RunCommandLine(EchoCommandLine(R"("")")) == "\n");
    REQUIRE(RunCommandLine(EchoCommandLine(R"(a "")")) == "a\n\n");
  }

  SECTION("An empty command line throws.") {
    REQUIRE_THROWS_AS(SubProcess::CreateFromCommandLine(""), cru::Exception);
    REQUIRE_THROWS_AS(SubProcess::CallFromCommandLine(""), cru::Exception);
    REQUIRE_THROWS_AS(SubProcess::CallFromCommandLine("   "), cru::Exception);
  }

  SECTION("A malformed command line is rejected before starting.") {
    REQUIRE_THROWS_AS(SubProcess::CallFromCommandLine(EchoCommandLine(R"("a)")),
                      cru::Exception);
    REQUIRE_THROWS_AS(
        SubProcess::CallFromCommandLine(EchoCommandLine(R"(a\nb)")),
        cru::Exception);
  }
}

TEST_CASE("ParseCommandLineArguments", "[subprocess]") {
  SECTION("Empty command line yields no argument.") {
    REQUIRE(ParseCommandLineArguments("").empty());
    REQUIRE(ParseCommandLineArguments("   ").empty());
  }

  SECTION("Rule 1: arguments are separated by ' '.") {
    REQUIRE(ParseCommandLineArguments("a b") ==
            std::vector<std::string>{"a", "b"});
    REQUIRE(ParseCommandLineArguments("  a   b  ") ==
            std::vector<std::string>{"a", "b"});
    REQUIRE(ParseCommandLineArguments("cmd --flag=1 -x") ==
            std::vector<std::string>{"cmd", "--flag=1", "-x"});
  }

  SECTION("Rule 1: only ' ' separates, other whitespace is literal.") {
    REQUIRE(ParseCommandLineArguments("a\tb") ==
            std::vector<std::string>{"a\tb"});
    REQUIRE(ParseCommandLineArguments("a\nb") ==
            std::vector<std::string>{"a\nb"});
  }

  SECTION("Rule 2: arguments containing spaces are quoted.") {
    REQUIRE(ParseCommandLineArguments(R"("a b")") ==
            std::vector<std::string>{"a b"});
    REQUIRE(ParseCommandLineArguments("'a b'") ==
            std::vector<std::string>{"a b"});
    REQUIRE(ParseCommandLineArguments(
                R"(cmd "arg with space" plain 'another one')") ==
            std::vector<std::string>{"cmd", "arg with space", "plain",
                                     "another one"});
  }

  SECTION("Rule 3: the other quote character is literal inside quotes.") {
    REQUIRE(ParseCommandLineArguments(R"("a'b")") ==
            std::vector<std::string>{"a'b"});
    REQUIRE(ParseCommandLineArguments(R"('a"b')") ==
            std::vector<std::string>{R"(a"b)"});
    REQUIRE(ParseCommandLineArguments(R"("a'b'c")") ==
            std::vector<std::string>{"a'b'c"});
    REQUIRE(ParseCommandLineArguments(R"('a"b"c')") ==
            std::vector<std::string>{R"(a"b"c)"});
  }

  SECTION("Rule 4: backslash escapes itself, space and both quotes.") {
    REQUIRE(ParseCommandLineArguments(R"(a\\b)") ==
            std::vector<std::string>{R"(a\b)"});
    REQUIRE(ParseCommandLineArguments(R"(a\ b)") ==
            std::vector<std::string>{"a b"});
    REQUIRE(ParseCommandLineArguments(R"(a\'b)") ==
            std::vector<std::string>{"a'b"});
    REQUIRE(ParseCommandLineArguments(R"(a\"b)") ==
            std::vector<std::string>{R"(a"b)"});
    REQUIRE(ParseCommandLineArguments(R"(\ a)") ==
            std::vector<std::string>{" a"});
    REQUIRE(ParseCommandLineArguments(R"(a\ b c)") ==
            std::vector<std::string>{"a b", "c"});
  }

  SECTION("Rule 4: an escaped character alone is a whole argument.") {
    REQUIRE(ParseCommandLineArguments(R"(\ )") ==
            std::vector<std::string>{" "});
    REQUIRE(ParseCommandLineArguments(R"(\")") ==
            std::vector<std::string>{R"(")"});
    REQUIRE(ParseCommandLineArguments(R"(\')") ==
            std::vector<std::string>{"'"});
    REQUIRE(ParseCommandLineArguments(R"(\\)") ==
            std::vector<std::string>{R"(\)"});
  }

  SECTION("Rule 4: escaping also works inside double quotes.") {
    REQUIRE(ParseCommandLineArguments(R"("a\ b")") ==
            std::vector<std::string>{"a b"});
    REQUIRE(ParseCommandLineArguments(R"("a\\b")") ==
            std::vector<std::string>{R"(a\b)"});
    REQUIRE(ParseCommandLineArguments(R"("a\"b")") ==
            std::vector<std::string>{R"(a"b)"});
    REQUIRE(ParseCommandLineArguments(R"("a\'b")") ==
            std::vector<std::string>{"a'b"});
    REQUIRE(ParseCommandLineArguments(R"("\"")") ==
            std::vector<std::string>{R"(")"});
  }

  SECTION("Rule 4: nothing is escaped inside single quotes.") {
    REQUIRE(ParseCommandLineArguments(R"('a\ b')") ==
            std::vector<std::string>{R"(a\ b)"});
    REQUIRE(ParseCommandLineArguments(R"('a\nb')") ==
            std::vector<std::string>{R"(a\nb)"});
    REQUIRE(ParseCommandLineArguments(R"('a\')") ==
            std::vector<std::string>{R"(a\)"});
  }

  SECTION("Rule 4: escaping other characters is an error.") {
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"(a\nb)"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"("a\nb")"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments("a\\\tb"), cru::Exception);
  }

  SECTION("Rule 4: a trailing escape character is an error.") {
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"(abc\)"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"("abc\)"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"(\)"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"("a"\)"), cru::Exception);
  }

  SECTION("Rule 5: unpaired quotes are an error.") {
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"("abc)"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments("'abc"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"(abc")"), cru::Exception);
    REQUIRE_THROWS_AS(ParseCommandLineArguments("abc'"), cru::Exception);
    // An escaped quote does not close the quoted section.
    REQUIRE_THROWS_AS(ParseCommandLineArguments(R"("a\")"), cru::Exception);
  }

  SECTION("Rule 6: quoted and unquoted parts are concatenated.") {
    REQUIRE(ParseCommandLineArguments(R"(a"b c")") ==
            std::vector<std::string>{"ab c"});
    REQUIRE(ParseCommandLineArguments(R"('a b'c)") ==
            std::vector<std::string>{"a bc"});
    REQUIRE(ParseCommandLineArguments(R"("a"'b')") ==
            std::vector<std::string>{"ab"});
    REQUIRE(ParseCommandLineArguments(R"(a"b"c)") ==
            std::vector<std::string>{"abc"});
    REQUIRE(ParseCommandLineArguments(R"("a"'b'"c")") ==
            std::vector<std::string>{"abc"});
  }

  SECTION("Rule 2: empty quoted arguments are empty string arguments.") {
    REQUIRE(ParseCommandLineArguments(R"("")") == std::vector<std::string>{""});
    REQUIRE(ParseCommandLineArguments("''") == std::vector<std::string>{""});
    REQUIRE(ParseCommandLineArguments(R"(a "" b)") ==
            std::vector<std::string>{"a", "", "b"});
    REQUIRE(ParseCommandLineArguments(R"("" '')") ==
            std::vector<std::string>{"", ""});
    REQUIRE(ParseCommandLineArguments(R"(""a)") ==
            std::vector<std::string>{"a"});
  }

  SECTION("Rule 2: empty arguments at the ends of the command line.") {
    REQUIRE(ParseCommandLineArguments(R"("" a)") ==
            std::vector<std::string>{"", "a"});
    REQUIRE(ParseCommandLineArguments(R"(a "")") ==
            std::vector<std::string>{"a", ""});
    REQUIRE(ParseCommandLineArguments(R"("" "" "")") ==
            std::vector<std::string>{"", "", ""});
    // Adjacent empty quotes are still a single empty argument.
    REQUIRE(ParseCommandLineArguments(R"("""")") ==
            std::vector<std::string>{""});
    // Empty quotes in the middle of an argument do not split it.
    REQUIRE(ParseCommandLineArguments(R"(a"" b)") ==
            std::vector<std::string>{"a", "b"});
  }

  SECTION("Quoted arguments can consist of spaces only.") {
    REQUIRE(ParseCommandLineArguments(R"(" ")") ==
            std::vector<std::string>{" "});
    REQUIRE(ParseCommandLineArguments("' '") == std::vector<std::string>{" "});
    REQUIRE(ParseCommandLineArguments(R"("a ")") ==
            std::vector<std::string>{"a "});
    REQUIRE(ParseCommandLineArguments(R"(" a")") ==
            std::vector<std::string>{" a"});
  }

  SECTION("Adjacent quoted arguments are separated by spaces.") {
    REQUIRE(ParseCommandLineArguments(R"("a" "b")") ==
            std::vector<std::string>{"a", "b"});
    REQUIRE(ParseCommandLineArguments("'a'  'b'") ==
            std::vector<std::string>{"a", "b"});
    REQUIRE(ParseCommandLineArguments(R"("a"'b' "c")") ==
            std::vector<std::string>{"ab", "c"});
  }

  SECTION("Rule 4: escapes next to quotes.") {
    // An escaped quote is literal and does not open a quoted section.
    REQUIRE(ParseCommandLineArguments(R"(\"a b)") ==
            std::vector<std::string>{R"("a)", "b"});
    // An escape right after a closing quote still belongs to the argument.
    REQUIRE(ParseCommandLineArguments(R"("a"\ b)") ==
            std::vector<std::string>{"a b"});
    // An escaped backslash does not escape the closing quote.
    REQUIRE(ParseCommandLineArguments(R"("a\\")") ==
            std::vector<std::string>{R"(a\)"});
  }
}
