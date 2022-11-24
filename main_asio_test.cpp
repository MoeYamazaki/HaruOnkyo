/******************************************************************************\
* ASIO sample program by Y. Haneda
*               *
\******************************************************************************/

//#undef __cplusplus

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <k4a/k4a.h>
#include <k4abt.h>

#include <string.h>
#include <math.h>
#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"
#include "asio_init.h"
#include "main_asio_test.h"
#include "kinect_open.h"
#include "main_openGL_source.h"
#include "frame_sound_allpass.h"
#include "get_xy_pos.h"


/******* FOR ASIO *********/
// �������牺�͉������o�͂̂��߂�ASIO�̐錾�B�������C���͕s�v���Ǝv���B
DriverInfo asioDriverInfo = { 0 };
extern ASIOCallbacks asioCallbacks;

//----------------------------------------------------------------------------------
// some external references
extern AsioDrivers* asioDrivers;
bool loadAsioDriver(char* name);

// internal prototypes (required for the Metrowerks CodeWarrior compiler)
int main(int argc, char* argv[]);

// callback prototypes
void bufferSwitch(long index, ASIOBool processNow);
ASIOTime* bufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool processNow);

// Test sound 
// �������牺�́C�C�����K�v�ȉӏ�
// ���̓f�o�b�O�p�ɗp�ӂ��Ă��邪�C�K�v�ɉ����ďC������K�v������B
// out_data ���ꂪ�`���l�������̏o�̓o�b�t�@�C8ch���̃|�C���^�Ȃ̂ł��Ƃő傫����錾����K�v������B
short* out_data[MAX_Channel_NUM];
short* prev_out_data[MAX_Channel_NUM];
short* in_data[4]; // �g���ĂȂ��H
long t_time;       //�@�g���ĂȂ��H
//float pi = 3.14159256;
float samp_freq = SAMPLE_RATE;
int data_second = 20; //�@�ǂݍ��񂾉����t�@�C���̑�̂̕b��
float xp = 0, yp = 0, zp = 0; // ��̈ʒu�@���C�������E�Ɖ��s�C���͍���
int count_time = 0;
int count_max = 8192; // �f�o�b�O�p�ɒl��\������^�C�~���O�𐧌䂵�Ă���C
int frame_size_max = 2048; // ���ۂ̃t���[���T�C�Y�ł͂Ȃ��B�t���[���T�C�Y�̓n�[�h�E�F�A�̐ݒ��ʂŎ��O��1024�Ƃ�512�Ƃ��ɐݒ肵�Ă���
long current_sample; // ���̃t���[���̐擪�T���v������
float old_yp;
float LS_x[MAX_Channel_NUM]; // �X�s�[�J�̈ʒu�i���j
float LS_y[MAX_Channel_NUM]; // �X�s�[�J�̈ʒu�i���j

short* raw_data; // 
int data_length; //
//�@�����f�[�^�̓ǂݍ��݁@�b����ł�wav�͓ǂ߂Ȃ��̂ŁC���O�ɂl�`�s�k�`�a��
// �����ɋ��ʂȃt�B���^����ݍ��񂾌��writebin(file_name,32768*x,'short')�ŕۑ����Ă���
// 32768�������Ă���ۑ����Ȃ��ƃf�[�^���S�ĂO�ɂȂ��Ă��܂��܂��B
void raw_data_read(char* file_name, short* raw_data, int data_length);

//�f�[�^��ASIO�Ɏ󂯓n�����߂̊֐�
// �f�[�^��short�^�����CASIO��24bit(32bit)�Ŏ󂯎�邽��bit�������܂����Ă���
void data_set_short(short* in, short* out, long n)
{
    int i;
    for (i = 0; i < n; i++) {
        *(out++) = 0;
        *(out++) = *(in++);
    }
}
/***************** End for asio init ****/
int c = 340;
double x_ref[2];
double g0[MAX_Channel_NUM] = { 0 };
double x_norm;
double dly_crnt_vec[MAX_Channel_NUM] = { 0 };
float pastc_x, pastc_y;
double xs_vec[2];

void init_dly() {
    int channel_num = MAX_Channel_NUM;
    struct xy_pos xyp = init_xy_pos();
    xp = xyp.xp;// 0;
    yp = xyp.yp;// leap_pos;
    xs_vec[0] = xp; xs_vec[1] = yp;
    pastc_x = xp, pastc_y = yp;
    x_ref[0] = 0; x_ref[1] = 2.2;
    for (int i = 0; i < channel_num; i++) {
        //x_norm[i] = sqrt(((LS_x[i] - xp)* (LS_x[i] - xp)) + ((LS_y[i] - yp)* (LS_y[i] - yp)));
        x_norm = sqrt(((LS_x[i] - xp) * (LS_x[i] - xp)) + ((LS_y[i] - yp) * (LS_y[i] - yp)));
        g0[i] = sqrt(2 * pi * sqrt(((LS_x[i] - x_ref[0]) * (LS_x[i] - x_ref[0])) + ((LS_y[i] - x_ref[1]) * (LS_y[i] - x_ref[1]))));
        //dly_crnt_vec[i] = -round(x_norm[i] / c * samp_freq);
        dly_crnt_vec[i] = -round(x_norm / c * samp_freq);
        //gain_crnt_vec[i] = -1 * g0[i] * (LS_y[i] - xs_vec[1]) / abs(pow(sqrt(x_norm[i]), 3));
    }
}

