#include "str.hpp"

#include "framework.hpp"

#include "ewa/file.hpp"


using std::string;

static void TestBeginsWith() {
    string str = "eric elias";

    AssertTrue(BeginsWith(str, "eric"));
    AssertNotTrue(BeginsWith(str, "ric"));
    AssertNotTrue(BeginsWith(str, "elias"));
}

static void TestGetFilePath() {
    AssertEquals(File::GetFilePath("shader/lib.glsl"),  string("shader/"));
    AssertEquals(File::GetFilePath("lib.glsl"), string( "./" ));
    AssertEquals(File::GetFilePath("shader/folder/lib.glsl"),  string("shader/folder/"));
}

static void TestGetFile() {
    AssertEquals(File::GetFile("shader/lib.glsl"),  string("lib.glsl"));
    AssertEquals(File::GetFile("lib.glsl"), string( "lib.glsl" ));
    AssertEquals(File::GetFile("shader/folder/lib.glsl"),  string("lib.glsl"));
}

static void TestAppendPaths() {
    AssertEquals(File::AppendPaths("shader/", "lib.glsl"),  string("shader/lib.glsl"));
    AssertEquals(File::AppendPaths("shader", "lib.glsl"),  string("shader/lib.glsl"));
    AssertEquals(File::AppendPaths("./", "lib.glsl"),  string("./lib.glsl"));
    AssertEquals(File::AppendPaths(".", "lib.glsl"),  string("./lib.glsl"));


}

void StrTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestBeginsWith, "TestBeginsWith");
    suite.emplace_back(TestGetFilePath, "TestGetFilePath");
    suite.emplace_back(TestAppendPaths, "TestAppendPaths");
    suite.emplace_back(TestGetFile, "TestGetFile");

    RunSuite(suite, "Str");
}
