#include <ncurses.h>
#include "ScoreBoard.h"

#define HEIGHT 30
#define WIDTH 50

class Box{
    protected:
        WINDOW *local_win;
    public:
        int opposite_x, opposite_y;
        Box(int x, int y, int sizex=2, int sizey=1, int gabx=2, int gaby=1){
            local_win = newwin(sizey, sizex, y*gaby, x*gabx);
        }

        Box(Box* box):local_win(box->local_win){
            delete box;
        }

        virtual void encounter(Snake& s){
            ;
        }

        virtual void show(){
            wbkgd(local_win, COLOR_PAIR(1));
            wrefresh(local_win);	
        }

        void delete_window(){
            delwin(local_win);
        }

        virtual bool is_empty(){
            return true;
        }

        virtual bool is_convertable(bool to_snake){
            if(to_snake) return true; //snake로 변환가능한지
            else return false; // portal로 변환가능한지
        }
};

class Item1:public Box{
    public:
        Item1(Box* box):Box(box){
            ;
        };
        void encounter(Snake& s){
            if(s.length<10) s.length++;
            s.pos_item_cnt++;
        }
        
        void show(){
            wbkgd(local_win, COLOR_PAIR(5));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return true;
            else return false;
        }
};

class Item2:public Box{
    public:
        Item2(Box* box):Box(box){
            ;
        };
        void encounter(Snake& s){
            s.length--;
            s.neg_item_cnt++;
            if(s.length<3){
                ScoreBoard::lose();
            }
        }

        void show(){
            wbkgd(local_win, COLOR_PAIR(4));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return true;
            else return false;
        }
};

class Item3:public Box{
    public:
        Item3(Box* box):Box(box){
            ;
        };
        void encounter(Snake& s){
            s.tick *= 5;
            s.pos_item_cnt++;
        }

        void show(){
            box(local_win,0,0);
            wbkgd(local_win, COLOR_PAIR(7));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return true;
            else return false;
        }
};

class Item4:public Box{
    public:
        Item4(Box* box):Box(box){
            ;
        };
        void encounter(Snake& s){
            s.tick /= 5;
            s.neg_item_cnt++;
        }

        void show(){
            box(local_win,0,0);
            wbkgd(local_win, COLOR_PAIR(8));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return true;
            else return false;
        }
};

class SnakeBody:public Box{
    public:
        SnakeBody(Box* box):Box(box){
            ;
        };
        void encounter(Snake& s){
            ScoreBoard::lose();
        }

        void show(){
            wbkgd(local_win, COLOR_PAIR(3));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return true;
            else return false;
        }
};

class Wall:public Box{
    public:
        Wall(Box* box):Box(box){
            ;
        };
        void encounter(Snake& s){
            ScoreBoard::lose();
        }

        void show(){
            wbkgd(local_win, COLOR_PAIR(2));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return false;
            else return true;
        }
};

class ImmuneWall:public Box{
    public:
        ImmuneWall(Box* box):Box(box){
            ;
        };
        void encounter(Snake& s){
            ScoreBoard::lose();
        }

        void show(){
            wbkgd(local_win, COLOR_PAIR(9));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return false;
            else return false;
        }
};

class Portal:public Box{
    public:
        bool empty_direction[4];
        Portal(Box* box, int ox, int oy, bool w_em, bool d_em, bool s_em, bool a_em):Box(box){
            opposite_x = ox;
            opposite_y = oy;
            empty_direction[0] = w_em; // 상대 출구 기준, w, d, s, a 방향으로 나올수 있는지
            empty_direction[1] = d_em;
            empty_direction[2] = s_em;
            empty_direction[3] = a_em;
        };
        void encounter(Snake& s){
            s.portal_enc_cnt++;
            s.pos_x = opposite_x;
            s.pos_y = opposite_y;
            s.direction = calculate_direction(s.pos_x, s.pos_y, s.direction);
        }

        int calculate_direction(int x, int y, int cur_dir){
            if(x==0) return 's';
            else if(x==HEIGHT-1) return 'w';
            else if(y==0) return 'd';
            else if(y==WIDTH-1) return 'a';
            else{
                switch(cur_dir){
                    case 'w':
                    if(empty_direction[0]) return 'w'; 
                    else if(empty_direction[1]) return 'd';
                    else if(empty_direction[3]) return 'a';
                    else if(empty_direction[2]) return 's';
                    else return -1;
                    case 'd':
                    if(empty_direction[1]) return 'd' ;
                    else if(empty_direction[2]) return 's';
                    else if(empty_direction[0]) return 'w'; 
                    else if(empty_direction[3]) return 'a';
                    else return -1;
                    case 's':
                    if(empty_direction[2]) return 's';
                    else if(empty_direction[3]) return 'a';
                    else if(empty_direction[1]) return 'd';
                    else if(empty_direction[0]) return 'w'; 
                    else return -1;
                    case 'a':
                    if(empty_direction[3]) return 'a';
                    else if(empty_direction[0]) return 'w'; 
                    else if(empty_direction[2]) return 's';
                    else if(empty_direction[1]) return 'd' ;
                    else return -1;
                    default:
                    return -1;
                }   
            }
        }

        void show(){
            wbkgd(local_win, COLOR_PAIR(6));
            wrefresh(local_win);
        }

        bool is_empty(){
            return false;
        }

        bool is_convertable(bool to_snake){
            if(to_snake) return false;
            else return false;
        }
};