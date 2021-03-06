#pragma once

#include "Timer.h"
#include "Renderer.h"
#include "NetManager.h"
#include "BoardLoader.h"
#include "MenuManager.h"
#include "ConfigLoader.h"
#include "MessageSender.h"
#include "PhysicsManager.h"

#include "structs/PlayerInfo.h"

enum class DirectionX{ Left, Middle, Right };

// Forward declarations
class TCPMessage;
class Logger;
enum class TileType : int;
enum class Player : int;
enum class MessageTarget : int;
struct GamePiece;
struct Bullet;
struct Paddle;
struct Ball;
struct Tile;
class GameManager
{
	public:
		GameManager();

		// Startup options
		bool Init( const std::string &localPlayerName, const SDL_Rect &size, bool startFS );
		void InitNetManager( std::string ip_, uint16_t port_ );

		// Setters
		void SetFPSLimit( unsigned short limit );
		void SetAIControlled( bool isAIControlled_ );

		void Run();
	private:
		// Bonus Boxes
		// ===========================================
		bool WasBonusBoxSpawned( int32_t tilesDestroyed ) const;

		void AddBonusBox(std::shared_ptr< Ball > triggerBall, double x, double y, int tilesDestroyed = 1 );
		void AddBonusBox( const Player &owner, Vector2f dir,  const Vector2f &pos, int tilesDestroyed = 1 );
		void RemoveBonusBox( std::shared_ptr< BonusBox >  bb );
		void DeleteAllBonusBoxes();
		void RemoveDeadBonusBoxes();

		std::shared_ptr< BonusBox > GetBonusBoxFromID( int32_t ID );
		void ApplyBonus( std::shared_ptr< BonusBox > ptr );
		void ApplyBonus_Death( const Player &player );
		void ApplyBonus_Life( const Player &player );
		void ApplyBonus_BallSplit( );

		// Ball
		// ===========================================
		void UpdateTileHit( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > tile );
		void UpdateBallSpeed();
		void IncreaseActiveBalls( const Player &player );
		void ReduceActiveBalls( const Player &player, uint32_t ballID );
		void CheckBallSpeedFastMode( double delta);
		void IncreaseBallSpeedFastMode( const Player &player, double delta );

		void DeleteDeadBalls();
		void DeleteAllBalls();


		void AddBall( );
		std::shared_ptr<Ball> AddBall( Player owner, unsigned int ballID );
		void RemoveBall( std::shared_ptr< Ball > ball );

		bool IsSuperBall( std::shared_ptr< Ball > ball );
		double GetBallSpeed( const Player &player ) const;
		bool CanPlayerFireBall( const Player &player ) const;

		// Bullets
		// ===========================================
		void CheckBulletTileIntersections( std::shared_ptr< Bullet > bullet );
		void CheckBulletOutOfBounds( std::shared_ptr< Bullet > bullet );
		void HandleBulletTileIntersection( std::shared_ptr< Bullet > bullet, std::shared_ptr< Tile > tile );

		void DeleteDeadBullets( );
		void DeleteAllBullets();
		void FireBullets();
		std::shared_ptr< Bullet >  FireBullet( int32_t id, const Player &owner, Vector2f pos );

		bool IsSuperBullet( const Player owner ) const;
		bool AnyFastModeActive() const;

		// Tiles
		// ==========================================
		void AddTile( const Vector2f &pos, TileType tileType, int32_t tileID  );
		void RemoveTile( std::shared_ptr< Tile > tile );
		void DeleteDeadTiles();

		void CheckBallTileIntersection( std::shared_ptr< Ball > ball );
		void RemoveClosestTile( std::shared_ptr< Ball > ball, std::shared_ptr< Tile > closestTile );

		// Config
		// ===========================================
		void LoadConfig();

		// Joystick
		// ==========================================
		DirectionX GetJoystickDirection( int32_t axis );
		void DoJoystickMovement( const DirectionX &dirX );

		// Game status
		// ==========================================
		void CheckForGameStateChange( );
		bool CheckGameLobbyChange( );
		void HandleStatusChange( );
		void UpdateGameState();
		bool IsLevelDone();
		void IsGameOVer();
		void Restart();

		// AI
		// ==========================================
		void AIMove();

