#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
float pi = 3.14159256;
double t_vec, *x_vec, * y_vec, * x_norm_vec, * sbj_dis_vec;

//必要なメモリを確保
void init_frame_sound_maker_allpass(long frame_size) {
    x_vec = (double*)calloc((size_t)frame_size, sizeof(double));
    y_vec = (double*)calloc((size_t)frame_size, sizeof(double));
    x_norm_vec = (double*)calloc((size_t)frame_size, sizeof(double));
    sbj_dis_vec = (double*)calloc((size_t)frame_size, sizeof(double));
}

//必要なくなったメモリを解放
void close_frame_sound_maker_allpass() {
    free(x_vec); free(y_vec); free(x_norm_vec); free(sbj_dis_vec);
}

void signal_spring(double* ys, short* s, double dly_crnt, double dly_prv, int time_crnt, int frame_size) {
    double tau = dly_crnt - dly_prv;
    double tau_sample = tau / frame_size;
    int g[2]{};
    if (tau < 0) { g[0] = 1; g[1] = 0; }
    else { g[0] = 0; g[1] = 1; }
    double tau_tmp, delay_net, alp, s_tau;
    //ys[0] = s[(int)(time_crnt - dly_prv-1)];
    ys[0] = s[(int)(time_crnt - dly_prv)];
    for (int n = 1; n < frame_size; n++) {
        //tau_tmp = tau_sample * (n - 1);
        tau_tmp = tau_sample * n;
        delay_net = floor(tau_tmp);
        alp = g[1] - (tau_tmp - delay_net);
        //s_tau = time_crnt - dly_prv + n - 1 - delay_net;
        s_tau = time_crnt - dly_prv + n - delay_net;
        //ys[n] = -alp * ys[n - 1] + alp * (double)s[(int)s_tau + g[0]-1] + (double)s[(int)s_tau - g[1]-1];
        ys[n] = -alp * ys[n - 1] + alp * (double)s[(int)s_tau + g[0]] + (double)s[(int)s_tau - g[1]];
    }
}

void gain_change(float* gain_vec, double g0, double* xs_vec, double x_prv_vec, double y_prv_vec, double sp_pos_y, double sp_pos_x, int frame_size) {
    double sbj_pos[] = { 0,2.2 };

    int is_min = 0;
    double gc;
    double gcs;
    for (int i = 0; i < frame_size; i++) {
        t_vec = (i + 1) / frame_size;//t_vec = (1:frame_size).'/frame_size;
        x_vec[i] = x_prv_vec + (xs_vec[0] - x_prv_vec) * t_vec;
        y_vec[i] = y_prv_vec + (xs_vec[1] - y_prv_vec) * t_vec;
        //x_norm_vec[i] = sqrt(x_vec * x_vec + y_vec[i] * y_vec[i]);
        x_norm_vec[i] = sqrt((sp_pos_x - x_vec[i]) * (sp_pos_x - x_vec[i]) + (sp_pos_y - y_vec[i]) * (sp_pos_y - y_vec[i]));
        sbj_dis_vec[i] = sqrt((sbj_pos[0] - x_vec[i]) * (sbj_pos[0] - x_vec[i]) + (sbj_pos[1] - y_vec[i]) * (sbj_pos[1] - y_vec[i]));
        if (sbj_dis_vec[i] < 0.15) { is_min = 1; }
    }
    for (int i = 0; i < frame_size; i++) {
        if (is_min) { sbj_dis_vec[i] += 0.15; }
        gcs = 1;
        /*if (-0.5 <= y_vec[i] && y_vec[i] < 0.5) {
        gc = 2.4 * exp(-16 * y_vec[i]) + 1;
        xs = sp_pos_x - x_vec[i];
        if (fabs(xs) < 0.1) {
        gcs = (gc - 1) * (1 - fabs(xs) / 0.1) + 1;
        }
        }*/
        gain_vec[i] = gcs * ( - g0 * (1 / sqrt(2 * pi)) * ((sp_pos_y - y_vec[i]) / (x_norm_vec[i] * sqrt(x_norm_vec[i]))) / sqrt(sbj_dis_vec[i]));//卒論の式(3.8)
    }
}