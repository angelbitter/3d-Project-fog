#include <Windows.h>

#include <algorithm> // Para usar la funcion sort()
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <SOIL2/SOIL2.h>

#include "vec3f.h"

using namespace std;

// De nuevo este codigo esta basado en AlphaBlending, comentaremos las lineas de codigo relativas a la niebla
// para facilitar el seguimiento de los nuevos conceptos presentados en este codigo, para encontrar informacion
// sobre los demas conceptos, vaya al proyecto AlphaBlending o al Word a la seccion apropiada

// VARIABLES GLOBALES Y STRUCTS
const float PI = 3.1415926535f;
const float BOX_SIZE = 7.0f; // Longitud de cada cara del cubo
const float ALPHA = 0.6f; // Opacidad


struct Face {
	Vec3f up;
	Vec3f right;
	Vec3f out;
};

struct Cube {
	Face top;
	Face bottom;
	Face left;
	Face right;
	Face front;
	Face back;
};


GLuint _textureId;
Cube _cube;

// FIN STRUCTS Y VARIABLES GLOBALES


// Nos permite rotar un vector undeterminado numero de grados en un determinado eje
Vec3f rotate(Vec3f v, Vec3f axis, float degrees) {
	axis = axis.normalize();
	float radians = degrees * PI / 180;
	float s = sin(radians);
	float c = cos(radians);
	return v * c + axis * axis.dot(v) * (1 - c) + v.cross(axis) * s;
}

// Como la anterior pero para caras del cubo
void rotate(Face& face, Vec3f axis, float degrees) {
	face.up = rotate(face.up, axis, degrees);
	face.right = rotate(face.right, axis, degrees);
	face.out = rotate(face.out, axis, degrees);
}

// Como la anterior pero para el cubo completo
void rotate(Cube& cube, Vec3f axis, float degrees) {
	rotate(cube.top, axis, degrees);
	rotate(cube.bottom, axis, degrees);
	rotate(cube.left, axis, degrees);
	rotate(cube.right, axis, degrees);
	rotate(cube.front, axis, degrees);
	rotate(cube.back, axis, degrees);
}

// Damos valores iniciales a las caras del cubo 
void initCube(Cube& cube) {
	cube.top.up = Vec3f(0, 0, -1);
	cube.top.right = Vec3f(1, 0, 0);
	cube.top.out = Vec3f(0, 1, 0);

	cube.bottom.up = Vec3f(0, 0, 1);
	cube.bottom.right = Vec3f(1, 0, 0);
	cube.bottom.out = Vec3f(0, -1, 0);

	cube.left.up = Vec3f(0, 0, -1);
	cube.left.right = Vec3f(0, 1, 0);
	cube.left.out = Vec3f(-1, 0, 0);

	cube.right.up = Vec3f(0, -1, 0);
	cube.right.right = Vec3f(0, 0, 1);
	cube.right.out = Vec3f(1, 0, 0);

	cube.front.up = Vec3f(0, 1, 0);
	cube.front.right = Vec3f(1, 0, 0);
	cube.front.out = Vec3f(0, 0, 1);

	cube.back.up = Vec3f(1, 0, 0);
	cube.back.right = Vec3f(0, 1, 0);
	cube.back.out = Vec3f(0, 0, -1);
}

// Funcion auxiliar que nos permite determinar si una cara se encuentra detras de otra
// Esto funciona porque el out de las caras se encuentra en el centro de las mismas, para otros poligonos no tiene
// por que ser asi, pero para cubos si
bool compareFaces(Face* face1, Face* face2) {
	return face1->out[2] < face2->out[2];
}

// esta funcion nos hace la vida mas facil ya que calcula los 4 vertices (esquinas) de una cara y las almacena en el
// vector vs
void faceVertices(Face& face, Vec3f* vs) {
	vs[0] = BOX_SIZE / 2 * (face.out - face.right - face.up);
	vs[1] = BOX_SIZE / 2 * (face.out - face.right + face.up);
	vs[2] = BOX_SIZE / 2 * (face.out + face.right + face.up);
	vs[3] = BOX_SIZE / 2 * (face.out + face.right - face.up);
}

void drawTopFace(Face& face, GLuint textureId) {
	Vec3f vs[4];
	faceVertices(face, vs);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f(1.0f, 1.0f, 1.0f, ALPHA);

	glBegin(GL_QUADS);

	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glTexCoord2f(0, 0);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glTexCoord2f(0, 1);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glTexCoord2f(1, 1);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glTexCoord2f(1, 0);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);

	glEnd();
}

void drawBottomFace(Face& face, GLuint textureId) {
	Vec3f vs[4];
	faceVertices(face, vs);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f(1.0f, 1.0f, 1.0f, ALPHA);

	glBegin(GL_QUADS);

	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glTexCoord2f(0, 0);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glTexCoord2f(0, 1);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glTexCoord2f(1, 1);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glTexCoord2f(1, 0);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);

	glEnd();
}

void drawLeftFace(Face& face, GLuint textureId) {
	Vec3f vs[4];
	faceVertices(face, vs);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f(1.0f, 1.0f, 1.0f, ALPHA);

	glBegin(GL_QUADS);

	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glTexCoord2f(0, 0);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glTexCoord2f(0, 1);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glTexCoord2f(1, 1);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glTexCoord2f(1, 0);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);

	glEnd();
}

