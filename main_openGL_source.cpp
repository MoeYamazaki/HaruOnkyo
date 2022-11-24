#include <GL/glut.h>
#include <k4a/k4a.h>
#include <k4abt.h>
#include "draw_body.h"
#include "draw_simulation.h"

int window1, window2;
int elapsedTime;
int elapsedTime_old;

void idle(void)
{
	//elapsedTime_old = elapsedTime;
	//elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	//printf("time:%d\n", elapsedTime - elapsedTime_old);
	glutSetWindow(window1);
	glutPostRedisplay();
	glutSetWindow(window2);
	glutPostRedisplay();
}

void display()
{
	if (glutGetWindow() == window1) {
		display_body();
	}
	else {
		display_simulation();
	}
}

//視点の設定
void resize(int w, int h)
{
	glViewport(0, 0, w, h);

	/* 透視変換行列の設定 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	/* モデルビュー変換行列の設定 */
	glMatrixMode(GL_MODELVIEW);
}

//キーボード入力
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':  /* '\033' は ESC の ASCII コード */
		exit(0);
	default:
		break;
	}
}

void main_opengl(int argc, char* argv[],long buffSize,float* ls_x, float* ls_y)
{
	//1st Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	window1 = glutCreateWindow("Body Position");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	init_gl();

	//2nd Window
	glutInitDisplayMode(GLUT_RGBA);
	window2 = glutCreateWindow("Simulation");
	glutDisplayFunc(display);
	init_gl2(buffSize,ls_x,ls_y);
	glutMainLoop();
}