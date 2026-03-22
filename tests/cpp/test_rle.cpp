#include <catch2/catch_test_macros.hpp>
#include "gol/rle.hpp"
#include "gol/grid.hpp"

using namespace gol;

TEST_CASE("Parse glider RLE", "[rle]") {
    std::string rle = "#N Glider\nx = 3, y = 3\nbo$2bo$3o!";
    auto pattern = parse_rle(rle);

    REQUIRE(pattern.name == "Glider");
    REQUIRE(pattern.width == 3);
    REQUIRE(pattern.height == 3);
    REQUIRE(pattern.alive_cells.size() == 5);
}

TEST_CASE("Load RLE onto grid", "[rle]") {
    Grid g(10, 10);
    std::string rle = "x = 3, y = 3\nbo$2bo$3o!";
    load_rle(g, rle, 2, 2);

    // Glider pattern at offset (2,2)
    REQUIRE(g.get_cell(3, 2) == true);  // bo -> (1,0)
    REQUIRE(g.get_cell(4, 3) == true);  // 2bo -> (2,1)
    REQUIRE(g.get_cell(2, 4) == true);  // 3o -> (0,2)
    REQUIRE(g.get_cell(3, 4) == true);  // 3o -> (1,2)
    REQUIRE(g.get_cell(4, 4) == true);  // 3o -> (2,2)
}

TEST_CASE("RLE round-trip", "[rle]") {
    Grid g(10, 10);
    g.set_cell(1, 0, true);
    g.set_cell(2, 1, true);
    g.set_cell(0, 2, true);
    g.set_cell(1, 2, true);
    g.set_cell(2, 2, true);

    std::string rle = to_rle(g);
    REQUIRE(rle.find("x = 10, y = 10") != std::string::npos);
    REQUIRE(rle.back() == '!');

    // Load it back onto a fresh grid
    Grid g2(10, 10);
    load_rle(g2, rle);

    REQUIRE(g2.get_cell(1, 0) == true);
    REQUIRE(g2.get_cell(2, 1) == true);
    REQUIRE(g2.get_cell(0, 2) == true);
    REQUIRE(g2.get_cell(1, 2) == true);
    REQUIRE(g2.get_cell(2, 2) == true);
    REQUIRE(g2.population() == 5);
}

TEST_CASE("Parse blinker RLE", "[rle]") {
    std::string rle = "x = 3, y = 1\n3o!";
    auto pattern = parse_rle(rle);
    REQUIRE(pattern.width == 3);
    REQUIRE(pattern.height == 1);
    REQUIRE(pattern.alive_cells.size() == 3);
}