void drawRightFace(Face& face, GLuint textureId) {
	Vec3f vs[4];
	faceVertices(face, vs);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f(1.0f, 1.0f, 1.0f, ALPHA);

	glBegin(GL_QUADS);

	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glTexCoord2f(0, 0);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glTexCoord2f(0, 1);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glTexCoord2f(1, 1);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glTexCoord2f(1, 0);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);

	glEnd();
}

void drawFrontFace(Face& face, GLuint textureId) {
	Vec3f vs[4];
	faceVertices(face, vs);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f(1.0f, 1.0f, 1.0f, ALPHA);

	glBegin(GL_QUADS);

	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glTexCoord2f(0, 0);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glTexCoord2f(0, 1);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glTexCoord2f(1, 1);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glTexCoord2f(1, 0);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);

	glEnd();
}

void drawBackFace(Face& face, GLuint textureId) {
	Vec3f vs[4];
	faceVertices(face, vs);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor4f(1.0f, 1.0f, 1.0f, ALPHA);

	glBegin(GL_QUADS);

	glNormal3f(face.out[0], face.out[1], face.out[2]);
	glTexCoord2f(0, 0);
	glVertex3f(vs[0][0], vs[0][1], vs[0][2]);
	glTexCoord2f(0, 1);
	glVertex3f(vs[1][0], vs[1][1], vs[1][2]);
	glTexCoord2f(1, 1);
	glVertex3f(vs[2][0], vs[2][1], vs[2][2]);
	glTexCoord2f(1, 0);
	glVertex3f(vs[3][0], vs[3][1], vs[3][2]);

	glEnd();
}

//Draws the indicated face on the specified cube.
void drawFace(Face* face, Cube& cube, GLuint textureId) {

	// Aqui dibujamos las cara pasada como argumento en su respectiva funcion los diferentes ifs ayudan a 
	// determinar que cara es cual y asi aplicar el codigo respectivo a cada una de ellas
	if (face == &(cube.top)) {
		drawTopFace(cube.top, textureId);
	}
	else if (face == &(cube.bottom)) {
		drawBottomFace(cube.bottom, textureId);
	}
	else if (face == &(cube.left)) {
		drawLeftFace(cube.left, textureId);
	}
	else if (face == &(cube.right)) {
		drawRightFace(cube.right, textureId);
	}
	else if (face == &(cube.front)) {
		drawFrontFace(cube.front, textureId);
	}
	else {
		drawBackFace(cube.back, textureId);
	}
}


void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
	}
}

// Convierte la Image en textura y devuelve su ID
GLuint loadTexture(const char* path) {
	GLuint textureId = SOIL_load_OGL_texture
	(
		path,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y

	);
	return textureId;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_FOG); // Para permitir el uso de niebla

	glClearColor(0.5f, 0.5f, 0.5f, 1); // Hacemos un color de fondo gris para ayudar a integrar la niebla

	_textureId = loadTexture("TexturaVidriera1.jpeg");
}

// Control de ajuste del tamaño de la ventana
void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}


void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -20.0f);

	// Niebla
	GLfloat fogColor[] = { 0.5f, 0.5f, 0.5f, 1 }; // Usamos el mismo color de niebla que el que usamos de background
	// Aqui establecemos los parametros de la niebla, el primero de todos es este mismo color
	glFogfv(GL_FOG_COLOR, fogColor);

	// Aqui llega lo interesante, le decimos que queremos una niebla linear, esto es: Hasta cierta distancia
	// cercana no se aplica niebla, al traspasarla, se incrementa la niebla linearmente hasta una distancia 
	// lejana maxima, a partir de esa distancia maxima la niebla adquiere su valor maximo
	// Nota: Cuando decimos que la niebla aumenta o disminuye nos referimos a cuanto se aplica el color de 
	// la niebla (gris en este caso) al vertice procesado, este  valor oscila entre 0 (para el valor de distancia 
	// mas cercana e inferiores) hasta 1 (para el valor de distancia maxima y superiores)
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 10.0f);
	glFogf(GL_FOG_END, 20.0f);

	// Luces
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	GLfloat lightColor[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat lightPos[] = { -2 * BOX_SIZE, BOX_SIZE, 4 * BOX_SIZE, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Añadimos todas las caras en el orden que sea
	vector<Face*> faces;
	faces.push_back(&(_cube.top));
	faces.push_back(&(_cube.bottom));
	faces.push_back(&(_cube.left));
	faces.push_back(&(_cube.right));
	faces.push_back(&(_cube.front));
	faces.push_back(&(_cube.back));

	//Aqui las ordenamos de mas atras a mas adelante
	sort(faces.begin(), faces.end(), compareFaces);

	// Y ahora, una vez ordenadas las dibujamos
	for (unsigned int i = 0; i < faces.size(); i++) {
		drawFace(faces[i], _cube, _textureId);
	}

	glutSwapBuffers();
}

// LLamada cada 25ms, la usamos para rotar el cubo constantemente, por eso llamamos a update dentro de si misma
// para dentro de los siguientes 25ms
void update(int value) {
	rotate(_cube, Vec3f(1, 1, 0), 1);

	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);

	glutCreateWindow("Niebla");
	initRendering();
	initCube(_cube);

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);

	glutMainLoop();
	return 0;
}
