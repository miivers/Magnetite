#ifndef _SCRIPTGAME_H_
#define _SCRIPTGAME_H_
#include "ScriptWrapper.h"
#include "prerequisites.h"
#include "BaseGame.h"

class ScriptGame : public BaseGame
{
protected:
	/**
	 * Stores the friendly name for this game
	 */
	std::string mGameName;
	
	/**
	 * The Javascript object!1
	 */
	PersistentObject mScriptObject;

public:
	ScriptGame();
	~ScriptGame();

	/**
	 * Returns the name of this Game for printing purposes.
	 */
	virtual std::string getName();
	
	/**
	 * Sets the friendly name for this game
	 */
	void setName( const std::string& name );
	
	/**
	 * Sets the internal object for javascript
	 */
	void _setScriptObject( PersistentObject object );

	/**
	 * Returns true in singleplayer mode 
	 */
	bool isSingleplayer();

	/**
	 * Creates a new, run of the mill character
	 */
	Character* createCharacter();

	/**
	 * Returns the local player
	 */
	Character* getLocalPlayer();

	//<== Game Events
	/**
	 * Called when a player enters the game
	 */
	void playerJoin( Character* player );

	/**
	 * Called when a player is spawning
	 */
	void playerSpawn( Character* player );

	/**
	 * Called when a player is killed
	 */
	void playerKilled( Character* player );

	/**
	 * Called when a character takes damage
	 */
	void characterDamage( Character* player );

	/**
	 * Called When a player primary fires
	 */
	void playerPrimaryClick( Character* player );

	/**
	 * Called When a player alt fires
	 */
	void playerAltClick( Character* player );
	
	/**
	 * Called when the game is ok to render the GUI
	 */
	void uiPaint(Renderer* r);
	
	/**
	 * Generic key down event
	 */
	void keyDown( size_t evt );
	
	/**
	 * Generic key up event
	 */
	void keyUp( size_t evt );

	//========================= Internal stuff

	/**
	 * Starts a singleplayer instance of this game
	 */
	virtual void _startGameSingle();

	/**
	 * Tells the game that this is a Multiplayer game
	 */
	virtual void _startGame();
	
	virtual void _loadGame();

	/**
	 * 
	 */
	void _playerJoined();

	/**
	 * Called on local input events
	 */
	void _inputEvents( const InputEvent& e );

	/**
	 * Called when a local player tries to move
	 */
	void _inputMovement( const Vector3& v );

	/**
	 * Called when a local player tries to sprint
	 */
	void _inputSprint( const Vector3& v );


	/**
	 * Called when a local player tries to look
	 */
	void _mouseMoved( const float x, const float y );

	/**
	 * Called when the local player click
	 */
	void _primary();

	/**
	 * Called when the local player clicks
	 */
	void _secondary();
};

#endif
