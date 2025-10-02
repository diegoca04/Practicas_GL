#define PROYECTO "Primer programa en OpenGL"

#include <iostream>	
#include <codebase.h>


// Funcion de atencion al evento de dibujo
void display()
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}
	
// Funcion de atencion al redimensionamiento
void reshape(GLint w, GLint h)
{
}

int main(int argc, char** argv)
{
	// Inicializaciones
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 400);
	glutInitWindowPosition(50, 600);

	// Crear ventana
	glutCreateWindow(PROYECTO);

	// Registrar callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	// Bucle de atencion a eventos
	glutMainLoop();
}