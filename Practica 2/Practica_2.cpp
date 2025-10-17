#define PROYECTO "Practica 2"

#include <iostream>	
#include <cmath>
#include <codebase.h>

GLuint anilloMenor, anilloMayor;

GLuint anillo(float diamExt, float diamInt, float grosor, float res = 20) {
	GLuint id = glGenLists(1); // Identificador del objeto
	glNewList(id, GL_COMPILE); // Abre la lista
	glBegin(GL_POLYGON);// Dibuja el pentagono en la lista
	glVertex3f(0.5 * cos(0 * 2 * 3.1415926 / 5), 0.5 * sin(0 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(1 * 2 * 3.1415926 / 5), 0.5 * sin(1 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(2 * 2 * 3.1415926 / 5), 0.5 * sin(2 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(3 * 2 * 3.1415926 / 5), 0.5 * sin(3 * 2 * 3.1415926 / 5), 0.0);
	glVertex3f(0.5 * cos(4 * 2 * 3.1415926 / 5), 0.5 * sin(4 * 2 * 3.1415926 / 5), 0.0);
	glEnd();
	glEndList(); // Cierra la lista
	return id;
}

void init()
// Funcion de inicializacion propia
{
	anilloMenor = anillo(1.5, 0.8, 0.2, 30);
	anilloMayor = anillo(2, 1, 0.2, 40);
}

// Funcion de atencion al evento de dibujo
void display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	// Dibujo como POLYGON relleno
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor3f(1.0, 0.0, 0.0);
	anilloMenor = anillo(1.5, 0.8, 0.2, 30);
	anilloMayor = anillo(2, 1, 0.2, 40);
	glCallList(anilloMenor);
	glCallList(anilloMayor);
}

// Funcion de atencion al redimensionamiento
void reshape(GLint w, GLint h)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv); // Inicializacion de GLUT
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // Alta de buffers a usar
	glutInitWindowSize(500, 500); // Tamanyo inicial de la ventana
	glutInitWindowPosition(1000, 100);
	glutCreateWindow(PROYECTO); // Creacion de la ventana con su titulo
	init();
	//std::cout << PROYECTO << " running" << std::endl; // Mensaje por consola 
	glutDisplayFunc(display); // Alta de la funcion de atencion a display
	glutReshapeFunc(reshape); // Alta de la funcion de atencion a reshape
	glutMainLoop(); // Puesta en marcha del programa 
}