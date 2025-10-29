#define PROYECTO "Practica 2"

#include <iostream>	
#include <cmath>
#include <codebase.h>

GLuint anilloMenor, anilloMayor;

const GLfloat vertices[];

const GLuint indices[];

int resolucion = 20;

GLuint anillo(float diamExt, float diamInt, float grosor, float res = 20) 
{
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);
	resolucion = res;
	//vertices = puntosCircunferencia(20, diamInt, 0);
	glEndList();
	return id;
}

/*std::vector<cb::Vec3> puntosCircunferencia(int numeroPuntos, float radio, float fase)
{
	return {}
}*/

void init()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	anilloMenor = anillo(1.5, 0.8, 0.2, 30);
	anilloMayor = anillo(2, 1, 0.2, 40);
}

void display()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
	anilloMenor = anillo(1.5, 0.8, 0.2, 30);
	anilloMayor = anillo(2, 1, 0.2, 40);

	//glCallList(anilloMenor);
	//glCallList(anilloMayor);
	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_QUADS, resolucion, GL_UNSIGNED_INT, indices);
	
	glColor3f(1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_QUADS, resolucion, GL_UNSIGNED_INT, indices);
	//glCallList(anilloMenor);
	//glCallList(anilloMayor);

	glFlush();
}

void reshape(GLint w, GLint h)
{
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(1000, 100);
	glutCreateWindow(PROYECTO);
	init();
	std::cout << PROYECTO << " running" << std::endl;
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}