double* sound_mat;
int main(int argc, char* argv[])
{
    /* asio*/
        // load the driver, this will setup all the necessary internal data structures
    int i;

    //    float freq = 880.0;
    float sound_velocity = 340.0;
    // for debug
    long out_data_length;
    long max_delay = 48000;

    // �t�@�C���̓ǂݍ��݂͐�΃p�X�łȂ��Ƃł��Ȃ������E�E�E�B\�͒��ڎg���Ȃ��̂ŁC�f�B���N�g���̋�؂��\\�ɂ��Ă���B

    //char file_name[] = "C:\\Users\\�R��G�b\\source\\repos\\ASIO_SDK_separate\\ASIO_SDK_separate\\guiter_mono.sb";
    //char file_name[] = "C:\\Users\\�R��G�b\\Documents\\MATLAB\\program_Kinect\\sound_data_45k.sb";
    //char file_name[] = "C:\\Users\\measure\\source\\repos\\ASIO_SDK_yamazaki\\ASIO_SDK_yamazaki\\guiter_mono.sb";
    //char file_name[] = "C:\\Users\\measure\\Documents\\MATLAB\\yamazaki\\sound_data_45k.sb";
    char file_name[] = "C:\\Users\\measure\\Documents\\MATLAB\\yamazaki\\mus.sb";

    //  int data_length;

    out_data_length = (long)frame_size_max;
    data_length = (int)(samp_freq * data_second);

    t_time = 0;
    //current_sample = 0;
    current_sample = frame_size_max;
    old_yp = 100.0;

    // out_data�̃������̊m��
    for (i = 0; i < MAX_Channel_NUM; i++) {
        out_data[i] = (short*)calloc((size_t)out_data_length, sizeof(short));
        prev_out_data[i] = (short*)calloc((size_t)out_data_length, sizeof(short));
    }
    sound_mat = (double*)calloc(data_length, sizeof(double));
    // raw_data �����t�@�C���p�̃������̊m�ۂƓǂݍ���
    raw_data = (short*)calloc(data_length, sizeof(short));
    raw_data_read(file_name, raw_data, data_length);

    //�@�擪�f�[�^�Ńf�[�^���ǂݍ��܂�Ă��邩�m�F
    fprintf(stdout, "data\n");
    for (i = 0; i < 20; i++)
        fprintf(stdout, "raw_data: %5d\t current Time: %5d \n", (int)raw_data[i], (int)current_sample);


    // �X�s�[�J�ʒu�̐ݒ�
    int x_half = MAX_Channel_NUM / 2;
    float sp_dis = 0.1;
    for (i = 0; i < MAX_Channel_NUM; i++) {
        if (i < x_half) {
            LS_x[i] = -(x_half - 0.5) * sp_dis + i * sp_dis;
        }
        else {
            LS_x[i] = sp_dis / 2 + (i - x_half) * sp_dis;
        }
        LS_y[i] = 0;
    }

    init_dly();

    fprintf(stdout, "\n Data ready.\n\n");
    /* end asio*/




    // ���������openGL���g�����`��Ɋւ��鏉�����Ȃ�
    //Kinect�̏�����
    if(!kinect_init())return 0;

    // �ʏ��open GL ��Unity, Kinect SDK�����[�v���񂵂Ċ��荞�݃X���b�h�����삷�邪�C���ꂾ�Ɖ����r�؂��\��������̂�
    // �����ł́Casio�����C���ŃX���b�h���񂵁Cleap,kinect��openGL�͌Ă΂ꂽ�Ƃ���ɂȂƂ��������삳����
    // asio�͐�ΗD��
    // start asio 

    if (loadAsioDriver(const_cast<char*>(ASIO_DRIVER_NAME)))
    {
        // initialize the driver
        if (ASIOInit(&asioDriverInfo.driverInfo) == ASE_OK)
        {
            printf("asioVersion:   %d\n"
                "driverVersion: %d\n"
                "Name:          %s\n"
                "ErrorMessage:  %s\n",
                asioDriverInfo.driverInfo.asioVersion, asioDriverInfo.driverInfo.driverVersion,
                asioDriverInfo.driverInfo.name, asioDriverInfo.driverInfo.errorMessage);
            if (init_asio_static_data(&asioDriverInfo) == 0)
            {
                // ASIOControlPanel(); you might want to check wether the ASIOControlPanel() can open

                // set up the asioCallback structure and create the ASIO data buffer
                asioCallbacks.bufferSwitch = &bufferSwitch;
                asioCallbacks.sampleRateDidChange = &sampleRateChanged;
                asioCallbacks.asioMessage = &asioMessages;
                asioCallbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;
                if (create_asio_buffers(&asioDriverInfo) == ASE_OK)
                {
                    if (ASIOStart() == ASE_OK)
                    {
                        // asio���X�^�[�g������GL�̃��[�v���X�^�[�g����
                        //main_opengl(argc, argv, asioDriverInfo.preferredSize,LS_x,LS_y);
                        //glutMainLoop();//�����͔񓯊��Ō��Ԏ��Ԃɓ����悤�ɐݒ肵�Ă����K�v������B
                        // Now all is up and running
                        fprintf(stdout, "\nASIO Driver started succefully.\n\n");
                        asioDriverInfo.stopped = 0;//��������Ȃ��Ɖ����ςɂȂ��Ă��܂�
                        while (!asioDriverInfo.stopped)
                        {
                        //Kinect�̐ݒ�(�����ɏ�����opengl���������Ƃ��ɔ������Ȃ��Ȃ��Ă��܂���)
                        //if (!kinect_setting()) break;
                                
                        // �����͖������[�v�ɂȂ邪�A����I�i�w�肵���o�b�t�@�T�C�Y�T���v���������ƂɁj��
                        //ASIOTime* bufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool processNow)���Ăяo�����
                        //�Ȃ̂ŁA���̊֐��̒��ŁAasioDriverInfo.stopped��1�ɂ���Α����~�܂�
                        }
                        ASIOStop();
                        //CloseConnection();
                        kinect_close();
                    }
                    ASIODisposeBuffers();
                }
            }
            ASIOExit();
        }
        asioDrivers->removeCurrentDriver();
    }
    // ���if������������ƁCgl������Casio������o���āC�ȉ��ɂ͈ꐶ����Ă��Ȃ��H

   // glutDestroyWindow(window);
    ASIOStop();
    ASIODisposeBuffers();
    ASIOExit();
    fprintf(stdout, "\nASIO closed succefully????\n\n");

    //millisleep(200);
    //DestroyConnection();
    asioDrivers->removeCurrentDriver();

    return 0;
}



