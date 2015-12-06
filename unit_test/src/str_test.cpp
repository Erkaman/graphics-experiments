#include "string_util.hpp"

#include "framework.hpp"

#include "ewa/file.hpp"


using std::string;
using std::vector;

static void TestBeginsWith() {
    string str = "eric elias";

    AssertTrue(StringUtil::BeginsWith(str, "eric"));
    AssertNotTrue(StringUtil::BeginsWith(str, "ric"));
    AssertNotTrue(StringUtil::BeginsWith(str, "elias"));
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

    vector<string> strs = StringUtil::SplitString("v  23 33  ", " ");

    AssertEquals(strs[0], string("v") );
    AssertEquals(strs[1], string("23") );
    AssertEquals(strs[2], string("33") );

    strs = StringUtil::SplitString("v  23", " ", true);

    AssertEquals(strs[0], string("v") );
    AssertEquals(strs[1], string(" ") );
    AssertEquals(strs[2], string(" ") );
    AssertEquals(strs[3], string("23") );


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
