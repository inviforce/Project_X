#include "houses.h"
#include <stdexcept>

int House::validateOdd(int value) {
    if (value % 2 == 0) {
        throw std::invalid_argument("Value must be an odd number!");
    }
    return value;
}

House::House(int member_id, int member_val, int w, int b, int x, int y)
    : Entities(member_id, member_val, x, y), 
      width(validateOdd(w)),    
      breadth(validateOdd(b))  
{
    // Implementation body
}