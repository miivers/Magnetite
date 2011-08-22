#include "MagnetiteCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "InputManager.h"
#include "StoneBlock.h"
#include "World.h"
#include "Character.h"
#include "BlockFactory.h"
#include "BaseGame.h"
#include "BulletDebug.h"
#include <ctime>

MagnetiteCore* MagnetiteCore::Singleton = 0;
static GLDebugDrawer debug;


/* Simple events */
void globalEventHandler( const InputEvent& evt )
{
	MagnetiteCore::Singleton->getGame()->_inputEvents( evt );
	if( evt.event == Inputs::SCREENSHOT && evt.down == true ) {
		MagnetiteCore::Singleton->screenshot();
	}
}

MagnetiteCore::MagnetiteCore(void)
: mContinue( true ),
mWorld( NULL ),
mTextureManager( NULL ),
mInputManager( NULL ),
mTimescale( 1.f ),
mLastX( 0.f ),
mLastY( 0.f ),
mPBroadphase( NULL ),
mPCConfig( NULL ),
mCCDispatch ( NULL ),
mSolver( NULL ),
mPhysicsWorld( NULL )
{
	MagnetiteCore::Singleton = this;
	mRenderer = new Renderer();
	mTextureManager = new TextureManager();
	mInputManager = new InputManager();
	mInputManager->setEventCallback( Inputs::FORWARD, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::BACK, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::LEFT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::RIGHT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::JUMP, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::SPRINT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::SCREENSHOT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::FLY, &globalEventHandler );
}

MagnetiteCore::~MagnetiteCore(void)
{
	unloadWorld();

	delete mRenderer;
	delete mTextureManager;
	delete mInputManager;

	mPhysicsWorld->removeRigidBody( mGroundBody );

	delete mGroundBody;
	delete mGroundShape;
	delete mGroundState;

	delete mPhysicsWorld;
	delete mSolver;
	delete mCCDispatch;
	delete mPCConfig;
	delete mPBroadphase;
}

void MagnetiteCore::init(int *argc, char **argv)
{
	sf::WindowSettings wnds;
	wnds.DepthBits = 24;
	wnds.DepthBits = 8;
	//wnds.AntialiasingLevel = 2;
	mWindow.Create(sf::VideoMode(800,600,32), "Magnetite", sf::Style::Close | sf::Style::Resize, wnds);
	mWindow.EnableKeyRepeat( false );
	//mWindow.UseVerticalSync(true);
	mRenderer->initialize(mWindow);
	wglSwapIntervalEXT(1); // Enable VSYNC, for greater justice
	mTextureManager->initalize();
	mRenderer->resizeViewport(0,0,800,600);
	initalizePhysics();
}

void MagnetiteCore::initalizePhysics()
{
	mPBroadphase = new btDbvtBroadphase();
	mPCConfig = new btDefaultCollisionConfiguration();
	mCCDispatch = new btCollisionDispatcher(mPCConfig);
	mSolver = new btSequentialImpulseConstraintSolver();
	mPhysicsWorld = new btDiscreteDynamicsWorld( mCCDispatch, mPBroadphase, mSolver, mPCConfig );
	mPhysicsWorld->setGravity( btVector3( 0.f, -9.81f, 0.f ) );

	mGroundShape = new btStaticPlaneShape( btVector3(0, 1, 0), 0 );
	mGroundState = new btDefaultMotionState;
	btRigidBody::btRigidBodyConstructionInfo ci( 0, mGroundState, mGroundShape, btVector3(0,0,0) );
	mGroundBody = new btRigidBody( ci );
	mPhysicsWorld->addRigidBody( mGroundBody );
	debug.setDebugMode( btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb );
	mPhysicsWorld->setDebugDrawer( &debug );
}

btDiscreteDynamicsWorld* MagnetiteCore::getPhysicsWorld()
{
	return mPhysicsWorld;
}

void MagnetiteCore::screenshot()
{
	sf::Image screen = mWindow.Capture();
	time_t rawt = time( NULL );
	tm* t = localtime( &rawt );
	char tstr[80];
	strftime(tstr, 80, "%Y-%m-%d_%H-%M-%S", t);
	std::string fname("opencraft_");
	fname.append( tstr );
	fname.append(".png");
	Util::log("Saving screenshot to: " + fname);
	screen.SaveToFile(fname);
}

void MagnetiteCore::startGame( const std::string& type )
{
	mGame = FactoryManager::getManager().createGame(type);
	if( mGame != NULL ) 
		Util::log("Starting game: " + mGame->getName() );
	mGame->_startGameSingle();
	
	newWorld("test");
	
	// No multiplayer yet so just force player join
	mGame->_playerJoined();
}

