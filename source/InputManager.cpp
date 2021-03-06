#include "InputManager.h"
#include <fstream>

InputManager::InputManager()
{
	/* Setup some default input events, this is a good a place as any. */
	createInput( sf::Keyboard::W, Inputs::FORWARD );
	createInput( sf::Keyboard::S, Inputs::BACK );
	createInput( sf::Keyboard::A, Inputs::LEFT );
	createInput( sf::Keyboard::D, Inputs::RIGHT );
	createInput( sf::Keyboard::Space, Inputs::JUMP );
	createInput( sf::Keyboard::LShift, Inputs::SPRINT );
	createInput( sf::Keyboard::F, Inputs::FLY );
	createInput( sf::Keyboard::F2, Inputs::SCREENSHOT );
	createInput( sf::Keyboard::R, Inputs::RESPAWN );
}

InputManager::~InputManager()
{

}

void InputManager::loadInputs()
{
	std::ifstream stream("../config/inputs");
	if( stream && !stream.bad() )
	{
		while( !stream.eof() ) 
		{
			std::string event = "";
			std::getline( stream, event, ':' );
			std::string key = "";
			std::getline( stream, key );
			size_t evtId = atoi( event.c_str() );
			size_t keyId = atoi( key.c_str() );
			for( InputList::iterator it = mInputs.begin(); it != mInputs.end(); it++ )
			{
				if( it->event == evtId )
					it->key = (sf::Keyboard::Key)keyId;
			}
		}
	}
}

std::string InputManager::inputToString( Inputs::Event evt )
{
	switch( evt ) {
		case Inputs::FORWARD:
			return "forward";
		case Inputs::BACK:
			return "back";
		case Inputs::LEFT:
			return "left";
		case Inputs::RIGHT:
			return "right";
		case Inputs::JUMP:
			return "jump";
		case Inputs::SPRINT:
			return "sprint";
		default:
			return "";
	}
	return "";
}

void InputManager::createInput( sf::Keyboard::Key key, Inputs::Event event )
{
	InputBinding input;
	input.event = event;
	input.key = key;
	input.callback = NULL;
	mInputs.push_back( input );
}

void InputManager::setEventCallback( Inputs::Event event, InputCallback callback )
{
	for(InputList::iterator it = mInputs.begin(); it != mInputs.end(); it++)
	{
		if( event == it->event )
			it->callback = callback;
	}
}

void InputManager::keyDown( sf::Keyboard::Key key )
{
	for(InputList::iterator it = mInputs.begin(); it != mInputs.end(); it++)
	{
		if( key == it->key ) {
			InputEvent event;
			event.down = true;
			event.event = it->event;
			if( it->callback != NULL )
				it->callback( event );
		}
	}
}

void InputManager::keyUp( sf::Keyboard::Key key )
{
	for(InputList::iterator it = mInputs.begin(); it != mInputs.end(); it++)
	{
		if( key == it->key ) {
			InputEvent event;
			event.down = false;
			event.event = it->event;
			if( it->callback != NULL )
				it->callback( event );
		}
	}
}

void InputManager::beginInput( Inputs::Event evt )
{
	
	Util::log("Received Input: " + inputToString( evt ));
}

void InputManager::endInput( Inputs::Event evt )
{
	Util::log("Finished Input: " + inputToString( evt ));
}
