#pragma once
#include "characters.h"
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include <unordered_map>

int getRandomToN(int n);

class GameGrid {
private:
    std::vector<std::unique_ptr<Characters>> characters; // Removed const so methods can modify state
    std::vector<std::vector<int>> GridMap;
    std::vector<std::vector<int>> directions={{1,1},{-1,-1},{1,0},{0,1},{0,-1},{-1,0},{-1,1},{1,-1}};

public:
    GameGrid(std::vector<std::unique_ptr<Characters>> chars, int bx, int by);
    
    int AutoModifier();
    int User_move(int character, int i, int j);
    void GridPrint();
};