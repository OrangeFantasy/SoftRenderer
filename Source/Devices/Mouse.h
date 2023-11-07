#pragma once

class FMouse
{
public:
    FMouse() = default;

    void SetPos(int NewX, int NewY)
    {
        X = NewX;
        Y = NewY;
    };

    int GetPosX() const { return X; };
    int GetPosY() const { return Y; };

private:
    int X = 0;
    int Y = 0;
};