#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <k4a/k4a.h>
#include <k4abt.h>
#include <math.h>
#include "main_asio_test.h"

double sim_range = 20;
double*** r_xys, *** tau_xys;
short* out_data_copy[MAX_Channel_NUM];
int buffer_size;

//OpenGLÇÃèâä˙âª
void init_gl2(long buffSize,float* ls_x, float* ls_y) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	buffer_size = buffSize;
	for (int i = 0; i < MAX_Channel_NUM; i++) {
		out_data_copy[i] = (short*)calloc((size_t)(buffSize), sizeof(short));
	}
	r_xys = new double** [MAX_Channel_NUM];
	tau_xys = new double** [MAX_Channel_NUM];
	for (int i = 0; i < MAX_Channel_NUM; i++) {
		r_xys[i] = new double* [sim_range];
		tau_xys[i] = new double* [sim_range];
		for (int j = 0; j < sim_range; j++) {
			r_xys[i][j] = new double[sim_range];
			tau_xys[i][j] = new double[sim_range];
		}
	}
	double l = 1 / sim_range/ 2;
	for (int s = 0; s < MAX_Channel_NUM; s++) {
		for (int i = 0; i < sim_range; i++) {
			for (int j = 0; j < sim_range; j++) {
				double y = -0.5 + (i * 2 + 1) * l;
				double x = (1 + 2 * j - sim_range) * l;
				r_xys[s][i][j] = sqrt((x - ls_x[s]) * (x - ls_x[s]) + (y - ls_y[s]) * (y - ls_y[s]));
				tau_xys[s][i][j] = floor(r_xys[s][i][j] * 48000 / 340);
			}
		}
	}
}

void draw_rect(double r, double g, double b, double x, double y, double l) {
	glPushMatrix();
	glTranslated(x - (1 - l), y + (1 - l), 0.0);
	glColor3d(r, g, b);
	glBegin(GL_POLYGON);
	glVertex2d(-l, -l);
	glVertex2d(l, -l);
	glVertex2d(l, l);
	glVertex2d(-l, l);
	glEnd();
	glPopMatrix();
}

void display_simulation() {
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < MAX_Channel_NUM; i++) {
		for (int j = 0; j < buffer_size; j++)
		{
			out_data_copy[i][j] = prev_out_data[i][j];
		}
	}
	double l = 1 / sim_range;
	double r;
	double r_max = 1000;
	for (int j = 0; j < 1 / l; j++) {
		for (int i = 0; i < 1 / l; i++) {
			r = 0;
			for (int s = 0; s < 8; s++) {
				//r = (r_xys[s][i][j] -r_max/2)/r_max;
				int d = floor(buffer_size / 2) - tau_xys[s][i][j];
				r += (1 / (4 * 3.14 * (r_xys[s][i][j])) * out_data_copy[s][d]) / r_max;
				//printf("%f\n", r);
			}
			draw_rect(r, r, r, i * 2 * l, -j * 2 * l, l);
		}
	}
	glFlush();
}