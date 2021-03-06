#pragma once

#include <vector>
//#include <SDL2/SDL.h>

#include "structs/menu_items/MenuItem.h"
#include "structs/rendering/RenderingItem.h"

#include "GameInfo.h"

#include "math/Rect.h"

struct List
{
	// General
	// ========================================================================================================
	List( );
	virtual ~List( );
	void Init( SDL_Renderer* renderer, SDL_Rect mainRect_, const SDL_Color &backgroundColor  );
	void InitTexture( SDL_Renderer* renderer, const std::string &text, TTF_Font* font, const SDL_Color &textColor );

	SDL_Rect GetRect() const;
	const RenderingItem< uint64_t > & GetMainArea() const;
	const RenderingItem< uint64_t > & GetCaption() const;

	// Scroll Bar
	// ========================================================================================================
	void InitScrollBar();
	void InitClipRect();

	SDL_Rect GetScrollBar() const;
	SDL_Rect GetTopArrow() const;
	SDL_Rect GetBottomArrow() const;
	const SDL_Rect* GetListClipRect() const;

	void CheckScrollBarIntersection( int32_t x, int32_t y );

	void IncrementItemsTop( int32_t amount )
	{
		itemsTop += amount;
	}
	void ResetItemsTop()
	{
		itemsTop = caption.rect.y + caption.rect.h + 10;
	}
	int32_t GetItemsTop()
	{
		return itemsTop;
	}
	int32_t GetItemsLeft()
	{
		return mainArea.rect.x + 10;
	}
	int32_t GetItemListTop() const
	{
		return listClipRect.y;
	}
	int32_t GetItemLisBottom() const
	{
		return listClipRect.y + listClipRect.h;
	}
	SDL_Color GetItemColor1() const;
	SDL_Color GetItemColor2() const;
	void SetItemColor1( const SDL_Color &clr );
	void SetItemColor2( const SDL_Color &clr );

	int32_t GetItemCount() const
	{
		return itemCount;
	}
	void IncrementItemCount()
	{
		++itemCount;
	}

	private:
	bool CanScrollUp() const;
	bool CanScrollDown() const;

	virtual void ScrollDown() = 0;
	virtual void ScrollUp() = 0;

	virtual int32_t FindTopItem() const = 0;
	virtual int32_t FindBottomItem() const =0;

	SDL_Rect scrollBar;
	SDL_Rect topArrow;
	SDL_Rect bottomArrow;
	SDL_Rect listClipRect;

	SDL_Color itemColor1;
	SDL_Color itemColor2;

	int32_t itemsTop;
	int32_t itemCount;

	RenderingItem< uint64_t > mainArea;
	RenderingItem< uint64_t > caption;
};
