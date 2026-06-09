#include "game_grid.h"

// Implement the standalone function
int getRandomToN(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(0, n);
    return distr(gen);
}

// Implement the Constructor
GameGrid::GameGrid(std::vector<int> chars, int bx, int by, std::unordered_map<int, std::pair<int, int>>& map_ref) 
    : characters(std::move(chars)), boundsx(bx), boundsy(by), mp(map_ref) {}

// Implement AutoModifier
int GameGrid::AutoModifier() {
    int n = characters.size();
    if (n == 0) return -1; 
    
    int i = getRandomToN(n - 1);
    
    mp[i].first = (mp[i].first + 1) % boundsx;
    mp[i].second = (mp[i].second + 1) % boundsy;
    return 1;
}

// Implement User_move
int GameGrid::User_move(int character, int i, int j){
    if(character >= characters.size()) return -2;
    
    int prev_posnx = mp[i].first;
    int prev_posny = mp[i].second;
    
    if((prev_posnx + i) < 0 || (prev_posny + j) < 0 || (prev_posnx + i) >= boundsx || (prev_posny + j) >= boundsy) {
        return -1;
    }
    
    mp[i].first = prev_posnx + i;
    mp[i].second = prev_posny + j;
    return 1;
}