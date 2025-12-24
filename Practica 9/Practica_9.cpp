#define PROYECTO "Practica 9"

#include <iostream>	
#include <sstream>
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>
#include <freeimage/FreeImage.h>

GLuint suelo, meteor, space, textura_suelo, textura_meteoritos, textura_espacio, textura_cabina, textura_explosion;

const int DIM_ESPACIO = 1000;

const int DIM_PLATAFORMA = 100;

const int NUM_METS = 60;

static int FPS = 0;

static int Wx = 1200, Wy = 700;

static float cam[9] = { 0, 0, 4, 0, 1, 4, 0, 0, 1 };

static double velocidad = 0;

static float giro = 0, altura = 0;

static bool luces = false, cabina = false;

static float angulo = 0;

struct Meteorito {
	float pos[3];
	float dir[3];
	float giro[3];
	float tamaño;
	float velocidad;
	bool explotando = false;
	int contador = 0;
};

Meteorito mets[NUM_METS];

GLuint loadTexture(const char* nombre)
{
	GLuint id;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(nombre, 0);
	FIBITMAP* imagen = FreeImage_Load(formato, nombre);
	FIBITMAP* imagen32b = FreeImage_ConvertTo32Bits(imagen);
	int w = FreeImage_GetWidth(imagen32b);
	int h = FreeImage_GetHeight(imagen32b);

	GLubyte* texeles = FreeImage_GetBits(imagen32b);

	int dataSize = w * h * 4;
	GLubyte* texCopy = new GLubyte[dataSize];
	memcpy(texCopy, texeles, dataSize);

	FreeImage_Unload(imagen);
	FreeImage_Unload(imagen32b);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, texCopy);

	delete[] texCopy;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	return id;
}

GLuint plano(int pos)
{
	GLuint id = glGenLists(1);

	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices = {};
	std::vector<GLfloat> texturas;

	for (int i = 0; i <= DIM_PLATAFORMA; i++) {
		for (int j = 0; j <= DIM_PLATAFORMA; j++) {
			cb::Vec3 v;
			v.x = -DIM_PLATAFORMA / 2 + i;
			v.y = -DIM_PLATAFORMA / 2 + j;
			v.z = pos; vertices.insert(vertices.end(), v);

			texturas.push_back((float)i / DIM_PLATAFORMA);
			texturas.push_back((float)j / DIM_PLATAFORMA);
		}
	}

	std::vector<GLuint> indices;
	for (int i = 0; i < DIM_PLATAFORMA; i++) {
		for (int j = 0; j < DIM_PLATAFORMA; j++) {
			int n = DIM_PLATAFORMA + 1;
			indices.push_back(i + j * n);
			indices.push_back(i + 1 + j * n);
			indices.push_back(i + 1 + (j + 1) * n);
			indices.push_back(i + (j + 1) * n);
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	glTexCoordPointer(2, GL_FLOAT, 0, texturas.data());

	glNormal3f(0, 0, 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glEndList();

	return id;
}

cb::Vec3 normalize(cb::Vec3 v)
{
	float norm = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (norm == 0.0f) return cb::Vec3(0, 0, 0);
	return cb::Vec3(v.x / norm, v.y / norm, v.z / norm);
}

GLuint meteorito()
{
	GLuint id = glGenLists(1);

	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices = {};
	std::vector<cb::Vec3> normales = {};
	std::vector<GLfloat> texturas;

	vertices.push_back(cb::Vec3(0, 0, 1.2));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.5);
	texturas.push_back(1);

	vertices.push_back(cb::Vec3(1, 0, 0.4));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.8);
	texturas.push_back(0.75);

	vertices.push_back(cb::Vec3(0.5, 0.9, 0));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.65);
	texturas.push_back(0.6);

	vertices.push_back(cb::Vec3(-0.5, 0.9, 0));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.35);
	texturas.push_back(0.6);

	vertices.push_back(cb::Vec3(-1, 0, -0.2));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.15);
	texturas.push_back(0.5);

	vertices.push_back(cb::Vec3(-0.5, -0.9, 0.2));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.35);
	texturas.push_back(0.25);

	vertices.push_back(cb::Vec3(0.5, -0.9, -0.1));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.65);
	texturas.push_back(0.25);

	vertices.push_back(cb::Vec3(0, 0, -1.1));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.5);
	texturas.push_back(0);

	vertices.push_back(cb::Vec3(0.8, 0.5, 0.6));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.75);
	texturas.push_back(0.85);

	vertices.push_back(cb::Vec3(-0.8, 0.4, 0.5));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.25);
	texturas.push_back(0.85);

	vertices.push_back(cb::Vec3(-0.6, -0.5, -0.6));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.25);
	texturas.push_back(0.15);

	vertices.push_back(cb::Vec3(0.6, -0.5, -0.5));
	normales.push_back(normalize(vertices.back()));
	texturas.push_back(0.75);
	texturas.push_back(0.15);

	std::vector<GLuint> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(8);

	indices.push_back(0);
	indices.push_back(8);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(9);

	indices.push_back(0);
	indices.push_back(9);
	indices.push_back(4);

	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(5);

	indices.push_back(0);
	indices.push_back(5);
	indices.push_back(6);

	indices.push_back(0);
	indices.push_back(6);
	indices.push_back(1);

	indices.push_back(7);
	indices.push_back(10);
	indices.push_back(4);

	indices.push_back(7);
	indices.push_back(4);
	indices.push_back(9);

	indices.push_back(7);
	indices.push_back(9);
	indices.push_back(3);

	indices.push_back(7);
	indices.push_back(3);
	indices.push_back(2);

	indices.push_back(7);
	indices.push_back(2);
	indices.push_back(11);

	indices.push_back(7);
	indices.push_back(11);
	indices.push_back(6);

	indices.push_back(7);
	indices.push_back(6);
	indices.push_back(5);

	indices.push_back(7);
	indices.push_back(5);
	indices.push_back(10);

	indices.push_back(1);
	indices.push_back(6);
	indices.push_back(11);

	indices.push_back(1);
	indices.push_back(11);
	indices.push_back(8);

	indices.push_back(2);
	indices.push_back(8);
	indices.push_back(11);

	indices.push_back(2);
	indices.push_back(11);
	indices.push_back(3);

	indices.push_back(3);
	indices.push_back(9);
	indices.push_back(10);

	indices.push_back(3);
	indices.push_back(10);
	indices.push_back(5);

	indices.push_back(4);
	indices.push_back(10);
	indices.push_back(9);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	glNormalPointer(GL_FLOAT, 0, normales.data());
	glTexCoordPointer(2, GL_FLOAT, 0, texturas.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glEndList();

	return id;
}

