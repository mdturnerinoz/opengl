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



#include <cmath>
#include <cstdlib>
#include <math.h>
#include <vector>

#include "game.h"

using namespace std;

namespace {
	const float PI = 3.1415926535f;
	
	//The amount of time by which the state of the crab is advanced in each call
	//to a crab's step() method
	const float CRAB_STEP_TIME = 0.01f;
	//The amount of time it takes for a crab to accelerate from no speed to its
	//maximum speed
	const float TIME_TO_MAXIMUM_SPEED = 0.18f;
	//The maximum angle formed by the direction at which a ball is hit off of a
	//crab and the normal direction for the crab
	const float MAX_CRAB_BOUNCE_ANGLE_OFFSET = 0.85f * PI / 2;
	//The maximum speed of the human-controlled crab
	const float PLAYER_MAXIMUM_SPEED = 2.2f;
	//The amount of time it takes for a ball to fade into play
	const float BALL_FADE_IN_TIME = 0.5f;
	//The amount of time it takes for a ball to fade out of play
	const float BALL_FADE_OUT_TIME = 0.5f;
	//The radius of the balls
	const float BALL_RADIUS = 0.04f;
	//The speed of the balls
	const float BALL_SPEED = 1.0f;
	//The ideal number of balls in play
	const int NUM_BALLS = 2;
	//The amount of time by which the state of the game is advanced in each call
	//to the game's step() method
	const float GAME_STEP_TIME = 0.01f;
	
	//Returns a random float from 0 to < 1
	float randomFloat() {
		return (float)rand() / ((float)RAND_MAX + 1);
	}
}

Crab::Crab(float maximumSpeed1) {
	maximumSpeed = maximumSpeed1;
	pos0 = 0.5f;
	dir0 = 0;
	speed0 = 0;
	timeUntilNextStep = 0;
}

void Crab::step() {
	//Accelerate the crab
	float ds = CRAB_STEP_TIME * acceleration();
	if (dir0 != 0) {
		speed0 += dir0 * ds;
		if (speed0 > maximumSpeed) {
			speed0 = maximumSpeed;
		}
		else if (speed0 < -maximumSpeed) {
			speed0 = -maximumSpeed;
		}
	}
	else {
		float s = abs(speed0);
		s -= ds;
		if (s < 0) {
			s = 0;
		}
		if (speed0 > 0) {
			speed0 = s;
		}
		else {
			speed0 = -s;
		}
	}
	
	//Move the crab
	pos0 += CRAB_STEP_TIME * speed0;
	if (pos0 < BARRIER_SIZE + CRAB_LENGTH / 2) {
		pos0 = BARRIER_SIZE + CRAB_LENGTH / 2;
		speed0 = 0;
	}
	else if (pos0 > 1 - BARRIER_SIZE - CRAB_LENGTH / 2) {
		pos0 = 1 - BARRIER_SIZE - CRAB_LENGTH / 2;
		speed0 = 0;
	}
}

float Crab::pos() {
	return pos0;
}

int Crab::dir() {
	return dir0;
}

float Crab::speed() {
	return speed0;
}

float Crab::acceleration() {
	return maximumSpeed / TIME_TO_MAXIMUM_SPEED;
}

void Crab::setDir(int dir1) {
	if (dir1 < 0) {
		dir0 = -1;
	}
	else if (dir1 > 0) {
		dir0 = 1;
	}
	else {
		dir0 = 0;
	}
}

void Crab::advance(float dt) {
	while (dt > 0) {
		if (timeUntilNextStep < dt) {
			dt -= timeUntilNextStep;
			step();
			timeUntilNextStep = CRAB_STEP_TIME;
		}
		else {
			timeUntilNextStep -= dt;
			dt = 0;
		}
	}
}





Ball::Ball(float radius1, float x1, float z1, float angle1) {
	r = radius1;
	x0 = x1;
	z0 = z1;
	angle0 = angle1;
	fadeAmount0 = 0;
	isFadingOut0 = false;
}

float Ball::radius() {
	return r;
}

float Ball::x() {
	return x0;
}

float Ball::z() {
	return z0;
}

float Ball::angle() {
	return angle0;
}

void Ball::setAngle(float angle1) {
	angle0 = angle1;
}

