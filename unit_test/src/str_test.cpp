#include "str.hpp"

#include "framework.hpp"

#include "ewa/file.hpp"


using std::string;
using std::vector;

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

static void TestSplitString() {

    vector<string> strs = SplitString("v  23 33", " ");



    AssertEquals(strs[0], string("v") );
    AssertEquals(strs[1], string("23") );
    AssertEquals(strs[2], string("33") );




}



void StrTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestBeginsWith, "TestBeginsWith");
    suite.emplace_back(TestGetFilePath, "TestGetFilePath");
    suite.emplace_back(TestAppendPaths, "TestAppendPaths");
    suite.emplace_back(TestGetFile, "TestGetFile");

    suite.emplace_back(TestSplitString, "TestSplitString");


    RunSuite(suite, "Str");
}
