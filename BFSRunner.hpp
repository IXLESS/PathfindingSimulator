#pragma once

#include "Grid.hpp"
#include <vector>
#include <queue>
#include <optional>

// Runner za BFS, ki teče po korakih (primerno za animacijo)
class BFSRunner {
public:
    explicit BFSRunner(const Grid& grid);

    // izvede en korak BFS; vrne true, ko je algoritem končan
    bool step();

    bool isFinished() const;
    bool hasPath()   const;

    bool isOpen(int gx, int gy) const;
    bool isClosed(int gx, int gy) const;
    bool isInPath(int gx, int gy) const;

    // statistika za HUD
    int getVisitedCount() const { return m_visitedCount; }
    int getOpenMaxSize()  const { return m_openMaxSize; }
    int getPathLength()   const { return m_pathLength; }

private:
    const Grid* m_grid = nullptr;
    int m_rows = 0;
    int m_cols = 0;

    sf::Vector2i m_start{};
    sf::Vector2i m_end{};

    std::vector<std::vector<bool>> m_visited;
    std::vector<std::vector<bool>> m_inOpen;
    std::vector<std::vector<bool>> m_inClosed;
    std::vector<std::vector<std::optional<sf::Vector2i>>> m_parent;
    std::vector<std::vector<bool>> m_inPath;

    // FIFO vrsta za BFS
    std::queue<sf::Vector2i> m_queue;

    bool m_finished = false;
    bool m_pathFound = false;

    // osnovna statistika BFS
    int m_visitedCount = 0;
    int m_openMaxSize  = 0;
    int m_pathLength   = 0;

    bool inBounds(int gx, int gy) const;
    void buildPath();  // sledenje parentov nazaj do starta
};
