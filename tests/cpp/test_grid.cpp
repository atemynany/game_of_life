#include <catch2/catch_test_macros.hpp>
#include "gol/grid.hpp"

using namespace gol;

TEST_CASE("Grid construction", "[grid]") {
    Grid g(100, 50);
    REQUIRE(g.width() == 100);
    REQUIRE(g.height() == 50);
    REQUIRE(g.population() == 0);
    REQUIRE(g.generation() == 0);
}

TEST_CASE("Set and get cells", "[grid]") {
    Grid g(10, 10);
    g.set_cell(3, 4, true);
    REQUIRE(g.get_cell(3, 4) == true);
    REQUIRE(g.get_cell(0, 0) == false);
    g.set_cell(3, 4, false);
    REQUIRE(g.get_cell(3, 4) == false);
}

TEST_CASE("Block is stable", "[grid]") {
    Grid g(10, 10);
    g.set_cell(1, 1, true);
    g.set_cell(2, 1, true);
    g.set_cell(1, 2, true);
    g.set_cell(2, 2, true);

    g.step();
    REQUIRE(g.get_cell(1, 1) == true);
    REQUIRE(g.get_cell(2, 1) == true);
    REQUIRE(g.get_cell(1, 2) == true);
    REQUIRE(g.get_cell(2, 2) == true);
    REQUIRE(g.population() == 4);
}

TEST_CASE("Blinker oscillates with period 2", "[grid]") {
    Grid g(10, 10);
    // Horizontal blinker at (3,4), (4,4), (5,4)
    g.set_cell(3, 4, true);
    g.set_cell(4, 4, true);
    g.set_cell(5, 4, true);

    g.step();
    // Should become vertical: (4,3), (4,4), (4,5)
    REQUIRE(g.get_cell(4, 3) == true);
    REQUIRE(g.get_cell(4, 4) == true);
    REQUIRE(g.get_cell(4, 5) == true);
    REQUIRE(g.get_cell(3, 4) == false);
    REQUIRE(g.get_cell(5, 4) == false);

    g.step();
    // Should be horizontal again
    REQUIRE(g.get_cell(3, 4) == true);
    REQUIRE(g.get_cell(4, 4) == true);
    REQUIRE(g.get_cell(5, 4) == true);
    REQUIRE(g.population() == 3);
}

TEST_CASE("Glider moves", "[grid]") {
    Grid g(20, 20);
    // Standard glider
    g.set_cell(1, 0, true);
    g.set_cell(2, 1, true);
    g.set_cell(0, 2, true);
    g.set_cell(1, 2, true);
    g.set_cell(2, 2, true);

    // After 4 steps, glider should shift (+1, +1)
    g.step_n(4);

    REQUIRE(g.get_cell(2, 1) == true);
    REQUIRE(g.get_cell(3, 2) == true);
    REQUIRE(g.get_cell(1, 3) == true);
    REQUIRE(g.get_cell(2, 3) == true);
    REQUIRE(g.get_cell(3, 3) == true);
    REQUIRE(g.population() == 5);
}

TEST_CASE("Population count", "[grid]") {
    Grid g(100, 100);
    g.set_cell(0, 0, true);
    g.set_cell(99, 99, true);
    g.set_cell(50, 50, true);
    REQUIRE(g.population() == 3);
}

TEST_CASE("Clear resets grid", "[grid]") {
    Grid g(10, 10);
    g.randomize(0.5, 42);
    REQUIRE(g.population() > 0);
    g.clear();
    REQUIRE(g.population() == 0);
}

TEST_CASE("Paste and extract", "[grid]") {
    Grid g(20, 20);
    Grid pattern(3, 1);
    pattern.set_cell(0, 0, true);
    pattern.set_cell(1, 0, true);
    pattern.set_cell(2, 0, true);

    g.paste(pattern, 5, 5);
    REQUIRE(g.get_cell(5, 5) == true);
    REQUIRE(g.get_cell(6, 5) == true);
    REQUIRE(g.get_cell(7, 5) == true);

    Grid sub = g.extract(5, 5, 3, 1);
    REQUIRE(sub.get_cell(0, 0) == true);
    REQUIRE(sub.get_cell(1, 0) == true);
    REQUIRE(sub.get_cell(2, 0) == true);
}

TEST_CASE("to_flat_bool", "[grid]") {
    Grid g(4, 4);
    g.set_cell(0, 0, true);
    g.set_cell(3, 3, true);

    uint8_t buf[16] = {};
    g.to_flat_bool(buf, 16);
    REQUIRE(buf[0] == 1);     // (0,0)
    REQUIRE(buf[15] == 1);    // (3,3)
    REQUIRE(buf[1] == 0);
}

TEST_CASE("Toroidal wrapping", "[grid]") {
    Grid g(5, 5);
    // Blinker at top edge should wrap
    g.set_cell(2, 4, true);
    g.set_cell(2, 0, true);
    g.set_cell(2, 1, true);

    g.step();
    REQUIRE(g.get_cell(1, 0) == true);
    REQUIRE(g.get_cell(2, 0) == true);
    REQUIRE(g.get_cell(3, 0) == true);
}

TEST_CASE("Randomize with seed is deterministic", "[grid]") {
    Grid g1(50, 50);
    Grid g2(50, 50);
    g1.randomize(0.3, 12345);
    g2.randomize(0.3, 12345);
    REQUIRE(g1.population() == g2.population());
    // Check a few cells
    for (size_t y = 0; y < 50; ++y) {
        for (size_t x = 0; x < 50; ++x) {
            REQUIRE(g1.get_cell(x, y) == g2.get_cell(x, y));
        }
    }
}
