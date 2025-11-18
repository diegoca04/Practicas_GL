#define PROYECTO "Practica 5"

#include <iostream>	
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>

static float angulo = 0.0;
static float angulo_camara = 0.0;

static float camara[] = { 7, 6, 23 };

static float velocidad = 1;
static float velocidad_camara = 0.5;

static const float pov = 21;

static const float colorprimario[3] = { 0.2, 0.2, 0.2 };
static const float colorsecundario[3] = { 0.58, 0.37, 0.07 };
static const float colorlineas[3] = { 0.5, 0.5, 0.5 };

static bool vista = false;

GLuint caja, engranajes[4], plataforma[2], tubos[2], techo, cab, bol;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          Creadores de figuras                                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	glColor3f(colorprimario[0], colorprimario[1], colorprimario[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indicesDientes.size(), GL_UNSIGNED_INT, indicesDientes.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	glColor3f(colorprimario[0], colorprimario[1], colorprimario[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indicesDientes.size(), GL_UNSIGNED_INT, indicesDientes.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	glColor3f(colorprimario[0], colorprimario[1], colorprimario[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indicesDientes.size(), GL_UNSIGNED_INT, indicesDientes.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indicesDientes.size(), GL_UNSIGNED_INT, indicesDientes.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

GLuint prisma_rectangular(float anchura, float altura, float profundidad)
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices;

	for (int i = 1; i <= 8; i++) {
		float x, y, z;
		if (i <= 4){ x = anchura / 2; }
		else { x = - anchura / 2; }
		if (i % 2) { z = profundidad / 2; }
		else { z = - profundidad / 2; }
		if (i > 2 && i < 7) { y = altura / 2; }
		else { y = - altura / 2; }
		vertices.push_back(cb::Vec3(x, y, z));
	}
	/*
	orden:
	(1 , -1,  1)   0
	(1 , -1, -1)   1
	(1 ,  1,  1)   2
	(1 ,  1, -1)   3
	(-1,  1,  1)   4
	(-1,  1, -1)   5
	(-1, -1,  1)   6
	(-1, -1, -1)   7
	*/

	vertices.push_back(cb::Vec3(0, 0.2, profundidad / 1.99));
	vertices.push_back(cb::Vec3(-0.2, 0, profundidad / 1.99));
	vertices.push_back(cb::Vec3(-0.1, 0, profundidad / 1.99));

	vertices.push_back(cb::Vec3(-0.08, 0.05, profundidad / 1.99));
	vertices.push_back(cb::Vec3(0.02, 0.05, profundidad / 1.99));
	vertices.push_back(cb::Vec3(-0.18, -0.15, profundidad / 1.99));

	std::vector<GLuint> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);		//cara lateral derecha
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(7);		//cara inferior
	indices.push_back(6);

	indices.push_back(6);
	indices.push_back(4);
	indices.push_back(5);		//cara lateral izquierda
	indices.push_back(7);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(3);		//cara superior
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(4);		//cara frontal
	indices.push_back(6);

	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(5);		//cara trasera
	indices.push_back(7);

	std::vector<GLuint> indices2;
	indices2.push_back(8);
	indices2.push_back(9);
	indices2.push_back(10);
								//rayo
	indices2.push_back(11);
	indices2.push_back(12);
	indices2.push_back(13);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());

	glColor3f(1, 1, 0.0);
	glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, indices2.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, indices2.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

GLuint cilindro(float diametro, float grosor, unsigned int res, int color = 0, int rotar = 1)
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices1 = puntosCircunferencia(res, diametro / 2.0f);
	std::vector<cb::Vec3> vertices2 = puntosCircunferencia(res, diametro / 2.0f, 0, grosor);

	int N = vertices1.size();

	cb::Vec3 origen = cb::Vec3(0, 0, 0);
	cb::Vec3 origen2 = cb::Vec3(0, 0, grosor);

	std::vector<cb::Vec3> vertices;
	vertices.insert(vertices.end(), vertices1.begin(), vertices1.end());
	vertices.insert(vertices.end(), vertices2.begin(), vertices2.end());
	vertices.insert(vertices.end(), origen);
	vertices.insert(vertices.end(), origen2);

	std::vector<GLuint> indices;
	std::vector<GLuint> indices3;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		if (i % 2 == 0) {
			indices.push_back(i);
			indices.push_back(next);
			indices.push_back(N * 2);
		}
		else {
			indices3.push_back(i);
			indices3.push_back(next);
			indices3.push_back(N * 2);
		}
	}
	for (int i = N; i < 2 * N; i++) {
		int next = (i + 1 - N) % N + N;
		if (i % 2 == 0) {
			indices.push_back(i);
			indices.push_back(next);
			indices.push_back(N * 2 + 1);
		}
		else {
			indices3.push_back(i);
			indices3.push_back(next);
			indices3.push_back(N * 2 + 1);
		}
	}

	std::vector<GLuint> indices2;
	std::vector<GLuint> indices4;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		if (i % 2 == 0) {
			indices2.push_back(i);
			indices2.push_back(next);
			indices2.push_back(next + N);
			indices2.push_back(i + N);
		}
		else {
			indices4.push_back(i);
			indices4.push_back(next);
			indices4.push_back(next + N);
			indices4.push_back(i + N);
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glColor3f(colorprimario[0], colorprimario[1], colorprimario[2]);
	if (color == 1) { glColor3f(colorsecundario[0], colorsecundario[1], colorsecundario[2]); }
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_QUADS, indices2.size(), GL_UNSIGNED_INT, indices2.data());

	if (color == 1) { glColor3f(0.42, 0.267, 0.051); }
	glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, indices3.data());
	glDrawElements(GL_QUADS, indices4.size(), GL_UNSIGNED_INT, indices4.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (color > 0 and rotar > 0) {
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
		glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, indices3.data());
	}
	if (rotar == 1) { 
		glDrawElements(GL_QUADS, indices2.size(), GL_UNSIGNED_INT, indices2.data()); 
		glDrawElements(GL_QUADS, indices4.size(), GL_UNSIGNED_INT, indices4.data());
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

GLuint cono(float diametro, float altura, int res)
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices = puntosCircunferencia(res, diametro / 2.0f);

	int N = vertices.size();

	cb::Vec3 origen_i = cb::Vec3(0, 0, 0);
	cb::Vec3 origen_s = cb::Vec3(0, 0, altura);

	vertices.insert(vertices.end(), origen_i);
	vertices.insert(vertices.end(), origen_s);

	std::vector<GLuint> indices;
	std::vector<GLuint> indices2;
	std::vector<GLuint> indices3;
	std::vector<GLuint> indices4;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		if (i % 2 == 0) {
			indices.push_back(i);
			indices.push_back(next);
			indices.push_back(N + 1);
			indices3.push_back(i);
			indices3.push_back(next);
			indices3.push_back(N);
		}
		else {
			indices2.push_back(i);
			indices2.push_back(next);
			indices2.push_back(N + 1);
			indices4.push_back(i);
			indices4.push_back(next);
			indices4.push_back(N);
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1, 0, 0);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glColor3f(1, 1, 0);
	glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, indices2.data());

	glColor3f(colorsecundario[0], colorsecundario[1], colorsecundario[2]);
	glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, indices3.data());

	glColor3f(0.42, 0.267, 0.051);
	glDrawElements(GL_TRIANGLES, indices4.size(), GL_UNSIGNED_INT, indices4.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, indices2.data());
	glDrawElements(GL_TRIANGLES, indices3.size(), GL_UNSIGNED_INT, indices3.data());
	glDrawElements(GL_TRIANGLES, indices4.size(), GL_UNSIGNED_INT, indices4	.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

GLuint bola(float diametro, int res) {
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices = puntosCircunferencia(res, diametro / 2.0f);

	int N = vertices.size();

	cb::Vec3 extremo_i = cb::Vec3(0, 0, diametro / 2);
	cb::Vec3 extremo_s = cb::Vec3(0, 0, - diametro / 2);

	vertices.insert(vertices.end(), extremo_i);
	vertices.insert(vertices.end(), extremo_s);

	std::vector<GLuint> indices;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(N + 1);
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(N);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0, 0, 0.8);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

GLuint caballo() 
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices;
	vertices.insert(vertices.end(), cb::Vec3(0.089828, 0.885953, 0.095313));
	vertices.insert(vertices.end(), cb::Vec3(0.169411, 1.505398, -0.380538));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 0.925767, -0.528762));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.535575, -0.146494));
	vertices.insert(vertices.end(), cb::Vec3(0.065832, 1.034748, -0.721631));
	vertices.insert(vertices.end(), cb::Vec3(0.271061, 1.543889, -0.69995));
	vertices.insert(vertices.end(), cb::Vec3(0.167213, 1.556439, -0.94901));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.641455, -0.76171));
	vertices.insert(vertices.end(), cb::Vec3(0.104116, 1.09503, -1.071542));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.547922, -1.03277));
	vertices.insert(vertices.end(), cb::Vec3(0.134125, 0.960811, 0.473343));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.598153, 0.237789));
	vertices.insert(vertices.end(), cb::Vec3(0.06333, 1.457772, 0.735399));
	vertices.insert(vertices.end(), cb::Vec3(-0.0, 1.321815, 0.697869));
	vertices.insert(vertices.end(), cb::Vec3(0.269966, 1.343064, -0.52326));
	vertices.insert(vertices.end(), cb::Vec3(0.276483, 1.117827, -0.583653));
	vertices.insert(vertices.end(), cb::Vec3(0.295738, 1.098368, 0.036956));
	vertices.insert(vertices.end(), cb::Vec3(0.112741, 1.232818, -1.126915));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.395454, -1.093052));
	vertices.insert(vertices.end(), cb::Vec3(-0.047722, 1.11808, -1.080726));
	vertices.insert(vertices.end(), cb::Vec3(0.117954, 1.618534, 0.720809));
	vertices.insert(vertices.end(), cb::Vec3(0.058284, 0.901525, 0.352211));
	vertices.insert(vertices.end(), cb::Vec3(0.319803, 1.080542, 0.271106));
	vertices.insert(vertices.end(), cb::Vec3(0.356547, 1.050452, -0.749536));
	vertices.insert(vertices.end(), cb::Vec3(0.337925, 1.145074, -0.927558));
	vertices.insert(vertices.end(), cb::Vec3(0.171328, 0.990736, -1.047264));
	vertices.insert(vertices.end(), cb::Vec3(0.215954, 0.651411, -0.975503));
	vertices.insert(vertices.end(), cb::Vec3(0.092355, 0.616012, -1.086611));
	vertices.insert(vertices.end(), cb::Vec3(0.119017, 0.633056, -0.938857));
	vertices.insert(vertices.end(), cb::Vec3(0.204912, 0.634333, -1.085927));
	vertices.insert(vertices.end(), cb::Vec3(0.145703, 0.368943, -1.002256));
	vertices.insert(vertices.end(), cb::Vec3(0.182295, 0.304543, -1.056096));
	vertices.insert(vertices.end(), cb::Vec3(0.094946, 0.27327, -1.073688));
	vertices.insert(vertices.end(), cb::Vec3(0.155257, 0.273679, -1.093102));
	vertices.insert(vertices.end(), cb::Vec3(0.13889, 0.001929, -0.857398));
	vertices.insert(vertices.end(), cb::Vec3(0.224418, 0.001879, -0.974062));
	vertices.insert(vertices.end(), cb::Vec3(0.126896, -2.8e-05, -1.045545));
	vertices.insert(vertices.end(), cb::Vec3(0.197421, 0.700524, 0.210023));
	vertices.insert(vertices.end(), cb::Vec3(0.162646, 0.624521, 0.340586));
	vertices.insert(vertices.end(), cb::Vec3(0.092233, 0.624207, 0.2541));
	vertices.insert(vertices.end(), cb::Vec3(0.100643, 0.372993, 0.248808));
	vertices.insert(vertices.end(), cb::Vec3(0.126474, 0.404736, 0.334786));
	vertices.insert(vertices.end(), cb::Vec3(0.183588, 0.295864, 0.263911));
	vertices.insert(vertices.end(), cb::Vec3(0.09737, 0.117513, 0.293406));
	vertices.insert(vertices.end(), cb::Vec3(0.195653, 0.023397, 0.31274));
	vertices.insert(vertices.end(), cb::Vec3(0.167287, 0.004241, 0.423163));
	vertices.insert(vertices.end(), cb::Vec3(0.070412, 0.006711, 0.298169));
	vertices.insert(vertices.end(), cb::Vec3(0.053213, 1.32356, 1.188024));
	vertices.insert(vertices.end(), cb::Vec3(-0.037069, 1.301924, 1.182546));
	vertices.insert(vertices.end(), cb::Vec3(0.003462, 1.383903, 1.312188));
	vertices.insert(vertices.end(), cb::Vec3(0.052211, 1.335348, 1.290803));
	vertices.insert(vertices.end(), cb::Vec3(-0.038749, 1.319357, 1.311391));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.508656, 0.877656));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.835813, 1.06537));
	vertices.insert(vertices.end(), cb::Vec3(0.092999, 1.513978, 0.914787));
	vertices.insert(vertices.end(), cb::Vec3(0.097408, 1.627414, 0.88626));
	vertices.insert(vertices.end(), cb::Vec3(0.137873, 1.836468, 0.960602));
	vertices.insert(vertices.end(), cb::Vec3(0.063689, 1.865706, 1.019376));
	vertices.insert(vertices.end(), cb::Vec3(0.141083, 0.177329, 0.252185));
	vertices.insert(vertices.end(), cb::Vec3(0.128271, 0.164188, 0.330226));
	vertices.insert(vertices.end(), cb::Vec3(0.133178, 0.21604, -0.980643));
	vertices.insert(vertices.end(), cb::Vec3(0.191065, 0.113913, -1.015535));
	vertices.insert(vertices.end(), cb::Vec3(0.055222, 0.00678, -0.994722));
	vertices.insert(vertices.end(), cb::Vec3(0.166264, 1.959051, 1.008834));
	vertices.insert(vertices.end(), cb::Vec3(0.120407, 0.981087, -0.575924));
	vertices.insert(vertices.end(), cb::Vec3(0.176643, 1.465662, 0.05481));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 0.840085, 0.028166));
	vertices.insert(vertices.end(), cb::Vec3(0.265384, 1.304249, 0.273393));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.366386, -1.091909));
	vertices.insert(vertices.end(), cb::Vec3(0.055199, 1.438715, -1.11639));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.25219, -1.208372));
	vertices.insert(vertices.end(), cb::Vec3(-0.0, 0.689176, -1.065917));
	vertices.insert(vertices.end(), cb::Vec3(0.113685, 0.895174, -1.098168));
	vertices.insert(vertices.end(), cb::Vec3(0.068616, 0.716493, -1.187024));
	vertices.insert(vertices.end(), cb::Vec3(0.006885, 0.452369, -1.203026));
	vertices.insert(vertices.end(), cb::Vec3(0.050145, 0.444577, -1.115371));
	vertices.insert(vertices.end(), cb::Vec3(0.046358, 1.802318, 1.108777));
	vertices.insert(vertices.end(), cb::Vec3(0.055934, 1.887677, 0.942155));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.780767, 1.119359));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.796445, 1.136238));
	vertices.insert(vertices.end(), cb::Vec3(0.041736, 1.864881, 0.783654));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.866646, 1.089469));
	vertices.insert(vertices.end(), cb::Vec3(0.0, 1.953028, 0.883282));
	vertices.insert(vertices.end(), cb::Vec3(0.081793, 1.523574, 0.251858));
	vertices.insert(vertices.end(), cb::Vec3(0.165276, 1.647195, 0.979107));
	vertices.insert(vertices.end(), cb::Vec3(0.125273, 1.688809, 1.114171));
	vertices.insert(vertices.end(), cb::Vec3(-0.089828, 0.885953, 0.095313));
	vertices.insert(vertices.end(), cb::Vec3(-0.169411, 1.505398, -0.380538));
	vertices.insert(vertices.end(), cb::Vec3(-0.06227, 1.052006, -0.767465));
	vertices.insert(vertices.end(), cb::Vec3(-0.205309, 1.586045, -0.809185));
	vertices.insert(vertices.end(), cb::Vec3(-0.06333, 1.457772, 0.735399));
	vertices.insert(vertices.end(), cb::Vec3(-0.269966, 1.343064, -0.52326));
	vertices.insert(vertices.end(), cb::Vec3(-0.276483, 1.117826, -0.583653));
	vertices.insert(vertices.end(), cb::Vec3(-0.295738, 1.098368, 0.036956));
	vertices.insert(vertices.end(), cb::Vec3(-0.33111, 1.414949, -0.827639));
	vertices.insert(vertices.end(), cb::Vec3(-0.270372, 1.17763, 0.380574));
	vertices.insert(vertices.end(), cb::Vec3(-0.112741, 1.232818, -1.126915));
	vertices.insert(vertices.end(), cb::Vec3(-0.117954, 1.618534, 0.720809));
	vertices.insert(vertices.end(), cb::Vec3(-0.057811, 0.900093, 0.351293));
	vertices.insert(vertices.end(), cb::Vec3(-0.326425, 1.097436, 0.213141));
	vertices.insert(vertices.end(), cb::Vec3(-0.356547, 1.050452, -0.749536));
	vertices.insert(vertices.end(), cb::Vec3(-0.337925, 1.145074, -0.927558));
	vertices.insert(vertices.end(), cb::Vec3(-0.171328, 0.990736, -1.047264));
	vertices.insert(vertices.end(), cb::Vec3(-0.215954, 0.651411, -0.975503));
	vertices.insert(vertices.end(), cb::Vec3(-0.119017, 0.633056, -0.938857));
	vertices.insert(vertices.end(), cb::Vec3(-0.092355, 0.616012, -1.086611));
	vertices.insert(vertices.end(), cb::Vec3(-0.204912, 0.634333, -1.085927));
	vertices.insert(vertices.end(), cb::Vec3(-0.145703, 0.368943, -1.002256));
	vertices.insert(vertices.end(), cb::Vec3(-0.182295, 0.304543, -1.056096));
	vertices.insert(vertices.end(), cb::Vec3(-0.094946, 0.27327, -1.073688));
	vertices.insert(vertices.end(), cb::Vec3(-0.155257, 0.27368, -1.093102));
	vertices.insert(vertices.end(), cb::Vec3(-0.13889, 0.001929, -0.857398));
	vertices.insert(vertices.end(), cb::Vec3(-0.224418, 0.001879, -0.974062));
	vertices.insert(vertices.end(), cb::Vec3(-0.126896, -2.8e-05, -1.045545));
	vertices.insert(vertices.end(), cb::Vec3(-0.1439, 0.96705, 0.466157));
	vertices.insert(vertices.end(), cb::Vec3(-0.197421, 0.700524, 0.210023));
	vertices.insert(vertices.end(), cb::Vec3(-0.162646, 0.624521, 0.340586));
	vertices.insert(vertices.end(), cb::Vec3(-0.092233, 0.624207, 0.2541));
	vertices.insert(vertices.end(), cb::Vec3(-0.100643, 0.372993, 0.248808));
	vertices.insert(vertices.end(), cb::Vec3(-0.183588, 0.295864, 0.263911));
	vertices.insert(vertices.end(), cb::Vec3(-0.126474, 0.404736, 0.334786));
	vertices.insert(vertices.end(), cb::Vec3(-0.09737, 0.117513, 0.293406));
	vertices.insert(vertices.end(), cb::Vec3(-0.167287, 0.004241, 0.423163));
	vertices.insert(vertices.end(), cb::Vec3(-0.070412, 0.006711, 0.298169));
	vertices.insert(vertices.end(), cb::Vec3(-0.074603, 1.414201, 1.198953));
	vertices.insert(vertices.end(), cb::Vec3(-0.092999, 1.513978, 0.914787));
	vertices.insert(vertices.end(), cb::Vec3(-0.097408, 1.627414, 0.88626));
	vertices.insert(vertices.end(), cb::Vec3(-0.137873, 1.836468, 0.960602));
	vertices.insert(vertices.end(), cb::Vec3(-0.046476, 1.472343, -1.111542));
	vertices.insert(vertices.end(), cb::Vec3(-0.063689, 1.865706, 1.019376));
	vertices.insert(vertices.end(), cb::Vec3(-0.027369, 1.701542, 1.172601));
	vertices.insert(vertices.end(), cb::Vec3(-0.141083, 0.177329, 0.252185));
	vertices.insert(vertices.end(), cb::Vec3(-0.128271, 0.164188, 0.330226));
	vertices.insert(vertices.end(), cb::Vec3(-0.195653, 0.023397, 0.31274));
	vertices.insert(vertices.end(), cb::Vec3(-0.133178, 0.21604, -0.980643));
	vertices.insert(vertices.end(), cb::Vec3(-0.191065, 0.113913, -1.015535));
	vertices.insert(vertices.end(), cb::Vec3(-0.055222, 0.00678, -0.994722));
	vertices.insert(vertices.end(), cb::Vec3(-0.166264, 1.959051, 1.008834));
	vertices.insert(vertices.end(), cb::Vec3(-0.120407, 0.981087, -0.575924));
	vertices.insert(vertices.end(), cb::Vec3(-0.176643, 1.465662, 0.054811));
	vertices.insert(vertices.end(), cb::Vec3(-0.265384, 1.304249, 0.273393));
	vertices.insert(vertices.end(), cb::Vec3(-0.060581, 1.286378, -1.139843));
	vertices.insert(vertices.end(), cb::Vec3(-0.113685, 0.895174, -1.098168));
	vertices.insert(vertices.end(), cb::Vec3(-0.068616, 0.716493, -1.187024));
	vertices.insert(vertices.end(), cb::Vec3(-0.071038, 0.476832, -1.125511));
	vertices.insert(vertices.end(), cb::Vec3(-0.046358, 1.802318, 1.108777));
	vertices.insert(vertices.end(), cb::Vec3(-0.055934, 1.887677, 0.942155));
	vertices.insert(vertices.end(), cb::Vec3(-0.041736, 1.864881, 0.783654));
	vertices.insert(vertices.end(), cb::Vec3(-0.081793, 1.523574, 0.251858));
	vertices.insert(vertices.end(), cb::Vec3(-0.165276, 1.647195, 0.979107));
	vertices.insert(vertices.end(), cb::Vec3(-0.134626, 1.670113, 1.100876));

	std::vector<GLuint> indices;
	indices.push_back(1);
	indices.push_back(7);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(4);
	indices.push_back(64);
	indices.push_back(7);
	indices.push_back(6);
	indices.push_back(9);
	indices.push_back(3);
	indices.push_back(83);
	indices.push_back(65);
	indices.push_back(21);
	indices.push_back(10);
	indices.push_back(98);
	indices.push_back(83);
	indices.push_back(20);
	indices.push_back(67);
	indices.push_back(22);
	indices.push_back(12);
	indices.push_back(10);
	indices.push_back(67);
	indices.push_back(20);
	indices.push_back(22);
	indices.push_back(6);
	indices.push_back(17);
	indices.push_back(69);
	indices.push_back(18);
	indices.push_back(17);
	indices.push_back(19);
	indices.push_back(5);
	indices.push_back(24);
	indices.push_back(6);
	indices.push_back(14);
	indices.push_back(23);
	indices.push_back(5);
	indices.push_back(64);
	indices.push_back(16);
	indices.push_back(0);
	indices.push_back(15);
	indices.push_back(67);
	indices.push_back(16);
	indices.push_back(16);
	indices.push_back(67);
	indices.push_back(22);
	indices.push_back(66);
	indices.push_back(21);
	indices.push_back(98);
	indices.push_back(8);
	indices.push_back(28);
	indices.push_back(4);
	indices.push_back(25);
	indices.push_back(27);
	indices.push_back(8);
	indices.push_back(24);
	indices.push_back(26);
	indices.push_back(29);
	indices.push_back(24);
	indices.push_back(25);
	indices.push_back(17);
	indices.push_back(64);
	indices.push_back(23);
	indices.push_back(15);
	indices.push_back(23);
	indices.push_back(28);
	indices.push_back(26);
	indices.push_back(25);
	indices.push_back(29);
	indices.push_back(27);
	indices.push_back(4);
	indices.push_back(28);
	indices.push_back(64);
	indices.push_back(27);
	indices.push_back(32);
	indices.push_back(28);
	indices.push_back(26);
	indices.push_back(30);
	indices.push_back(31);
	indices.push_back(29);
	indices.push_back(31);
	indices.push_back(33);
	indices.push_back(27);
	indices.push_back(33);
	indices.push_back(32);
	indices.push_back(28);
	indices.push_back(30);
	indices.push_back(26);
	indices.push_back(32);
	indices.push_back(60);
	indices.push_back(30);
	indices.push_back(10);
	indices.push_back(38);
	indices.push_back(22);
	indices.push_back(16);
	indices.push_back(22);
	indices.push_back(37);
	indices.push_back(22);
	indices.push_back(38);
	indices.push_back(37);
	indices.push_back(21);
	indices.push_back(38);
	indices.push_back(10);
	indices.push_back(0);
	indices.push_back(37);
	indices.push_back(39);
	indices.push_back(39);
	indices.push_back(41);
	indices.push_back(38);
	indices.push_back(49);
	indices.push_back(50);
	indices.push_back(130);
	indices.push_back(50);
	indices.push_back(47);
	indices.push_back(85);
	indices.push_back(85);
	indices.push_back(84);
	indices.push_back(56);
	indices.push_back(85);
	indices.push_back(53);
	indices.push_back(130);
	indices.push_back(85);
	indices.push_back(56);
	indices.push_back(57);
	indices.push_back(54);
	indices.push_back(52);
	indices.push_back(55);
	indices.push_back(55);
	indices.push_back(20);
	indices.push_back(56);
	indices.push_back(52);
	indices.push_back(12);
	indices.push_back(55);
	indices.push_back(32);
	indices.push_back(61);
	indices.push_back(62);
	indices.push_back(31);
	indices.push_back(61);
	indices.push_back(33);
	indices.push_back(60);
	indices.push_back(61);
	indices.push_back(31);
	indices.push_back(32);
	indices.push_back(62);
	indices.push_back(60);
	indices.push_back(77);
	indices.push_back(63);
	indices.push_back(56);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(65);
	indices.push_back(2);
	indices.push_back(64);
	indices.push_back(66);
	indices.push_back(14);
	indices.push_back(65);
	indices.push_back(67);
	indices.push_back(56);
	indices.push_back(80);
	indices.push_back(77);
	indices.push_back(53);
	indices.push_back(76);
	indices.push_back(78);
	indices.push_back(54);
	indices.push_back(84);
	indices.push_back(47);
	indices.push_back(47);
	indices.push_back(48);
	indices.push_back(54);
	indices.push_back(7);
	indices.push_back(87);
	indices.push_back(3);
	indices.push_back(66);
	indices.push_back(86);
	indices.push_back(138);
	indices.push_back(88);
	indices.push_back(102);
	indices.push_back(19);
	indices.push_back(88);
	indices.push_back(2);
	indices.push_back(138);
	indices.push_back(88);
	indices.push_back(8);
	indices.push_back(4);
	indices.push_back(7);
	indices.push_back(128);
	indices.push_back(89);
	indices.push_back(13);
	indices.push_back(114);
	indices.push_back(10);
	indices.push_back(148);
	indices.push_back(3);
	indices.push_back(139);
	indices.push_back(97);
	indices.push_back(148);
	indices.push_back(140);
	indices.push_back(90);
	indices.push_back(95);
	indices.push_back(114);
	indices.push_back(97);
	indices.push_back(140);
	indices.push_back(95);
	indices.push_back(128);
	indices.push_back(94);
	indices.push_back(89);
	indices.push_back(94);
	indices.push_back(96);
	indices.push_back(101);
	indices.push_back(96);
	indices.push_back(18);
	indices.push_back(19);
	indices.push_back(94);
	indices.push_back(101);
	indices.push_back(100);
	indices.push_back(87);
	indices.push_back(94);
	indices.push_back(91);
	indices.push_back(91);
	indices.push_back(100);
	indices.push_back(92);
	indices.push_back(138);
	indices.push_back(93);
	indices.push_back(92);
	indices.push_back(66);
	indices.push_back(98);
	indices.push_back(86);
	indices.push_back(105);
	indices.push_back(102);
	indices.push_back(88);
	indices.push_back(101);
	indices.push_back(103);
	indices.push_back(100);
	indices.push_back(19);
	indices.push_back(102);
	indices.push_back(96);
	indices.push_back(102);
	indices.push_back(101);
	indices.push_back(96);
	indices.push_back(100);
	indices.push_back(138);
	indices.push_back(92);
	indices.push_back(100);
	indices.push_back(104);
	indices.push_back(138);
	indices.push_back(102);
	indices.push_back(106);
	indices.push_back(101);
	indices.push_back(104);
	indices.push_back(88);
	indices.push_back(138);
	indices.push_back(109);
	indices.push_back(105);
	indices.push_back(104);
	indices.push_back(105);
	indices.push_back(110);
	indices.push_back(106);
	indices.push_back(107);
	indices.push_back(104);
	indices.push_back(103);
	indices.push_back(106);
	indices.push_back(108);
	indices.push_back(103);
	indices.push_back(134);
	indices.push_back(109);
	indices.push_back(107);
	indices.push_back(116);
	indices.push_back(114);
	indices.push_back(95);
	indices.push_back(86);
	indices.push_back(115);
	indices.push_back(93);
	indices.push_back(116);
	indices.push_back(98);
	indices.push_back(114);
	indices.push_back(98);
	indices.push_back(117);
	indices.push_back(86);
	indices.push_back(117);
	indices.push_back(131);
	indices.push_back(115);
	indices.push_back(120);
	indices.push_back(117);
	indices.push_back(116);
	indices.push_back(115);
	indices.push_back(119);
	indices.push_back(116);
	indices.push_back(49);
	indices.push_back(150);
	indices.push_back(124);
	indices.push_back(125);
	indices.push_back(52);
	indices.push_back(48);
	indices.push_back(130);
	indices.push_back(127);
	indices.push_back(150);
	indices.push_back(127);
	indices.push_back(126);
	indices.push_back(149);
	indices.push_back(149);
	indices.push_back(126);
	indices.push_back(125);
	indices.push_back(97);
	indices.push_back(126);
	indices.push_back(127);
	indices.push_back(90);
	indices.push_back(52);
	indices.push_back(126);
	indices.push_back(146);
	indices.push_back(137);
	indices.push_back(129);
	indices.push_back(137);
	indices.push_back(146);
	indices.push_back(127);
	indices.push_back(91);
	indices.push_back(139);
	indices.push_back(87);
	indices.push_back(147);
	indices.push_back(127);
	indices.push_back(146);
	indices.push_back(53);
	indices.push_back(145);
	indices.push_back(129);
	indices.push_back(130);
	indices.push_back(150);
	indices.push_back(49);
	indices.push_back(149);
	indices.push_back(125);
	indices.push_back(124);
	indices.push_back(66);
	indices.push_back(64);
	indices.push_back(0);
	indices.push_back(65);
	indices.push_back(83);
	indices.push_back(67);
	indices.push_back(1);
	indices.push_back(5);
	indices.push_back(7);
	indices.push_back(7);
	indices.push_back(5);
	indices.push_back(6);
	indices.push_back(10);
	indices.push_back(12);
	indices.push_back(13);
	indices.push_back(3);
	indices.push_back(11);
	indices.push_back(83);
	indices.push_back(83);
	indices.push_back(80);
	indices.push_back(20);
	indices.push_back(22);
	indices.push_back(20);
	indices.push_back(12);
	indices.push_back(6);
	indices.push_back(24);
	indices.push_back(17);
	indices.push_back(19);
	indices.push_back(17);
	indices.push_back(8);
	indices.push_back(18);
	indices.push_back(69);
	indices.push_back(17);
	indices.push_back(5);
	indices.push_back(23);
	indices.push_back(24);
	indices.push_back(1);
	indices.push_back(14);
	indices.push_back(5);
	indices.push_back(14);
	indices.push_back(15);
	indices.push_back(23);
	indices.push_back(64);
	indices.push_back(15);
	indices.push_back(16);
	indices.push_back(66);
	indices.push_back(0);
	indices.push_back(21);
	indices.push_back(8);
	indices.push_back(27);
	indices.push_back(28);
	indices.push_back(24);
	indices.push_back(23);
	indices.push_back(26);
	indices.push_back(8);
	indices.push_back(17);
	indices.push_back(25);
	indices.push_back(23);
	indices.push_back(64);
	indices.push_back(28);
	indices.push_back(25);
	indices.push_back(24);
	indices.push_back(29);
	indices.push_back(28);
	indices.push_back(32);
	indices.push_back(30);
	indices.push_back(27);
	indices.push_back(29);
	indices.push_back(33);
	indices.push_back(29);
	indices.push_back(26);
	indices.push_back(31);
	indices.push_back(30);
	indices.push_back(60);
	indices.push_back(31);
	indices.push_back(0);
	indices.push_back(16);
	indices.push_back(37);
	indices.push_back(21);
	indices.push_back(39);
	indices.push_back(38);
	indices.push_back(21);
	indices.push_back(0);
	indices.push_back(39);
	indices.push_back(39);
	indices.push_back(37);
	indices.push_back(58);
	indices.push_back(54);
	indices.push_back(48);
	indices.push_back(52);
	indices.push_back(85);
	indices.push_back(57);
	indices.push_back(53);
	indices.push_back(56);
	indices.push_back(84);
	indices.push_back(55);
	indices.push_back(84);
	indices.push_back(54);
	indices.push_back(55);
	indices.push_back(56);
	indices.push_back(20);
	indices.push_back(80);
	indices.push_back(55);
	indices.push_back(12);
	indices.push_back(20);
	indices.push_back(52);
	indices.push_back(13);
	indices.push_back(12);
	indices.push_back(32);
	indices.push_back(33);
	indices.push_back(61);
	indices.push_back(77);
	indices.push_back(57);
	indices.push_back(63);
	indices.push_back(14);
	indices.push_back(67);
	indices.push_back(15);
	indices.push_back(14);
	indices.push_back(1);
	indices.push_back(65);
	indices.push_back(53);
	indices.push_back(57);
	indices.push_back(76);
	indices.push_back(130);
	indices.push_back(50);
	indices.push_back(85);
	indices.push_back(47);
	indices.push_back(84);
	indices.push_back(85);
	indices.push_back(140);
	indices.push_back(148);
	indices.push_back(139);
	indices.push_back(88);
	indices.push_back(4);
	indices.push_back(2);
	indices.push_back(7);
	indices.push_back(89);
	indices.push_back(87);
	indices.push_back(88);
	indices.push_back(19);
	indices.push_back(8);
	indices.push_back(7);
	indices.push_back(9);
	indices.push_back(128);
	indices.push_back(13);
	indices.push_back(90);
	indices.push_back(114);
	indices.push_back(148);
	indices.push_back(11);
	indices.push_back(3);
	indices.push_back(10);
	indices.push_back(114);
	indices.push_back(98);
	indices.push_back(97);
	indices.push_back(147);
	indices.push_back(148);
	indices.push_back(90);
	indices.push_back(97);
	indices.push_back(95);
	indices.push_back(94);
	indices.push_back(128);
	indices.push_back(96);
	indices.push_back(96);
	indices.push_back(128);
	indices.push_back(18);
	indices.push_back(87);
	indices.push_back(89);
	indices.push_back(94);
	indices.push_back(91);
	indices.push_back(94);
	indices.push_back(100);
	indices.push_back(99);
	indices.push_back(95);
	indices.push_back(140);
	indices.push_back(138);
	indices.push_back(86);
	indices.push_back(93);
	indices.push_back(92);
	indices.push_back(93);
	indices.push_back(140);
	indices.push_back(93);
	indices.push_back(99);
	indices.push_back(140);
	indices.push_back(104);
	indices.push_back(105);
	indices.push_back(88);
	indices.push_back(101);
	indices.push_back(106);
	indices.push_back(103);
	indices.push_back(100);
	indices.push_back(103);
	indices.push_back(104);
	indices.push_back(102);
	indices.push_back(105);
	indices.push_back(106);
	indices.push_back(107);
	indices.push_back(109);
	indices.push_back(104);
	indices.push_back(103);
	indices.push_back(108);
	indices.push_back(107);
	indices.push_back(106);
	indices.push_back(110);
	indices.push_back(108);
	indices.push_back(105);
	indices.push_back(109);
	indices.push_back(110);
	indices.push_back(108);
	indices.push_back(134);
	indices.push_back(107);
	indices.push_back(93);
	indices.push_back(115);
	indices.push_back(99);
	indices.push_back(99);
	indices.push_back(116);
	indices.push_back(95);
	indices.push_back(99);
	indices.push_back(115);
	indices.push_back(116);
	indices.push_back(116);
	indices.push_back(117);
	indices.push_back(98);
	indices.push_back(86);
	indices.push_back(117);
	indices.push_back(115);
	indices.push_back(117);
	indices.push_back(118);
	indices.push_back(131);
	indices.push_back(131);
	indices.push_back(119);
	indices.push_back(115);
	indices.push_back(117);
	indices.push_back(120);
	indices.push_back(118);
	indices.push_back(116);
	indices.push_back(119);
	indices.push_back(120);
	indices.push_back(150);
	indices.push_back(127);
	indices.push_back(149);
	indices.push_back(53);
	indices.push_back(129);
	indices.push_back(130);
	indices.push_back(130);
	indices.push_back(129);
	indices.push_back(127);
	indices.push_back(125);
	indices.push_back(126);
	indices.push_back(52);
	indices.push_back(147);
	indices.push_back(97);
	indices.push_back(127);
	indices.push_back(97);
	indices.push_back(90);
	indices.push_back(126);
	indices.push_back(90);
	indices.push_back(13);
	indices.push_back(52);
	indices.push_back(92);
	indices.push_back(140);
	indices.push_back(91);
	indices.push_back(87);
	indices.push_back(139);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(66);
	indices.push_back(138);
	indices.push_back(91);
	indices.push_back(140);
	indices.push_back(139);
	indices.push_back(53);
	indices.push_back(78);
	indices.push_back(145);
	indices.push_back(150);
	indices.push_back(149);
	indices.push_back(124);
	indices.push_back(124);
	indices.push_back(125);
	indices.push_back(48);
	indices.push_back(41);
	indices.push_back(40);
	indices.push_back(59);
	indices.push_back(39);
	indices.push_back(58);
	indices.push_back(40);
	indices.push_back(39);
	indices.push_back(40);
	indices.push_back(41);
	indices.push_back(38);
	indices.push_back(41);
	indices.push_back(42);
	indices.push_back(40);
	indices.push_back(58);
	indices.push_back(43);
	indices.push_back(59);
	indices.push_back(44);
	indices.push_back(42);
	indices.push_back(132);
	indices.push_back(121);
	indices.push_back(118);
	indices.push_back(133);
	indices.push_back(132);
	indices.push_back(119);
	indices.push_back(109);
	indices.push_back(135);
	indices.push_back(110);
	indices.push_back(135);
	indices.push_back(108);
	indices.push_back(110);
	indices.push_back(135);
	indices.push_back(134);
	indices.push_back(108);
	indices.push_back(41);
	indices.push_back(59);
	indices.push_back(42);
	indices.push_back(37);
	indices.push_back(38);
	indices.push_back(42);
	indices.push_back(37);
	indices.push_back(42);
	indices.push_back(58);
	indices.push_back(59);
	indices.push_back(40);
	indices.push_back(43);
	indices.push_back(42);
	indices.push_back(44);
	indices.push_back(58);
	indices.push_back(119);
	indices.push_back(132);
	indices.push_back(120);
	indices.push_back(120);
	indices.push_back(132);
	indices.push_back(118);
	indices.push_back(118);
	indices.push_back(121);
	indices.push_back(131);
	indices.push_back(131);
	indices.push_back(133);
	indices.push_back(119);
	indices.push_back(109);
	indices.push_back(136);
	indices.push_back(135);
	indices.push_back(109);
	indices.push_back(134);
	indices.push_back(136);
	indices.push_back(60);
	indices.push_back(35);
	indices.push_back(61);
	indices.push_back(61);
	indices.push_back(36);
	indices.push_back(62);
	indices.push_back(60);
	indices.push_back(62);
	indices.push_back(34);
	indices.push_back(44);
	indices.push_back(46);
	indices.push_back(58);
	indices.push_back(59);
	indices.push_back(46);
	indices.push_back(45);
	indices.push_back(59);
	indices.push_back(45);
	indices.push_back(44);
	indices.push_back(45);
	indices.push_back(46);
	indices.push_back(44);
	indices.push_back(112);
	indices.push_back(134);
	indices.push_back(135);
	indices.push_back(113);
	indices.push_back(135);
	indices.push_back(136);
	indices.push_back(113);
	indices.push_back(136);
	indices.push_back(112);
	indices.push_back(123);
	indices.push_back(133);
	indices.push_back(131);
	indices.push_back(132);
	indices.push_back(123);
	indices.push_back(121);
	indices.push_back(122);
	indices.push_back(132);
	indices.push_back(133);
	indices.push_back(121);
	indices.push_back(123);
	indices.push_back(131);
	indices.push_back(123);
	indices.push_back(122);
	indices.push_back(133);
	indices.push_back(60);
	indices.push_back(34);
	indices.push_back(35);
	indices.push_back(61);
	indices.push_back(35);
	indices.push_back(36);
	indices.push_back(34);
	indices.push_back(62);
	indices.push_back(35);
	indices.push_back(36);
	indices.push_back(35);
	indices.push_back(62);
	indices.push_back(59);
	indices.push_back(43);
	indices.push_back(46);
	indices.push_back(43);
	indices.push_back(58);
	indices.push_back(46);
	indices.push_back(112);
	indices.push_back(111);
	indices.push_back(134);
	indices.push_back(113);
	indices.push_back(112);
	indices.push_back(135);
	indices.push_back(134);
	indices.push_back(111);
	indices.push_back(136);
	indices.push_back(112);
	indices.push_back(136);
	indices.push_back(111);
	indices.push_back(132);
	indices.push_back(122);
	indices.push_back(123);
	indices.push_back(6);
	indices.push_back(128);
	indices.push_back(9);
	indices.push_back(69);
	indices.push_back(70);
	indices.push_back(128);
	indices.push_back(68);
	indices.push_back(72);
	indices.push_back(69);
	indices.push_back(71);
	indices.push_back(75);
	indices.push_back(72);
	indices.push_back(73);
	indices.push_back(74);
	indices.push_back(70);
	indices.push_back(75);
	indices.push_back(74);
	indices.push_back(73);
	indices.push_back(57);
	indices.push_back(82);
	indices.push_back(81);
	indices.push_back(82);
	indices.push_back(77);
	indices.push_back(80);
	indices.push_back(81);
	indices.push_back(76);
	indices.push_back(57);
	indices.push_back(78);
	indices.push_back(76);
	indices.push_back(79);
	indices.push_back(53);
	indices.push_back(79);
	indices.push_back(81);
	indices.push_back(129);
	indices.push_back(137);
	indices.push_back(127);
	indices.push_back(141);
	indices.push_back(68);
	indices.push_back(128);
	indices.push_back(142);
	indices.push_back(68);
	indices.push_back(141);
	indices.push_back(144);
	indices.push_back(71);
	indices.push_back(142);
	indices.push_back(74);
	indices.push_back(143);
	indices.push_back(70);
	indices.push_back(74);
	indices.push_back(144);
	indices.push_back(143);
	indices.push_back(82);
	indices.push_back(129);
	indices.push_back(81);
	indices.push_back(11);
	indices.push_back(147);
	indices.push_back(82);
	indices.push_back(145);
	indices.push_back(81);
	indices.push_back(129);
	indices.push_back(57);
	indices.push_back(56);
	indices.push_back(63);
	indices.push_back(18);
	indices.push_back(68);
	indices.push_back(69);
	indices.push_back(6);
	indices.push_back(69);
	indices.push_back(128);
	indices.push_back(68);
	indices.push_back(71);
	indices.push_back(72);
	indices.push_back(69);
	indices.push_back(72);
	indices.push_back(73);
	indices.push_back(69);
	indices.push_back(73);
	indices.push_back(70);
	indices.push_back(72);
	indices.push_back(75);
	indices.push_back(73);
	indices.push_back(57);
	indices.push_back(77);
	indices.push_back(82);
	indices.push_back(11);
	indices.push_back(82);
	indices.push_back(80);
	indices.push_back(81);
	indices.push_back(79);
	indices.push_back(76);
	indices.push_back(80);
	indices.push_back(83);
	indices.push_back(11);
	indices.push_back(53);
	indices.push_back(78);
	indices.push_back(79);
	indices.push_back(128);
	indices.push_back(68);
	indices.push_back(18);
	indices.push_back(70);
	indices.push_back(141);
	indices.push_back(128);
	indices.push_back(142);
	indices.push_back(71);
	indices.push_back(68);
	indices.push_back(143);
	indices.push_back(142);
	indices.push_back(141);
	indices.push_back(70);
	indices.push_back(143);
	indices.push_back(141);
	indices.push_back(143);
	indices.push_back(144);
	indices.push_back(142);
	indices.push_back(144);
	indices.push_back(75);
	indices.push_back(71);
	indices.push_back(144);
	indices.push_back(74);
	indices.push_back(75);
	indices.push_back(82);
	indices.push_back(146);
	indices.push_back(129);
	indices.push_back(82);
	indices.push_back(147);
	indices.push_back(146);
	indices.push_back(145);
	indices.push_back(79);
	indices.push_back(81);
	indices.push_back(78);
	indices.push_back(79);
	indices.push_back(145);
	indices.push_back(11);
	indices.push_back(148);
	indices.push_back(147);
	indices.push_back(47);
	indices.push_back(51);
	indices.push_back(48);
	indices.push_back(124);
	indices.push_back(51);
	indices.push_back(49);
	indices.push_back(51);
	indices.push_back(50);
	indices.push_back(49);
	indices.push_back(47);
	indices.push_back(50);
	indices.push_back(51);
	indices.push_back(48);
	indices.push_back(51);
	indices.push_back(124);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0.42, 0.267, 0.051);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	
	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          Controlador del programa                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void onIdle()
{
	static int antes = 0;
	int ahora, tiempo_transcurrido;
	ahora = glutGet(GLUT_ELAPSED_TIME);
	tiempo_transcurrido = ahora - antes;
	if (ahora > 3000) {
		angulo += velocidad * tiempo_transcurrido / 1000.0;
		angulo_camara += velocidad_camara * tiempo_transcurrido / 1000.0;
		//camara[0] = 7 + pov * sin(angulo_camara / 2);
		//if (ahora > 8000) { camara[1] = 1.8 + 7 * sin(angulo_camara); }
		//camara[2] = 2 + pov * cos(angulo_camara / 2);
		//if (ahora > 37000) { 
			//if (ahora < 37005) { printf("t: %d, cam: x = %f, y = %f, z = %f\n", ahora, camara[0], camara[1], camara[2]); }
			//camara[0] = 0;
			//camara[1] = 0;
			//camara[2] = 0;
			// en 37001 es (23.786, -3.924, -10.618)
		//}
	}
	if (ahora > 12000) { velocidad = 2; }
	if (ahora > 14000) { velocidad = 3; }
	if (ahora > 16000) { velocidad = 4;  }
	if (ahora > 29000) { velocidad = 3; }
	if (ahora > 32000) { velocidad = 2; }
	if (ahora > 35000) { velocidad = 1; }

	//if (ahora == 10000) { glOrtho(-1.5, 1, -1.5, 1, -1, 10); }

	antes = ahora;

	glutPostRedisplay();
}

