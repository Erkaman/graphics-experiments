#include "random.hpp"

#include "framework.hpp"

static void TestRandomFloat() {
    Random rng(1);

    for(int i= 0; i < 2000; ++i) {

	float f = rng.RandomFloat(3.5f,5.9f);
	if(f < 3.5f || f > 5.9f) {
	    std::string message;
	    message += "The random number ";
	    message += std::to_string(f);
	    message += " was not in the expected range [3.5,5.9]";

	    AssertFailed(message);
	}

    }
    AssertPassed("All random number were in the expected range [3.5,5.9]");
}


void RandomTestSuite() {

    TestSuite suite;

    suite.emplace_back(TestRandomFloat, "TestRandomFloat");

    RunSuite(suite, "Random");
}
