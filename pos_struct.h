#pragma once
#include <k4abt.h>

#define JOIN_NUM 32

#define HEAD K4ABT_JOINT_HEAD
#define HAND_LEFT K4ABT_JOINT_HAND_LEFT
#define HAND_RIGHT K4ABT_JOINT_HAND_RIGHT
#define PELVIS K4ABT_JOINT_PELVIS
#define ANKLE_LEFT K4ABT_JOINT_ANKLE_LEFT
#define ANKLE_RIGHT K4ABT_JOINT_ANKLE_RIGHT
#define CLAVICLE_LEFT K4ABT_JOINT_CLAVICLE_LEFT
#define CLAVICLE_RIGHT K4ABT_JOINT_CLAVICLE_RIGHT
#define SPINE_CHEST K4ABT_JOINT_SPINE_CHEST
#define ELBOW_LEFT K4ABT_JOINT_ELBOW_LEFT
#define ELBOW_RIGHT K4ABT_JOINT_ELBOW_RIGHT
#define KNEE_LEFT K4ABT_JOINT_KNEE_LEFT
#define KNEE_RIGHT K4ABT_JOINT_KNEE_RIGHT
#define THUMB_LEFT K4ABT_JOINT_THUMB_LEFT
#define THUMB_RIGHT K4ABT_JOINT_THUMB_RIGHT
#define HANDTIP_LEFT K4ABT_JOINT_HANDTIP_LEFT
#define HANDTIP_RIGHT K4ABT_JOINT_HANDTIP_RIGHT
#define HIP_LEFT K4ABT_JOINT_HIP_LEFT
#define HIP_RIGHT K4ABT_JOINT_HIP_RIGHT
#define EAR_LEFT K4ABT_JOINT_EAR_LEFT
#define EAR_RIGHT K4ABT_JOINT_EAR_RIGHT

//#define HEAD K4ABT_JOINT_HEAD
//#define HAND_LEFT 1
//#define HAND_RIGHT 2
//#define PELVIS 3
//#define ANKLE_LEFT 4
//#define ANKLE_RIGHT 5
//#define CLAVICLE_LEFT 6
//#define CLAVICLE_RIGHT 7
//#define SPINE_CHEST 8
//#define ELBOW_LEFT 9
//#define ELBOW_RIGHT 10
//#define KNEE_LEFT 11
//#define KNEE_RIGHT 12
//#define THUMB_LEFT 13
//#define THUMB_RIGHT 14
//#define HANDTIP_LEFT 15
//#define HANDTIP_RIGHT 16
//#define HIP_LEFT 17
//#define HIP_RIGHT 18
//#define EAR_LEFT 19
//#define EAR_RIGHT 20

struct xy_pos {
	double xp;
	double yp;
};

struct body_pos {
    int id;
    int join_num = JOIN_NUM;
    k4a_float3_t joint_pos[JOIN_NUM];
    //k4a_float3_t head;
    //k4a_float3_t hand_left;
    //k4a_float3_t hand_right;
    //k4a_float3_t pelvis;
    //k4a_float3_t ankle_left;
    //k4a_float3_t ankle_right;
    //k4a_float3_t clavicle_left;
    //k4a_float3_t clavicle_right;
    //k4a_float3_t spine_chest;
    //k4a_float3_t elbow_left;
    //k4a_float3_t elbow_right;
    //k4a_float3_t knee_left;
    //k4a_float3_t knee_right;
    //k4a_float3_t thumb_left;
    //k4a_float3_t thumb_right;
    //k4a_float3_t handtip_left;
    //k4a_float3_t handtip_right;
    //k4a_float3_t hip_left;
    //k4a_float3_t hip_right;
    //k4a_float3_t ear_left;
    //k4a_float3_t ear_right;
};