float Ball::fadeAmount() {
	return fadeAmount0;
}

void Ball::fadeOut() {
	isFadingOut0 = true;
}

bool Ball::isFadingOut() {
	return isFadingOut0;
}

void Ball::advance(float dt) {
	if (isFadingOut0) {
		//Fade out
		fadeAmount0 -= dt / BALL_FADE_OUT_TIME;
		if (fadeAmount0 < 0) {
			fadeAmount0 = 0;
		}
	}
	else if (fadeAmount0 < 1) {
		//Fade in
		fadeAmount0 += dt / BALL_FADE_IN_TIME;
		if (fadeAmount0 > 1) {
			dt = (fadeAmount0 - 1) * BALL_FADE_IN_TIME;
			fadeAmount0 = 1;
		}
		else {
			dt = 0;
		}
	}
	
	if (dt <= 0) {
		return;
	}
	
	//Advance the position of the ball
	x0 += dt * cos(angle0) * BALL_SPEED;
	z0 += dt * sin(angle0) * BALL_SPEED;
}





Game::Game(float maximumSpeedForOpponents, int startingScore) {
	if (startingScore > 0) {
		crabs0[0] = new Crab(PLAYER_MAXIMUM_SPEED);
		for(int i = 1; i < 4; i++) {
			crabs0[i] = new Crab(maximumSpeedForOpponents);
		}
	}
	else {
		for(int i = 0; i < 4; i++) {
			crabs0[i] = NULL;
		}
	}
	
	for(int i = 0; i < 4; i++) {
		scores[i] = startingScore;
	}
	timeUntilNextStep = 0;
}

Game::~Game() {
	for(int i = 0; i < 4; i++) {
		if (crabs0[i] != NULL) {
			delete crabs0[i];
		}
	}
	for(unsigned int i = 0; i < balls0.size(); i++) {
		delete balls0[i];
	}
}

namespace {
	//Returns whether the point (dx, dz) lies within r units of (0, 0)
	bool intersectsCircle(float dx, float dz, float r) {
		return dx * dx + dz * dz < r * r;
	}
	
	//Returns whether a ball is colliding with a circle that is dx units to the
	//right and dz units inward from it, where r is the sum of the radius of the
	//ball and the radius of the circle and (vx, vz) is the velocity of the ball
	bool collisionWithCircle(float dx, float dz, float r, float vx, float vz) {
		return intersectsCircle(dx, dz, r) && vx * dx + vz * dz > 0;
	}
	
	//Returns the resultant angle when an object traveling at an angle of angle
	//bounces off of a wall whose normal is at an angle of normal.  The returned
	//angle will be between 0 and 2 * PI.  An angle of 0 indicates the positive
	//x direction, and an angle of PI / 2 indicates the positive z direction.
	float reflect(float angle, float normal) {
		angle = 2 * normal - PI - angle;
		while (angle < 0) {
			angle += 2 * PI;
		}
		while (angle > 2 * PI) {
			angle -= 2 * PI;
		}
		return angle;
	}
	
	//Adjusts the ball's angle in response to a collision with a circle at the
	//specified position
	void collideWithCircle(Ball* ball, float x, float z) {
		if (ball->fadeAmount() < 1) {
			return;
		}
		
		float dx = x - ball->x();
		float dz = z - ball->z();
		float normal = atan2(-dz, -dx);
		float newBallAngle = reflect(ball->angle(), normal);
		if (newBallAngle < 0) {
			newBallAngle += 2 * PI;
		}
		else if (newBallAngle > 2 * PI) {
			newBallAngle -= 2 * PI;
		}
		ball->setAngle(newBallAngle);
	}
	
	//Returns whether a crab at the indicated position has intercepted a ball at
	//the indicated position, where the positions are measured parallel to the
	//direction in which the crab moves
	bool collisionWithCrab(float crabPos, float ballPos) {
		return abs(crabPos - ballPos) < CRAB_LENGTH / 2;
	}
	
