#pragma once

namespace TA {
    class BoardInterface {
    public:
        enum class Tag {
            /// Tag is the data type for O or X
            // can be used in wintag or
            None,
            O,
            X,
            Tie
        };

        // pure virtual function
        // get the state for :
        // Derived in UltraBoard: for whole game who win
        // Derived in Board : for one board who win in local
        virtual Tag state(int x, int y) const = 0;

        Tag getWinTag() const {return wintag;}
        void setWinTag(Tag t) {wintag = t;}
    private:
        Tag wintag;// wintag is the state for who win the game or not yet or tie
    };




    class Board : public BoardInterface {
    public:
        Board() {
            reset();
        }

        void reset() {
            // reset the wintag of Board into None
            // reset 9 pos in one Board into None
            setWinTag(BoardInterface::Tag::None);
            for (int i=0;i<3;++i)
                for (int j=0;j<3;++j)
                    b[i][j] = Tag::None;
        }

        bool full() const {
            // check one Board is filled with OX or not
            for (int i=0;i<3;++i)
                for (int j=0;j<3;++j)
                    if (b[i][j] == Tag::None)
                        return false;
            return true;
        }

        Tag state(int x, int y) const override {
            // read the state by x,y
            return b[x][y];
        };

        Tag& get(int x, int y) {
            return b[x][y];
        }

    private:
        Tag b[3][3];
    };
} // Namespace TA
