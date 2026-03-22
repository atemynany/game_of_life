#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "gol/grid.hpp"
#include "gol/rle.hpp"
#include "gol/text_pattern.hpp"

namespace py = pybind11;

PYBIND11_MODULE(gol_engine, m) {
    m.doc() = "Game of Life C++ engine";

    py::class_<gol::Grid>(m, "Grid")
        .def(py::init<size_t, size_t>(), py::arg("width"), py::arg("height"))
        .def_property_readonly("width", &gol::Grid::width)
        .def_property_readonly("height", &gol::Grid::height)
        .def_property_readonly("generation", &gol::Grid::generation)
        .def_property_readonly("population", &gol::Grid::population)
        .def("set_cell", &gol::Grid::set_cell)
        .def("get_cell", &gol::Grid::get_cell)
        .def("step", &gol::Grid::step, py::call_guard<py::gil_scoped_release>())
        .def("step_n", &gol::Grid::step_n, py::arg("n"),
             py::call_guard<py::gil_scoped_release>())
        .def("clear", &gol::Grid::clear)
        .def("randomize", &gol::Grid::randomize,
             py::arg("density") = 0.1, py::arg("seed") = 0,
             py::call_guard<py::gil_scoped_release>())
        .def("paste", &gol::Grid::paste, py::arg("pattern"), py::arg("x"), py::arg("y"))
        .def("extract", &gol::Grid::extract,
             py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"))
        .def("to_numpy", [](const gol::Grid& g) {
            auto arr = py::array_t<uint8_t>({g.height(), g.width()});
            auto buf = arr.mutable_unchecked<2>();
            for (size_t y = 0; y < g.height(); ++y) {
                for (size_t x = 0; x < g.width(); ++x) {
                    buf(y, x) = g.get_cell(x, y) ? 1 : 0;
                }
            }
            return arr;
        })
        .def("to_numpy_packed", [](const gol::Grid& g) {
            return py::array_t<uint64_t>(
                {g.data_size()},
                {sizeof(uint64_t)},
                g.data(),
                py::cast(g)  // keep grid alive
            );
        })
        .def("to_ascii", [](const gol::Grid& g) {
            std::string result;
            result.reserve(g.height() * (g.width() + 1));
            for (size_t y = 0; y < g.height(); ++y) {
                for (size_t x = 0; x < g.width(); ++x) {
                    result += g.get_cell(x, y) ? '#' : '.';
                }
                if (y < g.height() - 1) result += '\n';
            }
            return result;
        })
        .def("to_ascii_region", [](const gol::Grid& g, size_t rx, size_t ry, size_t rw, size_t rh) {
            std::string result;
            for (size_t y = ry; y < ry + rh && y < g.height(); ++y) {
                for (size_t x = rx; x < rx + rw && x < g.width(); ++x) {
                    result += g.get_cell(x, y) ? '#' : '.';
                }
                if (y < ry + rh - 1) result += '\n';
            }
            return result;
        });

    // RLE functions
    py::class_<gol::RLEPattern>(m, "RLEPattern")
        .def_readonly("name", &gol::RLEPattern::name)
        .def_readonly("width", &gol::RLEPattern::width)
        .def_readonly("height", &gol::RLEPattern::height)
        .def_readonly("alive_cells", &gol::RLEPattern::alive_cells);

    m.def("parse_rle", &gol::parse_rle, py::arg("rle"));
    m.def("to_rle", &gol::to_rle, py::arg("grid"));
    m.def("load_rle", &gol::load_rle,
          py::arg("grid"), py::arg("rle"),
          py::arg("offset_x") = 0, py::arg("offset_y") = 0);
    m.def("text_to_pattern", &gol::text_to_pattern,
          py::arg("text"), py::arg("char_spacing") = 1);
}
