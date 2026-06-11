#pragma once
#include "entities.h"

class Characters : public Entities {
private:
    int posnx;
    int posny;

public:
    Characters(int member_id, int member_val, int x, int y);

    // Getters
    int getX() const;
    int getY() const;

    // Setters
    void setX(int x);
    void setY(int y);
};