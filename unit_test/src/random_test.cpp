#include "random.hpp"

#include "framework.hpp"

static void TestRandomFloat() {
    Random rng(1);

    constexpr int N = 2000;

    for(int i= 0; i < N; ++i) {

	float f = rng.RandomFloat(3.5f,5.9f);
	if(f < 3.5f || f > 5.9f) {
	    std::string message;
	    message += "The random number ";
	    message += std::to_string(f);
	    message += " was not in the expected range [3.5,5.9]";

	    AssertFailed(message);
	}
    }
    AssertPassed("1. All random number were in the expected range [3.5,5.9]");

    bool found = false;

    for(int i= 0; i < N; ++i) {

	float f = rng.RandomFloat(3.5f,5.9f);
	if(f > 5.8f) {
	    found = true;
	}
    }

    if(found)
	AssertPassed("2: All random number were in the expected range [3.5,5.9]");
    else
	AssertFailed("2: All random number were NOT in the expected range [3.5,5.9]");
}


void RandomTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestRandomFloat, "TestRandomFloat");

    RunSuite(suite, "Random");
}
