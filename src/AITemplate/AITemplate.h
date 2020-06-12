#pragma once

#include <UltraOOXX/Wrapper/AI.h>
#include <UltraOOXX/UltraBoard.h>
#include <algorithm>
#include <random>
#include <ctime>

class AI : public AIInterface
{
public:
    void init(bool order) override
    {
        // any way
    }

    void callbackReportEnemy(int x, int y) override
    {
        (void)x;
        (void)y;
        // give last step
    }

    std::pair<int, int> queryWhereToPut(TA::UltraBoard mainboard) override
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (mainboard.get(i, j) == TA::Board::Tag::None)
                    return std::make_pair(i, j);
            }
        }
        return std::make_pair(-1, -1);
    }
};
