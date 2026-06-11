#include "game_grid.h"

int getRandomToN(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(0, n);
    return distr(gen);
}

// Constructor mapping using character->getVal()
GameGrid::GameGrid(std::vector<std::unique_ptr<Characters>> chars, int bx, int by) 
    : characters(std::move(chars)) {
    
    GridMap.resize(bx); 
    for (int i = 0; i < bx; ++i) {
        GridMap[i].resize(by, 0);
    }
    
    for (size_t i = 0; i < characters.size(); ++i) {
        int x = characters[i]->getX();
        int y = characters[i]->getY();
        if (x >= 0 && x < bx && y >= 0 && y < by) {
            // CHANGED: Tracking by specific object value property
            GridMap[x][y] = characters[i]->getVal();
        }
    }
}

int GameGrid::AutoModifier() {
    int n{static_cast<int>(characters.size())};
    if (n == 0) return -1;

    int character{getRandomToN(n - 1)};
    int index{getRandomToN(7)};
    int count = 8;
    int bx = static_cast<int>(GridMap.size());
    int by = static_cast<int>(GridMap[0].size());
    
    while (count--) {
        int current_x = characters[character]->getX();
        int current_y = characters[character]->getY();

        int new_i = current_x + directions[index][0];
        int new_j = current_y + directions[index][1];
        
        if (new_j >= 0 && new_i >= 0 && new_j < by && new_i < bx && GridMap[new_i][new_j] == 0) {
            GridMap[current_x][current_y] = 0;
            characters[character]->setX(new_i);
            characters[character]->setY(new_j);
            
            // CHANGED: Tracking by specific object value property
            GridMap[new_i][new_j] = characters[character]->getVal();
            return 1;
        }
        index = (index + 1) % 8; 
    }
    return -1;
}

int GameGrid::User_move(int character, int i, int j) {
    if (character < 0 || character >= static_cast<int>(characters.size())) return -2;
    
    int prev_posnx = characters[character]->getX();
    int prev_posny = characters[character]->getY();

    int bx = static_cast<int>(GridMap.size());
    int by = static_cast<int>(GridMap[0].size());

    if ((prev_posnx + i) < 0 || (prev_posny + j) < 0 || (prev_posnx + i) >= bx || (prev_posny + j) >= by ||
        GridMap[prev_posnx + i][prev_posny + j] != 0) {
        return -1;
    }
    
    GridMap[prev_posnx][prev_posny] = 0;
    
    characters[character]->setX(prev_posnx + i);
    characters[character]->setY(prev_posny + j);
    
    // CHANGED: Tracking by specific object value property
    GridMap[prev_posnx + i][prev_posny + j] = characters[character]->getVal();
    
    return 1;
}

void GameGrid::GridPrint() {
    std::cout << "Printing Grid\n";
    for (const auto& row : GridMap) {
        for (int cell : row) {
            std::cout << cell << " ";
        }
        std::cout << '\n';
    }
    std::cout << "Printing Grid Done\n";
}