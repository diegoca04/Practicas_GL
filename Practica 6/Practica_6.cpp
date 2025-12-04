#define PROYECTO "Practica 6"

#include <iostream>	
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>

GLuint suelo;

const int DIM_ESPACIO = 200;

static float cam[9] = { 0, 0, 1, 0, 1, 1, 0, 0, 1 };

static int velocidad = 1;

static float giro = 0;

GLuint plano(int res, int pos) {
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices = {};

	float div = DIM_ESPACIO / res;
	for (int i = 0; i <= res; i++) {
		for (int j = 0; j <= res; j++) {
			cb::Vec3 v;
			v.x = - DIM_ESPACIO / 2 + i * div;
			v.y = - DIM_ESPACIO / 2 + j * div;
			v.z = pos;
			vertices.insert(vertices.end(), v);
		}
	}
	
	std::vector<GLuint> indices;
	for (int i = 0; i < res; i++) {
		for (int j = 0; j < res; j++) {
			int n = res + 1;
			indices.push_back(i + j * n);
			indices.push_back(i + 1 + j * n);
			indices.push_back(i + 1 + (j + 1) * n);
			indices.push_back(i + (j + 1) * n);
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor3f(0, 0, 0);
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	glEndList();
	return id;
}

void onIdle()
{
	cam[1] += (float) velocidad / 1000;
	if (cam[1] > 50 || cam[1] < -50) cam[1] = 0;
	cam[4] = cam[1] + 1;

	cam[0] -= giro * velocidad / 1000;
	if (cam[0] > 50 || cam[0] < - 50) cam[0] = 0;
	cam[3] = cam[0] - giro;
	
	glutPostRedisplay();
}

void init()
{
	suelo = plano(200, 0);
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
		//glRotatef(giro, 0, 0, 1);
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
	switch (tecla) 
	{
		case 'a': if (velocidad < 10) velocidad += 1; break;
		case 'z': if (velocidad > 0) velocidad -= 1; break;
	}
	glutPostRedisplay();
}

void onMouse(int button, int state, int x, int y)
{

	glutPostRedisplay();
}

void onMotion(int x, int y)
{

	glutPostRedisplay();
}

void onPassiveMotion(int x, int y)
{
	giro = (float) (350 - x) / 350;
	cam[5] = (float) - y / 350 + 2;

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
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutPassiveMotionFunc(onPassiveMotion);
	init();
	glutMainLoop();
}