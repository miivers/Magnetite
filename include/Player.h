#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "prerequisites.h"
#include "Camera.h"
#include "World.h"

class Player
{
	/**
	 * Simple position, can be bypassed if the player has a character
	 */
	Vector3	mPosition;
	
	/**
	 * Camera object
	 */
	Camera	mCamera;
	
public:
	
	Player();
	~Player();
	
	/**
	 * Sets the player's position in the world
	 */
	void setPosition( const Vector3& pos );
	
	/**
	 * Returns the players global coordinates
	 */
	Vector3 getPosition();
	
	Camera& getCamera();
	
	/**
	 * Adjusts the player's position by the given vector
	 */
	void move( const Vector3& offset );
	
	/**
	 * Gets a ray representing the Player's look
	 */
	raycast_r getEyeCast();
	
	/**
	 * Gets the direction the player is looking in
	 */
	Vector3 getLookDirection();
	
	/**
	 * Creates a character for the player to have a physical body
	 */
	void createCharacter() {};
	
	/**
	 * Destroys the players character if there is one
	 */
	void destroyCharacter() {};
};


#endif