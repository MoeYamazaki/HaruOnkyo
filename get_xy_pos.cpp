#include "body_move.h"

int count_get = 0;
xy_pos get_xy_pos(int selectMove, body_pos body) {
	struct xy_pos xyp {};
	xyp = init_xyp();
	if (selectMove == 1) {
		xyp = right_hand_follow(body);
	}
	else if (selectMove == 2) {
		xyp = left_hand_follow(body);
	}
	else if (selectMove == 3) {
		xyp = head_follow(body);
	}
	else if (selectMove == 4) {
		xyp = body_tilt(body, count_get);
	}
	else if (selectMove == 5) {
		xyp = archery(body, count_get);
	}
	else if (selectMove == 6) {
		xyp = grab_hand(body);
	}
	else if (selectMove == 7) {
		xyp = throw_sound(body);
	}
	else if (selectMove == 8) {
		xyp = throw_sound_auto(body);
	}
	else if (selectMove == 9) {
		xyp = throw_sound_slow(body);
	}
	else if (selectMove == 10) {
		xyp = throw_sound_fast(body);
	}
	else if (selectMove == 11) {
		xyp = throw_sound_long(body);
	}
	else if (selectMove == 12) {
		xyp = no_move(body);
	}
	else if (selectMove == 13) {
		xyp = no_move_vol_change(body);
	}
	count_get++;
	return xyp;
}

xy_pos init_xy_pos() {
	struct xy_pos xyp {};
	xyp = init_xyp();
	return xyp;
}