#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <vector>

#include "debug.h"
#include "Game.h"
#include "Textures.h"
#include "Sprite.h"
#include "Sprites.h"
#include "Animation.h"
#include "Animations.h"
#include "GameObject.h"
#include "KeyEventHandler.h"

#define WINDOW_CLASS_NAME L"LifeForceWindow"
#define MAIN_WINDOW_TITLE L"Life Force (NES) - Player & Bullets"
#define WINDOW_ICON_PATH L"mario.ico"

#define BACKGROUND_COLOR D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define ID_TEX_LIFEFORCE 1
#define TEXTURE_PATH_LIFEFORCE L"lifeforce.png"

#define ID_SPRITE_VIC_VIPER 10001
#define ID_SPRITE_BULLET    20001
#define ID_SPRITE_ENEMY     30001
#define ID_SPRITE_EXP_1     40001
#define ID_SPRITE_EXP_2     40002
#define ID_SPRITE_EXP_3     40003

#define SHIP_SPEED 0.15f
#define BULLET_SPEED 0.35f

class CBullet : public CGameObject
{
protected:
	float vx;
	bool isDeleted;
public:
	CBullet(float x, float y) : CGameObject(x, y)
	{
		this->vx = BULLET_SPEED;
		this->isDeleted = false;
	}

	void Update(DWORD dt)
	{
		x += vx * dt;
		int backBufferWidth = CGame::GetInstance()->GetBackBufferWidth();
		if (x > backBufferWidth)
		{
			isDeleted = true;
		}
	}

	void Render()
	{
		CSprites::GetInstance()->Get(ID_SPRITE_BULLET)->Draw(x, y);
	}

	void Delete() { isDeleted = true; }
	bool IsDeleted() { return isDeleted; }
};

std::vector<CBullet*> bullets;

class CShip : public CGameObject
{
protected:
	float vx;
	float vy;
public:
	CShip(float x, float y) : CGameObject(x, y)
	{
		this->vx = 0.0f;
		this->vy = 0.0f;
	}

	void SetSpeed(float vx, float vy)
	{
		this->vx = vx;
		this->vy = vy;
	}

	void Shoot()
	{
		bullets.push_back(new CBullet(x + 3.0f, y));
	}

	void Update(DWORD dt)
	{
		x += vx * dt;
		y += vy * dt;

		int backBufferWidth = CGame::GetInstance()->GetBackBufferWidth();
		int backBufferHeight = CGame::GetInstance()->GetBackBufferHeight();

		if (x < 12.0f) x = 12.0f;
		if (x > backBufferWidth - 12.0f) x = backBufferWidth - 12.0f;
		if (y < 8.0f) y = 8.0f;
		if (y > backBufferHeight - 8.0f) y = backBufferHeight - 8.0f;
	}

	void Render()
	{
		CSprites::GetInstance()->Get(ID_SPRITE_VIC_VIPER)->Draw(x, y);
	}
};

CShip* ship = NULL;

class CEnemy : public CGameObject
{
protected:
	bool isDeleted;
public:
	CEnemy(float x, float y) : CGameObject(x, y) { this->isDeleted = false; }
	void Update(DWORD dt) {}
	void Render()
	{
		CSprites::GetInstance()->Get(ID_SPRITE_ENEMY)->Draw(x, y);
	}
	void Delete() { isDeleted = true; }
	bool IsDeleted() { return isDeleted; }
};

std::vector<CEnemy*> enemies;

class CExplosion : public CGameObject
{
protected:
	ULONGLONG startTime;
	bool isDeleted;
public:
	CExplosion(float x, float y) : CGameObject(x, y)
	{
		this->startTime = GetTickCount64();
		this->isDeleted = false;
	}

	void Update(DWORD dt)
	{

		if (GetTickCount64() - startTime > 300)
		{
			isDeleted = true;
		}
	}

	void Render()
	{
		ULONGLONG elapsed = GetTickCount64() - startTime;
		int spriteId = ID_SPRITE_EXP_1;
		if (elapsed >= 200)
			spriteId = ID_SPRITE_EXP_3;
		else if (elapsed >= 100)
			spriteId = ID_SPRITE_EXP_2;

		CSprites::GetInstance()->Get(spriteId)->Draw(x, y);
	}

	bool IsDeleted() { return isDeleted; }
};

std::vector<CExplosion*> explosions;

class CShipKeyHandler : public CKeyEventHandler
{
public:
	void KeyState(BYTE* states)
	{
		CGame* game = CGame::GetInstance();
		float vx = 0.0f;
		float vy = 0.0f;

		if (game->IsKeyDown(DIK_RIGHT) || game->IsKeyDown(DIK_D)) vx += SHIP_SPEED;
		if (game->IsKeyDown(DIK_LEFT)  || game->IsKeyDown(DIK_A)) vx -= SHIP_SPEED;
		if (game->IsKeyDown(DIK_UP)    || game->IsKeyDown(DIK_W)) vy -= SHIP_SPEED;
		if (game->IsKeyDown(DIK_DOWN)  || game->IsKeyDown(DIK_S)) vy += SHIP_SPEED;

		if (ship != NULL)
		{
			ship->SetSpeed(vx, vy);
		}
	}

