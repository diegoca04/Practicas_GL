#define PROYECTO "Practica 8"

#include <iostream>	
#include <sstream>
#include <cmath>
#include <codebase.h>
#include <gl\freeglut.h>
#include <vector>
#include <freeimage/FreeImage.h>

GLuint suelo;

const int DIM_ESPACIO = 400;

const int NUM_METS = 20;

const int Wx = 1200, Wy = 700;

static int FPS = 60;

static float cam[9] = { 0, 0, 2, 0, 1, 2, 0, 0, 1 };

static double velocidad = 1;

static float giro = 0, altura = 0, angulo = 0;

static bool luces = false;

struct Meteorito {
	float pos[3];
	float dir[3];
	float init[3];
	float tamaño;
	float velocidad;
};

Meteorito mets[NUM_METS];

GLuint plano(int res, int pos) {
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);

	std::vector<cb::Vec3> vertices = {};

	float div = DIM_ESPACIO / res;
	for (int i = 0; i <= res; i++) {
		for (int j = 0; j <= res; j++) {
			cb::Vec3 v;
			v.x = -DIM_ESPACIO / 2 + i * div;
			v.y = -DIM_ESPACIO / 2 + j * div;
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_VERTEX_ARRAY);

	glEndList();
	return id;
}

void loadImageFile(char* nombre)
// Uso de FreeImage para cargar la imagen en cualquier formato
// nombre: nombre del fichero con extensión en el mismo directorio que el proyecto
// o con su path completo
{
	// Detección del formato, lectura y conversion a BGRA
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(nombre, 0);
	FIBITMAP* imagen = FreeImage_Load(formato, nombre);
	FIBITMAP* imagen32b = FreeImage_ConvertTo32Bits(imagen);
	// Lectura de dimensiones y colores
	int w = FreeImage_GetWidth(imagen32b);
	int h = FreeImage_GetHeight(imagen32b);
	GLubyte* texeles = FreeImage_GetBits(imagen32b);
	// Carga como textura actual
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, texeles);
	// Liberar recursos
	FreeImage_Unload(imagen);
	FreeImage_Unload(imagen32b);
}

void saveScreenshot(char* nombre, int ancho, int alto)
// Utiliza FreeImage para grabar un png
// nombre: Nombre del fichero con extensión p.e. salida.png
// ancho: Ancho del viewport en pixeles
// alto: Alto del viewport en pixeles
{
	int pix = ancho * alto;
	BYTE* pixels = new BYTE[3 * pix];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, ancho, alto, GL_BGR, GL_UNSIGNED_BYTE, pixels);
	FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels, ancho, alto,
		ancho * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
	FreeImage_Save(FIF_PNG, img, nombre, 0);
	delete pixels;
}

