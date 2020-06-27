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
        you_tag = order ? TA::Board::Tag::X : TA::Board::Tag::O;
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

        /* random AI */
        std::srand ( unsigned ( std::time(0) ) );
        std::random_shuffle(avaliable_points.begin(),avaliable_points.end());
        for(auto i :avaliable_points)
            return i;
        
        
        // TA::UltraBoard tmp_mainboard = mainboard;
        // TA::UltraBoard tmp2_mainboard = mainboard;

        // /* check 1 : can end the game */
        // for (auto i : avaliable_points){
        //     tmp_mainboard = mainboard;
        //     tmp_mainboard.get(i.first, i.second) = we_tag;
        //     if(tmp_mainboard.judgeWinState() == we_tag)
        //         return i;
        // }

        // /* check 1-2 :enemy can end the game */
        // std::vector<std::pair<int,int> > enemy_avaliable_points;
        // for (auto begin = avaliable_points.begin();begin!=avaliable_points.end();){
        //     enemy_avaliable_points.clear();
        //     auto tmp = *begin;
        //     tmp_mainboard = mainboard;
        //     tmp_mainboard.get(tmp.first, tmp.second) = we_tag;
        
        
        //     for(int i=0;i<3;i++){
        //         for(int j=0;i<3;j++){
        //             if(tmp_mainboard.get(tmp.first%3+i,tmp.second%3+j) == TA::Board::Tag::None)
        //                 enemy_avaliable_points.push_back(std::make_pair(tmp.first%3+i,tmp.second%3+j));
        //         }
        //     }
            
        //     for(auto begin2 = enemy_avaliable_points.begin(); begin2 != enemy_avaliable_points.end();){
        //         tmp = *begin2;
        //         tmp2_mainboard = tmp_mainboard;
        //         tmp2_mainboard.get(i.first, i.second) = you_tag;
        //         if(tmp_mainboard.judgeWinState() == you_tag){
        //             begin = avaliable_points.erase(begin);
        //             continue;
        //         }
        //         else
        //         {
        //             begin++
        //         }
                
        //     }
        //     if(tmp_mainboard.judgeWinState() == we_tag)
        //         begin = avaliable_points.erase(begin);
        //     else
        //         begin++;
        // }

        // /* check 2 : put the Board already had result */ 
        // for(auto i : avaliable_points){
        //     if(mainboard.sub(i.first/3, i.second/3).judgeWinState() != TA::Board::Tag::None)
        //         return i;
        // }

        // /* check 3 : middle of middle */
        // if(mainboard.get(4,4) == TA::Board::Tag::None)
        //     return std::make_pair(4,4);

        // /* check 4 : same board same place  */
        // for(auto i: avaliable_points){
        //     if(i.first/3 == i.first%3 && i.second/3 == i.second%3)
        //         return i;
        // }

        // /* check 5 : different board same place */
        // for(auto i: avaliable_points){
        //     auto enemy_board = std::make_pair(i.first%3, i.second%3);
        //     if(mainboard.sub(enemy_board.first, enemy_board.second).
        //         get(enemy_board.first, enemy_board.second) != TA::Board::Tag::None)
        //         return i;
        // }
        
        // /* check 6 : other middles */ 
        // for(auto i: avaliable_points){
        //     if(i.first%3 == 1 && i.second%3 == 1)
        //         return i;
        // }

        // /* check 7 : win Board */
        // for(auto i : avaliable_points){
        //     tmp_mainboard = mainboard;
        //     tmp_mainboard.get(i.first, i.second) = we_tag;
        //     if(tmp_mainboard.sub(i.first%3,i.second%3).judgeWinState()==we_tag)
        //         return i;
        // }

        // /* check 8 : empty place */
        // std::reverse(avaliable_points.begin(), avaliable_points.end());
        // for(auto i : avaliable_points)
        //     return i;
        
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
    TA::Board::Tag you_tag;
    int x;
    int y;

    void check_order(TA::UltraBoard mainboard){
        int count = 0;
        for(int i=0;i<8;i++)
            for(int j=0;j<8;j++)
                if(mainboard.get(i,j)!=TA::Board::Tag::None)
                    ++count;
        if(count%2 == 0){
            order = true;
            we_tag = TA::Board::Tag::O;
            you_tag = TA::Board::Tag::X;
        }
        else
        {
            order = false;
            we_tag = TA::Board::Tag::X;
            you_tag = TA::Board::Tag::O;
        }
        
    }
};