	void OnKeyDown(int KeyCode)
	{
		if (KeyCode == DIK_SPACE)
		{
			if (ship != NULL) ship->Shoot();
		}
		else if (KeyCode == DIK_R)
		{

			if (enemies.empty())
			{
				enemies.push_back(new CEnemy(250.0f, 60.0f));
				enemies.push_back(new CEnemy(250.0f, 110.0f));
				enemies.push_back(new CEnemy(250.0f, 160.0f));
			}
		}
	}

	void OnKeyUp(int KeyCode) {}
};

CShipKeyHandler* keyHandler = NULL;

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void LoadResources()
{
	CTextures* textures = CTextures::GetInstance();
	textures->Add(ID_TEX_LIFEFORCE, TEXTURE_PATH_LIFEFORCE);

	CSprites* sprites = CSprites::GetInstance();
	LPTEXTURE texLifeForce = textures->Get(ID_TEX_LIFEFORCE);

	sprites->Add(ID_SPRITE_VIC_VIPER, 56, 8, 79, 23, texLifeForce);

	sprites->Add(ID_SPRITE_BULLET, 250, 15, 254, 16, texLifeForce);

	sprites->Add(ID_SPRITE_ENEMY, 56, 32, 79, 47, texLifeForce);

	sprites->Add(ID_SPRITE_EXP_1, 24, 58, 47,  73, texLifeForce);
	sprites->Add(ID_SPRITE_EXP_2, 56, 58, 79,  73, texLifeForce);
	sprites->Add(ID_SPRITE_EXP_3, 88, 58, 111, 73, texLifeForce);

	ship = new CShip(40.0f, 110.0f);

	enemies.push_back(new CEnemy(250.0f, 60.0f));
	enemies.push_back(new CEnemy(250.0f, 110.0f));
	enemies.push_back(new CEnemy(250.0f, 160.0f));
}

void Update(DWORD dt)
{
	if (ship != NULL) ship->Update(dt);

	for (size_t i = 0; i < bullets.size(); i++)
	{
		for (size_t j = 0; j < enemies.size(); j++)
		{

			if (abs(bullets[i]->GetX() - enemies[j]->GetX()) < 16.0f &&
			    abs(bullets[i]->GetY() - enemies[j]->GetY()) < 10.0f)
			{

				explosions.push_back(new CExplosion(enemies[j]->GetX(), enemies[j]->GetY()));

				bullets[i]->Delete();
				enemies[j]->Delete();
				break;
			}
		}
	}

	for (size_t i = 0; i < enemies.size(); i++)
	{
		enemies[i]->Update(dt);
		if (enemies[i]->IsDeleted())
		{
			delete enemies[i];
			enemies.erase(enemies.begin() + i);
			i--;
		}
	}

	for (size_t i = 0; i < explosions.size(); i++)
	{
		explosions[i]->Update(dt);
		if (explosions[i]->IsDeleted())
		{
			delete explosions[i];
			explosions.erase(explosions.begin() + i);
			i--;
		}
	}

	for (size_t i = 0; i < bullets.size(); i++)
	{
		bullets[i]->Update(dt);
		if (bullets[i]->IsDeleted())
		{
			delete bullets[i];
			bullets.erase(bullets.begin() + i);
			i--;
		}
	}
}

void Render()
{
	CGame* g = CGame::GetInstance();

	ID3D10Device* pD3DDevice = g->GetDirect3DDevice();
	IDXGISwapChain* pSwapChain = g->GetSwapChain();
	ID3D10RenderTargetView* pRenderTargetView = g->GetRenderTargetView();
	ID3DX10Sprite* spriteHandler = g->GetSpriteHandler();

	if (pD3DDevice != NULL)
	{
		pD3DDevice->ClearRenderTargetView(pRenderTargetView, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);

		FLOAT NewBlendFactor[4] = { 0, 0, 0, 0 };
		pD3DDevice->OMSetBlendState(g->GetAlphaBlending(), NewBlendFactor, 0xffffffff);

		for (size_t i = 0; i < bullets.size(); i++)
		{
			bullets[i]->Render();
		}

		for (size_t i = 0; i < enemies.size(); i++)
		{
			enemies[i]->Render();
		}

		for (size_t i = 0; i < explosions.size(); i++)
		{
			explosions[i]->Render();
		}

		if (ship != NULL) ship->Render();

		spriteHandler->End();
		pSwapChain->Present(0, 0);
	}
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = (HICON)LoadImage(hInstance, WINDOW_ICON_PATH, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd)
	{
		DWORD ErrCode = GetLastError();
		DebugOut(L"[ERROR] CreateWindow failed! ErrCode: %d\nAt: %s %d \n", ErrCode, _W(__FILE__), __LINE__);
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SetDebugWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	ULONGLONG frameStart = GetTickCount64();
	ULONGLONG tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ULONGLONG now = GetTickCount64();
		ULONGLONG dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;
			CGame::GetInstance()->ProcessKeyboard();
			Update((DWORD)dt);
			Render();
		}
		else
			Sleep((DWORD)(tickPerFrame - dt));
	}

	return 1;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	CGame* game = CGame::GetInstance();
	game->Init(hWnd, hInstance);

	keyHandler = new CShipKeyHandler();
	game->InitKeyboard(keyHandler);

	LoadResources();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}
