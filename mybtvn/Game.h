#pragma once
#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Texture.h"
#include "KeyEventHandler.h"

#define MAX_FRAME_RATE 60
#define KEYBOARD_BUFFER_SIZE 1024
#define KEYBOARD_STATE_SIZE 256

class CGame
{
	static CGame* __instance;
	HWND hWnd;

	int backBufferWidth = 0;
	int backBufferHeight = 0;

	ID3D10Device* pD3DDevice = NULL;
	IDXGISwapChain* pSwapChain = NULL;
	ID3D10RenderTargetView* pRenderTargetView = NULL;
	ID3D10BlendState* pBlendStateAlpha = NULL;

	LPD3DX10SPRITE spriteObject;

	LPDIRECTINPUT8       di;
	LPDIRECTINPUTDEVICE8 didv;

	BYTE  keyStates[KEYBOARD_STATE_SIZE];
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];

	LPKEYEVENTHANDLER keyHandler;

	HINSTANCE hInstance;

public:

	void Init(HWND hWnd,HINSTANCE hInstance);

	void Draw(float x, float y, LPTEXTURE tex, RECT* rect = NULL);

	void Draw(float x, float y, LPTEXTURE tex, int l, int t, int r, int b)
	{
		RECT rect;
		rect.left = l;
		rect.top = t;
		rect.right = r;
		rect.bottom = b;
		this->Draw(x, y, tex, &rect);
	}

	LPTEXTURE LoadTexture(LPCWSTR texturePath);

	void InitKeyboard(LPKEYEVENTHANDLER handler);
	int IsKeyDown(int KeyCode);
	void ProcessKeyboard();

	ID3D10Device* GetDirect3DDevice() { return this->pD3DDevice; }
	IDXGISwapChain* GetSwapChain() { return this->pSwapChain; }
	ID3D10RenderTargetView* GetRenderTargetView() { return this->pRenderTargetView; }

	ID3DX10Sprite* GetSpriteHandler() { return this->spriteObject; }

	ID3D10BlendState* GetAlphaBlending() { return pBlendStateAlpha; };

	int GetBackBufferWidth() { return backBufferWidth; }
	int GetBackBufferHeight() { return backBufferHeight; }

	static CGame* GetInstance();

	~CGame();
};
