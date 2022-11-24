#pragma once
#include "pos_struct.h"

xy_pos init_xyp();
xy_pos right_hand_follow(body_pos body);
xy_pos left_hand_follow(body_pos body);
xy_pos head_follow(body_pos body);
xy_pos body_tilt(body_pos body, int c);
xy_pos archery(body_pos body, int c);
xy_pos grab_hand(body_pos body);
xy_pos throw_sound(body_pos body);
xy_pos throw_sound_auto(body_pos body);
xy_pos throw_sound_fast(body_pos body);
xy_pos throw_sound_slow(body_pos body);
xy_pos throw_sound_long(body_pos body);
xy_pos no_move(body_pos body);
xy_pos no_move_vol_change(body_pos body);