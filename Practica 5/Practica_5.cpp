#define PROYECTO "Practica 5"

#include <iostream>	
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>

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

GLuint engranajeExterior(float diametroPrimitivo, float alturaDiente, float
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

GLuint engranajeInterior(float diametroPrimitivo, float alturaDiente, float
	diametroExterior, float grosor, unsigned int numeroDientes)
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	float diamInt = diametroPrimitivo - alturaDiente / 2;

	std::vector<cb::Vec3> verticesInt = puntosCircunferencia(numeroDientes, diamInt / 2.0f);
	std::vector<cb::Vec3> verticesExt = puntosCircunferencia(numeroDientes, diametroExterior / 2.0f);

	std::vector<cb::Vec3> verticesInt2 = puntosCircunferencia(numeroDientes, diamInt / 2.0f, 0, grosor);
	std::vector<cb::Vec3> verticesExt2 = puntosCircunferencia(numeroDientes, diametroExterior / 2.0f, 0, grosor);

	float n = 360 / (2 * numeroDientes);

	std::vector<cb::Vec3> verticesDientes = puntosCircunferencia(numeroDientes, (diamInt - alturaDiente) / 2.0f, n);
	std::vector<cb::Vec3> verticesDientes2 = puntosCircunferencia(numeroDientes,
		(diamInt - alturaDiente) / 2.0f, n, grosor);

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
	for (int i = N; i < 2 * N; i++) {
		int next = (i + 1 - N) % N + N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(next + 2 * N);
		indices.push_back(i + 2 * N);
	}
	for (int i = 0; i < N; i++) {
		indices.push_back(i);
		indices.push_back(i + 4 * N);
		indices.push_back(i + 5 * N);
		indices.push_back(i + 2 * N);
	}
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(i + 4 * N);
		indices.push_back(next);
		indices.push_back(next + 2 * N);
		indices.push_back(i + 5 * N);
	}

	std::vector<GLuint> indicesDientes;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indicesDientes.push_back(i);
		indicesDientes.push_back(next);
		indicesDientes.push_back(i + 3 * N);
	}
	for (int i = 2 * N; i < 3 * N; i++) {
		int next = (i + 1 - 2 * N) % N + 2 * N;
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

GLuint engranajeCara(float diametroPrimitivo, float alturaDiente, float
	diametroInterior, float grosor, unsigned int numeroDientes)
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	float diamExt = diametroPrimitivo - diametroInterior + diametroPrimitivo;

	std::vector<cb::Vec3> verticesInt = puntosCircunferencia(numeroDientes, diametroInterior / 2.0f);
	std::vector<cb::Vec3> verticesExt = puntosCircunferencia(numeroDientes, diamExt / 2.0f);

	std::vector<cb::Vec3> verticesInt2 = puntosCircunferencia(numeroDientes, diametroInterior / 2.0f, 0, grosor);
	std::vector<cb::Vec3> verticesExt2 = puntosCircunferencia(numeroDientes, diamExt / 2.0f, 0, grosor);

	float n = 360 / (2 * numeroDientes);

	std::vector<cb::Vec3> verticesDientesInt = puntosCircunferencia(numeroDientes,
		(diametroInterior) / 2.0f, n, 0 - alturaDiente);
	std::vector<cb::Vec3> verticesDientesExt = puntosCircunferencia(numeroDientes,
		(diamExt) / 2.0f, n, 0 - alturaDiente);

	int N = verticesInt.size();

	std::vector<cb::Vec3> vertices;
	vertices.insert(vertices.end(), verticesInt.begin(), verticesInt.end());
	vertices.insert(vertices.end(), verticesExt.begin(), verticesExt.end());
	vertices.insert(vertices.end(), verticesInt2.begin(), verticesInt2.end());
	vertices.insert(vertices.end(), verticesExt2.begin(), verticesExt2.end());
	vertices.insert(vertices.end(), verticesDientesInt.begin(), verticesDientesInt.end());
	vertices.insert(vertices.end(), verticesDientesExt.begin(), verticesDientesExt.end());

	std::vector<GLuint> indices;
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
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(next);
		indices.push_back(i + 4 * N);
		indices.push_back(i + 5 * N);
		indices.push_back(next + N);
	}
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(next);
		indices.push_back(next + 4 * N);
		indices.push_back(next + 5 * N);
		indices.push_back(next + N);
	}

	std::vector<GLuint> indicesDientes;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indicesDientes.push_back(i);
		indicesDientes.push_back(next);
		indicesDientes.push_back(i + 4 * N);
	}
	for (int i = N; i < 2 * N; i++) {
		int next = (i + 1 - N) % N + N;
		indicesDientes.push_back(i);
		indicesDientes.push_back(next);
		indicesDientes.push_back(i + 4 * N);
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
	
}

void display()
{
	
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