#include "BFSRunner.hpp"
#include <algorithm>

BFSRunner::BFSRunner(const Grid& grid)
    : m_grid(&grid),
      m_rows(grid.getRows()),
      m_cols(grid.getCols()),
      m_visited(m_rows, std::vector<bool>(m_cols, false)),
      m_inOpen(m_rows, std::vector<bool>(m_cols, false)),
      m_inClosed(m_rows, std::vector<bool>(m_cols, false)),
      m_parent(m_rows, std::vector<std::optional<sf::Vector2i>>(m_cols, std::nullopt)),
      m_inPath(m_rows, std::vector<bool>(m_cols, false)),
      m_finished(false),
      m_pathFound(false),
      m_visitedCount(0),
      m_openMaxSize(0),
      m_pathLength(0)
{
    auto startOpt = grid.getStart();
    auto endOpt   = grid.getEnd();

    if (!startOpt || !endOpt) {
        m_finished = true;
        m_pathFound = false;
        return;
    }

    m_start = *startOpt;
    m_end   = *endOpt;

    // če je start ali end na zidu, nima smisla iskati poti
    if (grid.isWall(m_start.x, m_start.y) || grid.isWall(m_end.x, m_end.y)) {
        m_finished = true;
        m_pathFound = false;
        return;
    }

    m_queue.push(m_start);
    m_visited[m_start.y][m_start.x] = true;
    m_inOpen[m_start.y][m_start.x] = true;
    m_openMaxSize = std::max(m_openMaxSize, (int)m_queue.size());
}

bool BFSRunner::step() {
    if (m_finished || !m_grid)
        return true;

    if (m_queue.empty()) {
        m_finished = true;
        m_pathFound = false;
        return true;
    }

    sf::Vector2i cur = m_queue.front();
    m_queue.pop();

    m_inOpen[cur.y][cur.x] = false;
    m_inClosed[cur.y][cur.x] = true;
    ++m_visitedCount;

    if (cur == m_end) {
        m_finished = true;
        m_pathFound = true;
        buildPath();
        return true;
    }

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
        if (m_visited[ny][nx])         continue;

        m_visited[ny][nx] = true;
        m_parent[ny][nx] = cur;
        m_queue.push(sf::Vector2i{nx, ny});
        m_inOpen[ny][nx] = true;

        m_openMaxSize = std::max(m_openMaxSize, (int)m_queue.size());
    }

    return false;
}

bool BFSRunner::isFinished() const { return m_finished; }
bool BFSRunner::hasPath()   const { return m_pathFound; }

bool BFSRunner::isOpen(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inOpen[gy][gx];
}

bool BFSRunner::isClosed(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inClosed[gy][gx];
}

bool BFSRunner::isInPath(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inPath[gy][gx];
}

bool BFSRunner::inBounds(int gx, int gy) const {
    return gx >= 0 && gx < m_cols && gy >= 0 && gy < m_rows;
}

void BFSRunner::buildPath() {
    for (int y = 0; y < m_rows; ++y)
        std::fill(m_inPath[y].begin(), m_inPath[y].end(), false);

    m_pathLength = 0;

    sf::Vector2i cur = m_end;

    // sledenje parentov od cilja nazaj do starta
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
