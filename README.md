# Game of Life 🎮

A high-performance C++ implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) with memory-optimized bit-packing and OpenMP parallelization.

## 🚀 Quick Start

### Prerequisites

- C++ compiler with C++11 support or higher
- OpenMP library (usually included with GCC/Clang)
- Windows/Linux/macOS

### Compilation

```bash
g++ -fopenmp -O3 gol.cpp -o gol
```

### Running

```bash
./gol        # Linux/macOS
gol.exe      # Windows
```

## 🎯 How It Works

Conway's Game of Life is a cellular automaton where cells evolve based on simple rules:

1. **Survival**: A live cell with 2 or 3 neighbors stays alive
2. **Birth**: A dead cell with exactly 3 neighbors becomes alive
3. **Death**: All other cells die or stay dead

### Grid Configuration

The grid size is defined at compile-time using template parameters: 

```cpp
constexpr size_t x = 150;  // Width
constexpr size_t y = 40;   // Height
```

## 🛠️ Usage

### Random Initialization

```cpp
Gol<150, 40> gol(true);  // Creates a random grid with ~10% alive cells
```

### Empty Grid with Patterns

```cpp
Gol<150, 40> gol;  // Empty grid

// Add predefined patterns
Patterns<150, 40>::glider(gol, 30, 15);
Patterns<150, 40>:: blinker(gol, 50, 20);
Patterns<150, 40>::block(gol, 10, 10);
```

### Custom Patterns

```cpp
Gol<150, 40> gol;
gol.setCell(10, 10, true);  // Set cell at (10, 10) to alive
gol.setCell(11, 10, true);
gol.setCell(12, 10, true);
```

## 📐 Architecture

### Core Class:  `Gol<X, Y>`

The main class uses template parameters for grid dimensions:

- **Bit-packed storage**: Each byte stores 8 cells
- **Double buffering**: Uses `data` and `newdata` vectors for efficient updates
- **Toroidal topology**: Edges wrap around (cells on the right edge neighbor cells on the left edge)

### Key Methods

| Method | Description |
|--------|-------------|
| `setCell(x, y, alive)` | Set a specific cell's state |
| `getCell(x, y)` | Get a cell's current state |
| `countNeighbors(x, y)` | Count alive neighbors around a cell |
| `update()` | Compute the next generation |
| `print()` | Display the grid in console |

## 🎨 Available Patterns

### Block (Still Life)
```
##
##
```

### Blinker (Oscillator - Period 2)
```
###
```

### Glider (Spaceship)
```
 #
  #
###
```

## ⚡ Performance Optimizations

1. **Bit Packing**: 8x memory reduction compared to byte-per-cell storage
2. **OpenMP Parallelization**: Multi-threaded grid updates using `#pragma omp parallel for`
3. **Memory Swap**: Efficient `swap()` instead of copying data between generations
4. **Inline Functions**: Critical path functions marked `inline` for performance

## 🔧 Customization

### Adjust Speed

Uncomment the sleep line in `main()` to slow down the animation:

```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(200));
```

### Change Randomization Probability

Modify the Bernoulli distribution probability in the constructor:

```cpp
std::bernoulli_distribution dist(0.1);  // 10% chance of alive cells
```

### Modify Grid Size

Change the template parameters when creating the grid:

```cpp
Gol<200, 50> gol(true);  // 200x50 grid
```

## 📊 Technical Details

- **Language**: C++11
- **Dependencies**: OpenMP
- **Memory Usage**: ~(X * Y) / 8 bytes for grid storage
- **Topology**: Toroidal (wrapping edges)

## 🤝 Contributing

Feel free to fork this repository and submit pull requests for: 
- Additional patterns
- Performance improvements
- GUI implementation
- Different boundary conditions
- Additional game modes

## 📝 License

/

## 🔗 Resources

- [Conway's Game of Life - Wikipedia](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)
- [LifeWiki Pattern Collection](https://conwaylife.appspot.com/library/g)
- [OpenMP Documentation](https://www.openmp.org/)

---

**Note**: The `Gameoflife.ipynb` notebook is currently empty and will be used for future Python visualization with a comprehensive UI. 
