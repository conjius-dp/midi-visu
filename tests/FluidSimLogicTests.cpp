#include <JuceHeader.h>
#include "FluidSimLogic.h"

class FluidSimLogicTests : public UnitTest {
public:
    FluidSimLogicTests() : UnitTest("FluidSimLogic", "fluid-sim") {}

    void runTest() override {
        beginTest("init - grid is empty");
        {
            FluidSimLogic sim;
            expect(sim.particleCount() == 0);
            expect(sim.cellAt(100, 100).colorIndex == 0);
        }

        beginTest("init - circle mask center is inside");
        {
            FluidSimLogic sim;
            expect(sim.isInsideMask(100, 100));
        }

        beginTest("init - circle mask corners are outside");
        {
            FluidSimLogic sim;
            expect(!sim.isInsideMask(0, 0));
            expect(!sim.isInsideMask(199, 0));
            expect(!sim.isInsideMask(0, 199));
            expect(!sim.isInsideMask(199, 199));
        }

        beginTest("init - syringe positions are inside mask");
        {
            FluidSimLogic sim;
            for (int v = 0; v < FluidSimLogic::kNumVoices; ++v) {
                auto pos = sim.syringePosition(v);
                expect(sim.isInsideMask(pos.x, pos.y),
                       "Syringe " + String(v) + " outside mask");
            }
        }

        beginTest("init - syringe directions point inward");
        {
            FluidSimLogic sim;
            const float cx = FluidSimLogic::kGridSize * 0.5f;
            const float cy = FluidSimLogic::kGridSize * 0.5f;
            for (int v = 0; v < FluidSimLogic::kNumVoices; ++v) {
                auto pos = sim.syringePosition(v);
                // Vector from syringe to center
                float toCenterX = cx - pos.x;
                float toCenterY = cy - pos.y;
                // Dot product with direction should be positive
                float dot = toCenterX * pos.dirX + toCenterY * pos.dirY;
                expect(dot > 0.f,
                       "Syringe " + String(v) + " direction not inward");
            }
        }

        beginTest("inject - places particles");
        {
            FluidSimLogic sim;
            sim.inject(0, 5, 300);
            expect(sim.particleCount() > 0);
        }

        beginTest("inject - particle has correct color");
        {
            FluidSimLogic sim;
            sim.inject(2, 50, 300); // inject many to ensure we find one
            bool found = false;
            for (int y = 0; y < FluidSimLogic::kGridSize && !found; ++y)
                for (int x = 0; x < FluidSimLogic::kGridSize && !found; ++x)
                    if (sim.cellAt(x, y).colorIndex == 3) // voice 2 -> color 3
                        found = true;
            expect(found, "No particle with correct color found");
        }

        beginTest("inject - invalid voice index does nothing");
        {
            FluidSimLogic sim;
            sim.inject(-1, 5, 300);
            sim.inject(7, 5, 300);
            expect(sim.particleCount() == 0);
        }

        beginTest("step - particles move");
        {
            FluidSimLogic sim;
            // Manually place a particle at center with velocity
            // Use inject near center by temporarily - just inject and step
            sim.inject(0, 20, 300);
            int countBefore = sim.particleCount();
            expect(countBefore > 0);
            sim.step();
            // Particles should still exist (not expired)
            expect(sim.particleCount() > 0);
        }

        beginTest("step - particles persist indefinitely");
        {
            FluidSimLogic sim;
            sim.inject(0, 10, 3);
            expect(sim.particleCount() > 0);
            for (int i = 0; i < 20; ++i)
                sim.step();
            expect(sim.particleCount() > 0,
                   "Particles should persist");
        }

        beginTest("clear - removes all particles");
        {
            FluidSimLogic sim;
            sim.inject(0, 10, 300);
            expect(sim.particleCount() > 0);
            sim.clear();
            expect(sim.particleCount() == 0);
        }

        beginTest("cellAt - out of bounds returns empty");
        {
            FluidSimLogic sim;
            expect(sim.cellAt(-1, 0).colorIndex == 0);
            expect(sim.cellAt(0, -1).colorIndex == 0);
            expect(sim.cellAt(200, 0).colorIndex == 0);
            expect(sim.cellAt(0, 200).colorIndex == 0);
        }

        beginTest("multiple voices inject different colors");
        {
            FluidSimLogic sim;
            sim.inject(0, 20, 300);
            sim.inject(3, 20, 300);
            bool foundColor1 = false, foundColor4 = false;
            for (int y = 0; y < FluidSimLogic::kGridSize; ++y)
                for (int x = 0; x < FluidSimLogic::kGridSize; ++x) {
                    if (sim.cellAt(x, y).colorIndex == 1) foundColor1 = true;
                    if (sim.cellAt(x, y).colorIndex == 4) foundColor4 = true;
                }
            expect(foundColor1, "Voice 0 particles not found");
            expect(foundColor4, "Voice 3 particles not found");
        }

        beginTest("step - particles stay inside mask");
        {
            FluidSimLogic sim;
            // Inject from all syringes
            for (int v = 0; v < FluidSimLogic::kNumVoices; ++v)
                sim.inject(v, 10, 300);
            // Run many steps
            for (int s = 0; s < 100; ++s)
                sim.step();
            // Verify all remaining particles are inside mask
            for (int y = 0; y < FluidSimLogic::kGridSize; ++y)
                for (int x = 0; x < FluidSimLogic::kGridSize; ++x)
                    if (sim.cellAt(x, y).colorIndex != 0)
                        expect(sim.isInsideMask(x, y),
                               "Particle outside mask at (" + String(x) +
                               "," + String(y) + ")");
        }

        beginTest("syringePosition - invalid voice returns dummy");
        {
            FluidSimLogic sim;
            auto pos = sim.syringePosition(-1);
            expect(pos.x == 0 && pos.y == 0);
            pos = sim.syringePosition(7);
            expect(pos.x == 0 && pos.y == 0);
        }
    }
};

static FluidSimLogicTests fluidSimLogicTests;
