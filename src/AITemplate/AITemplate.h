#pragma once

#include <UltraOOXX/Wrapper/AI.h>
#include <UltraOOXX/UltraBoard.h>
#include <algorithm>
#include <random>
#include <ctime>
#include <queue>

class AI : public AIInterface
{
public:
    void init(bool order) override
    {
        // any way
        this->order = order;
        we_tag = order ? TA::Board::Tag::O : TA::Board::Tag::X;
    }

    void callbackReportEnemy(int x, int y) override
    {
        this->x = x;
        this->y = y;
        // give last step
    }

    std::pair<int, int> queryWhereToPut(TA::UltraBoard mainboard) override
    {
        auto target_board = WhereCanPut(mainboard); // check which board we can put
        std::vector<std::pair<int,int> > avaliable_points;
        if (target_board.first == -1)   // search the whole UltraBoard
        {
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    if (mainboard.get(i, j) == TA::Board::Tag::None)
                        avaliable_points.push_back(std::make_pair(i,j));
                }
            }
        }
        else    // search one Board
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (mainboard.get(target_board.first*3+i, target_board.second*3+j) == TA::Board::Tag::None)
                        avaliable_points.push_back(std::make_pair(target_board.first*3+i, target_board.second*3+j));
                }
            }
        }

        TA::UltraBoard tmp_mainboard = mainboard;

        /* check 1 : can end the game */
        for (auto i : avaliable_points){
            tmp_mainboard = mainboard;
            tmp_mainboard.get(i.first, i.second) = we_tag;
            if(tmp_mainboard.judgeWinState() == we_tag)
                return i;
        }

        /* check 2 : put the Board already had result */ 
        for(auto i : avaliable_points){
            if(mainboard.sub(i.first/3, i.second/3).judgeWinState() != TA::Board::Tag::None)
                return i;
        }

        /* check 3 : middle of middle */
        if(mainboard.get(4,4) == TA::Board::Tag::None)
            return std::make_pair(4,4);

        /* check 4 : same board same place  */
        for(auto i: avaliable_points){
            if(i.first/3 == i.first%3 && i.second/3 == i.second%3)
                return i;
        }

        /* check 5 : different board same place */
        for(auto i: avaliable_points){
            auto enemy_board = std::make_pair(i.first%3, i.second%3);
            if(mainboard.sub(enemy_board.first, enemy_board.second).
                get(enemy_board.first, enemy_board.second) != TA::Board::Tag::None)
                return i;
        }

        /* check 6 : other middles */ 
        for(auto i: avaliable_points){
            if(i.first%3 == 1 && i.second%3 == 1)
                return i;
        }

        /* check 7 : win Board */
        for(auto i : avaliable_points){
            tmp_mainboard = mainboard;
            tmp_mainboard.get(i.first, i.second) = we_tag;
            if(tmp_mainboard.sub(i.first%3,i.second%3).judgeWinState()==we_tag)
                return i;
        }

        /* check 8 : empty place */
        for(auto i : avaliable_points)
            return i;

        /* none */
        return std::make_pair(-1,-1);

    }

    std::pair<int, int> WhereCanPut(TA::UltraBoard mainboard)
    {
        auto target_board = std::make_pair(x % 3, y % 3);
        if (mainboard.sub(target_board.first, target_board.second).full())
            return std::make_pair(-1, -1);
        else
            return target_board;
    }

private:
    bool order;
    TA::Board::Tag we_tag;
    int x;
    int y;
};
