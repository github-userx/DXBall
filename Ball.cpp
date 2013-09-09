#include "Ball.h"
#include <cmath>
#include <iostream>

#include <SDL2/SDL.h>

#include "math/Vector2f.h"

Ball::Ball()
	:	speed( 0.0705f )
	,	dirX( -0.83205f )
	,	dirY(-0.5547f )
	,	paddleHitInPrevFrame( false )
	,	debugMode( false )
{
	rect.w = 20;
	oldRect.w = 20;

	rect.h = 20;
	oldRect.h = 20;

	Reset();
}

Ball::~Ball()
{
}

void Ball::Reset()
{
	speed = 0.5f;

	dirX = 0.2;
	dirY =  -0.87f;

	rect.x = 50;
	rect.y = 50;

	paddleHitInPrevFrame = false;

	NormalizeDirection();
}

void Ball::NormalizeDirection()
{
	double length = sqrt( ( dirX * dirX ) + ( dirY * dirY ) );
	dirX /= length;
	dirY /= length;
}

void Ball::Update( double tick )
{
	oldRect.x = rect.x;
	oldRect.y = rect.y;

	rect.x += tick * speed * dirX;
	rect.y += tick * speed * dirY;
}

bool Ball::BoundCheck( const SDL_Rect &boundsRect )
{

	double left  = static_cast< double > ( boundsRect.x );
	double right = static_cast< double > ( boundsRect.x ) + boundsRect.w;
	double top   = static_cast< double > ( boundsRect.y );

	if ( rect.x < left )
	{
		rect.x = left;
		dirX *= -1.0f;
		return true;
	}

	if ( ( rect.x + rect.w ) > right )
	{
		rect.x = ( right - rect.w );
		dirX *= -1.0f;
		return true;
	}

	if ( rect.y < top )
	{
		rect.y = top;
		dirY = ( dirY < 0.0f ) ? dirY * -1.0f : dirY;
		return true;
	}

	return false;
}

bool Ball::DeathCheck( const SDL_Rect &boundsRect )
{
	double bottom = static_cast< double > ( boundsRect.y + boundsRect.h );

	if (  ( rect.y + rect.h ) > bottom  )
	{
		Reset();
		dirY *= -1.0f;
		return true;
	}

	return false;
}

bool Ball::PaddleCheck( const Rect &paddleRect )
{
	if ( CheckTileIntersection( paddleRect, rect ) )
	{
		if ( !paddleHitInPrevFrame )
		{
			paddleHitInPrevFrame = true;
			HandlePaddleHit( paddleRect );
			return true;
		} else
		{
			return false;
		}
	}


	paddleHitInPrevFrame = false;
	return false;
}

void Ball::HandlePaddleHit( const Rect &paddleRect )
{
	double hitPosition = CalculatePaddleHitPosition( paddleRect );

	CalculateNewBallDirection( hitPosition );

	speed *= 1.0005f;
}
double Ball::CalculatePaddleHitPosition( const Rect &paddleRect ) const
{
	double ballMidpoint = rect.x + ( rect.w / 2.0 );
	double paddleMidpoint = paddleRect.x + ( paddleRect.w / 2.0 );

	// Returns on which ratio the ball hit. 1.0f is right edge, -1.0f is left edge and 0.0f is middle
	return ( ballMidpoint - paddleMidpoint ) / ( ( paddleRect.w / 2.0 ) + ( rect.w / 2.0 ));
}
void  Ball::CalculateNewBallDirection( double hitPosition )
{
	dirX = hitPosition;
	dirY = ( dirY > 0.0f ) ? dirY * -1.0f : dirY;

	NormalizeDirection();
}
bool Ball::TileCheck( const Rect &tileRect, unsigned int tileID )
{
	if ( !CheckTileIntersection( tileRect, rect ) )
		return false;

	if ( debugMode )
	{
		std::cout << "===============================================================================\n";
		std::cout << "Colliding in current frame\n";
		std::cout << "\tChecking sphere intersection....\n";
	}
	double dist = 0.0f;
	if ( !CheckTileSphereIntersection( tileRect, rect, dist  ) )
	{
		if ( debugMode )
			std::cout << "\tNo circle collision, skipping....\n";
		return false;
	}

	// If the ball collided with the same rect in the previous frame too,
	// This means the ball needs an extra frame to get fully out of the tile
	if ( debugMode )
		std::cout << "\tPrev checking sphere intersection....\n";


	if ( lastTileHit == tileID && CheckTileSphereIntersection( tileRect, oldRect, dist ) )
	{
		if ( debugMode )
			std::cout << "Collided in the prev frame too...\n";
		return false;
	}

	if ( debugMode )
	{
		std::cout << "\tTile ID : " << tileID << std::endl;
		PrintPosition( tileRect,"Tile");
		PrintPosition( rect    , "Ball cur ");
		PrintPosition( oldRect , "Ball old ");
	}
	lastTileHit = tileID;
	FindIntersectingSide( tileRect );
	//HandleTileIntersection( tileRect );
	//HandleTileIntersection2( tileRect );

	return true;
}



