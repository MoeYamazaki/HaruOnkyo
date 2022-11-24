#include "body_move.h"
#include <complex>
//#include <stdlib.h>

int leap_deno = 825; //適当です
double leap_pos = 0.4;
struct xy_pos xyp {};
struct xy_pos prev {};

double change_x(double x) {
	//return 1.0 - x / leap_deno;
	return  -x / leap_deno;
}

double change_y(double z) {
	//return leap_pos + z / leap_deno;
	return z / leap_deno;
}

xy_pos init_xyp() {
	xyp.xp = 0;
	xyp.yp = leap_pos;
	return xyp;
}

xy_pos right_hand_follow(body_pos body) {
	xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
	xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
	return xyp;
}

xy_pos left_hand_follow(body_pos body) {
	xyp.xp = change_x(body.joint_pos[HAND_LEFT].xyz.x);
	xyp.yp = change_y(body.joint_pos[HAND_LEFT].xyz.z);
	return xyp;
}

xy_pos head_follow(body_pos body) {
	xyp.xp = change_x(body.joint_pos[HEAD].xyz.x);
	xyp.yp = change_y(body.joint_pos[HEAD].xyz.z);
	return xyp;
}

xy_pos body_tilt(body_pos body, int c) {
	if (c == 0) {
		xyp.xp = 0;
		prev = xyp;
	}
	double tilt = (change_y(body.joint_pos[HAND_RIGHT].xyz.y) - change_y(body.joint_pos[HAND_LEFT].xyz.y)) / (change_x(body.joint_pos[HAND_RIGHT].xyz.x) - change_x(body.joint_pos[HAND_LEFT].xyz.x));
	xyp.xp = prev.xp + tilt / 100;
	xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
	prev = xyp;
	return xyp;
}

int is_grab_right = 0;
int is_grab_left = 0;
int prev_grab_right = 0;
int prev_grab_left = 0;
int is_released = 0;
struct xy_pos archery_param {};

xy_pos archery2(body_pos body, int c) {
	if ((abs(body.joint_pos[HANDTIP_LEFT].xyz.x - body.joint_pos[THUMB_LEFT].xyz.x) < 50)) {//左手がグーだったら
		if ((abs(body.joint_pos[HANDTIP_RIGHT].xyz.x - body.joint_pos[THUMB_RIGHT].xyz.x) < 50)) {//右手がグーだったら
			xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
			//xyp.yp = change_y(hand_right[id_i].xyz.z);
			//archery_param.xp = (prev.xp - xyp.xp) / sqrt((prev.xp - xyp.xp) * (prev.xp - xyp.xp) + (prev.yp - xyp.yp) * (prev.yp - xyp.yp));
			is_grab_right = 1;
			prev = xyp;
		}
		else if (is_grab_right) {//
			xyp.xp = prev.xp - 0.01;// archery_param.xp;
			//xyp.yp = prev.yp + archery_param.yp;
			prev = xyp;
		}
		else {
			xyp.xp = change_x(body.joint_pos[HAND_LEFT].xyz.x);
			//xyp.yp = change_y(hand_left[id_i].xyz.z);
		}
	}
	else {//左手がパーだったら
		xyp.xp = change_x(body.joint_pos[HAND_LEFT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_LEFT].xyz.z);
		is_grab_right = 0;
	}
	return xyp;
}

xy_pos archery(body_pos body, int c) {
	int crnt_grab_right = 0;
	int crnt_grab_left = 0;
	if ((abs(body.joint_pos[HANDTIP_LEFT].xyz.x - body.joint_pos[THUMB_LEFT].xyz.x) < 40)) { crnt_grab_left = 1; }
	if ((abs(body.joint_pos[HANDTIP_RIGHT].xyz.x - body.joint_pos[THUMB_RIGHT].xyz.x) < 40)) { crnt_grab_right = 1; }

	if (crnt_grab_left + prev_grab_left + is_grab_left >= 2) {
		is_grab_left = 1;
	}
	else {
		is_grab_left = 0;
	}

	if (crnt_grab_right + prev_grab_right + is_grab_right >= 2) {
		is_grab_right = 1;
	}
	else {
		is_grab_right = 0;
	}

	if (is_grab_left) {//左手がグーだったら
		if (is_released) {
			xyp.xp = prev.xp - 0.01;
			prev = xyp;
		}
		else if (is_grab_right) {//右手がグーだったら
			xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
			xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
			//is_released = 1;
			//prev = xyp;
		}
		else {
			is_released = 1;
			prev = xyp;
			//xyp.xp = change_x(hand_left[id_i].xyz.x);
			//xyp.yp = change_y(hand_left[id_i].xyz.z);
		}
	}
	else {//左手がパーだったら
		xyp.xp = change_x(body.joint_pos[HAND_LEFT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_LEFT].xyz.z);
		is_released = 0;
	}
	prev_grab_left = crnt_grab_left;
	prev_grab_right = crnt_grab_right;
	return xyp;
}

