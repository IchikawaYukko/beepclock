#ifndef __DRAWTIME_H__
#define __DRAWTIME_H__

void draw(HWND hwnd, const PSYSTEMTIME systemtime);
void draw2Digits(HDC hdc, int time, UINT x1, UINT y1, UINT x2, UINT y2);
void draw7Segment(HDC hdc, UINT digit, UINT x, UINT y);

#endif //__DRAWTIME_H__