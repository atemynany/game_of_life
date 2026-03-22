#pragma once

#include <string>
#include <vector>
#include <utility>

namespace gol {

class Grid;

struct RLEPattern {
    std::string name;
    size_t width = 0;
    size_t height = 0;
    std::vector<std::pair<size_t, size_t>> alive_cells;
};

RLEPattern parse_rle(const std::string& rle);
std::string to_rle(const Grid& grid);
void load_rle(Grid& grid, const std::string& rle, size_t offset_x = 0, size_t offset_y = 0);

} // namespace gol
