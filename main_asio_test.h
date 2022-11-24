#pragma once
#include "pos_struct.h"

#define MAX_BODIES 5
//ここで利用する最大チャネル数を定義する％％
//なお，使うA/D,D/Aの名称は，asio_init.hで手で書き直してコンパイルし直す必要がある
//#define MAX_Channel_NUM 8
#define MAX_Channel_NUM 32
extern int frame_size_max;
extern short* prev_out_data[MAX_Channel_NUM];
body_pos get_crnt_body_pos();