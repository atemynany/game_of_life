#include "gol/rle.hpp"
#include "gol/grid.hpp"
#include <sstream>
#include <cctype>

namespace gol {

RLEPattern parse_rle(const std::string& rle) {
    RLEPattern pattern;
    std::istringstream stream(rle);
    std::string line;
    std::string body;

    while (std::getline(stream, line)) {
        // Remove \r if present
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty()) continue;

        // Comment lines
        if (line[0] == '#') {
            if (line.size() > 2 && line[1] == 'N') {
                pattern.name = line.substr(3);
            }
            continue;
        }

        // Header line: x = ..., y = ...
        if (line[0] == 'x') {
            size_t xpos = line.find("x");
            size_t ypos = line.find("y");
            if (xpos != std::string::npos && ypos != std::string::npos) {
                // Parse x value
                size_t eq1 = line.find('=', xpos);
                if (eq1 != std::string::npos) {
                    pattern.width = std::stoul(line.substr(eq1 + 1));
                }
                // Parse y value
                size_t eq2 = line.find('=', ypos);
                if (eq2 != std::string::npos) {
                    pattern.height = std::stoul(line.substr(eq2 + 1));
                }
            }
            continue;
        }

        body += line;
    }

    // Parse RLE body
    size_t cx = 0, cy = 0;
    size_t run = 0;

    for (char ch : body) {
        if (ch == '!') break;

        if (std::isdigit(ch)) {
            run = run * 10 + (ch - '0');
            continue;
        }

        size_t count = (run == 0) ? 1 : run;
        run = 0;

        if (ch == 'b') {
            cx += count;
        } else if (ch == 'o') {
            for (size_t i = 0; i < count; ++i) {
                pattern.alive_cells.emplace_back(cx, cy);
                ++cx;
            }
        } else if (ch == '$') {
            cy += count;
            cx = 0;
        }
    }

    return pattern;
}

std::string to_rle(const Grid& grid) {
    std::ostringstream out;
    out << "x = " << grid.width() << ", y = " << grid.height() << "\n";

    for (size_t y = 0; y < grid.height(); ++y) {
        size_t run = 0;
        char last = 0;

        for (size_t x = 0; x < grid.width(); ++x) {
            char ch = grid.get_cell(x, y) ? 'o' : 'b';
            if (ch == last) {
                ++run;
            } else {
                if (last != 0) {
                    if (run > 1) out << run;
                    out << last;
                }
                last = ch;
                run = 1;
            }
        }

        // Write trailing alive cells (skip trailing dead)
        if (last == 'o') {
            if (run > 1) out << run;
            out << last;
        }

        if (y < grid.height() - 1) {
            out << '$';
        }
    }

    out << '!';
    return out.str();
}

void load_rle(Grid& grid, const std::string& rle, size_t offset_x, size_t offset_y) {
    RLEPattern pattern = parse_rle(rle);
    for (auto& [x, y] : pattern.alive_cells) {
        grid.set_cell(offset_x + x, offset_y + y, true);
    }
}

} // namespace gol