bool Ball::CheckTileIntersection( const Rect &tile, const Rect &ball ) const
{
	double tileLeft =   tile.x;
	double tileTop =    tile.y;
	double tileRight =  tile.x + tile.w;
	double tileBottom = tile.y + tile.h;

	double ballLeft =   ball.x;
	double ballTop =    ball.y;
	double ballRight =  ball.x + ball.w;
	double ballBottom = ball.y + ball.h;

	// Intersection test
	return !(
			ballTop    > tileBottom
			|| ballLeft   > tileRight
			|| ballRight  < tileLeft
			|| ballBottom < tileTop
		);
}
bool Ball::CheckTileSphereIntersection( const Rect &tile, const Rect &ball, double &retDistance ) const
{
	double ballRadius  = ball.w / 2.0;
	double ballCenterX = ball.x + ballRadius;
	double ballCenterY = ball.y + ballRadius;

	double tileHalfWidth  = ( tile.w / 2.0 );
	double tileHalfHeight = ( tile.h / 2.0 );
	double tileCenterX = tile.x + tileHalfWidth;
	double tileCenterY = tile.y + tileHalfHeight;

	double distX = fabs( ballCenterX - tileCenterX );
	double distY = fabs( ballCenterY - tileCenterY );

	// If distance from center of sphere to center of rect is larger than 
	// the sum of the raidus of the ball and the distance from the center of the rect to the edge
	if ( distX > ( tileHalfWidth + ballRadius ) )
		return false;

	if ( distY > ( tileHalfHeight + ballRadius ) )
		return false;

	// if the distance from the center of the sphere to the center of the rect is smaller or equal to
	// the the distance from the center of the rect to the edge
	if ( distX <= tileHalfWidth )
		return true;

	if ( distY <= tileHalfHeight )
		return true;

	// Get the distance from center of the sphere, to the edge of the rect squared.
	double cornerDistance = ( distX - tileHalfWidth ) * ( distX - tileHalfWidth );
	cornerDistance += ( distY - tileHalfHeight ) * ( distY - tileHalfHeight );

	if ( cornerDistance > ( ballRadius * ballRadius ) )
		return false;

	retDistance = sqrt( cornerDistance );

	return true;
}
void Ball::PrintPosition( const Rect &pos, const std::string &tileName ) const
{
	std::cout << "\t" << tileName << " position tl : " << pos.x              << " , " << pos.y             << std::endl;
	std::cout << "\t" << tileName << " position br : " << ( pos.x + pos.w )  << " , " << ( pos.y + pos.h ) << std::endl << std::endl;
}

