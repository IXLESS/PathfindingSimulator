# Pathfinding Simulator

An interactive pathfinding visualizer implemented in **C++** using **SFML**.  
The simulator allows you to draw walls, generate mazes, and visually compare different pathfinding algorithms in real time.


## Features

### Implemented Algorithms
- **BFS** (Breadth-First Search)
![BFS](https://github.com/user-attachments/assets/2951dd70-ddc0-49da-87eb-07fcb291cb5d)
- **Dijkstra’s Algorithm**
![Dij](https://github.com/user-attachments/assets/f6597c90-73bd-4c0b-83dd-c898addf390a)
- **A\*** (Manhattan distance heuristic)
![A*](https://github.com/user-attachments/assets/67be0c5b-dc06-4af8-a062-e818dd5ad027)

Algorithms display:
- Open set  
- Closed set  
- Final reconstructed path  
- Number of visited nodes  
- Maximum open-set size  
- Path length  
- Execution time (ms)


## Controls

| Key | Action |
|-----|--------|
| **B** | Run BFS |
| **D** | Run Dijkstra |
| **A** | Run A\* |
| **R** | Reset algorithms (keep walls/start/end) |
| **C** | Clear everything |
| **M** | Generate random maze |
| **W** | Wall mode |
| **S** | Set Start |
| **E** | Set End |
| **Left Mouse** | Paint |
| **Right Mouse** | Erase |


# Build & Run

## 1. Requirements
- **C++17** compatible compiler  
- **CMake ≥ 3.10**  
- **SFML ≥ 2.5**


## 2. Linux (Ubuntu / Debian / Kali)

### Install SFML
```bash
sudo apt update
sudo apt install libsfml-dev
```

Build the project
```bash
git clone https://github.com/IXLESS/PathfindingSimulator.git
cd PathfindingSimulator
mkdir build
cd build
cmake ..
cmake --build .
```

Run
```bash
./PathfindingSimulator
```
## Windows (MinGW or Visual Studio)

Install SFML

Download SFML 2.5+ from:
https://www.sfml-dev.org/download.php

Extract it, then tell CMake where SFML is located:
```bash
cmake -DSFML_DIR="C:/Path/To/SFML/lib/cmake/SFML" ..
```
Build (MSVC)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
Run
```bash
./Release/PathfindingSimulator.exe
```
## macOS

Install SFML
```bash
brew install sfml
```
Build
```bash
git clone https://github.com/IXLESS/PathfindingSimulator.git
cd PathfindingSimulator
mkdir build
cd build
cmake ..
cmake --build .
```
Run
```bash
./PathfindingSimulator
```
