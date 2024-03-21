#pragma once

static const int charNr = 12;

struct point
{
    int x = 0;
    int y = 0;
    
    friend point operator + (point, point); 
    friend point operator % (point, int); 
    friend point operator % (point, point); 
};

struct cell
{
    bool collapsed = false;
    int collapsedChar = 33;
    bool possibilities[charNr]{true};
    int entropy = charNr;

    cell();
    int calculateEntropy();
    void reset();
    
};