void MagnetiteCore::inputMovement( const Vector3 &v )
{
	mGame->_inputMovement( v );
}

void MagnetiteCore::mouseMoved( const float x, const float y )
{
	mGame->_mouseMoved( x, y );
}

void MagnetiteCore::go() 
{
	// Print factory list for testing
	FactoryManager::getManager().logFactories();
	
	int lastX = mWindow.GetWidth()/2;
	int lastY = mWindow.GetHeight()/2;

	mClock.Reset();
	while(mContinue && mWindow.IsOpened()) {
		float lDelta = mClock.GetElapsedTime();
		mClock.Reset();

		// Handle Events before we render.
		sf::Event lEvt;
		while( mWindow.GetEvent(lEvt) ) {
			if( lEvt.Type == sf::Event::Closed ) {
				exit();
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Escape) ) {
				exit();
			}

			if( lEvt.Type == sf::Event::KeyPressed )
				mInputManager->keyDown( lEvt.Key.Code );
			if( lEvt.Type == sf::Event::KeyReleased )
				mInputManager->keyUp( lEvt.Key.Code );

			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::R) ) {
					if(mRenderer->getRenderMode() == Renderer::RENDER_SOLID)
						mRenderer->setRenderMode(Renderer::RENDER_LIGHTING);
					else
						mRenderer->setRenderMode(Renderer::RENDER_SOLID);
			}

			//==========
			//= Debug Buttons
			//==========
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F1 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_OFF );
			}
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F3 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_STATS );
			}
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F4 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_PHYSICS );
			}
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F5 ) ) {
				Util::setLogLevel( Util::Verbose );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::F) ) {
					//mPlayer->enableFlying( !mPlayer->isFlying() );
					//Util::log( (Util::toString(mPlayer->isFlying())) + " Flying");
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::F) ) {
					//mPlayer->enableFlying( !mPlayer->isFlying() );
					//Util::log( (Util::toString(mPlayer->isFlying())) + " Flying");
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Add) ) {
					mRenderer->nextBlock();
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::F6) ) {
					mWorld->saveAllChunks();
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::F7) ) {
					mWorld->loadWorld("test");
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Num9) ) {
					mTimescale *= 0.5f;
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Num0) ) {
					mTimescale *= 2.f;
			}
			if( (lEvt.Type == sf::Event::MouseMoved) ) {
					mGame->_mouseMoved( -(lEvt.MouseMove.X - lastX), -(lEvt.MouseMove.Y  - lastY));
					lastX = mWindow.GetWidth()/2;
					lastY = mWindow.GetHeight()/2;
					mWindow.SetCursorPosition( mWindow.GetWidth() / 2, mWindow.GetHeight() / 2 );
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Left ) {
				mGame->_primary();
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Right ) {
				mGame->_secondary();
			}
			if( lEvt.Type == sf::Event::Resized ) {
				mRenderer->resizeViewport( 0, 0, lEvt.Size.Width, lEvt.Size.Height );
			}
		}

		lDelta *= mTimescale;
		
		mPhysicsWorld->stepSimulation( lDelta );
		
		//Ensure each loaded chunk is updated before being sent to the GPU
		mWorld->update( lDelta );

		// Update all the characters
		for( std::vector<Character*>::iterator it = mCharacters.begin(); it != mCharacters.end(); it++ )
		{
			(*it)->update( lDelta );
		}
	
		//Ensure the renderer has the correct camera
		mRenderer->setCamera( mGame->getLocalPlayer()->getCamera() );
		mRenderer->render(lDelta, mWorld);

		mWindow.Display();
	}

	mWindow.Close();

}

void MagnetiteCore::exit()
{
	mContinue = false;
}

TextureManager* MagnetiteCore::getTextureManager()
{
	return mTextureManager;
}

Renderer* MagnetiteCore::getRenderer()
{
	return mRenderer;
}

BaseGame* MagnetiteCore::getGame()
{
	return mGame;
}

float MagnetiteCore::getTimescale()
{
	return mTimescale;
}

void MagnetiteCore::newWorld( std::string name )
{
	unloadWorld();

	mWorld = new World();
	mWorld->newWorld( name );
}

void MagnetiteCore::unloadWorld()
{
	if( mWorld != NULL ) {
		// world handles saving.
		delete mWorld;
	}
	mWorld = NULL;
}

World* MagnetiteCore::getWorld()
{
	return mWorld;
}

Character* MagnetiteCore::createCharacter()
{
	Character* c = new Character();
	mCharacters.push_back( c );
	return c;
}

void MagnetiteCore::destroyCharacter( Character* c )
{
	for( std::vector<Character*>::iterator it = mCharacters.begin(); it != mCharacters.end(); it++ ) {
		if( c == (*it) )
			mCharacters.erase( it++ );
	}
}
