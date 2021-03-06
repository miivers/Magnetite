#ifndef _BASEBLOCK_H_
#define _BASEBLOCK_H_
#include "prerequisites.h"
#include <iostream>
#include <fstream>

typedef unsigned short GLedge;

enum {
	FACE_TOP		= (1<<0),
	FACE_BOTTOM	= (1<<1),
	FACE_LEFT	= (1<<2),
	FACE_RIGHT	= (1<<3),
	FACE_FORWARD	= (1<<4),
	FACE_BACK	= (1<<5),
	FACE_ALL	= FACE_TOP | FACE_BOTTOM | FACE_LEFT | FACE_RIGHT | FACE_FORWARD | FACE_BACK
};

enum {
	BMASK_VISFLAGS = ( (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) )
};

class World;
class Chunk;

struct BlockContext
{
	float worldX;
	float worldY;
	float worldZ;
	Chunk* chunk;
	World* world;
};

class BaseBlock
{
protected:
	unsigned char mDataFlags;
	
	void _setPosition( unsigned short x, unsigned  short y, unsigned short z );
public:
	BaseBlock();
	~BaseBlock(void);

	virtual void created() = 0;
	virtual void hit() = 0;

	/**
	 * Returns true if this object inherits FluidBlock
	 */
	virtual bool isFluid() { return false; }
	
	/**
	 * Returns true if block should stop occlusion
	 */
	virtual bool isOpaque() { return true; }

	/**
	 * Returns true if this block is solid.
	 */
	virtual bool isSolid() { return true; }

	/**
	 * Returns true if this block needs to think
	 */
	virtual bool isThinking() { return false; }
	
	/**
	 * Updates the block, if it needs that sort of thing.
	 */
	virtual void think( BlockContext &ctx, float dt ) { }

	/**
	 * Called when a nearby block changes.
	 * @param face The direction of the changed block.
	 */
	virtual void connectedChange( short face );
	virtual void getTextureCoords( short face, short &x, short &y );

	/**
	 * Updates the viewflags for this block
	 * @param flags the new bitfield
	 */
	void updateVisFlags(unsigned int flags);

	/**
	 * Returns the viewflags for this block
	 */
	unsigned short getVisFlags();

	/**
	 * Appends this block's data to a stream.
	 */
	void appendToStream( std::ofstream& stream );

	/**
	 * Reads the data for this block from a stream
	 */
	void readFromStream( std::ifstream& stream );

	virtual std::string getType() = 0;

	/*short getX();
	short getY();
	short getZ();

	void setPosition( long x, long y, long z );
	void setPosition( const Vector3& vec );*/
};

#endif