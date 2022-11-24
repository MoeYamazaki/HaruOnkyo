#pragma once
extern float pi;
void init_frame_sound_maker_allpass(long frame_size);
void close_frame_sound_maker_allpass();
void signal_spring(double* ys, short* s, double dly_crnt, double dly_prv, int time_crnt, int frame_size);
void gain_change(float* gain_vec, double g0, double* xs_vec, double x_prv_vec, double y_prv_vec, double sp_pos_y, double sp_pos_x, int frame_size);