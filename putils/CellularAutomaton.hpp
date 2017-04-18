#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include <algorithm>

#include "fwd.hpp"
#include "static_if.hpp"
#include "Point.hpp"

namespace putils
{
    template<typename Contained>
    struct Cell
    {
        Point<int, 3> pos;
        Contained obj;
    };

    template<typename Contained>
    class CellularAutomaton
    {
    public:
        using NeighborFetcher = std::function<std::vector<const Cell<Contained> *>(const std::vector<Cell<Contained>> &cells, const Point<int, 3> &pos)>;
        using ModifierCondition = std::function<bool(const std::vector<const Cell<Contained> *> &neighbors, const Cell<Contained> &cell)>;
        using Modifier = std::function<void(Cell<Contained> &cell)>;

    public:
        CellularAutomaton(std::vector<Cell<Contained>> &&cells,
                          std::size_t dimensions = 2)
                : _cells(std::move(cells)),  _dimensions(dimensions)
        {
        }

    public:
        CellularAutomaton(const putils::Point<std::size_t> &size,
                          const Contained &value = Contained{},
                          const putils::Point<int> &start = { 0, 0 },
                          std::size_t dimensions = 2)
                : _dimensions(dimensions)
        {
            for (std::size_t x = 0; x < size.x; ++x)
                for (std::size_t y = 0; y < size.y; ++y)
                    _cells.push_back({ { (int)x + start.x, (int)y + start.y }, value });
        }

    public:
        const std::vector<Cell<Contained>> &getCells() const { return _cells; }
        std::vector<Cell<Contained>> &getCells() { return _cells; }

    public:
        const std::vector<Cell<Contained>> &step(
                const ModifierCondition &condition,
                const Modifier &modifier,
                const NeighborFetcher &fetcher = nullptr)
        {
            std::vector<Cell<Contained>> _newCells(_cells);

            for (auto &cell : _newCells)
            {
                const auto neighbors =
                        (fetcher == nullptr) ? getNeighbors(cell.pos) : fetcher(_cells, cell.pos);

                if (condition(neighbors, cell))
                    modifier(cell);
            }

            _cells = std::move(_newCells);
            return _cells;
        }

    private:
        std::vector<const Cell<Contained> *> getNeighbors(const Point<int, 3> &pos)
        {
            std::vector<const Cell<Contained> *> ret;

            for (const auto &c : _cells)
                if ((x_neighbor(c.pos, pos) || y_neighbor(c.pos, pos)) &&
                        _dimensions == 2 || y_neighbor(c.pos, pos))
                    ret.push_back(&c);

            return ret;
        }

        static bool x_neighbor(const Point<int, 3> &a, const Point<int, 3> &b)
        {
            return (a.x == b.x - 1 || a.x == b.x + 1) && a.y == b.y && a.z == b.z;
        }

        static bool y_neighbor(const Point<int, 3> &a, const Point<int, 3> &b)
        {
            return a.x == b.x && (a.y == b.y - 1 || a.y == b.y + 1) && a.z == b.z;
        }

        static bool z_neighbor(const Point<int, 3> &a, const Point<int, 3> &b)
        {
            return a.x == b.x && a.y == b.y && (a.z == b.z - 1 || a.z == b.z + 1);
        }

    private:
        std::vector<Cell<Contained>> _cells;
        std::size_t _dimensions;
    };

    namespace test
    {
        inline void cellularAutomaton()
        {
            CellularAutomaton<bool> ca(
                        {
                                { { 0 }, false },
                                { { 1 }, false },
                                { { 2 }, true },
                                { { 3 }, false },
                                { { 4 }, false },
                        }
            );

            const auto &cells = ca.step(
                    [](const std::vector<const Cell<bool> *> &neighbors, const Cell<bool> &cell)
                    {
                        return std::find_if(neighbors.begin(), neighbors.end(), [](auto &&ptr){ return ptr->obj; })
                               != neighbors.end();
                    },
                    [](Cell<bool> &cell)
                    {
                        cell.obj = !cell.obj;
                    }
            );

            for (const auto &c : cells)
                std::cout << std::boolalpha << c.obj << std::endl;
        }
    }
}
