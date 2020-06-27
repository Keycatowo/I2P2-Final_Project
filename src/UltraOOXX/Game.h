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
#include <unistd.h>

#include "GUI/keyboard.h"

#define MAX_MODE 3
#define TEXT_SIZE 20

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
            mode = select_mode();
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
           switch (mode)
           {
            case 0:
               auto_play();
               break;
            case 1:
                step_play();
                break;
            default:
               auto_play();
           }
            
            
        }

   private:
        void clear_text(int clear_size = TEXT_SIZE)
        {
            for(int i(0);i<clear_size;i++)
                putToGui("\n");
        }

        void show_menu(int mode_pos = 0)
        {
            clear_text();
            putToGui("Select the mode\n"); 
            switch (mode_pos)
            {
            case 0:
                putToGui("\t\t>[auto play]\n");
                putToGui("\t\t [step play]\n");
                putToGui("\t\t [person play]\n");
                putToGui("\t\t [Authors]\n");
                break;
            case 1:
                putToGui("\t\t [auto play]\n");
                putToGui("\t\t>[step play]\n");
                putToGui("\t\t [person play]\n");
                putToGui("\t\t [Authors]\n");
                break;
            case 2:
                putToGui("\t\t [auto play]\n");
                putToGui("\t\t [step play]\n");
                putToGui("\t\t>[person play]\n");
                putToGui("\t\t [Authors]\n");
                break;
            case 3:
                putToGui("\t\t [auto play]\n");
                putToGui("\t\t [step play]\n");
                putToGui("\t\t [person play]\n");
                putToGui("\t\t>[Authors]\n");
                break;
            
            default:
                break;
            }
            clear_text(3);
        }

        int select_mode()
        {
            int menu_pos = 0;
            int d_pos = 0;
            while(1){
                show_menu(menu_pos);
                d_pos = get_arrow();
                switch (d_pos)
                {
                // up
                case 1:
                    menu_pos = menu_pos>0?menu_pos-1:0;
                    break;
                // down
                case 2:
                    menu_pos = menu_pos<MAX_MODE?menu_pos+1:MAX_MODE;
                    break;
                // enter
                case 5:
                    clear_text();
                    return menu_pos;
                    break;
                default:
                    continue;
                }
            }

        }

        void auto_play()
        {
            //下棋
            while (!checkGameover()) 
            {
                /* print the result of each Borad */
                //printBoard();
                usleep(200000);
                updateGuiGame();
                
                round++;
                if(round%2==0)
                {
                    if (playOneRound(m_P2, BoardInterface::Tag::O, m_P1)) 
                        continue;
                }
                else
                {
                    if (playOneRound(m_P1, BoardInterface::Tag::X, m_P2)) 
                        continue;
                }
                //if (!playOneRound(now_player, tag, next_player)) break;

                /* print the result of each Borad */
                //printBoard();
            }
            printValid();
            /* print the result of winner */
            printBoard();
            
            printWinner();
        }

        void step_play()
        {
            //下棋
            while (!checkGameover()) 
            {
                /* print the result of each Borad */
                //printBoard();
                std::string line;
                std::getline(std::cin,line);
                updateGuiGame();
                printValid();
                printBoard();
                round++;
                if(round%2==0)
                {
                    if (playOneRound(m_P2, BoardInterface::Tag::O, m_P1)) 
                        continue;
                }
                else
                {
                    if (playOneRound(m_P1, BoardInterface::Tag::X, m_P2)) 
                        continue;
                }
                //if (!playOneRound(now_player, tag, next_player)) break;

                /* print the result of each Borad */
                //printBoard();
                
            }
            printValid();
            /* print the result of winner */
            printBoard();
            
            printWinner();
        }


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
        void printValid()
        {
          
            for (int i = 0; i < 9; ++i)
            {
                for (int j = 0; j < 9; ++j)
                {               
                    std::pair<int, int> pos = std::make_pair(i, j);
                    if(checkValid(pos) == true)  putToGui(" ");
                    else putToGui("*");
                    
                    if (j == 2 || j == 5) putToGui("|");
                }
                putToGui("\n                    ");
                if (i == 2 || i == 5) putToGui("\n                    -------------------");
            }
            putToGui("\n");
        }

        void printWinner()
        {
            if(MainBoard.judgeWinState() == BoardInterface::Tag::O)
                putToGui("Ｏ wins\n");
            else if(MainBoard.judgeWinState() == BoardInterface::Tag::X)
                putToGui("Ｘ wins\n");
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
            return true;
        }

        bool checkValid( std::pair<int,int> pos)
        {
            if(pos.first<0 || pos.first >8 || pos.second <0 || pos.second >8)
                return false;
            //檢查是否下在非法的位置上
            //如果位置非法，則判定輸掉比賽
            //先檢查last_pos下在board的哪裡，然後再檢查pos下在ultra_board的位置
            if(TA::UltraOOXX::MainBoard.get(pos.first,pos.second)==BoardInterface::Tag::None)
            { 
                if(last_put.first == -1) return true;
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
        int mode;

        AIInterface *m_P1;
        AIInterface *m_P2;
        GUIInterface *gui;

        UltraBoard MainBoard;
    };
}
