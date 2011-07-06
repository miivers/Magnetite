#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_
#include "prerequisites.h"

/**
 todo:
 * Input callbacks
 * Input customization
 */

namespace Inputs {
	enum Event {
		LEFT,
		RIGHT,
		FORWARD,
		BACK,
		SPRINT,
		JUMP,
		PRIMARY,
		SECONDARY
	};
};

struct InputEvent {
	Inputs::Event event;
	bool down;
};

typedef void (*InputCallback)(const InputEvent&);
struct InputBinding {
	Inputs::Event event;
	sf::Key::Code key;
	InputCallback callback;
};

typedef std::vector<InputBinding> InputList;

class InputManager
{
protected:
	InputList mInputs;

public:
	InputManager();
	~InputManager();

	void loadInputs();

	void createInput( sf::Key::Code key, Inputs::Event event );
	void setEventCallback( Inputs::Event event, InputCallback callback );

	void keyDown( sf::Key::Code key );
	void keyUp( sf::Key::Code key );

	void beginInput( Inputs::Event evt );
	void endInput( Inputs::Event evt );

	std::string inputToString( Inputs::Event evt );
};


#endif // _InputManager_H_