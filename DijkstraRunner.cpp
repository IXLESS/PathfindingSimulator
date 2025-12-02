#include "DijkstraRunner.hpp"
#include <algorithm> // std::fill za reset poti

DijkstraRunner::DijkstraRunner(const Grid& grid)
    : m_grid(&grid),
      m_rows(grid.getRows()),
      m_cols(grid.getCols()),
      m_visited(m_rows, std::vector<bool>(m_cols, false)),
      m_inOpen(m_rows, std::vector<bool>(m_cols, false)),
      m_inClosed(m_rows, std::vector<bool>(m_cols, false)),
      m_parent(m_rows, std::vector<std::optional<sf::Vector2i>>(m_cols, std::nullopt)),
      m_inPath(m_rows, std::vector<bool>(m_cols, false)),
      m_distance(m_rows, std::vector<int>(m_cols, INF)),
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

    // če je start ali end na zidu, algoritem nima smisla
    if (grid.isWall(m_start.x, m_start.y) || grid.isWall(m_end.x, m_end.y)) {
        m_finished = true;
        m_pathFound = false;
        return;
    }

    m_distance[m_start.y][m_start.x] = 0;
    pushOpen(m_start, 0);
}

bool DijkstraRunner::step() {
    if (m_finished || !m_grid)
        return true;

    while (!m_openPQ.empty()) {
        auto [dist, pos] = m_openPQ.top();
        m_openPQ.pop();

        // stari entry v PQ (imamo boljšo razdaljo zanj)
        if (dist > m_distance[pos.y][pos.x])
            continue;

        if (m_inClosed[pos.y][pos.x])
            continue;

        sf::Vector2i cur = pos;

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
            if (m_inClosed[ny][nx])        continue;

            // tukaj so vse povezave istih stroškov (1 korak = cena 1)
            int newDist = m_distance[cur.y][cur.x] + 1;

            if (newDist < m_distance[ny][nx]) {
                m_distance[ny][nx] = newDist;
                m_parent[ny][nx]   = cur;
                pushOpen(sf::Vector2i{nx, ny}, newDist);
            }
        }

        // simulacija: na frame obdelamo en vozlišče
        return false;
    }

    // PQ je prazen kar pomeni da ni poti do cilja
    if (!m_finished) {
        m_finished = true;
        m_pathFound = false;
    }
    return true;
}

bool DijkstraRunner::isFinished() const { return m_finished; }
bool DijkstraRunner::hasPath()   const { return m_pathFound; }

bool DijkstraRunner::isOpen(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inOpen[gy][gx];
}

bool DijkstraRunner::isClosed(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inClosed[gy][gx];
}

bool DijkstraRunner::isInPath(int gx, int gy) const {
    if (!inBounds(gx, gy)) return false;
    return m_inPath[gy][gx];
}

int DijkstraRunner::getDistance(int gx, int gy) const {
    if (!inBounds(gx, gy)) return INF;
    return m_distance[gy][gx];
}

bool DijkstraRunner::inBounds(int gx, int gy) const {
    return gx >= 0 && gx < m_cols && gy >= 0 && gy < m_rows;
}

void DijkstraRunner::pushOpen(const sf::Vector2i& pos, int dist) {
    m_openPQ.emplace(dist, pos);
    m_inOpen[pos.y][pos.x] = true;
    m_visited[pos.y][pos.x] = true;
    m_openMaxSize = std::max(m_openMaxSize, (int)m_openPQ.size());
}

void DijkstraRunner::buildPath() {
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
