#include "game_grid.h" // Links the header
#include <iostream>

int main() {
    std::vector<int> myCharacters = {1, 2};
    int boundsX = 5;
    int boundsY = 5;
    std::unordered_map<int, std::pair<int, int>> positionsMap;
    positionsMap[0] = {1, 1}; 
    positionsMap[1] = {2, 2};
    // Create the object normally
    GameGrid myGrid(myCharacters, boundsX, boundsY, positionsMap);
    std::vector<std::vector<int>>v1(boundsX,std::vector<int>(boundsY,0));
    for(int i=0;i<myCharacters.size();i++){
        v1[positionsMap[i].first][positionsMap[i].second]=myCharacters[i];
    }
    for(auto it:v1){
        for(auto k:it){
            std::cout<<k<<" ";
        }std::cout<<'\n';
    }
    myGrid.AutoModifier();
    for(auto& it:v1){
        for(auto& k:it){
            k=0;
        }
    }std::cout<<'\n';
    for(int i=0;i<myCharacters.size();i++){
        v1[positionsMap[i].first][positionsMap[i].second]=i+1;
    }
    for(auto it:v1){
        for(auto k:it){
            std::cout<<k<<" ";
        }std::cout<<'\n';
    }
    return 0;
}