GLuint espacio()
{
	GLuint id = glGenLists(1);

	glNewList(id, GL_COMPILE);

	float s = DIM_ESPACIO;

	std::vector<cb::Vec3> vertices;
	std::vector<GLfloat> texturas;

	vertices.push_back(cb::Vec3(s, s, s));
	vertices.push_back(cb::Vec3(-s, s, s));
	vertices.push_back(cb::Vec3(-s, -s, s));	//delante
	vertices.push_back(cb::Vec3(s, -s, s));
	texturas.insert(texturas.end(), { 1,1,  0,1,  0,0,  1,0 });

	vertices.push_back(cb::Vec3(-s, s, -s));
	vertices.push_back(cb::Vec3(s, s, -s));
	vertices.push_back(cb::Vec3(s, -s, -s));	//detrás
	vertices.push_back(cb::Vec3(-s, -s, -s));
	texturas.insert(texturas.end(), { 1,1,  0,1,  0,0,  1,0 });

	vertices.push_back(cb::Vec3(s, s, -s));
	vertices.push_back(cb::Vec3(s, s, s));
	vertices.push_back(cb::Vec3(s, -s, s));		//derecha
	vertices.push_back(cb::Vec3(s, -s, -s));
	texturas.insert(texturas.end(), { 1,1,  0,1,  0,0,  1,0 });

	vertices.push_back(cb::Vec3(-s, s, s));
	vertices.push_back(cb::Vec3(-s, s, -s));
	vertices.push_back(cb::Vec3(-s, -s, -s));	//izquierda
	vertices.push_back(cb::Vec3(-s, -s, s));
	texturas.insert(texturas.end(), { 1,1,  0,1,  0,0,  1,0 });

	vertices.push_back(cb::Vec3(-s, s, -s));
	vertices.push_back(cb::Vec3(-s, s, s));
	vertices.push_back(cb::Vec3(s, s, s));		//arriba
	vertices.push_back(cb::Vec3(s, s, -s));
	texturas.insert(texturas.end(), { 0,0,  0,1,  1,1,  1,0 });

	vertices.push_back(cb::Vec3(-s, -s, s));
	vertices.push_back(cb::Vec3(-s, -s, -s));
	vertices.push_back(cb::Vec3(s, -s, -s));	//abajo
	vertices.push_back(cb::Vec3(s, -s, s));
	texturas.insert(texturas.end(), { 0,0,  0,1,  1,1,  1,0 });

	std::vector<GLuint> indices;

	for (int i = 0; i < 6; i++) {
		int cara = i * 4;
		indices.push_back(cara + 0);
		indices.push_back(cara + 1);
		indices.push_back(cara + 2);
		indices.push_back(cara + 3);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices.data());
	glTexCoordPointer(2, GL_FLOAT, 0, texturas.data());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glEndList();

	return id;
}