void raw_data_read(char* file_name, short* raw_data, int data_length) {
    //�����f�[�^�̓ǂݍ��݊֐�
    int data_full_length;
    FILE* file_pointer_data;
    errno_t error;

    printf("%s\n", file_name);

    error = fopen_s(&file_pointer_data, file_name, "rb");
    if (error != 0) {
        printf("Not open file\n");
    }
    else {
        data_full_length = fread(raw_data, sizeof(short), data_length, file_pointer_data);
        fclose(file_pointer_data);
    }

}


// ���̊֐��̒��g��ύX����B
// �������Cout_data�ɗ^����f�[�^�Ɋւ��镔���܂�
int bufferTime_countMax = 4;
int bufferTime_count = bufferTime_countMax + 1;
float x_start = 0, y_start = 0, x_end = 0, y_end = 0;
int move_num = 1;
ASIOTime* bufferSwitchTimeInfo(ASIOTime* timeInfo, long index, ASIOBool processNow)
{	// the actual processing callback.
    // Beware that this is normally in a seperate thread, hence be sure that you take care
    // about thread synchronization. This is omitted here for simplicity.
    
    ////Kinect�̐ݒ�
    if (!kinect_setting()) asioDriverInfo.stopped = 1;

    float freq;
    static long processedSamples = 0;
    int channel_num = MAX_Channel_NUM;
    long i, j;

    // buffer size in samples
    long buffSize = asioDriverInfo.preferredSize;
    init_frame_sound_maker_allpass(buffSize);
    float* c_vol = (float*)calloc(buffSize, sizeof(float));
    // perform the processing

    /*********����������LEAP��Kinect����K�v�ȏ���ǂ݂����Cout_data�����o���@*****************/
    pastc_x = xp, pastc_y = yp;
    struct xy_pos xyp {};
    int max_range = 4000;

    if (is_init) {
        xyp = get_xy_pos(move_num, get_crnt_body_pos());
        if (-max_range < xyp.xp && xyp.xp < max_range)xp = xyp.xp;
        if (-max_range < xyp.yp && xyp.yp < max_range)yp = xyp.yp;
        /************************���̂�����H�v���ď����K�v�����邩������Ȃ�************************/
    }

    if (bufferTime_count > bufferTime_countMax) {
        x_start = x_end;
        y_start = y_end;
        x_end = xp;
        y_end = yp;
        bufferTime_count = 1;
    }
    xp = bufferTime_count * (x_end - x_start) / bufferTime_countMax + x_start;
    yp = bufferTime_count * (y_end - y_start) / bufferTime_countMax + y_start;
    xs_vec[0] = xp; xs_vec[1] = yp;
    xp = xs_vec[0]; yp = xs_vec[1];
    /********************SOUND*******************/
    double dly_prv_vec[MAX_Channel_NUM] = { 0 };

    if (move_num == 13) {
        for (i = 0; i < channel_num; i++) {
            dly_prv_vec[i] = dly_crnt_vec[i];
            x_norm = sqrt(((LS_x[i] - 0.0) * (LS_x[i] - 0.0)) + ((LS_y[i] - 2.0) * (LS_y[i] - 2.0)));
            dly_crnt_vec[i] = -round(x_norm / c * samp_freq);
        }
    }
    else {
        for (i = 0; i < channel_num; i++) {
            dly_prv_vec[i] = dly_crnt_vec[i];
            x_norm = sqrt(((LS_x[i] - xp) * (LS_x[i] - xp)) + ((LS_y[i] - yp) * (LS_y[i] - yp)));
            dly_crnt_vec[i] = -round(x_norm / c * samp_freq);
        }
    }

    /*******************frame_sound_maker_allpass.m********************/
    for (i = 0; i < channel_num; i++) {
        signal_spring(sound_mat, raw_data, dly_crnt_vec[i], dly_prv_vec[i], current_sample, buffSize);
        gain_change(c_vol, g0[i], xs_vec, pastc_x, pastc_y, LS_y[i], LS_x[i], buffSize);
        for (j = 0; j < (int)buffSize; j++)
        {
            if (out_data != NULL) {
                out_data[i][j] = (short)(c_vol[j] * (float)sound_mat[j]);
            }
        }
    }
    for (i = 0; i < channel_num; i++) {
        for (j = 0; j < (int)buffSize; j++)
        {
                prev_out_data[i][j] = out_data[i][j];
        }
    }
    // data��asio�Ɉ����n���Ă��镔���F�ύX�s�v
    for (i = asioDriverInfo.inputBuffers; i < asioDriverInfo.inputBuffers + channel_num; i++) {
        data_set_short(out_data[i - asioDriverInfo.inputBuffers], (short*)asioDriverInfo.bufferInfos[i].buffers[index], buffSize);
    }

    /**************************************************************************/


    // finally if the driver supports the ASIOOutputReady() optimization, do it here, all data are in place
    if (asioDriverInfo.postOutput)
        ASIOOutputReady();


    t_time = t_time + buffSize;// �g���Ă��Ȃ��H

    // �Ƃ��ǂ��f�[�^�𐔒l�ŕ\���C�f�o�b�O�p�Ȃ̂ōŌ�ɂ͍폜���Ă��悢�B
    if (count_time >= count_max) {
        //fprintf(stdout, "Count :X: %5d\t z: %5d\t c_vol_0: %5f \t c_vol_7: %5f \t  current Time: %5d \n", (int)xp, (int)zp, (float)c_vol[0], (float)c_vol[7], (int)current_sample);
        count_time = 0;
    }
    else
        count_time = count_time + buffSize;

    // ���t���[���̐擪�T���v���ԍ����C���N�������g�F�d�v
    if (current_sample >= data_length - buffSize*2) {
        //current_sample = 0;
        current_sample = buffSize;
    }
    else
        current_sample = current_sample + buffSize;
    
    close_frame_sound_maker_allpass();
    free(c_vol);
    bufferTime_count++;
    return 0L;
}

//------------------�ȉ��͏C���s�v�F���������Ⴞ��----------------------------------------------------
void bufferSwitch(long index, ASIOBool processNow)
{	// the actual processing callback.
    // Beware that this is normally in a seperate thread, hence be sure that you take care
    // about thread synchronization. This is omitted here for simplicity.

    // as this is a "back door" into the bufferSwitchTimeInfo a timeInfo needs to be created
    // though it will only set the timeInfo.samplePosition and timeInfo.systemTime fields and the according flags
    ASIOTime  timeInfo;
    memset(&timeInfo, 0, sizeof(timeInfo));

    // get the time stamp of the buffer, not necessary if no
    // synchronization to other media is required
    if (ASIOGetSamplePosition(&timeInfo.timeInfo.samplePosition, &timeInfo.timeInfo.systemTime) == ASE_OK)
        timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;

    bufferSwitchTimeInfo(&timeInfo, index, processNow);
}


//End-of-Sample
