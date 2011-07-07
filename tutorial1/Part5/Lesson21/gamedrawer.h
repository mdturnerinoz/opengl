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



#ifndef GAME_DRAWER_H_INCLUDED
#define GAME_DRAWER_H_INCLUDED

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Game;
class MD2Model;

//Maitains the state of the game by using an enclosed Game object, and takes
//care of drawing the game
class GameDrawer {
	private:
		//The Game object maintaining the state of the game.  If no game has yet
		//been started, this is a placeholder game for which the maximum score
		//is 0.  If the program is waiting for the user to start a new game,
		//this is the game that was just finished.
		Game* game;
		//The model for the crab
		MD2Model* crabModel;
		//The id of the display list for the four barriers at the corners
		GLuint barriersDisplayListId;
		//The id of the display list for a "pole" drawn when a crab has been
		//eliminated.  It is drawn for the side connecting (0, 0, 0) and
		//(1, 0, 0).
		GLuint poleDisplayListId;
		//The id of the texture for the sand
		GLuint sandTextureId;
		//The id of the texture for the water
		GLuint waterTextureId;
		//The fraction that each crab is "faded in", from 0 to 1.  An element is
		//not 1 when the corresponding crab is shrinking or completely
		//disappeared after having been eliminated.
		float crabFadeAmounts[4];
		//The animation times for the crab model for the four crabs
		float animTimes[4];
		//The last known position of each crab.  Kept for when crabs become NULL
		//after being eliminated from play.
		float oldCrabPos[4];
		//The distance that the water has traveled, modulo the size of each
		//repetition of the water texture
		float waterTextureOffset;
		//Whether the game is currently over
		bool isGameOver0;
		//Whether any game has been started or finished
		bool waitingForFirstGame;
		//A negative number if the human-controlled crab is accelerating toward
		//the corner on its right, a positive number if it is accelerating
		//toward the other corner, and 0 if it is decelerating
		int playerCrabDir;
		//The amount of game time until the next call to step()
		float timeUntilNextStep;
		
		//Switches to use the specified Game object
		void setGame(Game* game);
		//Sets up the display list for the four barriers at the corners
		void setupBarriers();
		//Sets up the display list for the pole
		void setupPole();
		//Advances the state of the game by a short amount of time
		void step();
		
		//Sets up the lighting in OpenGL
		void setupLighting();
		//Draws the crabs and poles.  isReflected indicates whether the
		//reflections of the crabs and poles are being drawn rather than the
		//crabs and poles themselves.
		void drawCrabsAndPoles(bool isReflected);
		//Draws the four barriers at the corners.  isReflected indicates whether
		//the reflections of the barriers are being drawn rather than the
		//barriers themselves.
		void drawBarriers(bool isReflected);
		//Draws the players' scores.  isReflected indicates whether the
		//reflections of the scores are being drawn rather than the scores
		//themselves.
		void drawScores(bool isReflected);
		//Draws the balls.  isReflected indicates whether the reflections of the
		//balls are being drawn rather than the balls themselves.
		void drawBalls(bool isReflected);
		//Draws all of the objects that have reflections in the water.
		//isReflected indicates whether the reflections of the objects are being
		//drawn rather than the objects themselves.
		void drawReflectableObjects(bool isReflected);
		//Draws the sand
		void drawSand();
		//Draws the water, blending it onto the screen
		void drawWater();
		//Draws text indicating the winner of the game and/or some instructions
		//if a game is not currently in progress
		void drawWinner();
	public:
		GameDrawer();
		~GameDrawer();
		
		//Draws the game, positioned according to board coordinates, so that the
		//board's corners are at (0, 0, 0), (0, 0, 1), (1, 0, 0), and (1, 0, 1).
		void draw();
		//Advances the state of the game by the specified amount of time
		void advance(float dt);
		//Sets the direction toward which the human-controlled crab is
		//accelerating.  A negative number indicates to accelerate toward the
		//corner on its right, a positive number indicates to accelerate toward
		//the other corner, and 0 indicates to decelerate.
		void setPlayerCrabDir(int dir);
		//Returns false iff a game is not currently being played
		bool isGameOver();
		//Starts a new game with the specified starting score.
		//maximumSpeedForOpponents is the maximum speed at which the computer-
		//controlled crabs move.  It can be used to control the difficulty of
		//the opponents.
		void startNewGame(float maximumSpeedForOpponents, int startingScore);
};

//Performs some initialization required for the GameDrawer class to function
//properly.
void initGameDrawer();
//Frees some resources used by the GameDrawer class after a call to
//initGameDrawer().
void cleanupGameDrawer();










#endif