	//Adjusts the ball's angle in response to a collision with a crab.  The
	//positions are measured parallel to the direction in which the crab moves,
	//and the crab's position is its distance from its center to the corner to
	//its right.
	void collideWithCrab(Ball* ball,
						 int crabIndex,
						 float crabPos,
						 float ballPos) {
		float angle = (1 - crabIndex) * PI / 2 +
			MAX_CRAB_BOUNCE_ANGLE_OFFSET *
				(crabPos - ballPos) / (CRAB_LENGTH / 2);
		while (angle < 0) {
			angle += 2 * PI;
		}
		while (angle >= 2 * PI) {
			angle -= 2 * PI;
		}
		ball->setAngle(angle);
	}
}

void Game::handleCollisions() {
	for(unsigned int i = 0; i < balls0.size(); i++) {
		Ball* ball = balls0[i];
		
		if (ball->fadeAmount() < 1 || ball->isFadingOut()) {
			continue;
		}
		
		//Ball-barrier collisions
		for(float z = 0; z < 2; z += 1) {
			for(float x = 0; x < 2; x += 1) {
				if (collisionWithCircle(x - ball->x(), z - ball->z(),
										ball->radius() + BARRIER_SIZE,
										BALL_SPEED * cos(ball->angle()),
										BALL_SPEED * sin(ball->angle()))) {
					collideWithCircle(ball, x, z);
				}
			}
		}
		
		//Ball-ball collisions
		for(unsigned int j = i + 1; j < balls0.size(); j++) {
			Ball* ball2 = balls0[j];
			if (collisionWithCircle(ball2->x() - ball->x(),
									ball2->z() - ball->z(),
									ball->radius() + ball2->radius(),
									BALL_SPEED * (cos(ball->angle()) -
												  cos(ball2->angle())),
									BALL_SPEED * (sin(ball->angle()) -
												  sin(ball2->angle())))) {
				collideWithCircle(ball, ball2->x(), ball2->z());
				collideWithCircle(ball2, ball->x(), ball->z());
			}
		}
		
		//Ball-crab (and ball-pole) collisions
		int crabIndex;
		float ballPos;
		if (ball->z() < ball->radius()) {
			crabIndex = 0;
			ballPos = ball->x();
		}
		else if (ball->x() < ball->radius()) {
			crabIndex = 1;
			ballPos = 1 - ball->z();
		}
		else if (ball->z() > 1 - ball->radius()) {
			crabIndex = 2;
			ballPos = 1 - ball->x();
		}
		else if (ball->x() > 1 - ball->radius()) {
			crabIndex = 3;
			ballPos = ball->z();
		}
		else {
			crabIndex = -1;
			ballPos = 0;
		}
		
		if (crabIndex >= 0) {
			if (crabs0[crabIndex] != NULL) {
				float crabPos = crabs0[crabIndex]->pos();
				if (collisionWithCrab(crabPos, ballPos)) {
					collideWithCrab(ball, crabIndex, crabPos, ballPos);
				}
			}
			else {
				float normal = (1 - crabIndex) * PI / 2;
				float newAngle = reflect(ball->angle(), normal);
				ball->setAngle(newAngle);
			}
		}
	}
}

namespace {
	//Returns the position at which the specified crab will stop if it
	//immediately starts decelerating
	float stopPos(Crab* crab) {
		float d = crab->speed() * crab->speed() / crab->acceleration();
		if (crab->speed() > 0) {
			return crab->pos() + d;
		}
		else {
			return crab->pos() - d;
		}
	}
}

void Game::doAI() {
	for(int i = 1; i < 4; i++) {
		Crab* crab = crabs0[i];
		if (crab == NULL) {
			continue;
		}
		
		//Find the position of the ball that is nearest the crab's side, and
		//store the result in targetPos
		float closestBallDist = 100;
		float targetPos = 0.5f;
		for(unsigned int j = 0; j < balls0.size(); j++) {
			Ball* ball = balls0[j];
			
			float ballDist;
			float ballPos;
			switch(i) {
				case 1:
					ballDist = ball->x() - ball->radius();
					ballPos = 1 - ball->z();
					break;
				case 2:
					ballDist = 1 - ball->z() - ball->radius();
					ballPos = 1 - ball->x();
					break;
				case 3:
					ballDist = 1 - ball->x() - ball->radius();
					ballPos = ball->z();
					break;
			}
			
			if (ballDist < closestBallDist) {
				targetPos = ballPos;
				closestBallDist = ballDist;
			}
		}
		
		//Move toward targetPos.  Stop so that the ball is in the middle 70% of
		//the crab.
		if (abs(stopPos(crab) - targetPos) < 0.7f * (CRAB_LENGTH / 2)) {
			crab->setDir(0);
		}
		else if (targetPos < crab->pos()) {
			crab->setDir(-1);
		}
		else {
			crab->setDir(1);
		}
	}
}

