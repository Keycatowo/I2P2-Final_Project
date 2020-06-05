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

        bool full() const {
            // check UltraBoard is filled all Board or not
            for (int i=0;i<3;++i)
                for (int j=0;j<3;++j)
                    if (!b[i][j].full())
                        return false;
            return true;
        }

        Tag judgeWinState() override{
            // alrady have the result
            if(getWinTag()!=Tag::None)
                return getWinTag();
            // row and col
            for(int i=0;i<9;i++)
                b[i/3][i%3].judgeWinState();

            for(int i=0; i<3; ++i){
                if(b[0][i].getWinTag()==Tag::O &&b[1][i].getWinTag()==Tag::O &&b[2][i].getWinTag()==Tag::O)
                    return Tag::O;
                if(b[0][i].getWinTag()==Tag::X &&b[1][i].getWinTag()==Tag::X &&b[2][i].getWinTag()==Tag::X)
                    return Tag::X;
                if(b[i][0].getWinTag()==Tag::O &&b[i][1].getWinTag()==Tag::O &&b[i][2].getWinTag()==Tag::O)
                    return Tag::O;
                if(b[i][0].getWinTag()==Tag::X &&b[i][1].getWinTag()==Tag::X &&b[i][2].getWinTag()==Tag::X)
                    return Tag::X;
            }

            // slash
            if(b[0][0].getWinTag()==Tag::O &&b[1][1].getWinTag()==Tag::O &&b[2][2].getWinTag()==Tag::O)
                return Tag::O;
            if(b[0][0].getWinTag()==Tag::X &&b[1][1].getWinTag()==Tag::X &&b[2][2].getWinTag()==Tag::X)
                return Tag::X;
            if(b[0][2].getWinTag()==Tag::O &&b[1][1].getWinTag()==Tag::O &&b[2][0].getWinTag()==Tag::O)
                return Tag::O;
            if(b[0][2].getWinTag()==Tag::X &&b[1][1].getWinTag()==Tag::X &&b[2][0].getWinTag()==Tag::X)
                return Tag::X;

            if(full()){
                return Tag::Tie;
            }
            else{
                return Tag::None;
            }
        }

    private:
        Board b[3][3];  // 9 Boards
    };
} // Namespace TA
