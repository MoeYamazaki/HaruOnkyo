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

//���_�̐ݒ�
void resize(int w, int h)
{
	glViewport(0, 0, w, h);

	/* �����ϊ��s��̐ݒ� */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);

	/* ���f���r���[�ϊ��s��̐ݒ� */
	glMatrixMode(GL_MODELVIEW);
}

//�L�[�{�[�h����
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
	case '\033':  /* '\033' �� ESC �� ASCII �R�[�h */
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