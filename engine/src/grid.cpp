#include "gol/grid.hpp"
#include <random>
#include <algorithm>
#include <cstring>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace gol {

Grid::Grid(size_t width, size_t height)
    : width_(width), height_(height),
      words_per_row_((width + 63) / 64),
      data_(words_per_row_ * height, 0),
      buffer_(words_per_row_ * height, 0) {}

void Grid::set_cell(size_t x, size_t y, bool alive) {
    if (x >= width_ || y >= height_) return;
    size_t idx = word_index(x, y);
    uint64_t mask = bit_mask(x);
    if (alive)
        data_[idx] |= mask;
    else
        data_[idx] &= ~mask;
}

bool Grid::get_cell(size_t x, size_t y) const {
    if (x >= width_ || y >= height_) return false;
    return (data_[word_index(x, y)] & bit_mask(x)) != 0;
}

int Grid::count_neighbors(size_t x, size_t y) const {
    int count = 0;
    size_t w = width_;
    size_t h = height_;

    size_t xm = (x + w - 1) % w;
    size_t xp = (x + 1) % w;
    size_t ym = (y + h - 1) % h;
    size_t yp = (y + 1) % h;

    auto get = [&](size_t cx, size_t cy) -> int {
        return (data_[cy * words_per_row_ + cx / 64] >> (cx % 64)) & 1;
    };

    count += get(xm, ym);
    count += get(x,  ym);
    count += get(xp, ym);
    count += get(xm, y);
    count += get(xp, y);
    count += get(xm, yp);
    count += get(x,  yp);
    count += get(xp, yp);

    return count;
}

void Grid::step() {
    std::fill(buffer_.begin(), buffer_.end(), 0);

    #ifdef _OPENMP
    #pragma omp parallel for schedule(static)
    #endif
    for (size_t y = 0; y < height_; ++y) {
        for (size_t x = 0; x < width_; ++x) {
            int neighbors = count_neighbors(x, y);
            bool alive = get_cell(x, y);
            bool next = (alive && (neighbors == 2 || neighbors == 3)) ||
                        (!alive && neighbors == 3);
            if (next) {
                buffer_[y * words_per_row_ + x / 64] |= uint64_t(1) << (x % 64);
            }
        }
    }

    std::swap(data_, buffer_);
    ++generation_;
}

void Grid::step_n(size_t n) {
    for (size_t i = 0; i < n; ++i) {
        step();
    }
}

void Grid::clear() {
    std::fill(data_.begin(), data_.end(), 0);
    generation_ = 0;
}

void Grid::randomize(double density, uint64_t seed) {
    std::mt19937_64 rng(seed ? seed : std::random_device{}());
    std::bernoulli_distribution dist(density);

    std::fill(data_.begin(), data_.end(), 0);
    for (size_t y = 0; y < height_; ++y) {
        for (size_t x = 0; x < width_; ++x) {
            if (dist(rng)) {
                data_[y * words_per_row_ + x / 64] |= uint64_t(1) << (x % 64);
            }
        }
    }
    generation_ = 0;
}

void Grid::paste(const Grid& pattern, size_t ox, size_t oy) {
    for (size_t py = 0; py < pattern.height_; ++py) {
        for (size_t px = 0; px < pattern.width_; ++px) {
            if (pattern.get_cell(px, py)) {
                size_t tx = (ox + px) % width_;
                size_t ty = (oy + py) % height_;
                set_cell(tx, ty, true);
            }
        }
    }
}

Grid Grid::extract(size_t x, size_t y, size_t w, size_t h) const {
    Grid result(w, h);
    for (size_t ey = 0; ey < h; ++ey) {
        for (size_t ex = 0; ex < w; ++ex) {
            size_t sx = (x + ex) % width_;
            size_t sy = (y + ey) % height_;
            result.set_cell(ex, ey, get_cell(sx, sy));
        }
    }
    return result;
}

void Grid::to_flat_bool(uint8_t* out, size_t len) const {
    size_t total = width_ * height_;
    size_t n = std::min(len, total);
    for (size_t i = 0; i < n; ++i) {
        size_t x = i % width_;
        size_t y = i / width_;
        out[i] = get_cell(x, y) ? 1 : 0;
    }
}

size_t Grid::population() const {
    size_t count = 0;
    for (size_t y = 0; y < height_; ++y) {
        for (size_t w = 0; w < words_per_row_; ++w) {
            uint64_t word = data_[y * words_per_row_ + w];
            // Mask out bits beyond width in the last word of each row
            if (w == words_per_row_ - 1 && (width_ % 64) != 0) {
                word &= (uint64_t(1) << (width_ % 64)) - 1;
            }
            count += __builtin_popcountll(word);
        }
    }
    return count;
}

} // namespace gol
