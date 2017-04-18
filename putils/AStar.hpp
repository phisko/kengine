#pragma once

#include <cmath>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <vector>

#include "sign.hpp"
#include "Point.hpp"
#include "Direction.hpp"

namespace putils
{
    namespace
    {

        template<typename Precision>
        double heuristic_cost_estimate(const putils::Point<Precision> &start,
                                       const putils::Point<Precision> &goal) noexcept
        {
            return std::sqrt(
                    std::pow(goal.x - start.x, 2) +
                    std::pow(goal.y - start.y, 2)
            );
        }

        template<typename Precision>
        Direction reconstruct_path(const std::unordered_map<Point<Precision>, Point<Precision>> &cameFrom, putils::Point<Precision> &current, const putils::Point<Precision> &pos) noexcept
        {
            if (!cameFrom.size())
                return Directions::NoDirection;

            while (cameFrom.at(current) != pos)
                current = cameFrom.at(current);

            return Direction { putils::sign(current.x - pos.x), putils::sign(current.y - pos.y) };
        }
    }

//
// Pathfinding algorithm implementing the AStar algorithm described on Wikipedia
//
    class AStar
    {
        // Get the next move towards goal
    public:
        template<typename Precision>
        Direction getNextDirection(const Point<Precision> &start, const Point<Precision> &goal,
                                   const std::function<bool(const Point<Precision> &dest)> &canMoveTo) noexcept
        {
            // The set of nodes already evaluated.
            std::vector<Point<Precision>> closedSet;
            // The set of currently discovered nodes still to be evaluated.
            // Initially, only the start node is known.
            std::vector<Point<Precision>> openSet({start});
            // For each node, which node it can most efficiently be reached from.
            // If a node can be reached from many nodes, cameFrom will eventually contain the
            // most efficient previous step.
            std::unordered_map<Point<Precision>, Point<Precision>> cameFrom;

            // For each node, the cost of getting from the start node to that node.
            std::unordered_map<Point<Precision>, double> gScore;
            // The cost of going from start to start is zero.
            gScore.emplace(start, 0);

            // For each node, the total cost of getting from the start node to the goal
            // by passing by that node. That value is partly known, partly heuristic.
            std::unordered_map<Point<Precision>, double> fScore;
            // For the first node, that value is completely heuristic.
            fScore.emplace(start, heuristic_cost_estimate(start, goal));

            static const auto findClosest = [&fScore](const auto &l, const auto &r) { return fScore.at(l) < fScore.at(r); };

            while (openSet.size())
            {
                const auto it = std::min_element(openSet.cbegin(), openSet.cend(), findClosest);
                auto current = *it;

                if (goal == current)
                    return reconstruct_path(cameFrom, current, start);

                openSet.erase(it);
                closedSet.push_back(current);

                Point<Precision> neighbor;
                for (auto x = current.x - 1; x <= current.x + 1; ++x)
                    for (auto y = current.y - 1; y <= current.y + 1; ++y)
                    {
                        if ((x == current.x && y == current.y) ||
                            (x != current.x && y != current.y))
                            continue;

                        neighbor = {x, y};

                        if (std::find(closedSet.cbegin(), closedSet.cend(), neighbor) != closedSet.cend())
                            continue; // Ignore the neighbor which is already evaluated.

                        if (goal != neighbor && !canMoveTo(neighbor))
                        {
                            closedSet.push_back(neighbor);
                            continue;
                        }

                        // The distance from start to a neighbor
                        auto tentative_gScore = gScore.at(current) + 1;
                        if (std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end())
                            openSet.push_back(neighbor);
                        else if (tentative_gScore >= gScore[neighbor])
                            continue;                                // This is not a better path

                        // This path is the best until now. Record it!
                        cameFrom[neighbor] = current;
                        gScore[neighbor] = tentative_gScore;
                        fScore[neighbor] = heuristic_cost_estimate(neighbor, goal);
                    }
            }

            return Directions::NoDirection;
        }
    };
}
