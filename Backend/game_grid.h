#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <unordered_map>

int getRandomToN(int n);

class GameGrid {
private:
    std::vector<std::pair<int,int>> characters; // Removed const so methods can modify state
    std::vector<std::vector<int>> GridMap;
    std::vector<std::vector<int>> directions={{1,1},{-1,-1},{1,0},{0,1},{0,-1},{-1,0},{-1,1},{1,-1}};

public:
    GameGrid(std::vector<std::pair<int,int>> chars, int bx, int by);
    
    int AutoModifier();
    int User_move(int character, int i, int j);
    void GridPrint();
};