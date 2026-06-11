#pragma once

class Entities {
protected:
    int posnx;
    int posny;

private:
    const int id;
    int val;

public:
    Entities(int member_id, int member_val, int x, int y)
        : posnx(x),
          posny(y),
          id(member_id),
          val(member_val)
    {}
    int getVal() const { return val; }
    virtual ~Entities() = default;
};