xy_pos grab_hand(body_pos body) {
	if ((abs(body.joint_pos[HANDTIP_LEFT].xyz.x - body.joint_pos[THUMB_LEFT].xyz.x) < 50)) {//左手がグーだったら
		xyp.xp = change_x(body.joint_pos[HAND_LEFT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_LEFT].xyz.z);
	}
	else if ((abs(body.joint_pos[HANDTIP_RIGHT].xyz.x - body.joint_pos[THUMB_RIGHT].xyz.x) < 50)) {//右手がグーだったら
		xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
	}
	return xyp;
}

double max_xp = 0;
double max_yp = 0;
int is_throw = 0;
double mid_speed = 15;
xy_pos throw_sound(body_pos body) {
	if (body.joint_pos[ELBOW_RIGHT].xyz.y - body.joint_pos[HAND_RIGHT].xyz.y > 300 || change_y(body.joint_pos[ELBOW_RIGHT].xyz.z) < change_y(body.joint_pos[HAND_RIGHT].xyz.z)) {
		//printf("up!\n");
		xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
		is_throw = 0;
	}
	else {
		if (!is_throw) {
			max_xp = (change_x(body.joint_pos[HAND_RIGHT].xyz.x) - prev.xp) / mid_speed;
			max_yp = (change_y(body.joint_pos[HAND_RIGHT].xyz.z) - prev.yp) / mid_speed;
		}
		is_throw = 1;
		if (prev.yp <= 1.0 || prev.yp > 2.5) { max_xp = -max_xp; max_yp = -max_yp; }
		//printf("down!%lf     %lf\n", max_xp, max_yp);
		xyp.yp = prev.yp + max_yp;
		xyp.xp = prev.xp + max_xp;
	}
	prev = xyp;
	return xyp;
}

xy_pos throw_sound_slow(body_pos body) {
	if (body.joint_pos[ELBOW_RIGHT].xyz.y - body.joint_pos[HAND_RIGHT].xyz.y > 300 || change_y(body.joint_pos[ELBOW_RIGHT].xyz.z) < change_y(body.joint_pos[HAND_RIGHT].xyz.z)) {
		//printf("up!\n");
		xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
		is_throw = 0;
	}
	else {
		if (!is_throw) {
			max_xp = (change_x(body.joint_pos[HAND_RIGHT].xyz.x) - prev.xp) / (mid_speed * 2);
			max_yp = (change_y(body.joint_pos[HAND_RIGHT].xyz.z) - prev.yp) / (mid_speed * 2);
		}
		is_throw = 1;
		if (prev.yp <= 1.0 || prev.yp > 2.5) { max_xp = -max_xp; max_yp = -max_yp; }
		//printf("down!%lf     %lf\n", max_xp, max_yp);
		xyp.yp = prev.yp + max_yp;
		xyp.xp = prev.xp + max_xp;
	}
	prev = xyp;
	return xyp;
}

xy_pos throw_sound_fast(body_pos body) {
	if (body.joint_pos[ELBOW_RIGHT].xyz.y - body.joint_pos[HAND_RIGHT].xyz.y > 300 || change_y(body.joint_pos[ELBOW_RIGHT].xyz.z) < change_y(body.joint_pos[HAND_RIGHT].xyz.z)) {
		//printf("up!\n");
		xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
		is_throw = 0;
	}
	else {
		if (!is_throw) {
			max_xp = (change_x(body.joint_pos[HAND_RIGHT].xyz.x) - prev.xp) / (mid_speed / 2);
			max_yp = (change_y(body.joint_pos[HAND_RIGHT].xyz.z) - prev.yp) / (mid_speed / 2);
		}
		is_throw = 1;
		if (prev.yp <= 1.0 || prev.yp > 2.5) { max_xp = -max_xp; max_yp = -max_yp; }
		//printf("down!%lf     %lf\n", max_xp, max_yp);
		xyp.yp = prev.yp + max_yp;
		xyp.xp = prev.xp + max_xp;
	}
	prev = xyp;
	return xyp;
}

xy_pos throw_sound_long(body_pos body) {
	double mul = 5;
	double line = 2.0 - 0.4;
	if (change_y(body.joint_pos[HAND_RIGHT].xyz.z) < line) {
		double keisu = 1;// -mul * change_y(body.joint_pos[HAND_RIGHT].xyz.z) / line + mul;
		xyp.xp = -change_x(body.joint_pos[ELBOW_RIGHT].xyz.x-body.joint_pos[HAND_RIGHT].xyz.x) * keisu + change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = -change_y(body.joint_pos[ELBOW_RIGHT].xyz.z - body.joint_pos[HAND_RIGHT].xyz.z) * keisu + change_y(body.joint_pos[HAND_RIGHT].xyz.z);
		printf("keisu:    %lf\n", keisu);
	}
	else {
		xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
	}
	printf("%lf\n", xyp.yp);
	return xyp;
}

