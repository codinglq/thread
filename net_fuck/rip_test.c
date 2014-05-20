#include "net_def.h"
/*
*测试小程序
* */
int main()
{

    MY_ROUT_TABLE rou_1;
    MY_ROUT_TABLE rou_2;

    init_rout_table(&rou_1,6);
    init_rout_table(&rou_2,4);
////////////
    //create_rout_table(&rou_1);
    rou_1.rout_table[0][0]=2;
    rou_1.rout_table[0][1]=3;
    rou_1.rout_table[0][2]=4;
    
    rou_1.rout_table[1][0]=3;
    rou_1.rout_table[1][1]=4;
    rou_1.rout_table[1][2]=5;

    rou_1.curr_size=2;
//............
    /////////////
    rou_2.rout_table[0][0]=1;
    rou_2.rout_table[0][1]=3;
    rou_2.rout_table[0][2]=1;

    rou_2.rout_table[1][0]=2;
    rou_2.rout_table[1][1]=4;
    rou_2.rout_table[1][2]=2;
    
    rou_2.rout_table[2][0]=3;
    rou_2.rout_table[2][1]=1;
    rou_2.rout_table[2][2]=0;

    rou_2.rout_table[3][0]=8;
    rou_2.rout_table[3][1]=10;
    rou_2.rout_table[3][2]=5;
    
    rou_2.curr_size=4;
//............
    display_rout_table(&rou_1);
    display_rout_table(&rou_2);
//开始rip  我这个路由表没设置互斥量啊
    start_my_rip(&rou_2,&rou_1);

    display_rout_table(&rou_1);
    display_rout_table(&rou_2);
    return 0;
}
