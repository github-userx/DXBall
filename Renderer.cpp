#include "Renderer.h"

#include "Ball.h"
#include "Tile.h"
#include "Paddle.h"

#include <algorithm>

	Renderer::Renderer()
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	:	rmask( 0xff000000 )
	,	gmask( 0x00ff0000 )
	,	bmask( 0x0000ff00 )
	,	amask( 0x000000ff )
#else
	:	rmask( 0x000000ff )
	,	gmask( 0x0000ff00 )
	,	bmask( 0x00ff0000 )
	,	amask( 0xff000000 )
#endif

	,	SCREEN_WIDTH ( 1920 / 2 )
	,	SCREEN_HEIGHT( 1280 / 2 )
	,	SCREEN_BPP ( 32 )

	,	rects()
	,	textures()

	,	backgroundArea(  NULL )
	,	backgroundImage(  NULL )
	,	screen(  NULL )
	,	ballList( /*{ }*/ )
	,	font()
	,	bigFont()
	,	text()
	,	lives()
	,	points()
	,	textColor{ 0, 140, 0, 255 }
	,	tileColors{ {48, 9, 178, 255}, {255, 55, 13, 255}, {0, 0, 0, 255}, {255, 183, 13, 255} }
	,	tileSurfaces{ nullptr, nullptr, nullptr, nullptr }
	,	hardTileColors{ { 255, 243, 233, 255}, { 222, 212, 203, 255}, { 191, 183, 175, 255},{ 127, 122, 117, 255}, { 64, 61, 58, 255} }
	,	hardTileSurfaces{ nullptr, nullptr, nullptr, nullptr, nullptr }
{

}

Renderer::~Renderer()
{
	ballList.empty();

	SDL_FreeSurface( backgroundArea );
	SDL_FreeSurface( backgroundImage );
	SDL_FreeSurface (screen );

	SDL_FreeSurface( text );
	SDL_FreeSurface( lives );
	SDL_FreeSurface( points );
	SDL_FreeSurface( localPlayerCaption );

	SDL_Quit();
}

bool Renderer::Init()
{
	// Set up screen
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF );

	if ( screen == NULL )
		return false;

	if ( TTF_Init( ) == -1 )
		return false;

	if ( !LoadImages() )
		return false;

	if ( !LoadFontAndText() )
		return false;

	BlitBackground();

	return true;
}
void Renderer::SetLocalPaddle( std::shared_ptr< Paddle >  &paddle )
{
	localPaddle = paddle;
}

void Renderer::AddBall( const std::shared_ptr< Ball > &ball )
{
	ballList.push_back( ball );
}

void Renderer::RemoveBall(  const std::shared_ptr< Ball > &ball )
{
	ballList.erase( std::find( ballList.begin(), ballList.end(), ball ) );
}

void Renderer::AddTile( const std::shared_ptr< Tile > &tile )
{
	tileList.push_back( tile );
}
void Renderer::RemoveTile( const std::shared_ptr< Tile >  &tile )
{
	tileList.erase( std::find( tileList.begin(), tileList.end(), tile) );
}




SDL_Surface* Renderer::LoadImage( const std::string &filename, GamePiece::TextureType textureType )
{
	// Temporary stoare for the iamge that's loaded
	SDL_Surface* loadedImage = NULL;

	// The optimized image that will be used
	//
	SDL_Surface* optimizedImage = NULL;

	// Load the image
	loadedImage = IMG_Load( filename.c_str() );

	// If the image loaded
	if ( loadedImage != NULL )
	{
		//
		// Create an optimized image
		optimizedImage = SDL_DisplayFormat( loadedImage );

		textures[ textureType ] = optimizedImage;
		rects[ textureType ] = optimizedImage->clip_rect;

		// Free the old image
		SDL_FreeSurface( loadedImage );
	}

	// Return the optimized image
	return optimizedImage;
}

void Renderer::SetColorKey( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b )
{
	if ( source )
	{
		Uint32 colorKey = SDL_MapRGB( source->format, r, g, b );

		SDL_SetColorKey( source, SDL_SRCCOLORKEY, colorKey );
	}
}

void Renderer::SetColorKey( GamePiece::TextureType textureID, unsigned char r, unsigned char g, unsigned char b )
{
	SetColorKey( textures[ textureID ], r, g, b );
}

void Renderer::FillSurface( SDL_Surface* source, unsigned char r, unsigned char g, unsigned char b )
{
	SDL_FillRect( source, NULL, SDL_MapRGBA( source->format, r, g, b, 255 )  );
}
	
void Renderer::FillSurface( SDL_Surface* source, const SDL_Color &color )
{
	FillSurface( source, color.r, color.g, color.b );
}

void Renderer::ApplySurface( short x, short y, SDL_Surface* source, SDL_Surface* destination ) const
{
	// Make a temp rect to hold the offsets
	SDL_Rect offset;

	// Give the offsets of the rectangle
	offset.x = x;
	offset.y = y;

	// Blit the surface
	SDL_BlitSurface( source, NULL, destination, &offset );
}
void Renderer::ApplySurface( const SDL_Rect &r, SDL_Surface* source, SDL_Surface* destination ) const
{
	ApplySurface( r.x, r.y, source, destination );
}