bool Ball::LineLineIntersectionTestV2( const Vector2f &tile1, const Vector2f &tile2, const Vector2f &ball1, const Vector2f &ball2, double &ret ) const
{
	Vector2f tile( tile2 - tile1 );
	Vector2f ball( ball2 - ball1 );

	Vector2f ballTile( ball2 - tile2 );

	double dot = Math::PerpDot( tile, ball );
	double dotTile = Math::PerpDot( tile, ballTile );
	double dotBall = Math::PerpDot( ball, ballTile );

	if ( debugMode )
	{
		std::cout << "\tBall : " << ball << std::endl;
		std::cout << "\tTile : " << tile << std::endl;
	}

	if ( CheckDotProducts( dot, dotTile, dotBall ) )
	{
		ret = 1.0f - ( dotTile / dot );
		return true;
	} else
	{
		return false;
	}
}
bool Ball::CheckDotProducts( double dot, double dotTile, double dotBall ) const
{
	// Paralell check
	if ( dot == 0.0f )
	{
		if ( debugMode )
			std::cout << "\tDot product is 0, lines are paralell\n";
		return false;
	}

	if ( dot < 0.0 )
	{
		if ( dotTile > 0.0 )
		{
			if ( debugMode )
				std::cout << "\tLINE : " << __LINE__ << " " << "dot Tile is > 0 : " << dotTile << std::endl;
			return false;
		}
		if ( dotBall > 0.0 )
		{
			if ( debugMode )
				std::cout << "\tLINE : " << __LINE__ << " "  << "dot ball is > 0 : " << dotBall << std::endl;
			return false;
		}
		if ( dotTile < dot )
		{
			if ( debugMode )
				std::cout << "\tLINE : " << __LINE__ << " "  << "dot Tile is < dot : " << dotTile << " < " << dot << std::endl;
			return false;
		}
		if ( dotBall < dot )
		{
			if ( debugMode )
				std::cout << "\tLINE : " << __LINE__ << " "  << "dot Ball is < dot : " << dotBall << " < " << dot << std::endl;
			return false;
		}
	} else
	{
		if ( dotTile < 0.0 )
		{
			if ( debugMode )
				std::cout << "LINE : " << __LINE__ << " "  << "dot Tile is < 0 : " << dotTile << std::endl;
			return false;
		}
		if ( dotBall < 0.0 )
		{
			if ( debugMode )
				std::cout << "LINE : " << __LINE__ << " "  << "dot ball is < 0 : " << dotBall << std::endl;
			return false;
		}
		if ( dotTile > dot )
		{
			if ( debugMode )
				std::cout << "LINE : " << __LINE__ << " "  << "dot Tile is > dot : " << dotTile << " > " << dot << std::endl;
			return false;
		}
		if ( dotBall > dot )
		{
			if ( debugMode )
				std::cout << "LINE : " << __LINE__ << " "  << "dot Ball is > dot : " << dotBall << " > " << dot << std::endl;
			return false;
		}
	}

	return true;

}
int Ball::FindIntersectingSide( const Rect &tileRect )
{
	Vector2f ballDir = GetEsimtatedDir( );
	Vector2f ballCurrentPos( rect.x, rect.y );

	Vector2f ballEstOldPos( ballCurrentPos - ( ballDir * 5.0 ) );
	Vector2f ballEstNewPos( ballCurrentPos + ( ballDir * 20.0 ) );

	// Get ball positions
	Vector2f ballEstOldPos_Bl = Transform( ballEstOldPos, Corner::BottomLeft, rect );
	Vector2f ballEstNewPos_Bl = Transform( ballEstNewPos, Corner::BottomLeft, rect );

	Vector2f ballEstOldPos_Br = Transform( ballEstOldPos, Corner::BottomRight, rect );
	Vector2f ballEstNewPos_Br = Transform( ballEstNewPos, Corner::BottomRight, rect );

	Vector2f ballEstOldPos_Tl = Transform( ballEstOldPos, Corner::TopLeft, rect  );
	Vector2f ballEstNewPos_Tl = Transform( ballEstNewPos, Corner::TopLeft, rect  );

	Vector2f ballEstOldPos_Ml( ballEstOldPos.x, ballEstOldPos.y + ( rect.y / 2.0 ));
	Vector2f ballEstNewPos_Ml( ballEstNewPos.x, ballEstNewPos.y + ( rect.y / 2.0 ));

	Vector2f ballEstOldPos_Tr = Transform( ballEstOldPos, Corner::TopRight, rect  );

	Vector2f ballEstNewPos_Tr = Transform( ballEstNewPos, Corner::TopRight, rect  );


	if ( debugMode )
	{
		std::cout << "\tBall est old pos    : " << ballEstOldPos << std::endl;
		std::cout << "\tBall est future pos : " << ballEstOldPos << std::endl;
	}

	// Top left corner
	Vector2f rect_Tl( tileRect.x             , tileRect.y );
	Vector2f rect_Tr = Transform( rect_Tl, Corner::TopRight   , tileRect );


	Vector2f rect_Bl = Transform( rect_Tl, Corner::BottomLeft , tileRect );
	Vector2f rect_Br = Transform( rect_Tl, Corner::BottomRight, tileRect );

	double dist = 0.0f;
	double distMax = 0.0f;
	Side collisionSide = Side::Unknown;

	// Top collisions - bottom side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Tr, rect_Tl, ballEstNewPos_Bl, ballEstOldPos_Bl, dist ) )
	{
		if ( debugMode )
		{
			std::cout << "\n\tIntersected top\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		distMax = dist;
		collisionSide = Side::Top;
	}

	if ( LineLineIntersectionTestV2( rect_Tr, rect_Tl, ballEstNewPos_Br, ballEstOldPos_Br, dist  ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected top\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		distMax = dist;
		collisionSide = Side::Top;
	}

	// Right collisions - left side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Tr, rect_Br, ballEstNewPos_Tl, ballEstOldPos_Tl, dist  ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected rigt 1\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Right;
		}
	}

	if ( LineLineIntersectionTestV2( rect_Tr, rect_Br, ballEstNewPos_Bl, ballEstOldPos_Bl, dist ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected right 2\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Right;
		}
	}

	if ( LineLineIntersectionTestV2( rect_Tr, rect_Br, ballEstNewPos_Ml, ballEstOldPos_Ml, dist ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected right 3\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		/*if ( dist >= distMax )
		  {
		  distMax = dist;
		  collisionSide = Side::Right;
		  }*/
	}

	// Bottom collisions - top side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Bl, rect_Br, ballEstNewPos_Tl, ballEstOldPos_Tl, dist  ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected bottom\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Bottom;
		}
	}

	if ( LineLineIntersectionTestV2( rect_Bl, rect_Br, ballEstNewPos_Tr, ballEstOldPos_Tr, dist  ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected bottom\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Bottom;
		}
	}

	// Left collisions - right side of ball
	// ========================================================================================
	if ( LineLineIntersectionTestV2( rect_Bl, rect_Tl, ballEstNewPos_Br, ballEstOldPos_Br, dist  ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected left\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Left;
		}
	}

	// Check intersection left II
	if ( LineLineIntersectionTestV2( rect_Bl, rect_Tl, ballEstNewPos_Tr, ballEstOldPos_Tr, dist  ) )
	{

		if ( debugMode )
		{
			std::cout << "\n\tIntersected left\n";
			std::cout << "\tdist : " << dist << std::endl;
		}

		if ( dist >= distMax )
		{
			distMax = dist;
			collisionSide = Side::Left;
		}
	}

	switch ( collisionSide )
	{
		case Side::Top :
			dirY = ( dirY > 0.0f ) ? dirY * -1.0f : dirY;
			break;
		case Side::Right :
			dirX = ( dirX < 0.0f ) ? dirX * -1.0f : dirX;
			break;
		case Side::Bottom :
			dirY = ( dirY < 0.0f ) ? dirY * -1.0f : dirY;
			break;
		case Side::Left :
			dirX = ( dirX > 0.0f ) ? dirX * -1.0f : dirX;
			break;
		case Side::Unknown :
			std::cout << "Uknown collosion\n";
			std::cin.ignore();
			break;
	}

	return 0;
}
Vector2f Ball::GetEsimtatedDir( )
{
	Vector2f ballCurrentPos( rect.x   , rect.y );
	Vector2f ballPrevPos   ( oldRect.x, oldRect.y );

	return Vector2f( ballCurrentPos - ballPrevPos  );
}
Vector2f Ball::Transform( const Vector2f &vec, const Corner &side, const Rect &size ) const

