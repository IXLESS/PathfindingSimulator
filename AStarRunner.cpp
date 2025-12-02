#include "AStarRunner.hpp"
#include <algorithm>
#include <cmath>

AStarRunner::AStarRunner(const Grid& grid)
    : m_grid(&grid),
      m_rows(grid.getRows()),
      m_cols(grid.getCols()),
      m_inOpen(m_rows, std::vector<bool>(m_cols, false)),
      m_inClosed(m_rows, std::vector<bool>(m_cols, false)),
      m_parent(m_rows, std::vector<std::optional<sf::Vector2i>>(m_cols, std::nullopt)),
      m_inPath(m_rows, std::vector<bool>(m_cols, false)),
      m_gScore(m_rows, std::vector<int>(m_cols, INF)),
      m_finished(false),
      m_pathFound(false),
      m_visitedCount(0),
      m_openMaxSize(0),
      m_pathLength(0)
{
    auto startOpt = grid.getStart();
    auto endOpt   = grid.getEnd();

    // brez start ali end nima smisla računat
    if (!startOpt || !endOpt) {
        m_finished = true;
        m_pathFound = false;
        return;
    }

    m_start = *startOpt;
    m_end   = *endOpt;

    // če sta start ali end na zidu, A* takoj odpovemo
    if (grid.isWall(m_start.x, m_start.y) || grid.isWall(m_end.x, m_end.y)) {
        m_finished = true;
        m_pathFound = false;
        return;
    }

    m_gScore[m_start.y][m_start.x] = 0;
    int fStart = heuristic(m_start, m_end);
    pushOpen(m_start, fStart);
}

bool AStarRunner::step() {
    if (m_finished || !m_grid)
        return true;

    while (!m_openPQ.empty()) {
        auto [fScore, pos] = m_openPQ.top();
        m_openPQ.pop();

        // če je že v closed, ta zapis ignoriramo
        if (m_inClosed[pos.y][pos.x])
            continue;

        sf::Vector2i cur = pos;

        m_inOpen[cur.y][cur.x] = false;
        m_inClosed[cur.y][cur.x] = true;
        ++m_visitedCount;

        // cilj najden, zgradimo pot
        if (cur == m_end) {
            m_finished = true;
            m_pathFound = true;
            buildPath();
            return true;
        }

        // 4-sosedsko premikanje (gor/dol/levo/desno)
        static const sf::Vector2i dirs[4] = {
            { 1,  0},
            {-1,  0},
            { 0,  1},
            { 0, -1}
        };

        for (auto d : dirs) {
            int nx = cur.x + d.x;
            int ny = cur.y + d.y;

            if (!m_grid->inBounds(nx, ny)) continue;
            if (m_grid->isWall(nx, ny))    continue;
            if (m_inClosed[ny][nx])        continue;

            // vsak korak ima enako ceno (g + 1)
            int tentativeG = m_gScore[cur.y][cur.x] + 1;

            if (tentativeG < m_gScore[ny][nx]) {
                m_gScore[ny][nx] = tentativeG;
                m_parent[ny][nx] = cur;

                int fScoreNeighbour = tentativeG + heuristic(sf::Vector2i{nx, ny}, m_end);
                pushOpen(sf::Vector2i{nx, ny}, fScoreNeighbour);
            }
        }

        // v enem frame-u obdelamo en vozlišče
        return false;
    }

    // open set je prazen
    if (!m_finished) {
        m_finished = true;
        m_pathFound = false;
    }
    return true;
}

bool AStarRunner::isFinished() const { return m_finished; }
bool AStarRunner::hasPath()   const { return m_pathFound; }

bool AStarRunner::isOpen(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inOpen[gy][gx];
}

bool AStarRunner::isClosed(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inClosed[gy][gx];
}

bool AStarRunner::isInPath(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inPath[gy][gx];
}

int AStarRunner::getGScore(int gx, int gy) const {
    if (!inBounds(gx, gy)) return INF;
    return m_gScore[gy][gx];
}

bool AStarRunner::inBounds(int gx, int gy) const {
    return gx >= 0 && gx < m_cols && gy >= 0 && gy < m_rows;
}

void AStarRunner::pushOpen(const sf::Vector2i& pos, int fScore) {
    m_openPQ.emplace(fScore, pos);
    m_inOpen[pos.y][pos.x] = true;
    m_openMaxSize = std::max(m_openMaxSize, (int)m_openPQ.size());
}

void AStarRunner::buildPath() {
    for (int y = 0; y < m_rows; ++y)
        std::fill(m_inPath[y].begin(), m_inPath[y].end(), false);

    m_pathLength = 0;

    sf::Vector2i cur = m_end;

    // gremo nazaj po parentih od cilja do starta
    while (!(cur == m_start)) {
        m_inPath[cur.y][cur.x] = true;
        ++m_pathLength;

        auto& parentOpt = m_parent[cur.y][cur.x];
        if (!parentOpt) break;
        cur = *parentOpt;
    }

    m_inPath[m_start.y][m_start.x] = true;
    ++m_pathLength;
}

int AStarRunner::heuristic(const sf::Vector2i& a, const sf::Vector2i& b) const {
    // Manhattan razdalja (dela dobro za 4-sosedsko mrežo)
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
