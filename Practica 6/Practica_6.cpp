#define PROYECTO "Practica 6"

#include <iostream>	
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>

GLuint suelo;

const int DIM_ESPACIO = 10;

static float cam[9] = { 1, 0, 0, 0, 0, 0, 1, - 1, 0 };

GLuint plano(int res, int pos, float orientacion = NULL) {
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices = {};

	float div = DIM_ESPACIO / 2 * res;
	for (int i = 0; i <= res; i++) {
		for (int j = 0; j <= res; j++) {
			cb::Vec3 v;
			if (i < res / 2) v.x = i * div;
			else v.x = - i * div;
			if (j < res / 2) v.y = j * div;
			else v.y = - j * div;
			v.z = pos;
			vertices.insert(vertices.end(), v);
		}
	}
	
	std::vector<GLuint> indices;
	for (int i = 0; i < res; i++) {
		int n = res + 1;
		indices.push_back(i);
		indices.push_back(i + n);
		indices.push_back(i + 2 * n);
		indices.push_back(i + 3 * n);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0, 0, 0);
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	if (orientacion != NULL) {
		//completar
	}

	glEnd();

	glEndList();
	return id;
}

void onIdle()
{
	

	glutPostRedisplay();
}

void init()
{
	suelo = plano(10, 0);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(cam[0],cam[1],cam[2],cam[3],cam[4],cam[5],cam[6],cam[7],cam[8]);

	glPushMatrix();
		glCallList(suelo);
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, w / h, 1, 50);
}

void teclado(unsigned char tecla, int x, int y)
{
	

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(500, 50);
	glutCreateWindow(PROYECTO);
	std::cout << PROYECTO << " running" << std::endl;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(teclado);
	init();
	glutMainLoop();
}