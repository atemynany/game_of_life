#include <cstddef>
#include <bitset>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <omp.h>

template<size_t X, size_t Y>
class Gol {

    private:
    static constexpr size_t TOTAL_BITS = X * Y;
    std::bitset<X * Y> data;
    std::bitset<X * Y> newdata;

    public:

    // empty constructor
    Gol() : data(), newdata() {}

    // random constructor 
    Gol(bool randomize) : data(), newdata() {
        if (randomize) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::bernoulli_distribution dist(0.1);
            
            for (size_t y = 0; y < Y; ++y) {
                for (size_t x = 0; x < X; ++x) {
                    if (dist(gen)) {
                        data[y * X + x] = 1;
                    }
                }
            }
        }
    }

    // set function for custom patterns to be set on the data
    void setCell(size_t x, size_t y, bool alive) {
        if (x < X && y < Y) {
            data[y * X + x] = alive;
        }
    }

    inline bool getCell(size_t x, size_t y) const {
        if (x < X && y < Y) {
            return data[y * X + x];
        }
        return false;
    }

    // count Neighboring cells
    int countNeighbors(size_t x, size_t y) const {
        int count = 0;
        
        // check all 8 neighbors explicitly
        count += getCell((x - 1 + X) % X, (y - 1 + Y) % Y);  // top-left
        count += getCell(x,               (y - 1 + Y) % Y);  // top
        count += getCell((x + 1) % X,     (y - 1 + Y) % Y);  // top-right
        count += getCell((x - 1 + X) % X, y);                // left
        count += getCell((x + 1) % X,     y);                // right
        count += getCell((x - 1 + X) % X, (y + 1) % Y);      // bottom-left
        count += getCell(x,               (y + 1) % Y);      // bottom
        count += getCell((x + 1) % X,     (y + 1) % Y);      // bottom-right
        
        return count;
    }

    // apply the rules of the game of life
    bool applyRules(size_t x, size_t y) const {
        int neighbors = countNeighbors(x, y);
        bool alive = getCell(x, y);
        
        return (alive && (neighbors == 2 || neighbors == 3)) || (!alive && neighbors == 3);
    }

    void update() {
        newdata.reset();
        
        // paralize the nested loops
        #pragma omp parallel for collapse(2)
        for (size_t y = 0; y < Y; ++y) {
            for (size_t x = 0; x < X; ++x) {
                size_t index = y * X + x;
                if (applyRules(x, y)) {
                    newdata[index] = 1;
                }
            }
        }
        
        data = newdata;
    }
};