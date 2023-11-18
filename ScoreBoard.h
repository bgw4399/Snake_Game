#include <ncurses.h>
#include "Snake.h"

class ScoreBoard{
    WINDOW *local_win;
    public:
        ScoreBoard(int sizex, int sizey, int x, int y){
            local_win = newwin(sizey, sizex, y*1, x*2);
        }
        
        void show(const Snake& s){
            werase(local_win);
            wprintw(local_win, "Score Board\n");
            wprintw(local_win, "--------------\n");
            wprintw(local_win, "B: %d / 10 (%s)\n", s.length, (s.length==10)?"v":" ");
            wprintw(local_win, "+: %d (%s)\n", s.pos_item_cnt, (s.pos_item_cnt>=5)?"v":" ");
            wprintw(local_win, "-: %d (%s)\n", s.neg_item_cnt, (s.neg_item_cnt>=5)?"v":" ");
            wprintw(local_win, "G: %d (%s)\n", s.portal_enc_cnt, (s.portal_enc_cnt>=1)?"v":" ");
            wprintw(local_win, "Speed: %.2f\n", s.tick*10);
            wrefresh(local_win);	
        }

        ~ScoreBoard(){
            delwin(local_win);
        }
        static int game_over;
        static void lose(){
            game_over = 1;
        }
        static void start(){
            game_over = 0;
        }
};