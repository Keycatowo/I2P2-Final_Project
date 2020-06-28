#pragma once

#include <UltraOOXX/Wrapper/AI.h>
#include <UltraOOXX/UltraBoard.h>
#include <algorithm>
#include <random>
#include <ctime>
#include <queue>
#include <set>

class AI : public AIInterface
{
public:
    void init(bool order) override
    {
        // any way
        this->order = order;
        we_tag = order ? TA::Board::Tag::O : TA::Board::Tag::X;
        you_tag = order ? TA::Board::Tag::X : TA::Board::Tag::O;
        x = -1;
        y = -1;
        endgame_x= -1;
        endgame_y = -1;
        give_up.clear();
        // give_up.insert(std::make_pair(1,1));
    }

    void callbackReportEnemy(int x, int y) override
    {
        this->x = x;
        this->y = y;
        // give last step
    }

    std::pair<int, int> queryWhereToPut(TA::UltraBoard mainboard) override
    {
        check_order(mainboard);
        auto target_board = WhereCanPut(mainboard); // check which board we can put
        std::vector<std::pair<int, int>> avaliable_points;
        if (target_board.first == -1) // search the whole UltraBoard
        {
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    if (mainboard.get(i, j) == TA::Board::Tag::None)
                        avaliable_points.push_back(std::make_pair(i, j));
                }
            }
        }
        else // search one Board
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    if (mainboard.get(target_board.first * 3 + i, target_board.second * 3 + j) == TA::Board::Tag::None)
                        avaliable_points.push_back(std::make_pair(target_board.first * 3 + i, target_board.second * 3 + j));
                }
            }
        }

        /* middle of middle */
        if (mainboard.get(4, 4) == TA::Board::Tag::None)
        {
            give_up.insert(std::make_pair(1, 1));
            return std::make_pair(4, 4);
        }

        /* all board -> goto the other side */ 
        if (target_board.first == -1)
        {
            auto accross = std::make_pair(2 - x / 3, 2 - y / 3);
            if(mainboard.get(accross.first*3+x/3, accross.second*3+y/3) == TA::Board::Tag::None){
                return std::make_pair(accross.first*3+x/3, accross.second*3+y/3);
            }
            if(mainboard.get(accross.first*4,accross.second*4) == TA::Board::Tag::None){
                give_up.insert(accross);
                return std::make_pair(accross.first*4,accross.second*4);
            }
            // for (int i = 0; i < 3; i++)
            //     for (int j = 0; j < 3; j++){
            //         auto tmp_pos = std::make_pair(accross.first*3+i, accross.second*3+j);
            //         if(mainboard.get(tmp_pos.first, tmp_pos.second) == TA::Board::Tag::None){
            //             mainboard.get(tmp_pos.first, tmp_pos.second) = we_tag;
            //             //mainboard.sub(accross.first, accross.second).setWinTag(tmp_mainboard.sub(accross.first,accross.second).judgeWinState());
            //             if(mainboard.sub(accross.first,accross.second).judgeWinState()== we_tag)
            //                 return tmp_pos;
            //             else
            //             {
            //                 mainboard.get(tmp_pos.first, tmp_pos.second) = TA::Board::Tag::None;
            //             }
                        
            //         }
            //     }
            
        }
        

          // /* check 1 : can end the game */
        // for (auto i : avaliable_points){
        //     tmp_mainboard = mainboard;
        //     tmp_mainboard.get(i.first, i.second) = we_tag;
        //     if(tmp_mainboard.judgeWinState() == we_tag)
        //         return i;
        // }

        /* same board same place */
        for (auto i : avaliable_points)
            if (give_up.count(std::make_pair(i.first % 3, i.second % 3)) > 0 && !(mainboard.sub(i.first % 3, i.second % 3).full()))
                return i;
        
        for (auto i : avaliable_points)
            if (i.first / 3 == i.first % 3 && i.second / 3 == i.second % 3)
            {
                give_up.insert(std::make_pair(i.first % 3, i.second % 3));
                return i;
            }

        // // std::srand ( unsigned ( std::time(0) ) );
        // // std::random_shuffle(avaliable_points.begin(),avaliable_points.end());
        // // for(auto i :avaliable_points)
        // //     return i;

        // TA::UltraBoard tmp_mainboard = mainboard;
        // TA::UltraBoard tmp2_mainboard = mainboard;

        // /*if(we are first){
        //     if(step == some certain step) check4();
        //     else check5();
        // }*/

        // /* check 3 : middle of middle */
        // if(mainboard.get(4,4) == TA::Board::Tag::None)
        //     return std::make_pair(4,4);

        // /* check 5 : different board same place */

        // for(auto i: avaliable_points){
        //     while(check5 >0){
        //         auto enemy_board = std::make_pair(i.first%3, i.second%3);
        //         if(mainboard.sub(enemy_board.first, enemy_board.second).
        //             get(enemy_board.first, enemy_board.second) != TA::Board::Tag::None)
        //         return i;
        //     }
        //     check5--;
        // }
        // /*void check5(){
        //     for(auto i: avaliable_points){
        //         while(check5 >0){
        //             auto enemy_board = std::make_pair(i.first%3, i.second%3);
        //         if(mainboard.sub(enemy_board.first, enemy_board.second).
        //             get(enemy_board.first, enemy_board.second) != TA::Board::Tag::None)
        //             return i;
        //         }
        //         check5--;
        //     }
        // }*/
        // //check5();

        // /* check 4 : same board same place  */
        // for(auto i: avaliable_points){
        //     if(i.first/3 == i.first%3 && i.second/3 == i.second%3)
        //         return i;
        //     }
        // }

        // /* for(auto i: avaliable_points){
        //     if(i.first/3 == i.first%3 && i.second/3 == i.second%3)
        //         return i;
        //     }
        // }*/
        // //check4();

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

        /* check 2 : put the Board already had result */
        for(auto i : avaliable_points){
            if(mainboard.sub(i.first/3, i.second/3).judgeWinState() != TA::Board::Tag::None)
                return i;
        }

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

        std::srand(unsigned(time(0)));
        std::random_shuffle(avaliable_points.begin(),avaliable_points.end());
        // /* check 8 : empty place */
        // std::reverse(avaliable_points.begin(), avaliable_points.end());
        for(auto i : avaliable_points)
            return i;

        // /* none */
        return std::make_pair(-1, -1);
    }

    std::pair<int, int> WhereCanPut(TA::UltraBoard mainboard)
    {
        if (x == -1 && y == -1)
            return std::make_pair(-1, -1);
        auto target_board = std::make_pair(x % 3, y % 3);
        if (mainboard.sub(target_board.first, target_board.second).full())
            return std::make_pair(-1, -1);
        else
            return target_board;
    }

    bool cause_full(std::pair<int, int> pos, TA::UltraBoard mainboard)
    {
        return mainboard.sub(pos.first % 3, pos.second % 3).full();
    }

private:
    bool order;
    TA::Board::Tag we_tag;
    TA::Board::Tag you_tag;
    int x;
    int y;
    std::set<std::pair<int, int>> give_up;
    int endgame_x;
    int endgame_y;
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
