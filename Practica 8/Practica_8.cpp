#define PROYECTO "Practica 8"

#include <iostream>	
#include <sstream>
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>
#include <freeimage/FreeImage.h>

GLuint suelo, textura_suelo, textura_meteoritos, textura_espacio;

const int DIM_ESPACIO = 100;

const int NUM_METS = 20;

const int Wx = 1200, Wy = 700;

static int FPS = 60;

static float cam[9] = { 0, 0, 2, 0, 1, 2, 0, 0, 1 };

static double velocidad = 0;

static float giro = 0, altura = 0;

static bool luces = false;

float prueba = 0;

struct Meteorito {
	float pos[3];
	float dir[3];
	float init[3];
	float tamaño;
	float velocidad;
};

Meteorito mets[NUM_METS];

GLuint loadTexture(const char* nombre)
{
	GLuint id;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	FIBITMAP* imagen = FreeImage_Load(FIF_JPEG, nombre);
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
	/*
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	const GLfloat planoS[] = { 1,0,0,0 };
	const GLfloat planoT[] = { 0,1,0,0 };
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planoS);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planoT);
	*/
	// Configura la proyeccion con camara ortgrafica estandar
	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -10, 10);
	// Dibuja un ppligono textura que ocupa toda la ventana
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); // Salva el estado
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT); // Salva el estado de habilitados
	glDisable(GL_DEPTH_TEST); // Deshabilita el z-buffer
	glDisable(GL_LIGHTING); // Deshabilita la iluminacion
	glEnable(GL_TEXTURE_2D); // Habilita las texturas por si acaso
	glDisable(GL_TEXTURE_GEN_S); // Deshabilita la generacion automatica por si acaso
	glDisable(GL_TEXTURE_GEN_T);
	glBegin(GL_POLYGON); // Quad texturado
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, 0);
	glEnd();
	glPopMatrix(); // Restablece la modelview
	glPopAttrib(); // Restablece lo que hubiera habilitado
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); // Restablece la projetion
	glMatrixMode(GL_MODELVIEW); // Pone la modelview como corriente*/
	return id;
}

GLuint plano(int res, int pos) {
	GLuint id = glGenLists(1);

	glNewList(id, GL_COMPILE);

	glBegin(GL_QUADS);

	glNormal3f(0, 0, 1);

	float size = DIM_ESPACIO / 2;
	glTexCoord2f(0, 0); glVertex3f(- size, - size, pos);
	glTexCoord2f(1, 0); glVertex3f(size, - size, pos);
	glTexCoord2f(1, 1); glVertex3f(size, size, pos);
	glTexCoord2f(0, 1); glVertex3f(- size, size, pos);

	glEnd();

	glEndList();
	return id;
}

void onIdle()
{
	prueba += 0.05;

	static int antes = 0;
	int ahora, tiempo_transcurrido;
	ahora = glutGet(GLUT_ELAPSED_TIME);
	tiempo_transcurrido = ahora - antes;

	FPS++;
	if (tiempo_transcurrido > 1000) {
		stringstream titulo; 
		titulo << PROYECTO << "		FPS: " << FPS;
		glutSetWindowTitle(titulo.str().c_str());
		FPS = 0;
		antes = ahora;
	}

	cam[3] = sin(giro) * 1000;
	cam[4] = cos(giro) * 1000;
	cam[5] = altura * 1000;

	cam[0] += cam[3] * velocidad / 100000;
	if (cam[0] > 400) cam[0] = 400;
	if (cam[0] < - 400) cam[0] = - 400;

	cam[1] += cam[4] * velocidad / 100000;
	if (cam[1] > 400) cam[1] = 400;
	if (cam[1] < - 400) cam[1] = - 400;

	cam[2] += cam[5] * velocidad / 100000;
	if (cam[2] > 150) cam[2] = 150;
	if (cam[2] < -50) cam[2] = - 50;

	if (cam[0] < DIM_ESPACIO / 2 + 2 && cam[0] > - DIM_ESPACIO / 2 - 2 && 
		cam[1] < DIM_ESPACIO / 2 + 2 && cam[1] > - DIM_ESPACIO / 2 - 2) {
		if (cam[2] < 2 && cam[2] > 0) cam[2] = 2;
		if (cam[2] > - 2 && cam[2] < 0) cam[2] = -2;
	}
	
	for (int i = 0; i < NUM_METS; i++) {
		mets[i].pos[0] += (mets[i].dir[0] * mets[i].velocidad / 1000);
		mets[i].pos[1] += (mets[i].dir[1] * mets[i].velocidad / 1000);
		mets[i].pos[2] += (mets[i].dir[2] * mets[i].velocidad / 1000);
		if (fabs(mets[i].pos[0]) > 400 - mets[i].tamaño / 2 || fabs(mets[i].pos[1]) 
			> 400 - mets[i].tamaño / 2 || fabs(mets[i].pos[2]) > 200) {
			mets[i].pos[0] = mets[i].init[0];
			mets[i].pos[1] = mets[i].init[1];
			mets[i].pos[2] = mets[i].init[2];
		}
	}

	glutPostRedisplay();
}