void explosion(int id)
{
	mets[id].explotando = !mets[id].explotando;
	if (!mets[id].explotando) {
		mets[id].contador = 0;

		mets[id].pos[0] = cb::random(- 800, 800);
		mets[id].pos[1] = cb::random(- 800, 800);
		mets[id].pos[2] = cb::random(- 800, 800);

		mets[id].dir[0] = - (mets[id].pos[0] / 400);
		mets[id].dir[1] = - (mets[id].pos[1] / 400);
		mets[id].dir[2] = - (mets[id].pos[2] / 400);
	}
	else {
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glBindTexture(GL_TEXTURE_2D, textura_explosion);

		glPushMatrix();
			glTranslatef(mets[id].pos[0], mets[id].pos[1], mets[id].pos[2]);

			float s = mets[id].tamaño * 2.0f;

			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(- s, - s, 0);
			glTexCoord2f(1, 0); glVertex3f(  s, - s, 0);
			glTexCoord2f(1, 1); glVertex3f(  s,   s, 0);
			glTexCoord2f(0, 1); glVertex3f(- s,   s, 0);
			glEnd();
		glPopMatrix();

		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
	}
}

void mostrarFPS()
{
	int ahora, tiempo_transcurrido;
	static int antes = 0;
	FPS++;
	ahora = glutGet(GLUT_ELAPSED_TIME);
	tiempo_transcurrido = ahora - antes;
	if (tiempo_transcurrido > 1000) {
		stringstream titulo;
		titulo << PROYECTO << "		FPS: " << FPS;
		glutSetWindowTitle(titulo.str().c_str());
		antes = ahora;
		FPS = 0;
	}
}

