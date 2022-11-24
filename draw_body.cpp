#include <GL/glut.h>
#include <k4a/k4a.h>
#include <k4abt.h>
#include "main_asio_test.h"
int change_body_scale = 500;

void draw_joint(k4a_float3_t joint) {
	/* 図形の描画 */
	glPushMatrix();
	glTranslated(-joint.xyz.x / change_body_scale, -joint.xyz.y / change_body_scale, -joint.xyz.z / change_body_scale);//平行移動値の設定
	glutSolidSphere(0.05, 20, 20);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
	glPopMatrix();
}

void draw_line(k4a_float3_t joint1, k4a_float3_t joint2) {
	glVertex3d(-joint1.xyz.x / change_body_scale, -joint1.xyz.y / change_body_scale, -joint1.xyz.z / change_body_scale);
	glVertex3d(-joint2.xyz.x / change_body_scale, -joint2.xyz.y / change_body_scale, -joint2.xyz.z / change_body_scale);
}

int parent_joint[JOIN_NUM] = {
		0,
		K4ABT_JOINT_PELVIS,
		K4ABT_JOINT_SPINE_NAVEL,
		K4ABT_JOINT_SPINE_CHEST,
		K4ABT_JOINT_SPINE_CHEST,
		K4ABT_JOINT_CLAVICLE_LEFT,
		K4ABT_JOINT_SHOULDER_LEFT,
		K4ABT_JOINT_ELBOW_LEFT,
		K4ABT_JOINT_WRIST_LEFT,
		K4ABT_JOINT_HAND_LEFT,
		K4ABT_JOINT_WRIST_LEFT,
		K4ABT_JOINT_SPINE_CHEST,
		K4ABT_JOINT_CLAVICLE_RIGHT,
		K4ABT_JOINT_SHOULDER_RIGHT,
		K4ABT_JOINT_ELBOW_RIGHT,
		K4ABT_JOINT_WRIST_RIGHT,
		K4ABT_JOINT_HAND_RIGHT,
		K4ABT_JOINT_WRIST_RIGHT,
		K4ABT_JOINT_PELVIS,
		K4ABT_JOINT_HIP_LEFT,
		K4ABT_JOINT_KNEE_LEFT,
		K4ABT_JOINT_ANKLE_LEFT,
		K4ABT_JOINT_PELVIS,
		K4ABT_JOINT_HIP_RIGHT,
		K4ABT_JOINT_KNEE_RIGHT,
		K4ABT_JOINT_ANKLE_RIGHT,
		K4ABT_JOINT_NECK,
		K4ABT_JOINT_HEAD,
		K4ABT_JOINT_HEAD,
		K4ABT_JOINT_HEAD,
		K4ABT_JOINT_HEAD,
		K4ABT_JOINT_HEAD
};

int preview_joint[] = {
	HEAD,HAND_LEFT,HAND_RIGHT,PELVIS,ANKLE_LEFT,ANKLE_RIGHT,CLAVICLE_LEFT,CLAVICLE_RIGHT,SPINE_CHEST,ELBOW_LEFT,
	ELBOW_RIGHT,KNEE_LEFT,KNEE_RIGHT,HIP_LEFT,HIP_RIGHT
};

void draw_joints(body_pos body) {

	for (int i = 0; i < int(sizeof(preview_joint)/sizeof(preview_joint[0])); i++) {
		draw_joint(body.joint_pos[preview_joint[i]]);
	}
	glBegin(GL_LINES);
	for (int i = 1; i < body.join_num-5; i++) {
		if (i != HANDTIP_RIGHT && i != HANDTIP_LEFT && i != THUMB_RIGHT && i != THUMB_LEFT) {
			draw_line(body.joint_pos[i], body.joint_pos[parent_joint[i]]);
		}
	}
	glEnd();
}

//OpenGLの初期化
void init_gl(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void display_body(void)
{
	struct body_pos opengl_body_pos = get_crnt_body_pos();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	/* 視点位置と視線方向 */
	gluLookAt(0, 0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	if (opengl_body_pos.id % MAX_BODIES == 0) { glColor3d(0, 0, 1); }//青
	else if (opengl_body_pos.id % MAX_BODIES == 1) { glColor3d(1, 0, 0); }//赤
	else if (opengl_body_pos.id % MAX_BODIES == 2) { glColor3d(0, 1, 0); }//緑
	else if (opengl_body_pos.id % MAX_BODIES == 3) { glColor3d(0, 1, 1); }//水色
	else if (opengl_body_pos.id % MAX_BODIES == 4) { glColor3d(1, 0, 1); }//ピンク
	else { glColor3d(1, 1, 0); }//黄
	draw_joints(opengl_body_pos);
	glutSwapBuffers();
}