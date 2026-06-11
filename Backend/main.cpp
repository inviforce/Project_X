#include "game_grid.h" 
#include <iostream>
#include <vector>
#include <utility>
#include <chrono>   
#include <thread>   
#include <memory>    // Required for std::unique_ptr and std::make_unique
#include "characters.h"
#include "entities.h"

int main() {
    // 1. Define the grid boundaries
    int boundsX = 10;
    int boundsY = 10;

    // 2. Create a vector of unique pointers to Characters
    std::vector<std::unique_ptr<Characters>> myCharacters;

    // Raw initial coordinate pairs
    std::vector<std::pair<int, int>> initialCoords = {
        {1, 1}, {2, 5}, {4, 8}, {5, 2}, {6, 6}, 
        {7, 0}, {8, 9}, {9, 3}, {9, 7}, {0, 9}
    };

    // 3. Populate the unique_ptr vector
    // Arguments passed to Characters: member_id (i), member_val (i*10), x, y
    for (size_t i = 0; i < initialCoords.size(); ++i) {
        myCharacters.push_back(
            std::make_unique<Characters>(static_cast<int>(i + 1), static_cast<int>(i * 10), initialCoords[i].first, initialCoords[i].second)
        );
    }

    // Keep track of total character count before moving ownership
    int totalCharacters = static_cast<int>(myCharacters.size());

    // 4. Pass ownership to the Grid using std::move
    GameGrid myGrid(std::move(myCharacters), boundsX, boundsY);
    
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
            
            int randomCharID = getRandomToN(totalCharacters - 1);
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