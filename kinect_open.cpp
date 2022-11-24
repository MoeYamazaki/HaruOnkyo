#include <stdio.h>
#include <stdlib.h>

#include <k4a/k4a.h>
#include <k4abt.h>

#include "pos_struct.h"

#define VERIFY(result, error)                                                                            \
    if(result != K4A_RESULT_SUCCEEDED)                                                                   \
    {                                                                                                    \
        printf("%s \n - (File: %s, Function: %s, Line: %d)\n", error, __FILE__, __FUNCTION__, __LINE__); \
        exit(1);                                                                                         \
    }                                                                                                    \

body_pos get_joints_position(k4abt_skeleton_t skeleton ) {
    struct body_pos body;
    for (int i = 0; i < body.join_num; i++) {
        body.joint_pos[i] = skeleton.joints[i].position;
    }
    return body;
}

k4a_device_t device;
k4abt_tracker_t tracker;

int kinect_init() {
    device = NULL;
    VERIFY(k4a_device_open(0, &device), "Open K4A Device failed!");
    // Start camera. Make sure depth camera is enabled.
    k4a_device_configuration_t deviceConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    deviceConfig.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    deviceConfig.color_resolution = K4A_COLOR_RESOLUTION_OFF;
    VERIFY(k4a_device_start_cameras(device, &deviceConfig), "Start K4A cameras failed!");

    k4a_calibration_t sensor_calibration;
    if (K4A_RESULT_SUCCEEDED != k4a_device_get_calibration(device, deviceConfig.depth_mode, K4A_COLOR_RESOLUTION_OFF, &sensor_calibration))
    {
        printf("Get depth camera calibration failed!\n");
        return 0;
    }

    tracker = NULL;
    k4abt_tracker_configuration_t tracker_config = K4ABT_TRACKER_CONFIG_DEFAULT;
    tracker_config.sensor_orientation = K4ABT_SENSOR_ORIENTATION_DEFAULT;
    tracker_config.processing_mode = K4ABT_TRACKER_PROCESSING_MODE_GPU;
    //tracker_config.processing_mode = K4ABT_TRACKER_PROCESSING_MODE_GPU_CUDA;
    //tracker_config.gpu_device_id = 0;
    tracker_config.gpu_device_id = 1;
    if (K4A_RESULT_SUCCEEDED != k4abt_tracker_create(&sensor_calibration, tracker_config, &tracker))
    {
        printf("Body tracker initialization failed!\n");
        return 0;
    }
    return 1;
}

struct body_pos crnt_body_pos;
int is_init = 0;
int kinect_setting() {
    k4a_capture_t sensor_capture;
    //k4a_wait_result_t get_capture_result = k4a_device_get_capture(device, &sensor_capture, K4A_WAIT_INFINITE);
    k4a_wait_result_t get_capture_result = k4a_device_get_capture(device, &sensor_capture, 0);
    if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED)
    {
        k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(tracker, sensor_capture, 0);
        k4a_capture_release(sensor_capture); // Remember to release the sensor capture once you finish using it
        if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
        {
            printf("Error! Adding capture to tracker process queue failed!\n");
            return 0;
        }

        k4abt_frame_t body_frame = NULL;
        k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, 0);
        if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
        {
            // Successfully popped the body tracking result. Start your processing
            size_t num_bodies = k4abt_frame_get_num_bodies(body_frame);
            //printf("%zu bodies are detected!\n", num_bodies);
            //for (size_t i = 0; i < num_bodies; i++)
            //{
            //    k4abt_skeleton_t skeleton;
            //    k4abt_frame_get_body_skeleton(body_frame, i, &skeleton);
            //    uint32_t id = k4abt_frame_get_body_id(body_frame, i);
            //    // Get Joints Position
            //    struct body_pos main_body_pos = get_joints_position(skeleton);
            //    main_body_pos.id = id;
            //    crnt_body_pos = main_body_pos;
            //    is_init = 1;
            //}
            if(num_bodies != 0){
                k4abt_skeleton_t skeleton;
                k4abt_frame_get_body_skeleton(body_frame, 0, &skeleton);
                uint32_t id = k4abt_frame_get_body_id(body_frame, 0);
                // Get Joints Position
                struct body_pos main_body_pos = get_joints_position(skeleton);
                main_body_pos.id = id;
                crnt_body_pos = main_body_pos;
                is_init = 1;
            }

            k4abt_frame_release(body_frame); // Remember to release the body frame once you finish using it
        }
    }
    else if (get_capture_result == K4A_WAIT_RESULT_TIMEOUT)
    {
        // It should never hit time out when K4A_WAIT_INFINITE is set.
        //printf("Error! Get depth frame time out!\n");
        //return 0;
    }
    else
    {
        printf("Get depth capture returned error: %d\n", get_capture_result);
        return 0;
    }
    return 1;
}

void kinect_close() {
    printf("Finished body tracking processing!\n");
    k4abt_tracker_shutdown(tracker);
    k4abt_tracker_destroy(tracker);
    k4a_device_stop_cameras(device);
    k4a_device_close(device);
}

body_pos get_crnt_body_pos() {
    return crnt_body_pos;
}