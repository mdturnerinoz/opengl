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
/* File for "Particle Systems" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "imageloader.h"
#include "vec3f.h"

using namespace std;

const float PI = 3.1415926535f;

//Returns a random float from 0 to < 1
float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

//Represents a single particle.
struct Particle {
	Vec3f pos;
	Vec3f velocity;
	Vec3f color;
	float timeAlive; //The amount of time that this particle has been alive.
	float lifespan;  //The total amount of time that this particle is to live.
};

//Rotates the vector by the indicated number of degrees about the specified axis
Vec3f rotate(Vec3f v, Vec3f axis, float degrees) {
	axis = axis.normalize();
	float radians = degrees * PI / 180;
	float s = sin(radians);
	float c = cos(radians);
	return v * c + axis * axis.dot(v) * (1 - c) + v.cross(axis) * s;
}

//Returns the position of the particle, after rotating the camera
Vec3f adjParticlePos(Vec3f pos) {
	return rotate(pos, Vec3f(1, 0, 0), -30);
}

//Returns whether particle1 is in back of particle2
bool compareParticles(Particle* particle1, Particle* particle2) {
	return adjParticlePos(particle1->pos)[2] <
		adjParticlePos(particle2->pos)[2];
}

const float GRAVITY = 3.0f;
const int NUM_PARTICLES = 1000;
//The interval of time, in seconds, by which the particle engine periodically
//steps.
const float STEP_TIME = 0.01f;
//The length of the sides of the quadrilateral drawn for each particle.
const float PARTICLE_SIZE = 0.05f;

class ParticleEngine {
	private:
		GLuint textureId;
		Particle particles[NUM_PARTICLES];
		//The amount of time until the next call to step().
		float timeUntilNextStep;
		//The color of particles that the fountain is currently shooting.  0
		//indicates red, and when it reaches 1, it starts over at red again.  It
		//always lies between 0 and 1.
		float colorTime;
		//The angle at which the fountain is shooting particles, in radians.
		float angle;
		
		//Returns the current color of particles produced by the fountain.
		Vec3f curColor() {
			Vec3f color;
			if (colorTime < 0.166667f) {
				color = Vec3f(1.0f, colorTime * 6, 0.0f);
			}
			else if (colorTime < 0.333333f) {
				color = Vec3f((0.333333f - colorTime) * 6, 1.0f, 0.0f);
			}
			else if (colorTime < 0.5f) {
				color = Vec3f(0.0f, 1.0f, (colorTime - 0.333333f) * 6);
			}
			else if (colorTime < 0.666667f) {
				color = Vec3f(0.0f, (0.666667f - colorTime) * 6, 1.0f);
			}
			else if (colorTime < 0.833333f) {
				color = Vec3f((colorTime - 0.666667f) * 6, 0.0f, 1.0f);
			}
			else {
				color = Vec3f(1.0f, 0.0f, (1.0f - colorTime) * 6);
			}
			
			//Make sure each of the color's components range from 0 to 1
			for(int i = 0; i < 3; i++) {
				if (color[i] < 0) {
					color[i] = 0;
				}
				else if (color[i] > 1) {
					color[i] = 1;
				}
			}
			
			return color;
		}
		
		//Returns the average velocity of particles produced by the fountain.
		Vec3f curVelocity() {
			return Vec3f(2 * cos(angle), 2.0f, 2 * sin(angle));
		}
		
		//Alters p to be a particle newly produced by the fountain.
		void createParticle(Particle* p) {
			p->pos = Vec3f(0, 0, 0);
			p->velocity = curVelocity() + Vec3f(0.5f * randomFloat() - 0.25f,
												0.5f * randomFloat() - 0.25f,
												0.5f * randomFloat() - 0.25f);
			p->color = curColor();
			p->timeAlive = 0;
			p->lifespan = randomFloat() + 1;
		}
		
		//Advances the particle fountain by STEP_TIME seconds.
		void step() {
			colorTime += STEP_TIME / 10;
			while (colorTime >= 1) {
				colorTime -= 1;
			}
			
			angle += 0.5f * STEP_TIME;
			while (angle > 2 * PI) {
				angle -= 2 * PI;
			}
			
			for(int i = 0; i < NUM_PARTICLES; i++) {
				Particle* p = particles + i;
				
				p->pos += p->velocity * STEP_TIME;
				p->velocity += Vec3f(0.0f, -GRAVITY * STEP_TIME, 0.0f);
				p->timeAlive += STEP_TIME;
				if (p->timeAlive > p->lifespan) {
					createParticle(p);
				}
			}
		}
	public:
		ParticleEngine(GLuint textureId1) {
			textureId = textureId1;
			timeUntilNextStep = 0;
			colorTime = 0;
			angle = 0;
			for(int i = 0; i < NUM_PARTICLES; i++) {
				createParticle(particles + i);
			}
			for(int i = 0; i < 5 / STEP_TIME; i++) {
				step();
			}
		}
		
		//Advances the particle fountain by the specified amount of time.
		void advance(float dt) {
			while (dt > 0) {
				if (timeUntilNextStep < dt) {
					dt -= timeUntilNextStep;
					step();
					timeUntilNextStep = STEP_TIME;
				}
				else {
					timeUntilNextStep -= dt;
					dt = 0;
				}
			}
		}
		
		//Draws the particle fountain.
		void draw() {
			vector<Particle*> ps;
			for(int i = 0; i < NUM_PARTICLES; i++) {
				ps.push_back(particles + i);
			}
			sort(ps.begin(), ps.end(), compareParticles);
			
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			glBegin(GL_QUADS);
			for(unsigned int i = 0; i < ps.size(); i++) {
				Particle* p = ps[i];
				glColor4f(p->color[0], p->color[1], p->color[2],
						  (1 - p->timeAlive / p->lifespan));
				float size = PARTICLE_SIZE / 2;
				
				Vec3f pos = adjParticlePos(p->pos);
				
				glTexCoord2f(0, 0);
				glVertex3f(pos[0] - size, pos[1] - size, pos[2]);
				glTexCoord2f(0, 1);
				glVertex3f(pos[0] - size, pos[1] + size, pos[2]);
				glTexCoord2f(1, 1);
				glVertex3f(pos[0] + size, pos[1] + size, pos[2]);
				glTexCoord2f(1, 0);
				glVertex3f(pos[0] + size, pos[1] - size, pos[2]);
			}
			glEnd();
		}
};





const int TIMER_MS = 25; //The number of milliseconds to which the timer is set

ParticleEngine* _particleEngine;
GLuint _textureId;

void cleanup() {
	delete _particleEngine;
}

//Called when a key is pressed
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanup();
			exit(0);
	}
}

//Returns an array indicating pixel data for an RGBA image that is the same as
//image, but with an alpha channel indicated by the grayscale image alphaChannel
char* addAlphaChannel(Image* image, Image* alphaChannel) {
	char* pixels = new char[image->width * image->height * 4];
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			for(int j = 0; j < 3; j++) {
				pixels[4 * (y * image->width + x) + j] =
					image->pixels[3 * (y * image->width + x) + j];
			}
			pixels[4 * (y * image->width + x) + 3] =
				alphaChannel->pixels[3 * (y * image->width + x)];
		}
	}
	
	return pixels;
}

//Makes the image into a texture, using the specified grayscale image as an
//alpha channel and returns the id of the texture
GLuint loadAlphaTexture(Image* image, Image* alphaChannel) {
	char* pixels = addAlphaChannel(image, alphaChannel);
	
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 image->width, image->height,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 pixels);
	
	delete pixels;
	return textureId;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Image* image = loadBMP("circle.bmp");
	Image* alphaChannel = loadBMP("circlealpha.bmp");
	_textureId = loadAlphaTexture(image, alphaChannel);
	delete image;
	delete alphaChannel;
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -10.0f);
	glScalef(2.0f, 2.0f, 2.0f);
	
	_particleEngine->draw();
	
	glutSwapBuffers();
}

void update(int value) {
	_particleEngine->advance(TIMER_MS / 1000.0f);
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

int main(int argc, char** argv) {
	srand((unsigned int)time(0)); //Seed the random number generator
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Particle Systems - videotutorialsrock.com");
	initRendering();
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	
	_particleEngine = new ParticleEngine(_textureId);
	
	glutTimerFunc(TIMER_MS, update, 0);
	
	glutMainLoop();
	return 0;
}









