#include "characters.h"

// Constructor using initializer list
Characters::Characters(int member_id, int member_val, int x, int y)
    : Entities(member_id, member_val, x, y), 
      posnx(x), 
      posny(y) 
{
    
}

// Getters
int Characters::getX() const {
    return posnx;
}

int Characters::getY() const {
    return posny;
}

// Setters
void Characters::setX(int x) {
    posnx = x;
}

void Characters::setY(int y) {
    posny = y;
}