#include <ncurses.h>
#include <unistd.h>
#include <deque>
using namespace std;

#include "Box.cpp"

class GameMap{
    private:
        Box* gmap[HEIGHT][WIDTH];
        deque<pair<int, int>> body_history;
        deque<pair<int, int>> item_history;
        deque<pair<int, int>> portal_history;

    public:
        GameMap(){
            start_color();
            init_pair(1, COLOR_BLACK, COLOR_BLACK);
            init_pair(2, COLOR_WHITE, COLOR_WHITE);
            init_pair(3, COLOR_GREEN, COLOR_GREEN);
            init_pair(4, COLOR_RED, COLOR_RED);
            init_pair(5, COLOR_BLUE, COLOR_BLUE);
            init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA);
            init_pair(7, COLOR_RED, COLOR_BLUE);
            init_pair(8, COLOR_BLUE, COLOR_RED);
            init_pair(9, COLOR_YELLOW, COLOR_YELLOW);
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    gmap[i][j] = new Box(j, i);
                }
            }
        }

        ~GameMap(){
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    delete gmap[i][j];
                }
            }
        }

        template <typename T>
        void convert(int x, int y){
            if(!(x>=HEIGHT || x<0 || y>=WIDTH || y<0))
                gmap[x][y] = new T((Box *)gmap[x][y]);
        }

        void convert_to_portal(int x, int y, int ox, int oy){
            gmap[x][y] = new Portal((Box *)gmap[x][y], ox, oy, is_empty(ox-1, oy), is_empty(ox, oy+1), is_empty(ox-1, oy), is_empty(ox, oy-1));
            gmap[ox][oy] = new Portal((Box *)gmap[ox][oy], x, y, is_empty(x-1, y), is_empty(x, y+1), is_empty(x-1, y), is_empty(x, y-1));
        }

        bool is_empty(int x, int y){
            if(x>=HEIGHT || x<0 || y>=WIDTH || y<0) return false;
            return gmap[x][y]->is_empty();
        }

        bool is_wall(int x, int y){ //empty false, convertable false -> wall
            if(x>=HEIGHT || x<0 || y>=WIDTH || y<0) return false;
            return gmap[x][y]->is_convertable(false); // portal로 convertable한지
        }

        void map_encounter(int x, int y, Snake &s){
            gmap[x][y]->encounter(s);
            if(gmap[x][y]->is_convertable(true)) convert<SnakeBody>(x, y); // SnakeBody로 convertable한지
            body_history.push_back(make_pair(x, y));
            while(body_history.size() > s.length){
                auto item = body_history.front();
                if(gmap[item.first][item.second] -> is_convertable(true)) convert<Box>(item.first,item.second);
                else{
                    convert<Wall>(gmap[item.first][item.second]->opposite_x, gmap[item.first][item.second]->opposite_y);
                    convert<Wall>(item.first,item.second); // Portal의 경우
                }
                body_history.pop_front();
            } 
        }

        template <typename T>
        void item_insert(int x, int y){
            convert<T>(x, y);
            item_history.push_back(make_pair(x, y));
            if(item_history.size()==4){
                auto item = item_history.front();
                convert<Box>(item.first, item.second);
                item_history.pop_front();
            }
        }

        void portal_insert(int x, int y, int ox, int oy){
            convert_to_portal(x, y, ox, oy);
            convert_to_portal(ox, oy, x, y);
            portal_history.push_back(make_pair(x, y));
            portal_history.push_back(make_pair(ox, oy));

            while(portal_history.size()>=3){
                auto item = portal_history.front();
                portal_history.pop_front();
                convert<Wall>(item.first, item.second);
            }
        }

        void show_map(){
            for(int i=0;i<HEIGHT;i++){
                for(int j=0;j<WIDTH;j++){
                    gmap[i][j]->show();
                }
            }
        }
};