void onIdle()
{
	//Actualización de la cámara
	cam[3] = sin(giro) * 1000;
	cam[4] = cos(giro) * 1000;
	cam[5] = altura * 1000;

	//Movimiento de la nave
	cam[0] += cam[3] * velocidad / 100000;
	if (cam[0] > 800) cam[0] = 800;
	if (cam[0] < - 800) cam[0] = - 800;

	cam[1] += cam[4] * velocidad / 100000;
	if (cam[1] > 800) cam[1] = 800;
	if (cam[1] < - 800) cam[1] = - 800;

	cam[2] += cam[5] * velocidad / 100000;
	if (cam[2] > 800) cam[2] = 800;
	if (cam[2] < - 800) cam[2] = - 800;

	//Físicas de la plataforma con la nave
	if (cam[0] < DIM_PLATAFORMA / 2 + 4 && cam[0] > - DIM_PLATAFORMA / 2 - 4 &&
		cam[1] < DIM_PLATAFORMA / 2 + 4 && cam[1] > - DIM_PLATAFORMA / 2 - 4) {
		if (cam[2] < 4 && cam[2] > 2) cam[2] = 4;
		if (cam[2] > -4 && cam[2] < -2) cam[2] = -4;
	}

	//Movimiento de los meteoritos
	for (int i = 0; i < NUM_METS; i++) {
		mets[i].pos[0] += (mets[i].dir[0] * mets[i].velocidad / 1000);
		mets[i].pos[1] += (mets[i].dir[1] * mets[i].velocidad / 1000);
		mets[i].pos[2] += (mets[i].dir[2] * mets[i].velocidad / 1000);
	}
	angulo += 0.01;

	//Físicas de los meteoritos
	for (int i = 0; i < NUM_METS; i++) {
		float p0 = mets[i].pos[0], p1 = mets[i].pos[1], p2 = mets[i].pos[2], t = mets[i].tamaño;
		
		//Choque con el límite del espacio
		if (!mets[i].explotando) {
			if (fabs(p0) > 800) mets[i].dir[0] = - mets[i].dir[0];
			if (fabs(p1) > 800) mets[i].dir[1] = - mets[i].dir[1];
			if (fabs(p2) > 800) mets[i].dir[2] = - mets[i].dir[2];
		}

		//Choque con la plataforma
		if (p0 < DIM_PLATAFORMA / 2 + t && p0 > - DIM_PLATAFORMA / 2 - t &&
			p1 < DIM_PLATAFORMA / 2 + t && p1 > - DIM_PLATAFORMA / 2 - t &&
			p2 < t && p2 > -t) {
			mets[i].dir[2] = - mets[i].dir[2];
		}

		else if (!mets[i].explotando) {
			//Choque con la nave
			if (cam[0] < p0 + t && cam[0] > p0 - t &&
				cam[1] < p1 + t && cam[1] > p1 - t &&
				cam[2] < p2 + t && cam[2] > p2 - t) {
				explosion(i);
			}

			//Choque con otro meteorito
			else {
				for (int j = 0; j < NUM_METS; j++) {
					if (j != i) {
						float q0 = mets[j].pos[0], q1 = mets[j].pos[1], q2 = mets[j].pos[2], g = t + mets[j].tamaño;

						if (q0 < p0 + g && q0 > p0 - g &&
							q1 < p1 + g && q1 > p1 - g &&
							q2 < p2 + g && q2 > p2 - g) {
							explosion(i);
							explosion(j);
						}
					}
				}
			}
		}
		else {
			mets[i].contador += 1;
			if (mets[i].contador > 3000 && mets[i].pos[0] < 800 && 
				mets[i].pos[1] < 800 && mets[i].pos[2] < 800) explosion(i);
		}
	}

	glutPostRedisplay();
}

