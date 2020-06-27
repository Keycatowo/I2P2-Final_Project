#pragma once

#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include <UltraOOXX/UltraBoard.h>

namespace TA
{
    class GUIInterface
    {
    public:
        virtual void title() = 0;
        virtual void appendText(std::string str, int round) = 0;
        virtual void updateGame(UltraBoard b, int round) = 0;
    };

    using std::printf;
#define ESC "\033"   // For ANSI Escape Sequence
    class ASCII : public GUIInterface
    {
        const static int GRAPH_HIGHT = 7 + 15;
        const static int GRAPH_WIDTH = 80;

        const static int TEXT_HIGHT = 10;
        const static int TEXT_WIDTH = 80;

        std::string m_preparedText;
        std::string m_textbuf;

        // reset the screen
        void cls()
        {
            // ESC[H for moving cursor to home position(0, 0)
            // ESC[J for clearing the screen
            printf(ESC "[H" ESC "[J");
        }

        // move to (x,y)
        void gotoxy(int y, int x)
        {
            // ESC[{line};{column}f for moving cursor to line #, column #
            printf(ESC "\033[%d;%df", y, x);
        }

        // set m_preparedText and m_textbuf
        void updateTextBuf()
        {
            std::stringstream ss(m_textbuf);
            const std::string ban(TEXT_WIDTH, '-');  //string concatenation
            std::string tmp, last;

            m_preparedText.clear();
            // cut the string into the size that fit the screen width
            for (int L = 0; L < TEXT_HIGHT - 2; ++L)
            {
                if (last == "")
                    // Extracts characters from ss 
                    // and stores them into last 
                    // until the newline character is found.
                    getline(ss, last);

                // string::substr returns a newly constructed string object 
                // with its value initialized to a copy of a substring of this object.
                tmp = last.substr(0, std::min((size_t)TEXT_WIDTH, last.size()));

                if (tmp.size() == last.size())
                    // if string last is completely copied to tmp
                    // then clear the string
                    last = "";
                else
                    // else store the rest string in last
                    last = last.substr(TEXT_WIDTH);
                // m_preparedText is the string that cut into the size
                // that fit the screen width by '\n'
                m_preparedText = tmp + "\n" + m_preparedText;
            }
            // m_textbuf is the string that can fill in all screen
            m_textbuf = m_textbuf.substr(0, TEXT_HIGHT * TEXT_WIDTH);

            m_preparedText = ban + "\n" + m_preparedText + ban;
        }

        void showText(int round)
        {
            //gotoxy(GRAPH_HIGHT + 1, 0);             // move cursor
            //printf(ESC "[J");                   // clean the screen
            gotoxy(GRAPH_HIGHT*(round) + TEXT_HIGHT*(round-1), 0);             // move cursor
            puts(m_preparedText.c_str());         // inserts character c into the stream.
            gotoxy(GRAPH_HIGHT*round + TEXT_HIGHT*round + 1, 0);  // move cursor
            std::fflush(stdout);                  // clean the string in buff by printing
        }

    public:
        // put the pic-string in to buff
        virtual void title() override
        {
            cls();
            puts(
                R"( _   _ _ _             _____  _______   ____   __
| | | | | |           |  _  ||  _  \ \ / /\ \ / /
| | | | | |_ _ __ __ _| | | || | | |\ V /  \ V /
| | | | | __| '__/ _` | | | || | | |/   \  /   \
| |_| | | |_| | | (_| \ \_/ /\ \_/ / /^\ \/ /^\ \
 \___/|_|\__|_|  \__,_|\___/  \___/\/   \/\/   \/
)");
        }

        // by up dating m_textbuf
        virtual void appendText(std::string str, int round)
        {
            m_textbuf = str + m_textbuf;
            updateTextBuf();
            showText(round);
        }

        // print the tag on the board
        int toPrintChar(BoardInterface::Tag t) {
            switch (t) {
            case BoardInterface::Tag::O: return 'O';
            case BoardInterface::Tag::X: return 'X';
            default:
                return ' ';
            }
        }

        // print the board
        virtual void updateGame(UltraBoard b, int round)
        {
            int x;
            if (round == 0) x = 7 + 1;
            else x = GRAPH_HIGHT *round + TEXT_HIGHT *round + 2;
            gotoxy(x, 0);
            const std::string buf(20, ' ');

            for (int i = 0; i < 9; ++i)
            {
                std::printf("%s", buf.c_str());
                //print one row board
                for (int j = 0; j < 9; ++j)
                {
                    std::putchar(toPrintChar(b.get(i, j)));
                    if (j == 2 || j == 5) std::putchar('|');
                }
                std::putchar('\n');
                if (i == 2 || i == 5) {
                    std::printf("%s", buf.c_str());
                    std::puts(std::string(12, '-').c_str());
                }
            }

            gotoxy(0, 0);
        }
    };
#undef ESC
}
