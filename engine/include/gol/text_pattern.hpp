#pragma once

#include <string>

namespace gol {

class Grid;

Grid text_to_pattern(const std::string& text, int char_spacing = 1);

} // namespace gol
