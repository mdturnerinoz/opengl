/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Materials" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

const float RADIUS = 4.0f; //The radius of the sphere

float _angle = -60;

bool _highShininess = false; //Whether the shininess parameter is high
bool _lowSpecularity = false; //Whether the specularity parameter is high
bool _emission = false; //Whether the emission parameter is turned on

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			exit(0);
			break;
		case 's':
			_highShininess = !_highShininess;
			break;
		case 'p':
			_lowSpecularity = !_lowSpecularity;
			break;
		case 'e':
			_emission = !_emission;
			break;
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	//Disable color materials, so that glMaterial calls work
	glDisable(GL_COLOR_MATERIAL);
}

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
	glRotatef(_angle, 0, 1, 0);
	
	GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	GLfloat lightColor[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos[] = {1.5f * RADIUS, 2 * RADIUS, 1.5 * RADIUS, 1.0f};
	//Diffuse (non-shiny) light component
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	//Specular (shiny) light component
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
	//Determine the specularity, emissivity, and shininess parameters, based on
	//variables that can be toggled with keystrokes
	float specularity;
	if (_lowSpecularity) {
		specularity = 0.3f;
	}
	else {
		specularity = 1;
	}
	
	float emissivity;
	if (_emission) {
		emissivity = 0.05f;
	}
	else {
		emissivity = 0;
	}
	
	float shininess;
	if (_highShininess) {
		shininess = 25;
	}
	else {
		shininess = 12;
	}
	
	//The color of the sphere
	GLfloat materialColor[] = {0.2f, 0.2f, 1.0f, 1.0f};
	//The specular (shiny) component of the material
	GLfloat materialSpecular[] = {specularity, specularity, specularity, 1.0f};
	//The color emitted by the material
	GLfloat materialEmission[] = {emissivity, emissivity, emissivity, 1.0f};
	
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess); //The shininess parameter
	
	//Draw the sphere
	glutSolidSphere(RADIUS, 150, 80);
	
	glutSwapBuffers();
}

//Called every 25 milliseconds
void update(int value) {
	_angle += 1.5f;
	if (_angle > 360) {
		_angle -= 360;
	}
	
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Materials - videotutorialsrock.com");
	initRendering();
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









