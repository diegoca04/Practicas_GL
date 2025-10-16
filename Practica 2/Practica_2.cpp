#define PROYECTO "Practica 2"

#include <iostream>	
#include <cmath>
#include <codebase.h>

// Funcion de atencion al evento de dibujo
void display()
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT); // Borra la pantalla
	// Dibujo como POLYGON relleno
	glColor3f(1.0, 0.0, 0.0);
	GLuint anilloMenor, anilloMayor;
	anilloMenor = anillo(1.5, 0.8, 0.2, 30);
	anilloMayor = anillo(2, 1, 0.2, 40);
	glCallList(anilloMenor);
	glCallList(anilloMayor);
	// Dibujo como POLYGON solo aristas
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(2.0);
	glColor3f(0.0, 0.0, 1.0);
	glCallList(anilloMenor);
	glCallList(anilloMayor);
	// Dibujo como POLYGON solo vertices
	glPolygonMode(GL_FRONT, GL_POINT);
	glPointSize(10.0);
	glColor3f(0.0, 1.0, 0.0);
	glCallList(anilloMenor);
	glCallList(anilloMayor);
	glFlush();
}

// Funcion de atencion al redimensionamiento
void reshape(GLint w, GLint h)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv); // Inicializacion de GLUT
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // Alta de buffers a usar
	glutInitWindowSize(400, 400); // Tamanyo inicial de la ventana
	glutInitWindowPosition(50, 200);
	glutCreateWindow(PROYECTO); // Creacion de la ventana con su titulo
	std::cout << PROYECTO << " running" << std::endl; // Mensaje por consola 
	glutDisplayFunc(display); // Alta de la funcion de atencion a display
	glutReshapeFunc(reshape); // Alta de la funcion de atencion a reshape
	glutMainLoop(); // Puesta en marcha del programa 
}

GLuint anillo(float diamExt, float diamInt, float grosor, float res = 20) {
		GLuint anillo; // Identificador del objeto
		anillo = glGenLists(1); // Obtiene el identificador de la lista
		glNewList(anillo, GL_COMPILE); // Abre la lista
		// Dibuja el pentagono en la lista
		glBegin(GL_POLYGON);
		glVertex3f(0.5 * cos(0 * 2 * 3.1415926 / 5), 0.5 * sin(0 * 2 * 3.1415926 / 5), 0.0);
		glVertex3f(0.5 * cos(1 * 2 * 3.1415926 / 5), 0.5 * sin(1 * 2 * 3.1415926 / 5), 0.0);
		glVertex3f(0.5 * cos(2 * 2 * 3.1415926 / 5), 0.5 * sin(2 * 2 * 3.1415926 / 5), 0.0);
		glVertex3f(0.5 * cos(3 * 2 * 3.1415926 / 5), 0.5 * sin(3 * 2 * 3.1415926 / 5), 0.0);
		glVertex3f(0.5 * cos(4 * 2 * 3.1415926 / 5), 0.5 * sin(4 * 2 * 3.1415926 / 5), 0.0);
		glEnd();
		glEndList(); // Cierra la lista
		return anillo;
}