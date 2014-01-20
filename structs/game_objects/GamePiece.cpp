#include "GamePiece.h"

GamePiece::GamePiece()
	:	rect( )
	,	oldRect( )
	,	textureType( TextureType::e_Paddle )
	,	isAlive( true )
	,	scale( 1.0 )
	,	speed( 0.01 )
	{

	}