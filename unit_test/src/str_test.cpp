#include "str.hpp"

#include "framework.hpp"

using std::string;

static void TestBeginsWith() {
    string str = "eric elias";

    AssertTrue(BeginsWith(str, "eric"));
    AssertNotTrue(BeginsWith(str, "ric"));
    AssertNotTrue(BeginsWith(str, "elias"));


}

static void TestGetFileDirectory() {
    AssertEquals(GetFileDirectory("shader/lib.glsl"),  string("shader/"));
    AssertEquals(GetFileDirectory("lib.glsl"), string( "./" ));
}

static void TestAppendPaths() {
    AssertEquals(AppendPaths("shader/", "lib.glsl"),  string("shader/lib.glsl"));
    AssertEquals(AppendPaths("shader", "lib.glsl"),  string("shader/lib.glsl"));
    AssertEquals(AppendPaths("./", "lib.glsl"),  string("./lib.glsl"));
    AssertEquals(AppendPaths(".", "lib.glsl"),  string("./lib.glsl"));


}

void StrTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestBeginsWith, "TestBeginsWith");
    suite.emplace_back(TestGetFileDirectory, "TestGetFileDirectory");
    suite.emplace_back(TestAppendPaths, "TestAppendPaths");

    RunSuite(suite, "Str");
}
