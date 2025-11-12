#define PROYECTO "Practica 4"

#include <iostream>	
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>

GLuint engranaje1, engranaje2;

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

GLuint engranaje(float diametroPrimitivo, float alturaDiente, float
	diametroEje, float grosor, unsigned int numeroDientes)
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	float diamExt = diametroPrimitivo - alturaDiente / 2;

	std::vector<cb::Vec3> verticesInt = puntosCircunferencia(numeroDientes, diametroEje / 2.0f);
	std::vector<cb::Vec3> verticesExt = puntosCircunferencia(numeroDientes, diamExt / 2.0f);

	std::vector<cb::Vec3> verticesInt2 = puntosCircunferencia(numeroDientes, diametroEje / 2.0f, 0, grosor);
	std::vector<cb::Vec3> verticesExt2 = puntosCircunferencia(numeroDientes, diamExt / 2.0f, 0, grosor);

	float n = 360 / (2 * numeroDientes);

	std::vector<cb::Vec3> verticesDientes = puntosCircunferencia(numeroDientes, (diamExt + alturaDiente) / 2.0f, n);
	std::vector<cb::Vec3> verticesDientes2 = puntosCircunferencia(numeroDientes,
		(diamExt + alturaDiente) / 2.0f, n, grosor);

	int N = verticesInt.size();

	std::vector<cb::Vec3> vertices;
	vertices.insert(vertices.end(), verticesInt.begin(), verticesInt.end());
	vertices.insert(vertices.end(), verticesExt.begin(), verticesExt.end());
	vertices.insert(vertices.end(), verticesInt2.begin(), verticesInt2.end());
	vertices.insert(vertices.end(), verticesExt2.begin(), verticesExt2.end());
	vertices.insert(vertices.end(), verticesDientes.begin(), verticesDientes.end());
	vertices.insert(vertices.end(), verticesDientes2.begin(), verticesDientes2.end());

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
	for (int i = 0; i < N; i++) {
		indices.push_back(i + N);
		indices.push_back(i + 4 * N);
		indices.push_back(i + 5 * N);
		indices.push_back(i + 3 * N);
	}
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(i + 4 * N);
		indices.push_back(next + N);
		indices.push_back(next + 3 * N);
		indices.push_back(i + 5 * N);
	}

	std::vector<GLuint> indicesDientes;
	for (int i = N; i < 2 * N; i++) {
		int next = (i + 1 - N) % N + N;
		indicesDientes.push_back(i);
		indicesDientes.push_back(next);
		indicesDientes.push_back(i + 3 * N);
	}
	for (int i = 3 * N; i < 4 * N; i++) {
		int next = (i + 1 - 3 * N) % N + 3 * N;
		indicesDientes.push_back(i);
		indicesDientes.push_back(next);
		indicesDientes.push_back(i + 2 * N);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indicesDientes.size(), GL_UNSIGNED_INT, indicesDientes.data());
	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

void init()
{
	engranaje1 = engranaje(0.7, 0.2, 0.4, 0.2, 30);
	engranaje2 = engranaje(1.0, 0.2, 1.2, 0.2, 30);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	gluLookAt(0, 0, 0, -0.7, 0.3, -0.7, 0, 1, 0);

	cb::ejes();

	glPushMatrix();
	glRotatef(6, 0, 0, 1);
	glTranslatef(0, 0, 0);
	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(engranaje1);

	glColor3f(1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(engranaje1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 0);
	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(engranaje2);

	glColor3f(0, 1, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(engranaje2);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(engranaje3);

	glColor3f(0, 0, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(engranaje3);
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(GLint w, GLint h)
// Funcion de atencion al redimensionamiento
{
	/* 
	
	version 1:

	// Usamos toda el area de dibujo
	glViewport(0, 0, w, h);
	// Definimos la camara (matriz de proyeccion)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float razon = (float)w / h;
	gluPerspective(45, razon, 1, 10);

	version 2:

	// Razon de aspecto de la vista
	static const float razon = 2.0; // a/b = w'/h'
	// Razon de aspecto del area de dibujo
	float razonAD = float(w) / h;
	float wp, hp; // w',h'
	if (razonAD < razon) {
		wp = float(w);
		hp = wp / razon;
		glViewport(0, int(h / 2.0 - hp / 2.0), w, int(hp));
	}
	else {
		hp = float(h);
		wp = hp * razon;
		glViewport(int(w / 2.0 - wp / 2.0), 0, int(wp), h);
	}
	// Definimos la camara (matriz de proyeccion)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, razon, 1, 10);

	*/
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