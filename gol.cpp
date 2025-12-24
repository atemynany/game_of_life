#include <cstddef>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <omp.h>
#include <cstring>

template<size_t X, size_t Y>
class Gol {

    private:
    // Store 8 cells per byte, i.e each bit in the byte represents the cell with 1 = alive and 0 = dead 
    std::vector<uint8_t> data; // dynamic array of unsigned 8-bit integers --> each element stores one byte --> all stored in the heap allowing bigger data sizes
    std::vector<uint8_t> newdata;
    static constexpr size_t BITS_PER_BYTE = 8;
    static constexpr size_t BYTES_PER_ROW = (X + BITS_PER_BYTE - 1) / BITS_PER_BYTE; // make sure to round up to always allocate enough memory

    // Helper to get bit at position
    inline bool getBit(size_t index) const {
        return (data[index / BITS_PER_BYTE] >> (index % BITS_PER_BYTE)) & 1; //example: read cell 23 --> 23/8 = 2 --> acces byte 2 we se wich position allocates which byte and then find the bit thats contributed to this exact cell with %. finally move that bit to the right >> so we can isolate it and to a bitwise comparison with & so we check wether alive or dead
    }

    // Helper to set bit at position
    inline void setBit(size_t index, bool value) {
        if (value) {
            data[index / BITS_PER_BYTE] |= (1 << (index % BITS_PER_BYTE));
        } else {
            data[index / BITS_PER_BYTE] &= ~(1 << (index % BITS_PER_BYTE));
        }
    }

    public:

    // empty constructor
    Gol() : data(Y * BYTES_PER_ROW, 0), newdata(Y * BYTES_PER_ROW, 0) {}

    // random constructor
    Gol(bool randomize) : data(Y * BYTES_PER_ROW, 0), newdata(Y * BYTES_PER_ROW, 0) {
        if (randomize) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::bernoulli_distribution dist(0.1);
            
            for (size_t y = 0; y < Y; ++y) {
                for (size_t x = 0; x < X; ++x) {
                    if (dist(gen)) {
                        setBit(y * X + x, true);
                    }
                }
            }
        }
    }

    // set function for custom patterns to be set on the data
    void setCell(size_t x, size_t y, bool alive) {
        if (x < X && y < Y) {
            setBit(y * X + x, alive);
        }
    }


    inline bool getCell(size_t x, size_t y) const {
        if (x < X && y < Y) {
            return getBit(y * X + x);
        }
        return false;
    }

    // count Neighboring cells
    int countNeighbors(size_t x, size_t y) const {
        int count = 0;

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                
                int nx = (x + dx + X) % X;
                int ny = (y + dy + Y) % Y;
                
                if (getCell(nx, ny)) count++;
            }
        }
        return count;
    }

    // apply the rules of the game of life
    bool applyRules(size_t x, size_t y) const {
        int neighbors = countNeighbors(x, y);
        bool alive = getCell(x, y);
        
        return (alive && (neighbors == 2 || neighbors == 3)) || (!alive && neighbors == 3);
    }

    void update() {

        std::memset(newdata.data(), 0, newdata.size());
        
        // paralize the nested loops
        #pragma omp parallel for collapse(2)
        for (size_t y = 0; y < Y; ++y) {
            for (size_t x = 0; x < X; ++x) {
                size_t index = y * X + x;
                if (applyRules(x, y)) {
                    newdata[index / BITS_PER_BYTE] |= (1 << (index % BITS_PER_BYTE));
                }
            }
        }
        
        data.swap(newdata); //swap instead of copy
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