#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace gol {

class Grid {
public:
    Grid(size_t width, size_t height);

    size_t width() const { return width_; }
    size_t height() const { return height_; }

    void set_cell(size_t x, size_t y, bool alive);
    bool get_cell(size_t x, size_t y) const;

    void step();
    void step_n(size_t n);
    void clear();
    void randomize(double density = 0.1, uint64_t seed = 0);

    void paste(const Grid& pattern, size_t x, size_t y);
    Grid extract(size_t x, size_t y, size_t w, size_t h) const;

    const uint64_t* data() const { return data_.data(); }
    size_t data_size() const { return data_.size(); }
    void to_flat_bool(uint8_t* out, size_t len) const;

    size_t population() const;

    size_t generation() const { return generation_; }

private:
    size_t width_;
    size_t height_;
    size_t words_per_row_;
    size_t generation_ = 0;
    std::vector<uint64_t> data_;
    std::vector<uint64_t> buffer_;

    size_t word_index(size_t x, size_t y) const {
        return y * words_per_row_ + x / 64;
    }
    uint64_t bit_mask(size_t x) const {
        return uint64_t(1) << (x % 64);
    }
    int count_neighbors(size_t x, size_t y) const;
};

} // namespace gol
