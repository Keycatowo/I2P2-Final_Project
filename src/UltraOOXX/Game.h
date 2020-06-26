#pragma once

#include <UltraOOXX/UltraBoard.h>
#include <UltraOOXX/Wrapper/AI.h>
#include <GUI/GUIInterface.h>

#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdarg>
#include <future>
#include <type_traits>

namespace TA
{
    class UltraOOXX
    {
    public:
        UltraOOXX(
            std::chrono::milliseconds runtime_limit = std::chrono::milliseconds(1000)
        ):
            m_runtime_limit(runtime_limit),
            m_P1(nullptr),
            m_P2(nullptr),
            MainBoard()
        {
            gui = new ASCII;
        }

        void setPlayer1(AIInterface *ptr) { assert(checkAI(ptr)); m_P1 = ptr; }
        void setPlayer2(AIInterface *ptr) { assert(checkAI(ptr)); m_P2 = ptr; }

        void run()
        {
            gui->title();
            round = 0;
            if( !prepareState() ) return ;

            //Todo: Play Game
            /*
            for (int i=0; i<9; ++i)
                for (int j=0; j<9; ++j){
                    if((i+j)%2)
                        MainBoard.get(i,j)=BoardInterface::Tag::O;
                    else
                        MainBoard.get(i,j)=BoardInterface::Tag::X;
                    MainBoard.sub(i/3,j/3).setWinTag(MainBoard.sub(i/3,j/3).judgeWinState());
                    MainBoard.setWinTag(MainBoard.judgeWinState());
                }
            */

            //下棋
            while (!checkGameover()) 
            {
                AIInterface *now_player, *next_player;
                BoardInterface::Tag tag;

                round++;
                if(round%2==0)
                {
                    now_player = m_P1;
                    next_player = m_P2;
                    tag = BoardInterface::Tag::O;
                }
                else
                {
                    now_player = m_P2;
                    next_player = m_P1;
                    tag = BoardInterface::Tag::X;
                }

                if (!playOneRound(now_player, tag, next_player)) break;

                /* print the result of each Borad */
                printBoard();
            }
                    
            /* print the result of winner */
            printWinner();
        }

   private:

        void printBoard()
        {
            for (int i=0; i<3; ++i){
                for (int j=0; j<3; ++j){
                    if(MainBoard.sub(i,j).judgeWinState() == BoardInterface::Tag::O)        putToGui("O");
                    else if(MainBoard.sub(i,j).judgeWinState() == BoardInterface::Tag::X)   putToGui("X");
                    else if(MainBoard.sub(i,j).judgeWinState() == BoardInterface::Tag::Tie) putToGui("*");
                    else putToGui("-");
                }
                putToGui("\n");
            }
            //updateGuiGame();
        }

        void printWinner()
        {
            if(MainBoard.judgeWinState() == BoardInterface::Tag::O)
                putToGui("O wins\n");
            else if(MainBoard.judgeWinState() == BoardInterface::Tag::X)
                putToGui("X wins\n");
            else if(MainBoard.judgeWinState() == BoardInterface::Tag::Tie)
                putToGui("Tie\n");
            else if (0 <= round && round <=81)
            {
                if(round%2 == 0) putToGui("X wins\n");
                else putToGui("O wins\n");
            }
            
            updateGuiGame();
        }

        void updateGuiGame()
        {
            gui->updateGame(MainBoard);
        }

        bool playOneRound(AIInterface *now_player, BoardInterface::Tag tag, AIInterface *next_player)
        {
            auto pos = call(&AIInterface::queryWhereToPut, now_player, MainBoard);
            if(!checkValid(pos)) return false;
            MainBoard.get(pos.first, pos.second) = tag;
            next_player->callbackReportEnemy(pos.first, pos.second);
            if( MainBoard.sub(pos.first%3,pos.second%3).full() ) last_put=std::make_pair(-1,-1);
            else last_put=pos;
        }

        bool checkValid( std::pair<int,int> pos)
        {
            
            //檢查是否下在非法的位置上
            //如果位置非法，則判定輸掉比賽
            //先檢查last_pos下在board的哪裡，然後再檢查pos下在ultra_board的位置
            if(TA::UltraOOXX::MainBoard.get(pos.first,pos.second)==BoardInterface::Tag::None)
            { 
                if(last_put.first == -1 && last_put.second == -1) return true;
                else if(last_put.first%3==pos.first/3&&last_put.second%3==pos.second/3) return true;
            }
            return false;
        }

        bool checkGameover()
        {
            if(MainBoard.judgeWinState() != BoardInterface::Tag::None){
                MainBoard.setWinTag(MainBoard.judgeWinState());
                return true; // Gameover!
            }
            else
                return false;
        }

        bool prepareState()
        {
            call(&AIInterface::init, m_P1, true);
            call(&AIInterface::init, m_P2, false);
            last_put=std::make_pair(-1,-1);
            return true;
        }

        template<typename Func ,typename... Args,
            std::enable_if_t< std::is_void<
                    std::invoke_result_t<Func, AIInterface, Args...>
                >::value , int> = 0 >
        void call(Func func, AIInterface *ptr, Args... args)
        {
            std::future_status status;
            auto val = std::async(std::launch::async, func, ptr, args...);
            status = val.wait_for(std::chrono::milliseconds(m_runtime_limit));

            if( status != std::future_status::ready )
            {
                exit(-1);
            }
            val.get();
        }

        template<typename Func ,typename... Args,
            std::enable_if_t< std::is_void<
                    std::invoke_result_t<Func, AIInterface, Args...>
                >::value == false, int> = 0 >
        auto call(Func func, AIInterface *ptr, Args... args)
            -> std::invoke_result_t<Func, AIInterface, Args...>
        {
            std::future_status status;
            auto val = std::async(std::launch::async, func, ptr, args...);
            status = val.wait_for(std::chrono::milliseconds(m_runtime_limit));

            if( status != std::future_status::ready )
            {
                exit(-1);
            }
            return val.get();
        }

        void putToGui(const char *fmt, ...)
        {
            va_list args1;
            va_start(args1, fmt);
            va_list args2;
            va_copy(args2, args1);
            std::vector<char> buf(1+std::vsnprintf(nullptr, 0, fmt, args1));
            va_end(args1);
            std::vsnprintf(buf.data(), buf.size(), fmt, args2);
            va_end(args2);

            if( buf.back() == 0 ) buf.pop_back();
            gui->appendText( std::string(buf.begin(), buf.end()) );
        }

        bool checkAI(AIInterface *ptr)
        {
            return ptr->abi() == AI_ABI_VER;//to check the interface same with TA's
        }


        int m_size;
        std::vector<int> m_ship_size;
        std::chrono::milliseconds m_runtime_limit;
        std::pair<int,int> last_put;
        int round;

        AIInterface *m_P1;
        AIInterface *m_P2;
        GUIInterface *gui;

        UltraBoard MainBoard;
    };
}
