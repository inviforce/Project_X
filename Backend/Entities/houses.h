#pragma once
#include "entities.h" 

class House : public Entities { 
private:
    const int width;
    const int breadth;

    static int validateOdd(int value);

public:
    House(int member_id, int member_val, int w, int b, int x, int y);
};