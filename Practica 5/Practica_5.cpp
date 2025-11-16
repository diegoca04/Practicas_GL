#define PROYECTO "Practica 5"

#include <iostream>	
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>

static float angulo = 0.0;
static float camara[] = { 6, 6, 12 };
static const float velocidad = 0.5;

static const float pov = 10;

static const float colorprimario[] = { 0.2, 0.2, 0.2 };
static const float colorsecundario[] = { 0.58, 0.37, 0.07 };
static const float colorlineas[] = { 0.5, 0.5, 0.5 };

GLuint caja, engranaje1, engranaje2, engranaje3, engranaje4, plataforma_i, plataforma_s, tubo;

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

	//aqui modificar el indices

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

	//aqui modificar el indices

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

	//aqui modificar el indices

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

	//aqui modificar el indices

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, indices2.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	glEnd();

	glEndList();
	return id;
}

GLuint cilindro(float diametro, float grosor, unsigned int res, int color = 0)
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
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(N * 2);
	}
	for (int i = N; i < 2 * N; i++) {
		int next = (i + 1 - N) % N + N;
		indices.push_back(i);
		indices.push_back(next);
		indices.push_back(N * 2 + 1);
	}

	std::vector<GLuint> indices2;
	for (int i = 0; i < N; i++) {
		int next = (i + 1) % N;
		indices2.push_back(i);
		indices2.push_back(next);
		indices2.push_back(next + N);
		indices2.push_back(i + N);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices.data());

	glColor3f(colorprimario[0], colorprimario[1], colorprimario[2]);
	if (color == 1) { glColor3f(colorsecundario[0], colorsecundario[1], colorsecundario[2]); }
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_QUADS, indices2.size(), GL_UNSIGNED_INT, indices2.data());

	glColor3f(colorlineas[0], colorlineas[1], colorlineas[2]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDrawElements(GL_QUADS, indices2.size(), GL_UNSIGNED_INT, indices2.data());

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

	angulo += velocidad * tiempo_transcurrido / 1000.0;
	//camara[0] = pov*sin(angulo);
	//camara[1] = (sin(angulo) + cos(angulo)) / 2;
	//camara[2] = pov*cos(angulo);

	antes = ahora;

	glutPostRedisplay();
}

void init()
{
	caja = prisma_rectangular(3, 1, 2);
	engranaje1 = engranajeExterior(1.5, 0.2, 0.5, 0.2, 30);
	engranaje2 = engranajeCara(4.5, 0.2, 4.4, 0.3, 90);
	engranaje3 = engranajeExterior(0.75, 0.2, 0.2, 0.2, 15);
	engranaje4 = engranajeCara(9, 0.2, 8.8, 0.4, 180);
	plataforma_i = cilindro(8.8, 0.4, 180);
	plataforma_s = cilindro(9.2, 0.4, 30, 1);
	tubo = cilindro(1.5, 5, 30, 1);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camara[0], camara[1], camara[2], 7, 2, 0, 0, 1, 0);

	//cb::ejes();

	glPushMatrix();
		glTranslatef(0.0, 0.0, 0.0);
		glCallList(caja);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(1.2, 0.0, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(-60 * angulo, 0, 0, 1);
		glCallList(engranaje1);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(4.2, -0.2, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(6 + 20 * angulo, 0, 0, 1);
		glCallList(engranaje2);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(6.82, 0.0, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(12 - 120 * angulo, 0, 0, 1);
		glCallList(engranaje3);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 0.0, 0.0);
		glRotatef(270, 1, 0, 0);
		glRotatef(- 10 * angulo, 0, 0, 1);
		glCallList(engranaje4);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 0.4, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(10 * angulo, 0, 0, 1);
		glCallList(plataforma_i);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 0.8, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(10 * angulo, 0, 0, 1);
		glCallList(plataforma_s);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(11.74, 5.81, 0.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(10 * angulo, 0, 0, 1);
		glCallList(tubo);
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-1.5, 1, -1.5, 1, -1, 10);
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