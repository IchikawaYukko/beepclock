#include <windows.h>
#include "drawtime.h"

//ウィンドウ描画処理
void draw(HWND hwnd, const PSYSTEMTIME systemtime) {
	PAINTSTRUCT paint; HDC hdc;	//描画用構造体とコンテキストハンドル
	HPEN hpen; HBRUSH hbrush;	//ペン・ブラシハンドル
	RECT clientRect;

	hdc = BeginPaint(hwnd, &paint);
	hpen = CreatePen(PS_SOLID, 0, RGB(0,255,255));		//水色ペン作成
	hbrush = CreateSolidBrush(RGB(0,0,0));				//黒ブラシ作成
	SelectObject(hdc, hpen);
	SelectObject(hdc, hbrush);

	// TODO ちらつきをおさえる。メモリデバイスコンテキストを使う
	GetClientRect(hwnd, &clientRect);	//ウィンドウのクライアント・サイズを取得
	Rectangle(hdc,
		clientRect.left,
		clientRect.top,
		clientRect.right,
		clientRect.bottom);	//クライアントエリア塗りつぶし

	// TODO マジックナンバーをやめる
	draw2Digits(hdc, systemtime->wHour, 20, 20, 50, 20);
	draw2Digits(hdc, systemtime->wMinute, 80, 20, 110, 20);
	draw2Digits(hdc, systemtime->wSecond, 140, 20, 170, 20);
	
	SelectObject(hdc, GetStockObject(NULL_PEN));
	DeleteObject(hpen);
	EndPaint(hwnd, &paint);
}

//timeで指定したデジタル数字２つを描画
void draw2Digits(HDC hdc, int time, UINT x1, UINT y1, UINT x2, UINT y2) {
	draw7Segment(hdc, time / 10, x1, y1);
	draw7Segment(hdc, time % 10, x2, y2);
}

//digitで指定したデジタル数字を１つ描画
void draw7Segment(HDC hdc, UINT digit, UINT x, UINT y) {
	POINT a[] = { x     , y     , x + 20, y };
	POINT b[] = { x + 20, y     , x + 20, y + 20};
	POINT c[] = { x + 20, y + 20, x + 20, y + 40};
	POINT d[] = { x     , y + 40, x + 20, y + 40};
	POINT e[] = { x     , y + 20, x     , y + 40};
	POINT f[] = { x     , y     , x     , y + 20};
	POINT g[] = { x     , y + 20, x + 20, y + 20};

	struct {
		BOOL	a;
		BOOL	b;
		BOOL	c;
		BOOL	d;
		BOOL	e;
		BOOL	f;
		BOOL	g;
	} light7seg[] = {
	{TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , FALSE},	//0
	{FALSE, TRUE , TRUE , FALSE, FALSE, FALSE, FALSE},	//1
	{TRUE , TRUE , FALSE, TRUE , TRUE , FALSE, TRUE},	//2
	{TRUE , TRUE , TRUE , TRUE , FALSE, FALSE, TRUE},	//3
	{FALSE, TRUE , TRUE , FALSE, FALSE, TRUE , TRUE},	//4
	{TRUE , FALSE, TRUE , TRUE , FALSE, TRUE , TRUE},	//5
	{TRUE , FALSE, TRUE , TRUE , TRUE , TRUE , TRUE},	//6
	{TRUE , TRUE , TRUE , FALSE, FALSE, FALSE, FALSE},	//7
	{TRUE , TRUE , TRUE , TRUE , TRUE , TRUE , TRUE},	//8
	{TRUE , TRUE , TRUE , TRUE , FALSE, TRUE , TRUE},	//9
	};
	BOOL show7seg[7];

	show7seg[0] = light7seg[digit].a;
	show7seg[1] = light7seg[digit].b;
	show7seg[2] = light7seg[digit].c;
	show7seg[3] = light7seg[digit].d;
	show7seg[4] = light7seg[digit].e;
	show7seg[5] = light7seg[digit].f;
	show7seg[6] = light7seg[digit].g;

	if (show7seg[0]) {
		MoveToEx(hdc, a[0].x, a[0].y, NULL);
		LineTo(hdc, a[1].x, a[1].y);
	}
	if (show7seg[1]) {
		MoveToEx(hdc, b[0].x, b[0].y, NULL);
		LineTo(hdc, b[1].x, b[1].y);
	}
	if (show7seg[2]) {
		MoveToEx(hdc, c[0].x, c[0].y, NULL);
		LineTo(hdc, c[1].x, c[1].y);
	}
	if (show7seg[3]) {
		MoveToEx(hdc, d[0].x, d[0].y, NULL);
		LineTo(hdc, d[1].x, d[1].y);
	}
	if (show7seg[4]) {
		MoveToEx(hdc, e[0].x, e[0].y, NULL);
		LineTo(hdc, e[1].x, e[1].y);
	}
	if (show7seg[5]) {
		MoveToEx(hdc, f[0].x, f[0].y, NULL);
		LineTo(hdc, f[1].x, f[1].y);
	}
	if (show7seg[6]) {
		MoveToEx(hdc, g[0].x, g[0].y, NULL);
		LineTo(hdc, g[1].x, g[1].y);
	}
}