bool Renderer::LoadImages()
{

	LoadImage( "media/paddles/paddle30x120.png", GamePiece::Paddle );
	SetColorKey( GamePiece::Paddle, 0xff,0xff,0xff );

	LoadImage( "media/ball.png", GamePiece::Ball );
	SetColorKey( GamePiece::Ball, 0xff,0xff,0xff );

	backgroundImage= LoadImage( "media/background.png", GamePiece::Background );
	FillSurface( backgroundImage, 0, 0, 0 );

	backgroundArea = SDL_CreateRGBSurface( 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, rmask, gmask, bmask, amask);

	textures[GamePiece::Tile] = SDL_CreateRGBSurface( 0, 60, 20, 32, rmask, gmask, bmask, amask);
	FillSurface( textures[GamePiece::Tile], tileColors[0] );

	std::cout << "Adding tile surfaces : " << std::endl;
	for ( size_t i = 0; i < tileSurfaces.size() ; ++i )
		SetTileColorSurface( i, tileColors[ i ], tileSurfaces );

	std::cout << "Adding hard tile surfaces : " << std::endl;
	for ( size_t i = 0; i < hardTileSurfaces.size() ; ++i )
		SetTileColorSurface( i, hardTileColors[ i ], hardTileSurfaces );

	return true;
}
bool Renderer::LoadFontAndText()
{
	font = TTF_OpenFont( "lazy.ttf", 28 );

	if ( font == NULL )
		return false;

	bigFont = TTF_OpenFont( "lazy.ttf", 57 );

	if ( bigFont == NULL )
		return false;

	localPlayerCaption = TTF_RenderText_Solid( font, "Local player : ", textColor );

	return true;
}

void Renderer::SetTileColorSurface( size_t index, const SDL_Color &color, std::vector< SDL_Surface* > &list  )
{
	SDL_Surface* surf = SDL_CreateRGBSurface( 0, 60, 20, 32, rmask, gmask, bmask, amask);
	FillSurface( surf, color );
	list.at( index ) = surf;
}

bool Renderer::Render( )
{
	BlitBackground();
	BlitText();
	BlitForeground();

	ApplySurface( 0, 0, backgroundArea, screen );

	if ( SDL_Flip( screen ) == -1 )

		return false;
	else
		return true;
}

void Renderer::BlitBackground() const
{
	ApplySurface( 0, 0, backgroundImage, backgroundArea );
}

void Renderer::BlitForeground()
{
	// Draw balls
	for ( std::shared_ptr< Ball > gp : ballList )
	{
		ApplySurface( gp->rect.x, gp->rect.y, textures[GamePiece::Ball], backgroundArea );
	}

	// Draw tiles
	for ( std::shared_ptr< Tile > gp : tileList)
	{
		if ( gp->GetTileType() == TileTypes::Hard )
			ApplySurface( gp->rect.x, gp->rect.y, hardTileSurfaces[ 5 - gp->GetHitsLeft()], backgroundArea );
		else
			ApplySurface( gp->rect.x, gp->rect.y, tileSurfaces[ gp->GetTileTypeAsIndex() ], backgroundArea );
	}

	// Draw paddles
	ApplySurface( localPaddle->rect.x, localPaddle->rect.y, textures[GamePiece::Paddle], backgroundArea );
}

void Renderer::BlitText()
{
	if ( lives )
		ApplySurface( 10, static_cast<short> ( localPlayerCaption->h + 5 ), lives, backgroundArea );

	if ( points )
		ApplySurface( 10, static_cast< short > ( localPlayerCaption->h + lives->h + 5 ), points, backgroundArea );

	SDL_Rect screenSize = GetWindowSize();

	if ( text )
		ApplySurface(  ( screenSize.w / 2 ) - ( text->clip_rect.w / 2 ), screenSize.h / 2, text, backgroundArea );

	if ( localPlayerCaption )
		ApplySurface( 0, 0, localPlayerCaption, backgroundArea );
}
void Renderer::RenderText( const std::string &textToRender )
{
	text = TTF_RenderText_Solid( bigFont, textToRender.c_str(), textColor );
}
void Renderer::RemoveText()
{
	text = nullptr;
}
void Renderer::RenderLives( unsigned long lifeCount )
{
	std::stringstream ss;
	ss << "Lives : " << lifeCount;
	lives = TTF_RenderText_Solid( font, ss.str().c_str(), textColor );
}

void Renderer::RenderPoints( unsigned int pointCount )
{
	std::stringstream ss;
	ss << "Points : " << pointCount;
	points = TTF_RenderText_Solid( font, ss.str().c_str(), textColor );
}
SDL_Rect Renderer::GetTileSize()
{
	return rects[GamePiece::Paddle];
}
SDL_Rect Renderer::GetWindowSize()
{
	return rects[GamePiece::Background];
}
