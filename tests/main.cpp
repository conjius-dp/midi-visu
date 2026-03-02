#include <JuceHeader.h>

// Test registrations happen via static objects in the test .cpp files.

int main(int /*argc*/, char** /*argv*/) {
    juce::UnitTestRunner runner;
    runner.runAllTests();

    int totalFailures = 0;

    for (int i = 0; i < runner.getNumResults(); ++i) {
        auto* r = runner.getResult(i);
        juce::String status = (r->failures == 0) ? "PASS" : "FAIL";
        juce::Logger::writeToLog("[" + status + "] " + r->unitTestName
            + ": " + juce::String(r->passes) + " passed, "
            + juce::String(r->failures) + " failed");
        totalFailures += r->failures;
    }

    return totalFailures > 0 ? 1 : 0;
}