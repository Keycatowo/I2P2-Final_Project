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
            int round = 0;
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

            
            while (!checkGameover()) {
                round++;
                AIInterface *first = m_P1;
                AIInterface *second = m_P2;
                BoardInterface::Tag tag = BoardInterface::Tag::O;

                if (!playOneRound(first, tag, second)) {
                    break;
                }
                updateGuiGame();
            }
            
           




            /* print the result of each Borad */
            for (int i=0; i<3; ++i){
                for (int j=0; j<3; ++j){
                    if(MainBoard.sub(i,j).judgeWinState() == BoardInterface::Tag::O)
                        putToGui("O");
                    else if(MainBoard.sub(i,j).judgeWinState() == BoardInterface::Tag::X)
                        putToGui("X");
                    else
                        putToGui("*");
                }
                putToGui("\n");
            }

            /* print the result of winner */
            if(MainBoard.judgeWinState() == BoardInterface::Tag::O)
                putToGui("O wins\n");
            else if(MainBoard.judgeWinState() == BoardInterface::Tag::X)
                putToGui("X wins\n");
            else if(MainBoard.judgeWinState() == BoardInterface::Tag::Tie)
                putToGui("Tie\n");
            else
                putToGui("None\n");
            updateGuiGame();


        }

   private:
        void updateGuiGame()
        {
            gui->updateGame(MainBoard);
        }

        bool playOneRound(AIInterface *user, BoardInterface::Tag tag, AIInterface *enemy)
        {

            auto pos = call(&AIInterface::queryWhereToPut, user, MainBoard);
            if(pos.first==-1)
                return false;
            MainBoard.get(pos.first, pos.second) = tag;
            enemy->callbackReportEnemy(pos.first,pos.second);
            
            pos = call(&AIInterface::queryWhereToPut, enemy, MainBoard);
            if(pos.first==-1)
                return false;
            MainBoard.get(pos.first, pos.second) = (tag == Board::Tag::O)? Board::Tag::X : Board::Tag::O;
            user->callbackReportEnemy(pos.first,pos.second);
            return true;
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

        AIInterface *m_P1;
        AIInterface *m_P2;
        GUIInterface *gui;

        UltraBoard MainBoard;
    };
}
