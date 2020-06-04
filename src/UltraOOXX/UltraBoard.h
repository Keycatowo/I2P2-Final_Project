#pragma once

#include <UltraOOXX/Board.h>
#include <iostream>
namespace TA {
    class UltraBoard : public BoardInterface {
    public:
        UltraBoard() {
            // initial the UltraBoard to empty
            reset();
        }

        void reset() {
            //for each board(3*3), reset it
            for (int i=0;i<3;++i)
                for (int j=0;j<3;++j)
                    b[i][j].reset();
        }

        Board::Tag& get(int x, int y) {
            // for UltraBoard pos (x,y) from 0 to 8
            // get where the Board
            // get the pos where the x,y really is
            return b[x/3][y/3].get(x%3, y%3);
        }

        Board& sub(int x, int y) {
            // get one Board by pos (x,y)
            return b[x][y];
        }

        Tag state(int x, int y) const override {
            // check one Board the win state
            return b[x][y].getWinTag();
        };

    private:
        Board b[3][3];  // 9 Boards
    };
} // Namespace TA
