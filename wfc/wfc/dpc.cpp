#include "dpc.h"

point operator + (point one, point two)
{
    return {one.x + two.x, one.y + two.y};
}

point operator % (point one, int mod)
{
    return {one.x % mod, one.y % mod};
}

point operator % (point one, point mod)
{
    return {one.x % mod.x, one.y % mod.y};
}

cell::cell()
{
    for(int i = 0; i < charNr; i++)
        possibilities[i] = true;
}

int cell::calculateEntropy()
{
    this->entropy = 0;
    for(int i = 0; i < charNr; i++)
        if(possibilities[i])
            this->entropy++;
    return entropy;
}

void cell::reset()
{
    this->collapsed = false;
    this->entropy = charNr;
    this->collapsedChar = 33;
    for(int i = 0; i < charNr; i++)
    {
        this->possibilities[i] = true;
    }
}

