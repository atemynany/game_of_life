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

    // print the data in console for ui implementation not needed 
    void print() const {
        for (size_t y = 0; y < Y; ++y) {
            for (size_t x = 0; x < X; ++x) {
                std::cout << (getCell(x, y) ? '#' : '.');
            }
            std::cout << '\n';
        }
    }
};

// ...existing code...

// add patterns manually to validate --> taken from wiki / pattern libary
template<size_t X, size_t Y>
class Patterns {

    public:

    // Block - 2x2 square (stable)
    static void block(Gol<X, Y>& gol, size_t x, size_t y) {
        gol.setCell(x, y, true);
        gol.setCell(x + 1, y, true);
        gol.setCell(x, y + 1, true);
        gol.setCell(x + 1, y + 1, true);
    }

    // Blinker - oscillates (period 2)
    static void blinker(Gol<X, Y>& gol, size_t x, size_t y) {
        gol.setCell(x, y, true);
        gol.setCell(x + 1, y, true);
        gol.setCell(x + 2, y, true);
    }

    // Glider - moves diagonally
    static void glider(Gol<X, Y>& gol, size_t x, size_t y) {
        gol.setCell(x + 1, y, true);
        gol.setCell(x + 2, y + 1, true);
        gol.setCell(x, y + 2, true);
        gol.setCell(x + 1, y + 2, true);
        gol.setCell(x + 2, y + 2, true);
    }
};

int main() {

    constexpr size_t x = 150;
    constexpr size_t y = 40;

    Gol<x, y> gol(true);  // Random initialization
    // OR
    //Gol<x, y> gol;     // Empty data

    // Use patterns with template parameters
    //Patterns<x, y>::block(gol, 10, 10);
    //Patterns<x, y>::glider(gol, 30, 15);
    //Patterns<x, y>::blinker(gol, 50, 20);
    
    int generation = 0;
    while (true) {
        std::cout << "\033[2J\033[H"; // clean console look
        std::cout << "Generation " << generation << '\n';
        gol.print();
        gol.update();
        generation++;
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(200)); // manual delay
    }
    
    return 0;
}