{
	switch ( side )
	{
		case Corner::TopLeft:
			return vec;
		case Corner::TopRight:
			return Vector2f( vec.x + size.w, vec.y );
		case Corner::BottomLeft:
			return Vector2f( vec.x, vec.y + size.h );
		case Corner::BottomRight:
			return Vector2f( vec.x + size.w, vec.y + size.h );
	}
}

void Ball::HandleTileIntersection( const Rect &tileRect )
{
	// Check Which Face Collided
	// 0,0 = upper left
	double tileLeft =   tileRect.x;
	double tileTop =    tileRect.y;
	double tileRight =  tileRect.x + tileRect.w;
	double tileBottom = tileRect.y + tileRect.h;

	double oldLeft   = oldRect.x;
	double oldTop    = oldRect.y;
	double oldRight  = oldLeft + rect.w;
	double oldBottom = oldTop  + rect.h;

	if ( oldTop > tileBottom )
	{
		if ( debugMode )
			std::cout << "\tColided with bottom side of tile\n";

		if ( dirY < 0.0f )
			dirY *= -1.0f;
	}

	else if ( oldBottom < tileTop )
	{
		if ( debugMode )
			std::cout << "\tColided with top side of tile\n";

		if ( dirY > 0.0f )
			dirY *= -1.0f;
	}
	else if ( oldRight < tileLeft )
	{
		if ( debugMode )
			std::cout << "\tColided with left side of tile\n";

		if ( dirX > 0.0f )
			dirX *= -1.0f;
	}
	else if ( oldLeft > tileRight )
	{
		if ( debugMode )
			std::cout << "\tColided with right side of tile\n";

		if ( dirX < 0.0f )
			dirX *= -1.0f;
	} else
	{
		// At this point, we know there was a collision in the previous frame.
		std::cout << "\tCould not determine collision edge\n";

		std::cout << "\tBall direction       : " << dirX << " , " << dirY << std::endl << std::endl;

		//std::cin.ignore();
		//dirX *= -1.0f; dirY *= -1.0f;
	}

	if ( debugMode )
	{
		std::cout << "\tNew ball direction   : " << dirX << " , " << dirY << std::endl << std::endl;
	}
}
void Ball::HandleTileIntersection2( const Rect &tileRect )
{
	bool stop = true;

	double tileLeft =   tileRect.x;
	double tileTop =    tileRect.y;
	double tileRight =  tileRect.x + tileRect.w;
	double tileBottom = tileRect.y + tileRect.h;

	double oldLeft   = oldRect.x;
	double oldTop    = oldRect.y;
	double oldRight  = oldLeft + rect.w;
	double oldBottom = oldTop  + rect.h;

	PrintPosition( tileRect,"Tile");
	PrintPosition( rect    , "Ball cur ");
	PrintPosition( oldRect , "Ball old ");

	if ( dirY < 0.0f )
	{
		std::cout << "\tChecking collision bottom : \n\n";
		double distBottom = oldTop - tileBottom;

		double intersect = distBottom / dirY;
		double intersectPosLeft  = oldLeft + dirX * fabs( intersect );
		double intersectPosRight = intersectPosLeft + rect.w;

		std::cout << "\tDist bottom      : " << distBottom << std::endl;
		std::cout << "\tIntersect        : " << intersect << std::endl;
		std::cout << "\tIntersect left   : " << intersectPosLeft << std::endl;
		std::cout << "\tIntersect right  : " << intersectPosRight << std::endl;

		std::cout << "\tif ( " << intersectPosLeft  << " > " << tileLeft  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosRight << " < " << tileRight << " )" << std::endl;

		if ( distBottom > 0.0f && intersectPosRight > tileLeft && intersectPosLeft < tileRight )
		{
			std::cout << "\t\tIntersected bottom" << std::endl;
			stop = false;
		}
		else
		{
			std::cout << "\t\tMissed bottom" << std::endl;
		}
	} else 
	{
		std::cout << "\tChecking collision top : \n\n";
		double distTop = tileTop - oldBottom;

		double intersect = distTop / dirY;
		double intersectPosLeft  = oldLeft + dirX * intersect;
		double intersectPosRight = intersectPosLeft + rect.w;

		std::cout << "\tDist bottom      : " << distTop << std::endl;
		std::cout << "\tIntersect        : " << intersect << std::endl;
		std::cout << "\tIntersect left   : " << intersectPosLeft << std::endl;
		std::cout << "\tIntersect right  : " << intersectPosRight << std::endl;

		std::cout << "\tif ( " << intersectPosLeft  << " > " << tileLeft  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosRight << " < " << tileRight << " )" << std::endl;

		if ( distTop > 0.0f &&  intersectPosRight > tileLeft && intersectPosLeft < tileRight )
		{
			std::cout << "\t\tIntersected top" << std::endl;
			stop = false;
		}
		else
			std::cout << "\t\tMissed top" << std::endl;
	}

	if ( dirX > 0.0f )
	{
		std::cout << "\tChecking collision left : \n\n";
		double distLeft = tileLeft - oldRight;

		double intersect = distLeft / dirX;
		double intersectPosTop     = oldTop + dirY * fabs( intersect );
		double intersectPosBottom  = intersectPosTop + rect.h;

		std::cout << "\tDist lwf          : " << distLeft           << std::endl;
		std::cout << "\tIntersect         : " << intersect          << std::endl;
		std::cout << "\tIntersect top     : " << intersectPosTop    << std::endl;
		std::cout << "\tIntersect bottom  : " << intersectPosBottom << std::endl;

		std::cout << "\tif ( " << intersectPosTop    << " < " << tileBottom  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosBottom << " > " << tileTop     << " )" << std::endl;

		if ( distLeft > 0.0f && intersectPosTop < tileBottom && intersectPosBottom > tileTop )
		{
			std::cout << "\t\tIntersected left" << std::endl;
			stop = false;
		}
		else
		{
			std::cout << "\t\tMissed left" << std::endl;
		}
	} else if ( dirX < 0.0f )
	{
		std::cout << "\tChecking collision left : \n\n";
		double distRight = oldLeft - tileRight;

		double intersect = distRight / dirX;
		double intersectPosTop     = oldTop + dirY * fabs( intersect );
		double intersectPosBottom  = intersectPosTop + rect.h;

		std::cout << "\tDist right        : " << distRight          << std::endl;
		std::cout << "\tIntersect         : " << intersect          << std::endl;
		std::cout << "\tIntersect top     : " << intersectPosTop    << std::endl;
		std::cout << "\tIntersect bottom  : " << intersectPosBottom << std::endl;

		std::cout << "\tif ( " << intersectPosTop    << " < " << tileBottom  << " )" << std::endl;
		std::cout << "\tif ( " << intersectPosBottom << " > " << tileTop     << " )" << std::endl;

		if ( distRight > 0.0f && intersectPosTop < tileBottom && intersectPosBottom > tileTop )
		{
			std::cout << "\t\tIntersected right" << std::endl;
			stop = false;
		}
		else
		{
			std::cout << "\t\tMissed right" << std::endl;
		}
	}
}
