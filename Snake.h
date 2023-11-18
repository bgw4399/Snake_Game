
class Snake{
    public:
        int length, direction, pos_x, pos_y, pos_item_cnt, neg_item_cnt, portal_enc_cnt;
        float tick;
        Snake(int length=3):length(length), direction('d'), pos_x(7), pos_y(7), tick(0.8){
            pos_item_cnt=0; neg_item_cnt=0; portal_enc_cnt=0;
        };
};