void init()
{
	caja = prisma_rectangular(3, 1, 2);
	engranajes[0] = engranajeExterior(1.5, 0.2, 0.5, 0.2, 30);
	engranajes[1] = engranajeCara(4.5, 0.2, 4.4, 0.3, 90);
	engranajes[2] = engranajeExterior(0.75, 0.2, 0.2, 0.2, 15);
	engranajes[3] = engranajeCara(9, 0.2, 8.8, 0.4, 180);
	plataforma[0] = cilindro(8.8, 0.4, 180);
	plataforma[1] = cilindro(9.2, 0.4, 30, 1);
	tubos[0] = cilindro(1.2, 5, 30, 1);
	tubos[1] = cilindro(0.2, 5, 15, 0, 0);
	techo = cono(9.2, 2, 30);
	cab = caballo();
	bol = bola(2, 30);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camara[0], camara[1], camara[2], 7, 3, 0, 0, 1, 0);

	glPushMatrix();
		glTranslatef(0.0, 0.0, 0.0);
		glCallList(caja);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1.2, 0.0, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(-120 * angulo, 0, 0, 1);
		glCallList(engranajes[0]);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(4.2, -0.2, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(6 + 40 * angulo, 0, 0, 1);
		glCallList(engranajes[1]);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(6.82, 0.0, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(12 - 240 * angulo, 0, 0, 1);
		glCallList(engranajes[2]);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 0.0, 0.0);
		glRotatef(270, 1, 0, 0);
		glRotatef(- 20 * angulo, 0, 0, 1);
		glCallList(engranajes[3]);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 0.4, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(20 * angulo, 0, 0, 1);
		glCallList(plataforma[0]);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 0.8, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(20 * angulo, 0, 0, 1);
		glCallList(plataforma[1]);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 5.81, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(20 * angulo, 0, 0, 1);
		glCallList(tubos[0]);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 5.81, 0.0);
		glRotatef(270, 1, 0, 0);
		glRotatef(- 20 * angulo, 0, 0, 1);
		glCallList(techo);
	glPopMatrix();

	float angulo_lento = angulo * 0.349f;
	for (int i = 0; i < 8; i++) {
		glPushMatrix();
			glTranslatef(11.74 + 3.5 * cos(angulo_lento + i * PI/4), 5.81, 3.5 * sin(angulo_lento + i * PI/4));
			glRotatef(90, 1, 0, 0);
			glCallList(tubos[1]);
		glPopMatrix();

		if (i % 2 == 0) {
			glPushMatrix();
				glTranslatef(11.74+3.5*cos(angulo_lento+i*PI/4), 2+1.2*sin(3*angulo_lento), 3.5*sin(angulo_lento+i* PI/4));
				glRotatef(- 10 - 45 * i - 20 * angulo, 0, 1, 0);
				glCallList(cab);
			glPopMatrix();
		}
		else {
			glPushMatrix();
				glTranslatef(11.74+3.5*cos(angulo_lento+i*PI/4), 2+1.2*sin(3*angulo_lento), 3.5*sin(angulo_lento+i*PI/4));
				glRotatef(90, 1, 0, 0);
				glRotatef(-20 * angulo, 0, 0, 1);
				glCallList(bol);
			glPopMatrix();
		}
	}
	
	glutSwapBuffers();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, w / h, 1, 50);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1400, 700);
	glutInitWindowPosition(100, 50);
	glutCreateWindow(PROYECTO);
	std::cout << PROYECTO << " running" << std::endl;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(onIdle);
	init();
	glutMainLoop();
}