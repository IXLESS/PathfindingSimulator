#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <random>

class Grid {
public:
    Grid(int cols, int rows, int cellSize)
        : m_cols(cols),
          m_rows(rows),
          m_cellSize(cellSize),
          m_walls(rows, std::vector<bool>(cols, false)) {}

    int getCols() const { return m_cols; }
    int getRows() const { return m_rows; }
    int getCellSize() const { return m_cellSize; }

    bool inBounds(int gx, int gy) const {
        return gx >= 0 && gx < m_cols && gy >= 0 && gy < m_rows;
    }

    // zidovi
    bool isWall(int gx, int gy) const {
        return m_walls[gy][gx];
    }

    void setWall(int gx, int gy, bool value) {
        if (!inBounds(gx, gy)) return;
        m_walls[gy][gx] = value;
    }

    // start / end
    void setStart(int gx, int gy) {
        if (!inBounds(gx, gy)) return;
        m_walls[gy][gx] = false;
        m_start = sf::Vector2i{gx, gy};
        if (m_end && *m_end == *m_start)
            m_end.reset();
    }

    void setEnd(int gx, int gy) {
        if (!inBounds(gx, gy)) return;
        m_walls[gy][gx] = false;
        m_end = sf::Vector2i{gx, gy};
        if (m_start && *m_start == *m_end)
            m_start.reset();
    }

    bool hasStart() const { return m_start.has_value(); }
    bool hasEnd()   const { return m_end.has_value();  }

    std::optional<sf::Vector2i> getStart() const { return m_start; }
    std::optional<sf::Vector2i> getEnd()   const { return m_end;   }

    bool isStart(int gx, int gy) const {
        return m_start && m_start->x == gx && m_start->y == gy;
    }

    bool isEnd(int gx, int gy) const {
        return m_end && m_end->x == gx && m_end->y == gy;
    }

    void clearStart() { m_start.reset(); }
    void clearEnd()   { m_end.reset();   }

    void clearCell(int gx, int gy) {
        if (!inBounds(gx, gy)) return;
        m_walls[gy][gx] = false;
        if (isStart(gx, gy)) clearStart();
        if (isEnd(gx, gy))   clearEnd();
    }

    // počisti vse zidove in odstrani start/end
    void clearAll() {
        for (int y = 0; y < m_rows; ++y)
            for (int x = 0; x < m_cols; ++x)
                m_walls[y][x] = false;

        clearStart();
        clearEnd();
    }

    // preslikava med (gx, gy) in piksel koordinatami
    sf::Vector2f cellToWorld(int gx, int gy) const {
        return sf::Vector2f{
            static_cast<float>(gx * m_cellSize),
            static_cast<float>(gy * m_cellSize)
        };
    }

    std::optional<sf::Vector2i> worldToCell(const sf::Vector2i& pixelPos) const {
        int gx = pixelPos.x / m_cellSize;
        int gy = pixelPos.y / m_cellSize;
        if (!inBounds(gx, gy))
            return std::nullopt;
        return sf::Vector2i{gx, gy};
    }

    // generiranje labirinta z DFS (recursive backtracker, iterativno)
    void generateMaze() {
        // začnemo z "polnim" zidom
        for (int y = 0; y < m_rows; ++y)
            for (int x = 0; x < m_cols; ++x)
                m_walls[y][x] = true;

        clearStart();
        clearEnd();

        // mazeCols/mazeRows so celice labirinta, dejanski grid ima med njimi stene
        int mazeCols = (m_cols - 1) / 2;
        int mazeRows = (m_rows - 1) / 2;

        if (mazeCols <= 0 || mazeRows <= 0) {
            clearAll();
            return;
        }

        using Cell = sf::Vector2i; // (cx, cy) v maze koordinatah
        std::vector<Cell> stack;
        std::vector<std::vector<bool>> visited(
            mazeRows, std::vector<bool>(mazeCols, false)
        );

        auto carveCell = [&](int cx, int cy) {
            visited[cy][cx] = true;
            int wx = 2 * cx + 1;
            int wy = 2 * cy + 1;
            m_walls[wy][wx] = false;
        };

        // začnemo v (0,0) v maze koordinatah
        carveCell(0, 0);
        stack.push_back(Cell{0, 0});

        std::mt19937 rng{std::random_device{}()};

        while (!stack.empty()) {
            Cell cur = stack.back();

            std::vector<Cell> neighbors;
            const int dirs[4][2] = {
                { 1,  0},
                {-1,  0},
                { 0,  1},
                { 0, -1}
            };

            // pogledamo še neobiskane sosede v maze koordinatah
            for (auto& d : dirs) {
                int nx = cur.x + d[0];
                int ny = cur.y + d[1];
                if (nx < 0 || nx >= mazeCols || ny < 0 || ny >= mazeRows)
                    continue;
                if (visited[ny][nx])
                    continue;
                neighbors.push_back(Cell{nx, ny});
            }

            if (neighbors.empty()) {
                stack.pop_back();
                continue;
            }

            std::uniform_int_distribution<int> dist(0, (int)neighbors.size() - 1);
            Cell next = neighbors[dist(rng)];

            // trenutna celica v grid koordinatah
            int wx1 = 2 * cur.x + 1;
            int wy1 = 2 * cur.y + 1;
            // naslednja celica v grid koordinatah
            int wx2 = 2 * next.x + 1;
            int wy2 = 2 * next.y + 1;
            // stena med njima (sredina)
            int wxm = (wx1 + wx2) / 2;
            int wym = (wy1 + wy2) / 2;

            m_walls[wym][wxm] = false;
            carveCell(next.x, next.y);
            stack.push_back(next);
        }

        // start in end nastavimo v dva kota labirinta
        int sx = 1;
        int sy = 1;
        int ex = 2 * (mazeCols - 1) + 1;
        int ey = 2 * (mazeRows - 1) + 1;

        if (inBounds(sx, sy)) setStart(sx, sy);
        if (inBounds(ex, ey)) setEnd(ex, ey);
    }

private:
    int m_cols;
    int m_rows;
    int m_cellSize;

    std::vector<std::vector<bool>> m_walls;
    std::optional<sf::Vector2i> m_start;
    std::optional<sf::Vector2i> m_end;
};
