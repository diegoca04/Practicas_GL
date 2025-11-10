#define PROYECTO "Practica 3"

#include <iostream>	
#include <cmath>
#include <codebase.h>
#include <vector>

GLuint anilloMenor, anilloMayor;

std::vector<cb::Vec3> puntosCircunferencia(int numeroPuntos, float radio, float fase = 0, float z = 0.0f)
{
	std::vector<cb::Vec3> res = {};

	if (numeroPuntos <= 0) return res;

	float fase_rad = fase * PI / 180.0f;
	float paso = 2.0f * PI / numeroPuntos;

	for (int i = 0; i < numeroPuntos; i++) {
		float ang = fase_rad + i * paso;
		float x = radio * cos(ang);
		float y = radio * sin(ang);
		res.push_back(cb::Vec3(x, y, z));
	}

	return res;
}

GLuint anillo(float diamExt, float diamInt, float grosor, float res = 20)
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);
	//glBegin(GL_POINTS);

	std::vector<cb::Vec3> verticesInt = puntosCircunferencia(res, diamInt / 2.0f);
	std::vector<cb::Vec3> verticesExt = puntosCircunferencia(res, diamExt / 2.0f);

	std::vector<cb::Vec3> verticesInt2 = puntosCircunferencia(res, diamInt / 2.0f, 0, grosor);
	std::vector<cb::Vec3> verticesExt2 = puntosCircunferencia(res, diamExt / 2.0f, 0, grosor);

	int N = verticesInt.size();

	std::vector<cb::Vec3> vertices;
	vertices.insert(vertices.end(), verticesInt.begin(), verticesInt.end());
	vertices.insert(vertices.end(), verticesExt.begin(), verticesExt.end());
	vertices.insert(vertices.end(), verticesInt2.begin(), verticesInt2.end());
	vertices.insert(vertices.end(), verticesExt2.begin(), verticesExt2.end());

	std::vector<GLuint> indices;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(next + N);
		indices.push_back(i + N);
	}
	for (int i = 2 * N; i < 3 * N; i++) {
		int next = (i + 1 - 2 * N) % N + 2 * N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(next + N);
		indices.push_back(i + N);
	}
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(next + 2 * N);
		indices.push_back(i + 2 * N);
	}
	for (int i = N; i < 2 * N; i++) {
		int next = (i + 1 - N) % N + N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(next + 2 * N);
		indices.push_back(i + 2 * N);
	}

	/*glBegin(GL_LINE_LOOP);
	for (auto& p : verticesInt)
		glVertex3fv(p);
	glEnd();

	glBegin(GL_LINE_LOOP);
	for (auto& p : verticesExt)
		glVertex3fv(p);
	glEnd();*/

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDisableClientState(GL_VERTEX_ARRAY);

	//glEnd();
	glEndList();
	return id;
}

void init()
{
	anilloMenor = anillo(1.2, 0.6, 0.2, 30);
	anilloMayor = anillo(2, 1.6, 0.2, 40);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, -0.7, 0.3, -0.7, 0, 1, 0);

	cb::ejes();

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(anilloMenor);
	glCallList(anilloMayor);

	glColor3f(1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(anilloMenor);
	glCallList(anilloMayor);

	glutSwapBuffers();
	//glFlush();
}

void reshape(GLint w, GLint h)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(500, 50);
	glutCreateWindow(PROYECTO);
	init();
	std::cout << PROYECTO << " running" << std::endl;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}