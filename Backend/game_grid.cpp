#include "game_grid.h"

int getRandomToN(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(0, n);
    return distr(gen);
}

// Implement the constructor here completely
GameGrid::GameGrid(std::vector<std::pair<int,int>> chars, int bx, int by) 
    : characters(std::move(chars)) {
    
    GridMap.resize(bx); 
    for (int i = 0; i < bx; ++i) {
        GridMap[i].resize(by, 0);
    }
    
    for (size_t i = 0; i < characters.size(); ++i) {
        int x = characters[i].first;
        int y = characters[i].second;
        if (x >= 0 && x < bx && y >= 0 && y < by) {
            GridMap[x][y] = static_cast<int>(i + 1);
        }
    }
}

int GameGrid::AutoModifier() {
    int n{static_cast<int>(characters.size())};
    int character{getRandomToN(n-1)};
    int index{getRandomToN(7)};
    int count=8;
    int bx=static_cast<int>(GridMap.size());
    int by=static_cast<int>(GridMap[0].size());
    while(count--){
        int new_i=characters[character].first + directions[index][0];
        int new_j=characters[character].second + directions[index][1];
        if(new_j>=0 && new_i>=0 && new_j<by && new_i<bx && GridMap[new_i][new_j]==0){
            GridMap[characters[character].first][characters[character].second]=0;
            characters[character].first = new_i;
            characters[character].second = new_j;
            GridMap[new_i][new_j]=character+1;
            return 1;
        }
    }
    return -1;
}

int GameGrid::User_move(int character, int i, int j){
    if(character < 0 || character >= static_cast<int>(characters.size())) return -2;
    
    // FIX: Use 'character' as the index, not 'i'
    int prev_posnx = characters[character].first;
    int prev_posny = characters[character].second;

    int bx=static_cast<int>(GridMap.size());
    int by=static_cast<int>(GridMap[0].size());

    if((prev_posnx + i) < 0 || (prev_posny + j) < 0 || (prev_posnx + i) >= bx || (prev_posny + j) >= by ||
        GridMap[prev_posnx + i][prev_posny + j]!=0) {
        return -1;
    }
    
    GridMap[prev_posnx][prev_posny]=0;
    characters[character].first=prev_posnx+i;
    characters[character].second=prev_posny+j;
    
    GridMap[prev_posnx + i][prev_posny + j]=character+1;
    
    return 1;
}

void GameGrid::GridPrint(){
    std::cout<<"Printing Grid"<<'\n';
    for(auto it:GridMap){
        for(auto k:it){
            std::cout<<k<<" ";
        }std::cout<<'\n';
    }
    std::cout<<"Printing Grid Done"<<'\n';
}

