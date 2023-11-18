#include <random>
#include <unistd.h>
#include <ncurses.h>

#include "Map.cpp"

class GamePlay{
    std::random_device rd;
    std::mt19937_64 generator;
    GameMap gmap;
    Snake s;
    ScoreBoard score;
    int timer, temp, level, portal_cnt;
    public:
        GamePlay(int level):s(8), score(30, 7, WIDTH+3, 5), generator(rd()), level(level){
            generate_random_map();
            timer = 0;     
            portal_cnt = 0; // s가 조건 만족 후 60 frame 후에 첫 등장;
        }

        void generate_random_map(){
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    if(is_at_point(i, j)) gmap.convert<ImmuneWall>(i,j);
                    else if(i==0 || j==0 || i==HEIGHT-1 || j==WIDTH-1) gmap.convert<Wall>(i,j);
                    else gmap.convert<Box>(i,j);
                }
            }

            int cnt=0;
            while(cnt<level){
                if(generate_wall(generator() % 20, 1+(generator()%(WIDTH-2)), 1+(generator()%(HEIGHT-2)), generator()%2==0)) cnt++;
            }
        }

        void start_message(int level){
            WINDOW* result = newwin(HEIGHT, WIDTH*2, 0, 0);
            wmove(result, HEIGHT/2-3, WIDTH-6);
            wprintw(result, "level %d start\n", level);
            wrefresh(result);
            sleep(1.5);
            delwin(result);
        }

        bool play(){
            int temp;
            start_message(level);
            ScoreBoard::start();
            while(1){
                gmap.show_map();
                score.show(s);
                usleep(s.tick*10000);
                move();
                get_random_item();

                if(win_status_check()) return true;
                if(ScoreBoard::game_over) return false;
            }
        }

        bool generate_wall(int length, int x, int y, bool is_vertical){
            if(!gmap.is_empty(x, y)) return false;
            if(length==0) return true;
            if(is_vertical){
                if(!generate_wall(length-1, x-1, y, is_vertical)) return false;
                gmap.convert<Wall>(x, y);
                return true;
            }
            else{
                if(!generate_wall(length-1, x, y+1, is_vertical)) return false;
                gmap.convert<Wall>(x, y);
                return true;
            }
        }

        bool win_status_check(){
            if((s.length==10) && (s.pos_item_cnt>=5) && (s.neg_item_cnt>=5) && (s.portal_enc_cnt>=1)) return true;
            return false;
        }

        void move(){
            timer++;
            temp = getch();
            s.direction = (temp != -1) ? temp : s.direction;
            switch(s.direction){
                case 'w':
                gmap.map_encounter(--s.pos_x, s.pos_y, s);
                break;
                case 's':
                gmap.map_encounter(++s.pos_x, s.pos_y, s);
                break;
                case 'a':
                gmap.map_encounter(s.pos_x, --s.pos_y, s);
                break;
                case 'd':
                gmap.map_encounter(s.pos_x, ++s.pos_y, s);
                break;
            }
        }

        void get_random_item(){
            if (timer%50==0){
                get_random_item_box();
            }

            if(s.length>=10){
                portal_cnt++;
                if(portal_cnt%60==0) get_portal_box();
            }
        }

        void get_random_item_box(){
            int pos_x = generator()%HEIGHT, pos_y = generator()%WIDTH;
            if(gmap.is_empty(pos_x, pos_y)){ // Box인지
                switch(generator()%4){
                    case 0:
                        gmap.item_insert<Item1>(pos_x, pos_y);
                        break;
                    case 1:
                        gmap.item_insert<Item2>(pos_x, pos_y);
                        break;
                    case 2:
                        gmap.item_insert<Item3>(pos_x, pos_y);
                        break;
                    case 3:
                        gmap.item_insert<Item4>(pos_x, pos_y);
                        break;
                }
            }
            else{
                get_random_item_box();
            }
        }

        bool is_at_point(int x, int y){
            if(x==0&&(y==0||y==WIDTH-1)) return true;
            if(x==HEIGHT-1&&(y==0||y==WIDTH-1)) return true;
            return false;
        }

        void get_portal_box(){
            portal_cnt=0;
            int pos_x = generator()%HEIGHT, pos_y = generator()%WIDTH, pos_ox = generator()%HEIGHT, pos_oy = generator()%WIDTH;
            if(gmap.is_wall(pos_x, pos_y) && gmap.is_wall(pos_ox, pos_oy) && !((pos_x==pos_ox)&&(pos_y==pos_oy))){
                gmap.portal_insert(pos_x, pos_y, pos_ox, pos_oy);
            }
            else{
                get_portal_box();
            }
        }
};

static void game_result(bool win){
    WINDOW* result = newwin(HEIGHT, WIDTH*2, 0, 0);
    wmove(result, HEIGHT/2-3, WIDTH-6);
    if(win) wprintw(result, "Game WIN\n");
    else wprintw(result, "Game OVER\n");
    wrefresh(result);
    sleep(1.5);
    delwin(result);
}

int ScoreBoard::game_over = 0;
int main(){
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    game_result(GamePlay(1).play());
    game_result(GamePlay(1).play());
    game_result(GamePlay(2).play());
    game_result(GamePlay(3).play());
    game_result(GamePlay(4).play());
    game_result(GamePlay(5).play());
    endwin();
}