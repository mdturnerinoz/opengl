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
/* File for "A Sample Game: Crab Pong" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <math.h>
#include <sstream>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "game.h"
#include "gamedrawer.h"
#include "imageloader.h"
#include "md2model.h"
#include "text3d.h"

using namespace std;

namespace {
	const float PI = 3.1415926535f;
	
	//The amount of time by which the state of the game is advanced in each call
	//to the step() method
	const float STEP_TIME = 0.01f;
	//The duration of a single loop of the walking animation
	const float WALK_ANIM_TIME = 0.4f;
	//The duration of a single loop of the standing animation
	const float STAND_ANIM_TIME = 2.0f;
	//The amount of time it takes for a crab that has just been eliminated to
	//shrink and disappear
	const float CRAB_FADE_OUT_TIME = 1.5f;
	//The number of points used to approximate a circle in the barrier model
	const int NUM_BARRIER_POINTS = 30;
	//The number of points used to approximate a circle in the pole model
	const int NUM_POLE_POINTS = 6;
	//The height of the barrier model
	const float BARRIER_HEIGHT = BARRIER_SIZE;
	//The radius of the pole model
	const float POLE_RADIUS = 0.02f;
	//The height of the center of a pole above the ground
	const float POLE_HEIGHT = 0.07f;
	//The number of units the human player's crab model should be translated in
	//the z direction, and that other crabs should similarly be translated
	const float CRAB_OFFSET = -0.032f;
	//The amount of time until the water travels WATER_TEXTURE_SIZE units
	const float WATER_TEXTURE_TIME = 8.0f;
	//The length of a single repetition of the water texture image
	const float WATER_TEXTURE_SIZE = 0.7f;
	//The opacity of the water
	const float WATER_ALPHA = 0.8f;
	
	//Makes the image into a texture, and returns the id of the texture
	GLuint loadTexture(Image *image) {
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGB,
					 image->width, image->height,
					 0,
					 GL_RGB,
					 GL_UNSIGNED_BYTE,
					 image->pixels);
		return textureId;
	}
}

GameDrawer::GameDrawer() {
	game = NULL;
	playerCrabDir = 0;
	
	//Start a new placeholder game with a maximum score of 0
	startNewGame(0, 0);
	
	waterTextureOffset = 0;
	
	vector<const char*> textureFilenames;
	textureFilenames.push_back("crab1.bmp");
	textureFilenames.push_back("crab2.bmp");
	textureFilenames.push_back("crab3.bmp");
	textureFilenames.push_back("crab4.bmp");
	crabModel = MD2Model::load("crab.md2", textureFilenames);
	
	setupBarriers();
	setupPole();
	
	Image* image = loadBMP("sand.bmp");
	sandTextureId = loadTexture(image);
	delete image;
	
	image = loadBMP("water.bmp");
	waterTextureId = loadTexture(image);
	delete image;
}

GameDrawer::~GameDrawer() {
	delete game;
}

void GameDrawer::setGame(Game* game1) {
	if (game != NULL) {
		delete game;
	}
	
	game = game1;
	game->setPlayerCrabDir(playerCrabDir);
	timeUntilNextStep = 0;
	
	isGameOver0 = (game->score(0) == 0);
	waitingForFirstGame = isGameOver0;
	
	for(int i = 0; i < 4; i++) {
		animTimes[i] = 0;
		
		if (!isGameOver0) {
			crabFadeAmounts[i] = 1;
		}
		else {
			crabFadeAmounts[i] = 0;
		}
		
		Crab* crab = game->crabs()[i];
		if (crab != NULL) {
			oldCrabPos[i] = game->crabs()[i]->pos();
		}
		else {
			oldCrabPos[i] = 0.5f;
		}
	}
}

void GameDrawer::setupBarriers() {
	Image* image = loadBMP("vtr.bmp");
	GLuint textureId = loadTexture(image);
	delete image;
	
	GLuint barrierDisplayListId = glGenLists(1);
	glNewList(barrierDisplayListId, GL_COMPILE);
	
	//Draw the top circle
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	GLfloat materialColor[] = {1, 1, 1, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor);
	
	glNormal3f(0, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(0, BARRIER_HEIGHT, 0);
	for(int i = NUM_BARRIER_POINTS; i >= 0; i--) {
		float angle = 2 * PI * (float)i / (float)NUM_BARRIER_POINTS;
		glTexCoord2f(-cos(angle) / 2 + 0.5f, sin(angle) / 2 + 0.5f);
		glVertex3f(BARRIER_SIZE * cos(angle),
				   BARRIER_HEIGHT,
				   BARRIER_SIZE * sin(angle));
	}
	glEnd();
	
	//Draw the bottom circle
	GLfloat materialColor2[] = {1.0f, 0.0f, 0.0f, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor2);
	
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.0f, 0.0f);
	glNormal3f(0, -1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for(int i = 0; i <= NUM_BARRIER_POINTS; i++) {
		float angle = 2 * PI * (float)i / (float)NUM_BARRIER_POINTS;
		glVertex3f(BARRIER_SIZE * cos(angle), 0, BARRIER_SIZE * sin(angle));
	}
	glEnd();
	
	//Draw the cylinder part
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i <= NUM_BARRIER_POINTS; i++) {
		float angle = 2 * PI * ((float)i - 0.5f) / (float)NUM_BARRIER_POINTS;
		glNormal3f(cos(angle), 0, sin(angle));
		float angle2 = 2 * PI * (float)i / (float)NUM_BARRIER_POINTS;
		glVertex3f(BARRIER_SIZE * cos(angle2), 0, BARRIER_SIZE * sin(angle2));
		glVertex3f(BARRIER_SIZE * cos(angle2),
				   BARRIER_HEIGHT,
				   BARRIER_SIZE * sin(angle2));
	}
	glEnd();
	glEndList();
	
	
	
	//Make a display list with four copies of the barrier
	barriersDisplayListId = glGenLists(1);
	glNewList(barriersDisplayListId, GL_COMPILE);
	glDisable(GL_COLOR_MATERIAL);
	
	//Add a little specularity
	GLfloat materialSpecular[] = {1, 1, 1, 1};
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, 15.0f);
	
	for(float z = 0; z < 2; z++) {
		for(float x = 0; x < 2; x++) {
			glPushMatrix();
			glTranslatef(x, 0, z);
			glCallList(barrierDisplayListId);
			glPopMatrix();
		}
	}
	glEnable(GL_COLOR_MATERIAL);
	
	//Disable specularity
	GLfloat materialColor3[] = {1, 1, 1, 1};
	GLfloat materialSpecular2[] = {0, 0, 0, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materialColor3);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular2);
	
	glEndList();
}

void GameDrawer::setupPole() {
	poleDisplayListId = glGenLists(1);
	glNewList(poleDisplayListId, GL_COMPILE);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0f, 0.8f, 0.0f);
	
	//Draw the left circle
	glNormal3f(-1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(BARRIER_SIZE, POLE_HEIGHT, -POLE_RADIUS);
	for(int i = NUM_POLE_POINTS; i >= 0; i--) {
		float angle = 2 * PI * (float)i / (float)NUM_POLE_POINTS;
		glVertex3f(BARRIER_SIZE,
				   POLE_HEIGHT + POLE_RADIUS * cos(angle),
				   POLE_RADIUS * (sin(angle) - 1));
	}
	glEnd();
	
	//Draw the right circle
	glNormal3f(1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(1 - BARRIER_SIZE, POLE_HEIGHT, -POLE_RADIUS);
	for(int i = 0; i <= NUM_POLE_POINTS; i++) {
		float angle = 2 * PI * (float)i / (float)NUM_POLE_POINTS;
		glVertex3f(1 - BARRIER_SIZE,
				   POLE_HEIGHT + POLE_RADIUS * cos(angle),
				   POLE_RADIUS * (sin(angle) - 1));
	}
	glEnd();
	
	//Draw the cylinder part
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i <= NUM_POLE_POINTS; i++) {
		float angle = 2 * PI * ((float)i - 0.5f) / (float)NUM_POLE_POINTS;
		glNormal3f(0, cos(angle), sin(angle));
		float angle2 = 2 * PI * (float)i / (float)NUM_POLE_POINTS;
		glVertex3f(1 - BARRIER_SIZE,
				   POLE_HEIGHT + POLE_RADIUS * cos(angle2),
				   POLE_RADIUS * (sin(angle2) - 1));
		glVertex3f(BARRIER_SIZE,
				   POLE_HEIGHT + POLE_RADIUS * cos(angle2),
				   POLE_RADIUS * (sin(angle2) - 1));
	}
	glEnd();
	
	glEndList();
}

void GameDrawer::step() {
	//Advance the game
	game->advance(STEP_TIME);
	
	//Advance the water
	waterTextureOffset += STEP_TIME / WATER_TEXTURE_TIME;
	while (waterTextureOffset > WATER_TEXTURE_SIZE) {
		waterTextureOffset -= WATER_TEXTURE_SIZE;
	}
	
	//Update animTimes, crabFadeAmounts, and isGameOver0
	bool opponentAlive = false;
	for(int i = 0; i < 4; i++) {
		Crab* crab = game->crabs()[i];
		
		if (crab != NULL) {
			oldCrabPos[i] = crab->pos();
		}
		
		//Update animation time
		if (crab != NULL || crabFadeAmounts[i] > 0) {
			if (crab != NULL && crab->dir() != 0) {
				if (crab->dir() > 0) {
					animTimes[i] += STEP_TIME / WALK_ANIM_TIME;
				}
				else {
					animTimes[i] -= STEP_TIME / WALK_ANIM_TIME;
				}
			}
			else {
				animTimes[i] += STEP_TIME / STAND_ANIM_TIME;
			}
			
			while (animTimes[i] > 1) {
				animTimes[i] -= 1;
			}
			while (animTimes[i] < 0) {
				animTimes[i] += 1;
			}
		}
		
		//Update fade amount
		if (crab == NULL) {
			crabFadeAmounts[i] -= STEP_TIME / CRAB_FADE_OUT_TIME;
			if (crabFadeAmounts[i] < 0) {
				if (i == 0) {
					isGameOver0 = true;
				}
				crabFadeAmounts[i] = 0;
			}
			else if (i != 0) {
				opponentAlive = true;
			}
		}
		else if (i != 0) {
			opponentAlive = true;
		}
	}
	
	if (!opponentAlive) {
		isGameOver0 = true;
	}
}

void GameDrawer::setupLighting() {
	GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	//Put one light above each of the four corners
	int index = 0;
	for(float z = 0; z < 2; z += 1) {
		for(float x = 0; x < 2; x += 1) {
			glEnable(GL_LIGHT0 + index);
			GLfloat lightColor[] = {0.2f, 0.2f, 0.2f, 1};
			GLfloat lightPos[] = {x, 1.5f, z, 1};
			glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, lightColor);
			glLightfv(GL_LIGHT0 + index, GL_SPECULAR, lightColor);
			glLightfv(GL_LIGHT0 + index, GL_POSITION, lightPos);
			index++;
		}
	}
}

void GameDrawer::drawCrabsAndPoles(bool isReflected) {
	if (crabModel != NULL) {
		glEnable(GL_NORMALIZE);
		for(int i = 0; i < 4; i++) {
			Crab* crab = game->crabs()[i];
			
			//Translate and rotate to the appropriate side of the board
			glPushMatrix();
			switch(i) {
				case 1:
					glTranslatef(0, 0, 1);
					glRotatef(90, 0, 1, 0);
					break;
				case 2:
					glTranslatef(1, 0, 1);
					glRotatef(180, 0, 1, 0);
					break;
				case 3:
					glTranslatef(1, 0, 0);
					glRotatef(270, 0, 1, 0);
					break;
			}
			
			if (crab != NULL || crabFadeAmounts[i] > 0) {
				//Draw the crab
				glPushMatrix();
				
				float crabPos;
				if (crab != NULL) {
					crabPos = crab->pos();
				}
				else {
					crabPos = oldCrabPos[i];
				}
				glTranslatef(crabPos, 0.055f, CRAB_OFFSET);
				if (crab == NULL) {
					//Used for the shrinking effect, whereby crabs shrink
					//until they disappear when they are eliminated from play
					glTranslatef(0, -0.055f * (1 - crabFadeAmounts[i]), 0);
					glScalef(crabFadeAmounts[i],
							 crabFadeAmounts[i],
							 crabFadeAmounts[i]);
				}
				
				glRotatef(-90, 0, 1, 0);
				glRotatef(-90, 1, 0, 0);
				glScalef(0.05f, 0.05f, 0.05f);
				
				if (crab == NULL || crab->dir() == 0) {
					crabModel->setAnimation("stand");
				}
				else {
					crabModel->setAnimation("run");
				}
				
				glColor3f(1, 1, 1);
				crabModel->draw(i, animTimes[i]);
				glPopMatrix();
			}
			
			if (crab == NULL) {
				//Draw the pole
				if (isReflected) {
					glDisable(GL_NORMALIZE);
				}
				glCallList(poleDisplayListId);
				if (isReflected) {
					glEnable(GL_NORMALIZE);
				}
			}
			
			glPopMatrix();
		}
	}
}

void GameDrawer::drawBarriers(bool isReflected) {
	if (isReflected) {
		glEnable(GL_NORMALIZE);
	}
	else {
		glDisable(GL_NORMALIZE);
	}
	glCallList(barriersDisplayListId);
}

void GameDrawer::drawScores(bool isReflected) {
	float d = 0.1f;
	for(int i = 0; i < 4; i++) {
		ostringstream oss;
		oss << game->score(i);
		string str = oss.str();
		
		glPushMatrix();
		int hAlign;
		int vAlign;
		switch(i) {
			case 0:
				glTranslatef(0.5f, 0, -d);
				hAlign = 0;
				vAlign = -1;
				break;
			case 1:
				glTranslatef(-d, 0, 0.5f);
				hAlign = -1;
				vAlign = 0;
				break;
			case 2:
				glTranslatef(0.5f, 0, 1 + d);
				hAlign = 0;
				vAlign = 1;
				break;
			case 3:
				glTranslatef(1 + d, 0, 0.5f);
				hAlign = 1;
				vAlign = 0;
				break;
		}
		
		glTranslatef(0, 0.04f, 0);
		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		glScalef(0.1f, 0.1f, 0.1f);
		t3dDraw3D(str, hAlign, vAlign, 0.3f);
		glPopMatrix();
	}
}

void GameDrawer::drawBalls(bool isReflected) {
	if (isReflected) {
		glEnable(GL_NORMALIZE);
	}
	else {
		glDisable(GL_NORMALIZE);
	}
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	vector<Ball*> balls = game->balls();
	for(unsigned int i = 0; i < balls.size(); i++) {
		Ball* ball = balls[i];
		
		if (ball->fadeAmount() < 1) {
			glEnable(GL_BLEND);
			glColor4f(0.75f, 0.75f, 0.75f, ball->fadeAmount());
		}
		else {
			glColor3f(0.75f, 0.75f, 0.75f);
		}
		
		glPushMatrix();
		glTranslatef(ball->x(), ball->radius() + 0.01f, ball->z());
		glutSolidSphere(ball->radius(), 10, 6);
		glPopMatrix();
		
		if (ball->fadeAmount() < 1) {
			glDisable(GL_BLEND);
		}
	}
}

void GameDrawer::drawReflectableObjects(bool isReflected) {
	drawCrabsAndPoles(isReflected);
	drawBarriers(isReflected);
	drawScores(isReflected);
	drawBalls(isReflected);
}

void GameDrawer::drawSand() {
	//The height of the sand above the water
	float height = 0.01f;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sandTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glDisable(GL_NORMALIZE);
	glColor3f(1, 1, 1);
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(0, height, 0);
	glTexCoord2f(1, 1);
	glVertex3f(0, height, 1);
	glTexCoord2f(0, 1);
	glVertex3f(1, height, 1);
	glTexCoord2f(0, 0);
	glVertex3f(1, height, 0);
	glEnd();
}

void GameDrawer::drawWater() {
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, waterTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glDisable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glColor4f(1, 1, 1, WATER_ALPHA);
	glNormal3f(0, 1, 0);
	
	glBegin(GL_QUADS);
	glTexCoord2f(200 / WATER_TEXTURE_SIZE,
				 -waterTextureOffset / WATER_TEXTURE_SIZE);
	glVertex3f(-100, 0, -100);
	glTexCoord2f(200 / WATER_TEXTURE_SIZE,
				 (200 - waterTextureOffset) / WATER_TEXTURE_SIZE);
	glVertex3f(-100, 0, 100);
	glTexCoord2f(0, (200 - waterTextureOffset) / WATER_TEXTURE_SIZE);
	glVertex3f(100, 0, 100);
	glTexCoord2f(0, -waterTextureOffset / WATER_TEXTURE_SIZE);
	glVertex3f(100, 0, -100);
	glEnd();
	
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void GameDrawer::drawWinner() {
	if (!isGameOver0) {
		return;
	}
	
	glColor3f(0.2f, 0.2f, 1.0f);
	
	if (!waitingForFirstGame) {
		//Draw the winner
		string str;
		if (game->score(0) > 0) {
			str = "You win!";
		}
		else {
			str = "You lose.";
		}
		
		glPushMatrix();
		glTranslatef(0.5f, 0.05f, 0.5f);
		glScalef(0.1f, 0.1f, 0.1f);
		glTranslatef(0, 0.15f, 0);
		glRotatef(180, 0, 1, 0);
		glRotatef(-90, 1, 0, 0);
		t3dDraw3D(str, 0, 0, 0.3f);
		glPopMatrix();
	}
	
	//Draw instructions
	glPushMatrix();
	glTranslatef(0.5f, 0.05f, 0.35f);
	glScalef(0.05f, 0.05f, 0.05f);
	glTranslatef(0, 0.15f, 0);
	glRotatef(180, 0, 1, 0);
	glRotatef(-90, 1, 0, 0);
	t3dDraw3D("Press ENTER for a new game\nPress ESC to quit\n"
			  "(Use left and right to move)", 0, -1, 0.3f);
	glPopMatrix();
}

void GameDrawer::draw() {
	//Set the background to be sky blue
	glClearColor(0.7f, 0.9f, 1.0f, 1);
	
	//Draw reflections
	glCullFace(GL_FRONT);
	glPushMatrix();
	glScalef(1, -1, 1);
	setupLighting();
	drawReflectableObjects(true);
	glPopMatrix();
	
	//Draw normally
	glCullFace(GL_BACK);
	setupLighting();
	drawWater();
	drawSand();
	drawWinner();
	drawReflectableObjects(false);
}

void GameDrawer::advance(float dt) {
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

void GameDrawer::setPlayerCrabDir(int dir) {
	playerCrabDir = dir;
	game->setPlayerCrabDir(dir);
}

bool GameDrawer::isGameOver() {
	return isGameOver0;
}

void GameDrawer::startNewGame(float maximumSpeedForOpponents,
							  int startingScore) {
	setGame(new Game(maximumSpeedForOpponents, startingScore));
}

void initGameDrawer() {
	t3dInit();
}

void cleanupGameDrawer() {
	t3dCleanup();
}










