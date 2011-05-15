#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "StoneBlock.h"
#include "World.h"
#include "Character.h"
#include "BlockFactory.h"

OpencraftCore* OpencraftCore::Singleton = 0;

OpencraftCore::OpencraftCore(void)
: mContinue( true ),
mWorld( NULL ),
mPlayer( NULL )
{
	OpencraftCore::Singleton = this;
	mRenderer = new Renderer();
	mTextureManager = new TextureManager();
}

OpencraftCore::~OpencraftCore(void)
{
	unloadWorld();

	delete mRenderer;
	delete mTextureManager;
}

void OpencraftCore::createWindow(int *argc, char **argv)
{
	sf::WindowSettings wnds;
	wnds.DepthBits = 24;
	wnds.DepthBits = 8;
	//wnds.AntialiasingLevel = 2;
	mWindow.Create(sf::VideoMode(800,600,32), "Opencraft", sf::Style::Close | sf::Style::Resize, wnds);
	mWindow.EnableKeyRepeat( false );
	//mWindow.UseVerticalSync(true);
	mRenderer->initialize(argc, argv);
	mTextureManager->initalize();
	mRenderer->resizeViewport(0,0,800,600);
}

void OpencraftCore::go(int *argc, char **argv) 
{
	createWindow(argc, argv);

	// Print factory list for testing
	FactoryManager::logFactories();

	int lastX = mWindow.GetWidth()/2;
	int lastY =  mWindow.GetHeight()/20;

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
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::R) ) {
					if(mRenderer->getRenderMode() == Renderer::RENDER_SOLID)
						mRenderer->setRenderMode(Renderer::RENDER_WIRE);
					else
						mRenderer->setRenderMode(Renderer::RENDER_SOLID);
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::A) ) {
					mPlayer->addMoveDelta( Vector3( -1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::A) ) {
					mPlayer->addMoveDelta( Vector3( 1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::D) ) {
					mPlayer->addMoveDelta( Vector3( 1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::D) ) {
					mPlayer->addMoveDelta( Vector3( -1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::W) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, -1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::W) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, 1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::S) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, 1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::S) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, -1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::LShift) ) {
					mPlayer->enableSprint( true );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::LShift) ) {
					mPlayer->enableSprint( false );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Add) ) {
					mRenderer->nextBlock();
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Subtract) ) {
					mRenderer->lastBlock();
			}
			if( (lEvt.Type == sf::Event::MouseMoved) ) {
					mPlayer->getCamera()->pitch( -(lEvt.MouseMove.Y  - lastY) );
					mPlayer->getCamera()->yaw( -(lEvt.MouseMove.X - lastX) );
					lastY = lEvt.MouseMove.Y;
					lastX = lEvt.MouseMove.X;
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Left ) {
				placeEyeBlock();
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Right ) {
				removeEyeBlock();
			}
			if( lEvt.Type == sf::Event::Resized ) {
				mRenderer->resizeViewport( 0, 0, lEvt.Size.Width, lEvt.Size.Height );
			}
		}

		mPlayer->update( lDelta );

		//Ensure each loaded chunk is updated before being sent to the GPU
		for( ChunkList::iterator it = mWorld->getChunks().begin(); it != mWorld->getChunks().end(); ++it ) {
			(*it)->update(lDelta);
		}

		//Ensure the renderer has the correct camera
		mRenderer->setCamera(mPlayer->getCamera());
		mRenderer->render(lDelta, mWorld->getChunks());

		mWindow.Display();
	}

	mWindow.Close();

}

void OpencraftCore::exit()
{
	mContinue = false;
}

TextureManager* OpencraftCore::getTextureManager()
{
	return mTextureManager;
}

void OpencraftCore::newWorld( std::string name )
{
	unloadWorld();

	mWorld = new World();
	// Creates a testing world for now.
	mWorld->createWorld();
	createCharacter();
}

void OpencraftCore::unloadWorld()
{
	if( mWorld != NULL ) {
		// world handles saving.
		delete mWorld;
		// If the player still exists, delete it.
		if( mPlayer != NULL ) {
			delete mPlayer;
		}
	}
	mPlayer = NULL;
	mWorld = NULL;
}

World* OpencraftCore::getWorld()
{
	return mWorld;
}

Character* OpencraftCore::createCharacter()
{
	if( mPlayer == NULL )
		mPlayer = new Character();
	mPlayer->setPosition( Vector3( 0.f, 10.f, 0.f )  );
	return mPlayer;
}

void OpencraftCore::removeEyeBlock()
{
	raycast_r ray = mPlayer->getEyeCast();
	ray = mWorld->raycastWorld(ray);
	if(ray.hit)
	{
		Vector3 cIndex = mWorld->worldToChunks( ray.worldHit );
		Vector3 bIndex = mWorld->worldToBlock( ray.worldHit );
		WorldChunk* chunk = mWorld->getChunk( cIndex.x, cIndex.y, cIndex.z );
		if(chunk) {
			chunk->removeBlockAt( bIndex.x, bIndex.y - 1, bIndex.z );
		}
	}
}

void OpencraftCore::placeEyeBlock()
{
	raycast_r ray = mPlayer->getEyeCast();
	ray = mWorld->raycastWorld(ray);
	if(ray.hit)
	{
		Vector3 cIndex = mWorld->worldToChunks( ray.worldHit );
		Vector3 bIndex = mWorld->worldToBlock( ray.worldHit );
		WorldChunk* chunk = mWorld->getChunk( cIndex.x, cIndex.y, cIndex.z );
		//Util::log("Ray hit block: " + Util::toString(Vector3(bX,bY,bZ)) + " in chunk " +  Util::toString(cIndex));
		if(chunk) {
			BaseBlock* block = FactoryManager::createBlock(	mRenderer->blockType );
			if( block != NULL ) {
				block->setPosition( bIndex );
				chunk->addBlockToChunk( block );
			}
		}
	}
}