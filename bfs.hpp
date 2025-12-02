#pragma once

#include "Grid.hpp"
#include <vector>
#include <queue>
#include <optional>
#include <algorithm> // std::reverse

// Najde najkrajšo pot z BFS.
// Vrne true, če pot obstaja, in v outPath zapiše zaporedje celic od start do end.
inline bool runBFS(const Grid& grid, std::vector<sf::Vector2i>& outPath) {
    outPath.clear();

    auto startOpt = grid.getStart();
    auto endOpt   = grid.getEnd();

    if (!startOpt || !endOpt)
        return false; // nimamo start ali end

    sf::Vector2i start = *startOpt;
    sf::Vector2i end   = *endOpt;

    int rows = grid.getRows();
    int cols = grid.getCols();

    // visited[y][x]
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    // parent[y][x] -> od kod smo prišli na to celico
    std::vector<std::vector<std::optional<sf::Vector2i>>> parent(
        rows,
        std::vector<std::optional<sf::Vector2i>>(cols, std::nullopt)
    );

    std::queue<sf::Vector2i> q;
    visited[start.y][start.x] = true;
    q.push(start);

    const sf::Vector2i dirs[4] = {
        { 1,  0},
        {-1,  0},
        { 0,  1},
        { 0, -1}
    };

    bool found = false;

    while (!q.empty()) {
        sf::Vector2i cur = q.front();
        q.pop();

        if (cur == end) {
            found = true;
            break;
        }

        for (const auto& d : dirs) {
            int nx = cur.x + d.x;
            int ny = cur.y + d.y;

            if (!grid.inBounds(nx, ny))  continue;
            if (grid.isWall(nx, ny))     continue;
            if (visited[ny][nx])         continue;

            visited[ny][nx] = true;
            parent[ny][nx] = cur;
            q.push(sf::Vector2i{nx, ny});
        }
    }

    if (!found)
        return false;

    // Rekonstrukcija poti od end do start preko parent
    sf::Vector2i cur = end;
    while (!(cur == start)) {
        outPath.push_back(cur);
        cur = *parent[cur.y][cur.x]; // parent mora obstajati, ker je del najdene poti
    }
    outPath.push_back(start);

    std::reverse(outPath.begin(), outPath.end());
    return true;
}