void init()
{
	suelo = plano(DIM_ESPACIO, 0);

	for (int i = 0; i < NUM_METS; i++) {
		mets[i].dir[0] = cb::random(-2, 2);
		mets[i].dir[1] = cb::random(-2, 2);
		mets[i].dir[2] = cb::random(-2, 2);

		mets[i].pos[0] = cb::random(- 250, 250);
		mets[i].pos[1] = cb::random(- 250, 250);
		mets[i].pos[2] = cb::random(- 250, 250);

		mets[i].init[0] = mets[i].pos[0];
		mets[i].init[1] = mets[i].pos[1];
		mets[i].init[2] = mets[i].pos[2];

		mets[i].velocidad = cb::random(10, 30);

		mets[i].tamaño = cb::random(2, 15);
	}

	textura_suelo = loadTexture("Texturas/Textura_suelo.jpg");
	textura_meteoritos = loadTexture("Texturas/Textura_meteorito.jpg");
	textura_espacio = loadTexture("Texturas/Textura_espacio.jpg");
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat material_emission[] = { 0.1, 0.1, 0.1, 1 };
	GLfloat material_ambient[] = { 0.2, 0.2, 0.2, 1 };
	GLfloat material_diffuse[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat material_specular[] = { 0.8, 0.8, 0.8, 1 };
	GLfloat material_shininess[] = { 30 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	gluLookAt(cam[0], cam[1], cam[2], cam[3], cam[4], cam[5], cam[6], cam[7], cam[8]);

	glEnable(GL_LIGHT1);
	GLfloat AmbSol[] = { 0.3, 0.3, 0.3, 1 };
	GLfloat DifSol[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat SpecSol[] = { 0.9, 0.9, 0.9, 1 };
	GLfloat posSol[] = { 1000, 1000, 1000, 1 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, AmbSol);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, DifSol);
	glLightfv(GL_LIGHT1, GL_SPECULAR, SpecSol);
	glLightfv(GL_LIGHT1, GL_POSITION, posSol);

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
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 7);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 100);

		glEnable(GL_LIGHT3);
		glLightfv(GL_LIGHT3, GL_AMBIENT, AmbFoc);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, DifFoc);
		glLightfv(GL_LIGHT3, GL_SPECULAR, SpecFoc);
		GLfloat posFoc2[] = { cam[0] - cos(giro) * 3, cam[1] + sin(giro) * 3, cam[2], 1 };
		glLightfv(GL_LIGHT3, GL_POSITION, posFoc2);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, dirFoc);
		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 8);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 80);
	}
	else
	{
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
	}

	//glShadeModel(GL_FLAT);

	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, textura_suelo);
		glCallList(suelo);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	for (int i = 0; i < NUM_METS; i++) {
		glPushMatrix();
			glTranslatef(mets[i].pos[0], mets[i].pos[1], mets[i].pos[2]);
			glScalef(mets[i].tamaño, mets[i].tamaño,mets[i].tamaño);
			glutSolidIcosahedron();
		glPopMatrix();
	}
	
	glutSwapBuffers();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, w / h, 1, 1000);
}

void teclado(unsigned char tecla, int x, int y)
{
	switch (tecla)
	{
	case 'a': if (velocidad < 10) velocidad += 1; break;
	case 'z': if (velocidad > 0) velocidad -= 1; break;
	case 'l': luces = !luces; if (luces) printf("Luces activadas\n"); else printf("Luces desactivadas\n"); break;
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
	giro = (float) x / Wx * 2 * PI;
	altura = (float) (Wy / 2 - y) / (Wy / 2);
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