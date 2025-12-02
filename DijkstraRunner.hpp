#pragma once

#include "Grid.hpp"
#include <vector>
#include <queue>
#include <optional>
#include <limits>

class DijkstraRunner {
public:
    explicit DijkstraRunner(const Grid& grid);

    bool step();          // en korak algoritma

    bool isFinished() const;
    bool hasPath()   const;

    bool isOpen(int gx, int gy) const;
    bool isClosed(int gx, int gy) const;
    bool isInPath(int gx, int gy) const;

    int getDistance(int gx, int gy) const;

    // statistika za HUD
    int getVisitedCount() const { return m_visitedCount; }
    int getOpenMaxSize()  const { return m_openMaxSize;  }
    int getPathLength()   const { return m_pathLength;   }

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
    std::vector<std::vector<int>>  m_distance;

    static constexpr int INF = std::numeric_limits<int>::max();

    // element v priority queue: (distance, pozicija)
    using PQEntry = std::pair<int, sf::Vector2i>;
    struct PQCompare {
        bool operator()(const PQEntry& a, const PQEntry& b) const {
            return a.first > b.first; // manjša razdalja ima prednost
        }
    };
    std::priority_queue<PQEntry, std::vector<PQEntry>, PQCompare> m_openPQ;

    bool m_finished = false;
    bool m_pathFound = false;

    // statistika za primerjavo algoritmov
    int m_visitedCount = 0;
    int m_openMaxSize  = 0;
    int m_pathLength   = 0;

    bool inBounds(int gx, int gy) const;
    void pushOpen(const sf::Vector2i& pos, int dist); // dodaj v PQ
    void buildPath();                                  // rekonstrukcija poti
};