int frame_count = 1000;
int sound_direction = 0;
xy_pos throw_sound_auto(body_pos body) {
	if (frame_count >= 1000) {
		xyp.yp = 2;
		xyp.xp = 0;
		sound_direction++;
		frame_count = 0;
		if (sound_direction == 1) {
			max_xp = -0.01;
			max_yp = -0.01;
		}
		else if (sound_direction == 2) {
			max_xp = 0.01;
			max_yp = -0.01;
		}
		else {
			max_xp = 0;
			max_yp = -0.01;
			sound_direction = 0;
		}
	}
	else {
		if (prev.yp <= 1.0 || prev.yp > 2.5) { max_xp = -max_xp; max_yp = -max_yp; }
		xyp.yp = prev.yp + max_yp;
		xyp.xp = prev.xp + max_xp;
		frame_count++;
	}
	prev = xyp;
	return xyp;
}

xy_pos throw_sound3(body_pos body) {
	if (abs(body.joint_pos[HIP_LEFT].xyz.x - body.joint_pos[HIP_RIGHT].xyz.x) < 100) {
		if (body.joint_pos[ELBOW_RIGHT].xyz.y - body.joint_pos[HANDTIP_RIGHT].xyz.y > 300 || change_x(body.joint_pos[ELBOW_RIGHT].xyz.x) < change_x(body.joint_pos[HAND_RIGHT].xyz.x)) {
			//printf("up!\n");
			is_throw = 0;
			xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
			xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
			prev = xyp;
		}
		else {
			if (!is_throw) {
				max_xp = (prev.xp - change_x(body.joint_pos[HAND_RIGHT].xyz.x)); max_yp = (prev.yp - change_x(body.joint_pos[HAND_RIGHT].xyz.z));
			}
			is_throw = 1;
			//printf("down!:%lf\n", max_xp);
			xyp.xp = prev.xp + max_xp;// 0.01;// archery_param.xp;
			xyp.yp = prev.yp + max_yp;
			prev = xyp;
		}
	}
	else {
		if (body.joint_pos[ELBOW_RIGHT].xyz.y - body.joint_pos[HANDTIP_RIGHT].xyz.y > 300 || change_y(body.joint_pos[ELBOW_RIGHT].xyz.z) < change_y(body.joint_pos[HAND_RIGHT].xyz.z)) {
			//printf("up!\n");
			is_throw = 0;
			xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
			xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
			prev = xyp;
		}
		else {
			if (!is_throw) { max_xp = (prev.xp - change_x(body.joint_pos[HAND_RIGHT].xyz.x)); max_yp = (prev.yp - change_x(body.joint_pos[HAND_RIGHT].xyz.z)); }
			is_throw = 1;
			//printf("down!:%lf\n", max_xp);
			xyp.xp = prev.xp + max_xp;// 0.01;// archery_param.xp;
			xyp.yp = prev.yp + max_yp;
			prev = xyp;
		}
	}

	return xyp;
}

xy_pos throw_sound2(body_pos body) {
	if (body.joint_pos[ELBOW_RIGHT].xyz.y - body.joint_pos[HANDTIP_RIGHT].xyz.y > 300 || change_y(body.joint_pos[ELBOW_RIGHT].xyz.z) < change_y(body.joint_pos[HAND_RIGHT].xyz.z)) {
		//printf("up!\n");
		is_throw = 0;
		xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
		prev = xyp;
	}
	else {
		if (!is_throw) { max_xp = (prev.xp - change_x(body.joint_pos[HAND_RIGHT].xyz.x)); max_yp = (prev.yp - change_x(body.joint_pos[HAND_RIGHT].xyz.z)); }
		is_throw = 1;
		//printf("down!:%lf\n", max_xp);
		xyp.xp = prev.xp + max_xp;// 0.01;// archery_param.xp;
		xyp.yp = prev.yp + max_yp;
		prev = xyp;
	}
	return xyp;
}

xy_pos no_move(body_pos body) {
	xyp.xp = 0;
	xyp.yp = 2;
	return xyp;
}

xy_pos no_move_vol_change(body_pos body) {
	double mul = 5;
	double line = 2.0 - 0.4;
	if (change_y(body.joint_pos[HAND_RIGHT].xyz.z) < line) {
		double keisu = 1;// -mul * change_y(body.joint_pos[HAND_RIGHT].xyz.z) / line + mul;
		xyp.xp = -change_x(body.joint_pos[ELBOW_RIGHT].xyz.x - body.joint_pos[HAND_RIGHT].xyz.x) * keisu + change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = -change_y(body.joint_pos[ELBOW_RIGHT].xyz.z - body.joint_pos[HAND_RIGHT].xyz.z) * keisu + change_y(body.joint_pos[HAND_RIGHT].xyz.z);
		printf("keisu:    %lf\n", keisu);
	}
	else {
		xyp.xp = change_x(body.joint_pos[HAND_RIGHT].xyz.x);
		xyp.yp = change_y(body.joint_pos[HAND_RIGHT].xyz.z);
	}
	printf("%lf\n", xyp.yp);
	return xyp;
}