void init()
{
	suelo = plano(0);

	meteor = meteorito();

	space = espacio();

	//Inicialización de los meteoritos
	for (int i = 0; i < NUM_METS; i++) {
		mets[i].dir[0] = cb::random(-2, 2);
		mets[i].dir[1] = cb::random(-2, 2);
		mets[i].dir[2] = cb::random(-2, 2);

		mets[i].giro[0] = cb::random(0, 1);
		mets[i].giro[1] = cb::random(0, 1);
		mets[i].giro[2] = cb::random(0, 1);

		mets[i].pos[0] = cb::random(-600, 600);
		mets[i].pos[1] = cb::random(-600, 600);
		mets[i].pos[2] = cb::random(-600, 600);

		mets[i].velocidad = cb::random(5, 40);

		mets[i].tamaño = cb::random(5, 50);
	}

	//Inicialización de las texturas
	textura_suelo = loadTexture("Texturas/Textura_suelo.jpg");
	textura_meteoritos = loadTexture("Texturas/Textura_meteorito.jpg");
	textura_espacio = loadTexture("Texturas/Textura_espacio.jpg");
	textura_cabina = loadTexture("Texturas/Textura_cabina.png");
	textura_explosion = loadTexture("Texturas/Textura_explosion.jpg");
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Material
	GLfloat material_emission[] = { 0.1, 0.1, 0.1, 1 };
	GLfloat material_ambient[] = { 0.3, 0.3, 0.3, 1 };
	GLfloat material_diffuse[] = { 0.9, 0.9, 0.9, 1 };
	GLfloat material_specular[] = { 1, 1, 1, 1 };
	GLfloat material_shininess[] = { 30 };
	glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	gluLookAt(cam[0], cam[1], cam[2], cam[3], cam[4], cam[5], cam[6], cam[7], cam[8]);

	//Luz Solar
	glEnable(GL_LIGHT1);
	GLfloat AmbSol[] = { 0.3, 0.3, 0.3, 1 };
	GLfloat DifSol[] = { 0.5, 0.5, 0.5, 1 };
	GLfloat SpecSol[] = { 0.1, 0.1, 0.1, 1 };
	GLfloat posSol[] = { 1000, 1000, 1000, 0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, AmbSol);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, DifSol);
	glLightfv(GL_LIGHT1, GL_SPECULAR, SpecSol);
	glLightfv(GL_LIGHT1, GL_POSITION, posSol);

	//Luces focales (faros)
	if (luces)
	{
		GLfloat dirFoc[] = { cam[3] - cam[0], cam[4] - cam[1], cam[5] - cam[2] };
		float norm = sqrt(dirFoc[0] * dirFoc[0] + dirFoc[1] * dirFoc[1] + dirFoc[2] * dirFoc[2]);
		if (norm != 0) {
			dirFoc[0] /= norm;
			dirFoc[1] /= norm;
			dirFoc[2] /= norm;
		}
		GLfloat AmbFoc[] = { 0.05, 0.05, 0.05, 1 };
		GLfloat DifFoc[] = { 0.9, 0.9, 1, 1 };
		GLfloat SpecFoc[] = { 0.9, 0.9, 0.9, 1 };

		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_AMBIENT, AmbFoc);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, DifFoc);
		glLightfv(GL_LIGHT2, GL_SPECULAR, SpecFoc);
		GLfloat posFoc1[] = { cam[0] + cos(giro) * 3, cam[1] - sin(giro) * 3, cam[2], 1 };
		glLightfv(GL_LIGHT2, GL_POSITION, posFoc1);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dirFoc);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 10);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 60);

		glEnable(GL_LIGHT3);
		glLightfv(GL_LIGHT3, GL_AMBIENT, AmbFoc);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, DifFoc);
		glLightfv(GL_LIGHT3, GL_SPECULAR, SpecFoc);
		GLfloat posFoc2[] = { cam[0] - cos(giro) * 3, cam[1] + sin(giro) * 3, cam[2], 1 };
		glLightfv(GL_LIGHT3, GL_POSITION, posFoc2);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, dirFoc);
		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 10);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 60);
	}
	else
	{
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
	}

	glShadeModel(GL_SMOOTH);

	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	glEnable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);

	//Entorno espacial
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, textura_espacio);
		glCallList(space);
	glPopMatrix();

	glEnable(GL_LIGHTING);

	//Plataforma
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, textura_suelo);
		glCallList(suelo);
	glPopMatrix();

	//Meteoritos
	for (int i = 0; i < NUM_METS; i++) {
		if (!mets[i].explotando) {
			glBindTexture(GL_TEXTURE_2D, textura_meteoritos);

			glPushMatrix();
				glTranslatef(mets[i].pos[0], mets[i].pos[1], mets[i].pos[2]);
				glRotatef(angulo, mets[i].giro[0], mets[i].giro[1], mets[i].giro[2]);
				glScalef(mets[i].tamaño, mets[i].tamaño, mets[i].tamaño);
				glCallList(meteor);
			glPopMatrix();
	}

	//Cabina
	if (cabina)
	{
		glBindTexture(GL_TEXTURE_2D, textura_cabina);

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glMatrixMode(GL_PROJECTION);

		glPushMatrix();
			glLoadIdentity();
			glOrtho(0, Wx, 0, Wy, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
				glLoadIdentity();
				glColor4f(1, 1, 1, 1);
				glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex2f(0, 0);
				glTexCoord2f(1, 0); glVertex2f(Wx, 0);
				glTexCoord2f(1, 1); glVertex2f(Wx, Wy);
				glTexCoord2f(0, 1); glVertex2f(0, Wy);
				glEnd();
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);

		glDisable(GL_BLEND);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, (float) Wx / (float) Wy, 1.0, 3000.0);
		glMatrixMode(GL_MODELVIEW);
	}

	glDisable(GL_TEXTURE_2D);

	mostrarFPS();

	glutSwapBuffers();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float) w / (float) h, 1, 3000);
	Wx = w; Wy = h;
}

void teclado(unsigned char tecla, int x, int y)
{
	switch (tecla)
	{
	case 'a': if (velocidad < 15) velocidad += 1; break;
	case 'z': if (velocidad > 0) velocidad -= 1; break;
	case 'l': luces = !luces; if (luces) printf("Luces activadas\n"); else printf("Luces desactivadas\n"); break;
	case 'c': cabina = !cabina; if (cabina) printf("Cabina activada\n"); else printf("Cabina desactivada\n"); break;
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
	giro = (float)x / Wx * 2 * PI;
	altura = (float)(Wy / 2 - y) / (Wy / 2);
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	FreeImage_Initialise();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(Wx, Wy);
	glutInitWindowPosition(200, 50);
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
	FreeImage_DeInitialise();
}