void loadTexture()
// Funcion de carga de texturas e inicializacion
{
	//1a. GENERAR UN OBJETO TEXTURA
	GLuint tex0;
	glGenTextures(1, &tex0);
	//1b. ACTIVAR EL OBJETO TEXTURA
	glBindTexture(GL_TEXTURE_2D, tex0);
	//1c. CARGAR LA IMAGEN QUE SERVIRA DE TEXTURA
	FREE_IMAGE_FORMAT formato = FreeImage_GetFileType("upv.jpg", 0);
	// Automatically detects the format
	FIBITMAP* imagen = FreeImage_Load(formato, "upv.jpg");
	if (imagen == NULL) cerr << endl << "NO SE ENCONTRO LA IMAGEN" << endl;
	FIBITMAP* imagen32b = FreeImage_ConvertTo32Bits(imagen);
	int w = FreeImage_GetWidth(imagen32b);
	int h = FreeImage_GetHeight(imagen32b);
	GLubyte* pixeles = FreeImage_GetBits(imagen32b);
	// FreeImage loads in BGR format, so you need GL_BGRA.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixeles);
	FreeImage_Unload(imagen);
	FreeImage_Unload(imagen32b);
	//1d. HABILITAR LAS TEXTURAS
	glEnable(GL_TEXTURE_2D);
	//2a. SELECCIONAR EL OBJETO TEXTURA
	//glBindTexture(GL_TEXTURE_2D,tex0
	//2b. DEFINIR COMO SE APLICARÁ LA TEXTURA EN ESE OBJETO
	// Texel menor que pixel
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Texel mayor que pixel
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// La textura se repite en abcisas
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// La textura se repite en ordenadas
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//2c. DEFINIR LA FORMA DE COMBINAR
	// Asigna solo el color de la textura al fragmento
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void onIdle()
{
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

	angulo += velocidad * giro / 1000;

	cam[3] = cos(angulo) * 2000;
	cam[4] = sin(angulo) * 2000;
	cam[5] += altura * velocidad;
	if (cam[5] > 1000) cam[5] = 1000;
	if (cam[5] < -1000) cam[5] = -1000;

	cam[0] += cam[3] * velocidad / 100000;
	if (cam[0] > 200) cam[0] = 200;
	if (cam[0] < -200) cam[0] = -200;

	cam[1] += cam[4] * velocidad / 100000;
	if (cam[1] > 200) cam[1] = 200;
	if (cam[1] < -200) cam[1] = -200;

	cam[2] += cam[5] * velocidad / 100000;
	if (cam[2] > 50) cam[2] = 50;
	if (cam[2] < 2) cam[2] = 2;

	for (int i = 0; i < NUM_METS; i++) {
		mets[i].pos[0] += (mets[i].dir[0] * mets[i].velocidad / 1000);
		mets[i].pos[1] += (mets[i].dir[1] * mets[i].velocidad / 1000);
		mets[i].pos[2] += (mets[i].dir[2] * mets[i].velocidad / 1000);
		if (fabs(mets[i].pos[0]) > 200 - mets[i].tamaño / 2 || fabs(mets[i].pos[1]) > 200 - mets[i].tamaño / 2
			|| mets[i].pos[2] < mets[i].tamaño) {
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
		mets[i].dir[2] = cb::random(-4, -1);

		mets[i].pos[0] = cb::random(- 150, 150);
		mets[i].pos[1] = cb::random(- 150, 150);
		mets[i].pos[2] = cb::random(150, 200);

		mets[i].init[0] = mets[i].pos[0];
		mets[i].init[1] = mets[i].pos[1];
		mets[i].init[2] = mets[i].pos[2];

		mets[i].velocidad = cb::random(10, 30);

		mets[i].tamaño = cb::random(2, 15);
	}

	//texturaSuelo = loadTexture("suelo.jpg");
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
	GLfloat AmbSol[] = { 0.2, 0.2, 0.2, 1 };
	GLfloat DifSol[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat SpecSol[] = { 0.9, 0.9, 0.9, 1 };
	GLfloat posSol[] = { 1000, 1000, 1000, 0 };
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
		GLfloat DifFoc[] = { 1, 1, 1, 1 };
		GLfloat SpecFoc[] = { 1, 1, 1, 1 };

		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_AMBIENT, AmbFoc);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, DifFoc);
		glLightfv(GL_LIGHT2, GL_SPECULAR, SpecFoc);
		GLfloat posFoc1[] = { cam[0] + sin(angulo) * 3, cam[1] - cos(angulo) * 3, cam[2], 1 };
		glLightfv(GL_LIGHT2, GL_POSITION, posFoc1);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dirFoc);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 7);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 60);

		glEnable(GL_LIGHT3);
		glLightfv(GL_LIGHT3, GL_AMBIENT, AmbFoc);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, DifFoc);
		glLightfv(GL_LIGHT3, GL_SPECULAR, SpecFoc);
		GLfloat posFoc2[] = { cam[0] - sin(angulo) * 3, cam[1] + cos(angulo) * 3, cam[2], 1 };
		glLightfv(GL_LIGHT3, GL_POSITION, posFoc2);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, dirFoc);
		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 7);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 60);
	}
	else
	{
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
	}

	glShadeModel(GL_FLAT);

	glPushMatrix();
		glNormal3f(0, 0, 1);
		glCallList(suelo);
	glPopMatrix();
	
	for (int i = 0; i < NUM_METS; i++) {
		glPushMatrix();
			glTranslatef(mets[i].pos[0], mets[i].pos[1], mets[i].pos[2]);
			glutSolidSphere(mets[i].tamaño, 16, 16);
		glPopMatrix();
	}

	glutSwapBuffers();
}

void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, w / h, 1, 400);
}

void teclado(unsigned char tecla, int x, int y)
{
	switch (tecla)
	{
	case 'a': if (velocidad < 5) velocidad += 0.5; break;
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
	giro = (float)(Wx / 2 - x) / (Wx / 2);
	if (giro < 0.05 && giro > -0.05) giro = 0;
	altura = (float)(Wy / 2 - y) / Wy;
	if (altura < 0.05 && altura > -0.05) altura = 0;
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