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



/* The game board is a square with corners (0, 0), (0, 1), (1, 0), and (1, 1) in
 * the x-z plane.  There are balls and four crabs (paddles) in the game board.
 * There is one crab of length CRAB_LENGTH on each edge of the square.  Each
 * crab other than the first, the one on the edge connecting (0, 0) and (1, 0),
 * is controlled by the computer.  A player is scored on when a ball reaches the
 * player's side, but the crab isn't there to block it.  When a player is scored
 * on, he loses a point.  When a player reaches 0 points, he's eliminated, and
 * the side where his crab was turns into a wall.  There are four circles of
 * radius BARRIER_SIZE centered at the corners that limit balls' and crabs'
 * positions.  There may be multiple balls in play at once.  New balls are added
 * to play by fading in at the center of the board.  As in the original Pong,
 * the direction of a ball after hitting a crab depends on the place where the
 * ball hit the crab.
 */

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <vector>

//The length of a crab
const float CRAB_LENGTH = 0.2f;
//The radius of the four barriers positioned at the corners
const float BARRIER_SIZE = 0.12f;

//Represents a single crab (paddle)
class Crab {
	private:
		//The maximum speed of the crab
		float maximumSpeed;
		//The distance from the center of the crab to the corner on its right
		float pos0;
		//-1 if the crab is accelerating toward the corner on its right, 1 if it
		//is accelerating toward the other corner, and 0 if it is decelerating
		int dir0;
		//The velocity of the crab
		float speed0;
		//The amount of game time until the next call to step()
		float timeUntilNextStep;
		
		//Advances the state of the crab by a short amount of time
		void step();
	public:
		//Constructs a new Crab with the specified maximum speed
		Crab(float maximumSpeed1);
		
		//Returns the distance from the center of the crab to the corner to its
		//right
		float pos();
		//Returns a negative number if the crab is accelerating toward the
		//corner on its right, a positive number if it is accelerating toward
		//the other corner, and 0 if it is decelerating
		int dir();
		//Returns the velocity of the crab
		float speed();
		//Returns the acceleration of the crab when it is accelerating or
		//decelerating
		float acceleration();
 		//Sets the direction toward which the crab is accelerating.  A negative
		//number indicates to accelerate toward the corner on its right, a
		//positive number indicates to accelerate toward the other corner, and 0
		//indicates to decelerate.
		void setDir(int dir1);
		//Advances the state of the crab by the specified amount of time
		void advance(float dt);
};

//Represents a single ball
class Ball {
	private:
		//The radius of the ball
		float r;
		//The x coordinate of the ball
		float x0;
		//The z coordinate of the ball
		float z0;
		//The angle at which the ball is traveling.  An angle of 0 indicates the
		//positive x direction, while an angle of PI / 2 indicates the positive
		//z direction.  The angle is always between 0 and 2 * PI.
		float angle0;
		//The fraction that the ball is "faded in", from 0 to 1.  It is not 1
		//when the ball is fading in or out.
		float fadeAmount0;
		//Whether the ball is fading out
		bool isFadingOut0;
	public:
		//Constructs a new ball with the specified radius, x and z coordinates,
		//and angle of travel.  An angle of 0 indicates the positive x
		//direction, while an angle of PI / 2 indicates the positive z
		//direction.  The angle must be between 0 and 2 * PI.
		Ball(float radius1, float x1, float z1, float angle1);
		
		//Returns the radius of the ball
		float radius();
		//Returns the x coordinate of the ball
		float x();
		//Returns the z coordinate of the ball
		float z();
		//Returns the angle at which the ball is traveling.  An angle of 0
		//indicates the positive x direction, while an angle of PI / 2 indicates
		//the positive z direction.  The returned angle is between 0 and 2 * PI.
		float angle();
		//Sets the angle at which the ball is traveling.  An angle of 0
		//indicates the positive x direction, while an angle of PI / 2 indicates
		//the positive z direction.  The angle must be between 0 and 2 * PI.
		void setAngle(float angle1);
		//Returns the fraction that the ball is "faded in", from 0 to 1.  It is
		//not 1 when the ball is fading in or out.
		float fadeAmount();
		//Causes the ball to begin fading out
		void fadeOut();
		//Returns whether the ball is fading out
		bool isFadingOut();
		//Advances the state of the ball by the specified amount of time
		void advance(float dt);
};

//Manages the state of the game
class Game {
	private:
		//The four crabs.  The first is the one on the edge connecting (0, 0)
		//and (1, 0), and each subsequent crab is one edge counterclockwise with
		//respect to the previous one.  A crab is NULL if it has been eliminated
		//by reaching a score of 0.
		Crab* crabs0[4];
		//The balls that are currently in play
		std::vector<Ball*> balls0;
		//The scores for each of the players
		int scores[4];
		//The amount of game time until the next call to step()
		float timeUntilNextStep;
		
		//Handles all collisions at the current instant
		void handleCollisions();
		//Adjusts the directions in which the computer-controlled crabs are
		//accelerating, using some AI
		void doAI();
		//Advances the state of the game by a short amount of time
		void step();
	public:
		//Constructs a new Game with the specified starting score.
		//maximumSpeedForOpponents is the maximum speed at which the computer-
		//controlled crabs move.  It can be used to control the difficulty of
		//the opponents.
		Game(float maximumSpeedForOpponents, int startingScore);
		~Game();
		
		//Sets the direction toward which the human-controlled crab is
		//accelerating.  A negative number indicates to accelerate toward the
		//corner on its right, a positive number indicates to accelerate toward
		//the other corner, and 0 indicates to decelerate.
		void setPlayerCrabDir(int dir);
		//Returns the score of the specified player
		int score(int crabNum);
		//Advances the state of the game by the specified amount of time
		void advance(float dt);
		//Returns the four crabs.  The first is the one on the edge connecting
		//(0, 0) and (1, 0), and each subsequent crab is one edge
		//counterclockwise with respect to the previous one.  A crab is NULL if
		//it has been eliminated by reaching a score of 0.
		Crab** crabs();
		//Returns the balls that are currently in play
		std::vector<Ball*> balls();
};










#endif
