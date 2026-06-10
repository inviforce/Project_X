#include "game_grid.h" 
#include <iostream>
#include <vector>
#include <utility>
#include <chrono>   
#include <thread>   

int main() {
    std::vector<std::pair<int, int>> myCharacters = {
        {1, 1}, {2, 5}, {4, 8}, {5, 2}, {6, 6}, 
        {7, 0}, {8, 9}, {9, 3}, {9, 7}, {0, 9}
    };
    int boundsX = 10;
    int boundsY = 10;

    GameGrid myGrid(myCharacters, boundsX, boundsY);
    
    std::cout << "--- Initial Board State ---" << '\n';
    myGrid.GridPrint();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        int choiceRoll = getRandomToN(10);
        std::cout << "\nChoice roll: " << choiceRoll << " -> ";

        if (choiceRoll % 2 != 0) {
            std::cout << "[Executing AutoModifier]\n";
            int result = myGrid.AutoModifier();
            std::cout << "AutoModifier result: " << result << '\n';
        } 
        else {
            std::cout << "[Executing User_move]\n";
            
            // Randomly select a character index (0 or 1 for 2 characters)
            int randomCharID = getRandomToN(static_cast<int>(myCharacters.size()) - 1);
            int moveX = getRandomToN(1);
            int moveY = getRandomToN(1);
            
            std::cout << "Selected Character ID: " << randomCharID 
                      << " | Offsets chosen: (" << moveX << ", " << moveY << ")\n";

            int result = myGrid.User_move(randomCharID, moveX, moveY);
            std::cout << "User_move result: " << result << '\n';
        }
        myGrid.GridPrint();
    }

    return 0;
}