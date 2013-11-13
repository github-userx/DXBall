#pragma once

#include "Renderer.h"
#include "MenuItem.h"

#include "MenuList.h"
#include "HostInfo.h"

#include "enums/GameState.h"
#include "enums/LobbyState.h"
#include "enums/LobbyMenuItem.h"
#include "enums/MainMenuItemType.h"
#include "enums/PauseMenuItemType.h"

class Renderer;
class MenuManager
{
public:
	MenuManager();

	//GameState currentState;
	void AddMenuElememts( Renderer &renderer );
	void AddPauseMenuElememts( Renderer &renderer );
	void AddLobbyMenuElememts( Renderer &renderer );

	void CheckItemMouseOver( int x, int y, Renderer &renderer ) const;
	void CheckItemMouseOver_Pause( int x, int y, Renderer &renderer ) const;
	void CheckItemMouseOver_MainMenu( int x, int y, Renderer &renderer ) const;
	void CheckItemMouseOver_Lobby( int x, int y, Renderer &renderer ) const;

	bool CheckItemMouseClick( int x, int y);
	//bool CheckItemMouseClick_Pause( int x, int y);

	GameState GetGameState() const;
	void SetGameState( const GameState &gs );
	bool IsGameStateChangeValid( const GameState &gs) const;

	GameState GoBackToPreviousMenuState();
	GameState GoToMenu();
	LobbyMenuItem GetLobbyState();

	GameState GetPrevGameState() const;
	bool HasGameStateChanged();
	bool HasLobbyStateChanged();

	bool WasGameStarted() const;
	bool WasGameQuited() const;
	bool IsTwoPlayerMode() const;
	bool IsInAMenu() const;

	void Init( Renderer &renderer )
	{
		lobbyGameList = std::make_shared< MenuList >();//( "Available games :" , { 0, 0, 0, 0 } );
		lobbyGameList->Init( "Available Games : ",  { 260, 180, 400, 200 }, renderer  );
		renderer.AddMenuList( lobbyGameList.get() );
	}
	void AddGameToList( Renderer &renderer, HostInfo hostInfo )
	{
		lobbyGameList->AddItem( hostInfo, renderer );
		//lobbyGameList->AddItem( "ads",  renderer );
	}
	int32_t GetSelectedGame() const
	{
		return seletedGameID;
	}
	bool IsAnItemSelected() const
	{
		return ( seletedGameID >= 0 );
	}
	HostInfo GetSelectedGameInfo() const
	{
		return lobbyGameList->GetHostInfoForIndex( seletedGameID);;
	}
private:
	MainMenuItemType CheckIntersections( int x, int y ) const;
	PauseMenuItemType CheckIntersections_Pause( int x, int y ) const;
	LobbyMenuItem CheckIntersections_Lobby( int x, int y ) const;

	void RemoevAllUnderscores( Renderer &renderer  );

	GameState currentGameState;
	GameState prevGameState;
	bool hasGameStateChanged;
	bool isTwoPlayerMode;

	// Main menu
	MenuItem singlePlayer;
	MenuItem multiPlayer;
	MenuItem options;
	MenuItem quit;

	// Pause
	MenuItem pauseResumeButton;
	MenuItem pauseMainMenuButton;
	MenuItem pauseQuitButton;

	// Lobby
	MenuItem lobbyNewGameButton;
	MenuItem lobbyUpdateButton;
	MenuItem lobbyBackButton;
	std::shared_ptr <MenuList > lobbyGameList;
	int32_t seletedGameID;
	LobbyMenuItem lobbyState;
	bool lobbyStateChanged;
};