void Game::step() {
	//Advance the crabs
	for(int i = 0; i < 4; i++) {
		Crab* crab = crabs0[i];
		if (crab != NULL) {
			crab->advance(GAME_STEP_TIME);
		}
	}
	
	//Advance the balls
	for(unsigned int i = 0; i < balls0.size(); i++) {
		balls0[i]->advance(GAME_STEP_TIME);
	}
	
	//Handle collisions
	handleCollisions();
	
	//Check for balls that have scored on a player
	vector<Ball*> newBalls;
	for(unsigned int i = 0; i < balls0.size(); i++) {
		Ball* ball = balls0[i];
		if (ball->fadeAmount() == 1 && !ball->isFadingOut()) {
			newBalls.push_back(ball);
			
			int scoredOn;
			if (ball->z() < ball->radius() && (ball->angle() > PI)) {
				scoredOn = 0;
			}
			else if (ball->x() < ball->radius() &&
					 (ball->angle() > PI / 2 && ball->angle() < 3 * PI / 2)) {
				scoredOn = 1;
			}
			else if (ball->z() > 1 - ball->radius() && (ball->angle() < PI)) {
				scoredOn = 2;
			}
			else if (ball->x() > 1 - ball->radius() &&
					 (ball->angle() < PI / 2 || ball->angle() > 3 * PI / 2)) {
				scoredOn = 3;
			}
			else {
				scoredOn = -1;
			}
			
			if (scoredOn >= 0 && crabs0[scoredOn] != NULL) {
				scores[scoredOn]--;
				if (scores[scoredOn] == 0) {
					delete crabs0[scoredOn];
					crabs0[scoredOn] = NULL;
				}
				
				ball->fadeOut();
			}
		}
		else if (ball->fadeAmount() > 0 || !ball->isFadingOut()) {
			newBalls.push_back(ball);
		}
		else {
			delete ball;
		}
	}
	balls0 = newBalls;
	
	//Check whether the game is over
	bool isGameOver;
	if (crabs0[0] != NULL) {
		isGameOver = true;
		for(int i = 1; i < 4; i++) {
			if (crabs0[i] != NULL) {
				isGameOver = false;
			}
		}
	}
	else {
		isGameOver = true;
	}
	
	if (!isGameOver) {
		//Try to get to NUM_BALLS balls
		while (balls0.size() < (unsigned int)NUM_BALLS) {
			//Try to place a ball at the center of the board
			bool ballFits = true;
			for(unsigned int i = 0; i < balls0.size(); i++) {
				Ball* ball = balls0[i];
				if (intersectsCircle(ball->x() - 0.5f, ball->z() - 0.5f,
									 2 * BALL_RADIUS)) {
					ballFits = false;
					break;
				}
			}
			
			if (ballFits) {
				Ball* ball =
					new Ball(BALL_RADIUS, 0.5f, 0.5f, 2 * PI * randomFloat());
				balls0.push_back(ball);
			}
			else {
				break;
			}
		}
	}
	else {
		for(unsigned int i = 0; i < balls0.size(); i++) {
			balls0[i]->fadeOut();
		}
	}
	
	//Run the AI for the computer-controlled crabs
	doAI();
}

void Game::setPlayerCrabDir(int dir) {
	if (crabs0[0] != NULL) {
		crabs0[0]->setDir(dir);
	}
}

int Game::score(int crabNum) {
	return scores[crabNum];
}

void Game::advance(float dt) {
	while (dt > 0) {
		if (timeUntilNextStep < dt) {
			dt -= timeUntilNextStep;
			step();
			timeUntilNextStep = CRAB_STEP_TIME;
		}
		else {
			timeUntilNextStep -= dt;
			dt = 0;
		}
	}
}

Crab** Game::crabs() {
	return crabs0;
}

vector<Ball*> Game::balls() {
	return balls0;
}