		// Menu
		// ==========================================
		void CreateMenu();
		void CreateMainMenu();
		void CreatePauseMenu();
		void CreateLobbyMenu();

		// New Game / Join Game / Update
		// ==========================================
		void StartNewGame();
		void JoinGame();

		// Explotion related
		// ===========================================
		int HandleExplosions( std::shared_ptr< Tile > explodingTile, Player ballOwner  );

		// Board handling
		// ===========================================
		void GenerateBoard();
		bool CanGenerateNewBoard();
		void ClearBoard();
		std::string StripLevelName( std::string levelName );
		void SetLevelName( const std::string &levelName );

		// Points / Lives
		// ===========================================
		void IncrementPoints( const TileType &tileType, bool isDestroyed, const Player &ballOwner );
		void ReducePlayerLifes( Player player );

		// Update
		// ===========================================
		void Update( double delta );
		void UpdateGameObjects( double delta );
		void UpdateBonusBoxes( double delta );
		void UpdateBullets( double delta );
		void UpdateBalls( double delta );

		void UpdateLobbyState();
		void UpdateJoystick( );
		void UpdateGameList();
		void UpdateNetwork();
		void UpdateBoard();

		// Input
		// ===========================================
		void HandleEvent( const SDL_Event &event );
		void HandleMouseEvent(  const SDL_MouseButtonEvent &buttonEvent );
		void HandleKeyboardEvent( const SDL_Event &event );
		void HandleMenuKeys( const SDL_Event &event );
		void HandleGameKeys( const SDL_Event &event );
		void HandleJoystickEvent( const SDL_JoyButtonEvent &event );

		// Input
		// ===========================================
		void InitRenderer();
		void InitJoystick();
		void InitPaddles();
		void InitMenu();

		// Network
		// ===========================================
		void PrintRecv( const TCPMessage &msg, int32_t line );

		// Recieve messages
		// ===========================================
		void ReadMessages( );
		void ReadMessagesFromServer( );
		void HandleRecieveMessage( const TCPMessage &message );

		void RecieveJoinGameMessage( const TCPMessage &message );
		void RecieveNewGameMessage( const TCPMessage &message );
		void RecieveEndGameMessage( const TCPMessage &message );
		void RecievePlayerNameMessage( const TCPMessage &message );
		void RecieveGameSettingsMessage( const TCPMessage &message);
		void RecieveGameStateChangedMessage( const TCPMessage &message );

		void RecieveTileSpawnMessage( const TCPMessage &message );
		void RecieveBallSpawnMessage( const TCPMessage &message );
		void RecieveBallDataMessage( const TCPMessage &message );
		void RecieveBallKillMessage( const TCPMessage &message );
		void RecieveTileHitMessage( const TCPMessage &message );
		void RecievePaddlePosMessage( const TCPMessage &message );
		void RecieveBonusBoxSpawnedMessage( const TCPMessage &message );
		void RecieveBonusBoxPickupMessage( const TCPMessage &message );
		void RecieveBulletFireMessage( const TCPMessage &message );
		void RecieveBulletKillMessage( const TCPMessage &message );

		void DoFPSDelay( unsigned int ticks );

		// Rendering
		// ===========================================
		void RendererScores();
		void RenderMainText( );

		// Variables
		// ===========================================
		PlayerInfo localPlayerInfo;
		PlayerInfo remotePlayerInfo;

		BoardLoader boardLoader;
		Renderer renderer;
		Timer timer;
		MenuManager menuManager;
		ConfigLoader gameConfig;
		NetManager netManager;
		MessageSender messageSender;
		PhysicsManager physicsManager;
		Logger* logger;

		bool runGame;

		std::string ip;
		uint16_t port;
		int32_t gameID;

		std::shared_ptr < Paddle > localPaddle;
		std::shared_ptr < Paddle > remotePaddle;

		bool isAIControlled;

		std::vector< std::shared_ptr< Ball     > > ballList;
		std::vector< std::shared_ptr< Tile     > > tileList;
		std::vector< std::shared_ptr< BonusBox > > bonusBoxList;
		std::vector< std::shared_ptr< Bullet   > > bulletList;

		SDL_Rect windowSize;
		double remoteResolutionScale;

		unsigned short fpsLimit;
		double frameDuration;

		SDL_Joystick *stick;
		bool respawnBalls;
};
