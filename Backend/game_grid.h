#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <unordered_map>

// Declare the standalone function
int getRandomToN(int n);

// Declare the class and its members
class GameGrid {
private:
    const std::vector<int> characters;
    const int boundsx;
    const int boundsy;
    std::unordered_map<int, std::pair<int, int>>& mp; 

public:
    GameGrid(std::vector<int> chars, int bx, int by, std::unordered_map<int, std::pair<int, int>>& map_ref);
    int AutoModifier();
    int User_move(int character, int i, int j);
};