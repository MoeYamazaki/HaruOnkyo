#pragma once
#include "pos_struct.h"

#define MAX_BODIES 5
//�����ŗ��p����ő�`���l�������`���道��
//�Ȃ��C�g��A/D,D/A�̖��̂́Casio_init.h�Ŏ�ŏ��������ăR���p�C���������K�v������
//#define MAX_Channel_NUM 8
#define MAX_Channel_NUM 32
extern int frame_size_max;
extern short* prev_out_data[MAX_Channel_NUM];
body_pos get_crnt_body_pos();