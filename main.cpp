#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <string>
#include <iostream>

#include "Grid.hpp"
#include "BFSRunner.hpp"
#include "DijkstraRunner.hpp"
#include "AStarRunner.hpp"

// Način risanja z miško (kaj delamo z levim klikom)
enum class PaintMode {
    Walls,
    Start,
    End
};

// Kateri algoritem je trenutno aktiven v simulaciji
enum class ActiveAlgo {
    None,
    BFS,
    Dijkstra,
    AStar
};

int main() {
    const int cellSize = 20;
    const int cols = 40;
    const int rows = 30;

    // Dimenzije mreže in HUD-a
    const int gridWidth  = cols * cellSize;
    const int gridHeight = rows * cellSize;
    const int hudWidth   = 400;
    const int windowWidth  = gridWidth + hudWidth;
    const int windowHeight = gridHeight;

    Grid grid(cols, rows, cellSize);
    PaintMode mode = PaintMode::Walls;

    // Runnerji za posamezne algoritme (po en naenkrat)
    std::optional<BFSRunner>       bfsRunner;
    std::optional<DijkstraRunner>  dijkstraRunner;
    std::optional<AStarRunner>     aStarRunner;

    ActiveAlgo activeAlgo = ActiveAlgo::None;
    std::string lastAlgoName = "None";

    // Čas izvajanja trenutnega algoritma
    sf::Clock algoClock;
    bool timing = false;
    float elapsedMs = 0.0f;

    sf::RenderWindow window(
        sf::VideoMode({static_cast<unsigned int>(windowWidth),
                       static_cast<unsigned int>(windowHeight)}),
        "PATH finder"
    );

    // Font za HUD tekst (probamo par lokacij)
    sf::Font font;
    bool fontLoaded = false;

    if (font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        fontLoaded = true;
    } else if (font.openFromFile("DejaVuSans.ttf")) {
        fontLoaded = true;
    } else if (font.openFromFile("arial.ttf")) {
        fontLoaded = true;
    } else {
        std::cerr << "WARNING: Font could not be loaded, HUD text will not show.\n";
    }

    while (window.isOpen()) {
        // DOGODKI (tipke, zapiranje okna ...)
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::W:
                        mode = PaintMode::Walls;
                        break;
                    case sf::Keyboard::Key::S:
                        mode = PaintMode::Start;
                        break;
                    case sf::Keyboard::Key::E:
                        mode = PaintMode::End;
                        break;

                    // BFS
                    case sf::Keyboard::Key::B: {
                        if (grid.hasStart() && grid.hasEnd()) {
                            bfsRunner.emplace(grid);
                            dijkstraRunner.reset();
                            aStarRunner.reset();
                            activeAlgo = ActiveAlgo::BFS;
                            lastAlgoName = "BFS";

                            algoClock.restart();
                            timing = true;
                            elapsedMs = 0.0f;
                        } else {
                            bfsRunner.reset();
                            dijkstraRunner.reset();
                            aStarRunner.reset();
                            activeAlgo = ActiveAlgo::None;
                            timing = false;
                            lastAlgoName = "None";
                        }
                        break;
                    }

                    // Dijkstra
                    case sf::Keyboard::Key::D: {
                        if (grid.hasStart() && grid.hasEnd()) {
                            dijkstraRunner.emplace(grid);
                            bfsRunner.reset();
                            aStarRunner.reset();
                            activeAlgo = ActiveAlgo::Dijkstra;
                            lastAlgoName = "Dijkstra";

                            algoClock.restart();
                            timing = true;
                            elapsedMs = 0.0f;
                        } else {
                            bfsRunner.reset();
                            dijkstraRunner.reset();
                            aStarRunner.reset();
                            activeAlgo = ActiveAlgo::None;
                            timing = false;
                            lastAlgoName = "None";
                        }
                        break;
                    }

                    // A*
                    case sf::Keyboard::Key::A: {
                        if (grid.hasStart() && grid.hasEnd()) {
                            aStarRunner.emplace(grid);
                            bfsRunner.reset();
                            dijkstraRunner.reset();
                            activeAlgo = ActiveAlgo::AStar;
                            lastAlgoName = "A*";

                            algoClock.restart();
                            timing = true;
                            elapsedMs = 0.0f;
                        } else {
                            bfsRunner.reset();
                            dijkstraRunner.reset();
                            aStarRunner.reset();
                            activeAlgo = ActiveAlgo::None;
                            timing = false;
                            lastAlgoName = "None";
                        }
                        break;
                    }

                    // C = popoln reset (mreža + start/end + algoritmi + čas)
                    case sf::Keyboard::Key::C: {
                        grid.clearAll();
                        bfsRunner.reset();
                        dijkstraRunner.reset();
                        aStarRunner.reset();
                        activeAlgo = ActiveAlgo::None;
                        timing = false;
                        elapsedMs = 0.0f;
                        lastAlgoName = "None";
                        break;
                    }

                    // R = reset samo algoritmov (mreža ostane)
                    case sf::Keyboard::Key::R: {
                        bfsRunner.reset();
                        dijkstraRunner.reset();
                        aStarRunner.reset();
                        activeAlgo = ActiveAlgo::None;
                        timing = false;
                        elapsedMs = 0.0f;
                        lastAlgoName = "None";
                        break;
                    }

                    // M = generiraj nov random labirint
                    case sf::Keyboard::Key::M: {
                        grid.generateMaze();
                        bfsRunner.reset();
                        dijkstraRunner.reset();
                        aStarRunner.reset();
                        activeAlgo = ActiveAlgo::None;
                        timing = false;
                        elapsedMs = 0.0f;
                        lastAlgoName = "None";
                        break;
                    }

                    default:
                        break;
                }
            }
        }

        // REAL-TIME MIŠ (risanje po mreži)
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (auto cell = grid.worldToCell(mousePos)) {
            int gx = cell->x;
            int gy = cell->y;

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                switch (mode) {
                    case PaintMode::Walls:
                        grid.setWall(gx, gy, true);
                        break;
                    case PaintMode::Start:
                        grid.setStart(gx, gy);
                        break;
                    case PaintMode::End:
                        grid.setEnd(gx, gy);
                        break;
                }
                // vsaka sprememba mreže invalidira prejšnji algoritem
                bfsRunner.reset();
                dijkstraRunner.reset();
                aStarRunner.reset();
                activeAlgo = ActiveAlgo::None;
                timing = false;
            } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                grid.clearCell(gx, gy);
                bfsRunner.reset();
                dijkstraRunner.reset();
                aStarRunner.reset();
                activeAlgo = ActiveAlgo::None;
                timing = false;
            }
        }

        // KORAKI ALGORITMOV (en ali več stepov na frame)
        if (activeAlgo == ActiveAlgo::BFS && bfsRunner && !bfsRunner->isFinished()) {
            bfsRunner->step();
            if (bfsRunner->isFinished()) {
                elapsedMs = algoClock.getElapsedTime().asMilliseconds();
                timing = false;
            }
        } else if (activeAlgo == ActiveAlgo::Dijkstra && dijkstraRunner && !dijkstraRunner->isFinished()) {
            dijkstraRunner->step();
            if (dijkstraRunner->isFinished()) {
                elapsedMs = algoClock.getElapsedTime().asMilliseconds();
                timing = false;
            }
        } else if (activeAlgo == ActiveAlgo::AStar && aStarRunner && !aStarRunner->isFinished()) {
            aStarRunner->step();
            if (aStarRunner->isFinished()) {
                elapsedMs = algoClock.getElapsedTime().asMilliseconds();
                timing = false;
            }
        }

        // RISANJE
        window.clear(sf::Color::Black);

        sf::RectangleShape cellShape({19.f, 19.f});

        // MREŽA (celice + obarvani seti)
        for (int y = 0; y < grid.getRows(); ++y) {
            for (int x = 0; x < grid.getCols(); ++x) {

                bool inPath   = false;
                bool inOpen   = false;
                bool inClosed = false;

                if (activeAlgo == ActiveAlgo::BFS && bfsRunner) {
                    inPath   = bfsRunner->isInPath(x, y);
                    inOpen   = bfsRunner->isOpen(x, y);
                    inClosed = bfsRunner->isClosed(x, y);
                } else if (activeAlgo == ActiveAlgo::Dijkstra && dijkstraRunner) {
                    inPath   = dijkstraRunner->isInPath(x, y);
                    inOpen   = dijkstraRunner->isOpen(x, y);
                    inClosed = dijkstraRunner->isClosed(x, y);
                } else if (activeAlgo == ActiveAlgo::AStar && aStarRunner) {
                    inPath   = aStarRunner->isInPath(x, y);
                    inOpen   = aStarRunner->isOpen(x, y);
                    inClosed = aStarRunner->isClosed(x, y);
                }

                if (grid.isStart(x, y)) {
                    cellShape.setFillColor(sf::Color::Green);
                } else if (grid.isEnd(x, y)) {
                    cellShape.setFillColor(sf::Color::Red);
                } else if (inPath) {
                    cellShape.setFillColor(sf::Color(128, 0, 128));
                } else if (inClosed) {
                    cellShape.setFillColor(sf::Color::Yellow);
                } else if (inOpen) {
                    cellShape.setFillColor(sf::Color::Blue);
                } else if (grid.isWall(x, y)) {
                    cellShape.setFillColor(sf::Color(100, 100, 100));
                } else {
                    cellShape.setFillColor(sf::Color(50, 50, 50));
                }

                cellShape.setPosition(grid.cellToWorld(x, y));
                window.draw(cellShape);
            }
        }

        // HUD ozadje na desni strani
        sf::RectangleShape hudBackground(sf::Vector2f{
            static_cast<float>(hudWidth),
            static_cast<float>(windowHeight)
        });
        hudBackground.setFillColor(sf::Color(0, 0, 0, 220));
        hudBackground.setPosition(sf::Vector2f{
            static_cast<float>(gridWidth), 0.f
        });
        window.draw(hudBackground);

        // HUD: čas + statistika + kontrole
        if (fontLoaded) {
            sf::Text timerText(font);
            timerText.setCharacterSize(22);
            timerText.setFillColor(sf::Color::White);

            float msShown = timing ? algoClock.getElapsedTime().asMilliseconds()
                                   : elapsedMs;

            std::string algoLabel = lastAlgoName;
            if (activeAlgo == ActiveAlgo::None && lastAlgoName == "None") {
                algoLabel = "None";
            }

            int nodesVisited = 0;
            int pathLength   = 0;
            int openMaxSize  = 0;

            if (activeAlgo == ActiveAlgo::BFS && bfsRunner) {
                nodesVisited = bfsRunner->getVisitedCount();
                pathLength   = bfsRunner->getPathLength();
                openMaxSize  = bfsRunner->getOpenMaxSize();
            } else if (activeAlgo == ActiveAlgo::Dijkstra && dijkstraRunner) {
                nodesVisited = dijkstraRunner->getVisitedCount();
                pathLength   = dijkstraRunner->getPathLength();
                openMaxSize  = dijkstraRunner->getOpenMaxSize();
            } else if (activeAlgo == ActiveAlgo::AStar && aStarRunner) {
                nodesVisited = aStarRunner->getVisitedCount();
                pathLength   = aStarRunner->getPathLength();
                openMaxSize  = aStarRunner->getOpenMaxSize();
            }

            timerText.setString(
                "Algo: " + algoLabel + "\n" +
                "Time: " + std::to_string(static_cast<int>(msShown)) + " ms\n" +
                "Nodes visited: " + std::to_string(nodesVisited) + "\n" +
                "Path length:   " + std::to_string(pathLength) + "\n" +
                "Max open size: " + std::to_string(openMaxSize) + "\n\n" +
                "Controls:\n"
                "  B = Run BFS\n"
                "  D = Run Dijkstra\n"
                "  A = Run A*\n"
                "\n"
                "  R = Reset algorithms\n"
                "  C = Clear ALL\n"
                "  M = Random maze\n"
                "\n"
                "  W = Set Walls\n"
                "  S = Set Start\n"
                "  E = Set End\n"
                "\n"
                "Left Click  = paint\n"
                "Right Click = erase\n"
            );

            timerText.setPosition(sf::Vector2f{
                static_cast<float>(gridWidth) + 20.f,
                10.f
            });
            window.draw(timerText);
        }

        window.display();
    }

    return 0;
}
