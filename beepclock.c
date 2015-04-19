/************************************************************************************
 *	Hardware Beep Clock ver 1.2
 *	ファイル名： beepclock.c
 *	ファイルの内容：beepclock メインプログラム
 *	作成者：fuwuyuan/市川ゆり子
 *	修正：
 *
 *	プロジェクト設定：
 *	C/C++
 *		コード生成
 *			ランタイムライブラリ：マルチスレッド (/MT)
 *			※MSVCR80.dll非依存化
 *	全般
 *		文字セット
 *			マルチバイト文字を使用する
 *			※Win98対応
 *
 *	ライセンス/Licensing:
 *	このファイルは、著作者である fuwuyuan/市川ゆり子 によってMIT Licenseのもと公開されています。
 *
 *	Copyright (c) 2015 fuwuyuan/市川ゆり子
 *	This software is released under the MIT License, see LICENSE.txt.
 *	http://opensource.org/licenses/mit-license.php
************************************************************************************/
// TODO 設定ダイアログを作る
// TODO 時刻表示国際対応	getTimeFormat()
// TODO beep音をサウンドカードから出せるようにする
// TODO バージョン情報を表示

#define WINVER	 0x0401		//for Windows 98

#include <windows.h>
#include <tchar.h>
#include <stdio.h>	//for _stprintf -> sprintf
//#include <assert.h>
#include "resource.h"
#include "beepclock.h"
#include "drawtime.h"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR szCmdLine, int iCmdShow) {
	HWND hwnd;
	MSG msg;
	BOOL bRet;
	HANDLE hAppMutex;		//二重起動防止用ミューテックスハンドル
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(wndclass));

	hAppIcon = LoadIcon(NULL, IDI_ASTERISK);	//アプリケーションのアイコンをロード;

	//２重起動防止処理
	hAppMutex = CreateMutex(NULL, TRUE, TEXT("beepclock mutex"));
	if(hAppMutex == NULL) {
		MessageBox(NULL, TEXT("ミューテックス取得に失敗しました。"), TEXT("エラー"), MB_ICONERROR);
		return EXIT_FAILURE;
	} else if( GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("２重起動はできません。"), TEXT("エラー"), MB_ICONERROR);
		return EXIT_FAILURE;		
	}

	/*	ウインドウクラスの定義	*/
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= hAppIcon;
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH) GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= szAppName;

	/*	ウインドウクラスの登録	*/
	if (!RegisterClass (&wndclass)) {
		_stprintf_s(test, sizeof(test), TEXT("%d"),GetLastError());
		MessageBox(NULL, test,
					szAppName, MB_ICONERROR);	//登録失敗ならエラーを通知
		//MessageBox(NULL, TEXT("Program requires WindowsNT!"),
//					szAppName, MB_ICONERROR);
		return EXIT_FAILURE;
	}
	/*	メインウィンドウの作成	*/
	hwnd = CreateWindow(szAppName, TEXT("BeepClock"),
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,CW_USEDEFAULT,
						210 + 8,80 + 34,
						NULL, NULL, hInstance, NULL);
	//タスクトレイに格納したので非表示
	//ShowWindow(hwnd, iCmdShow);	//作成したウィンドウの表示

	/*	メッセージループ	*/
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0) { 
	    if (bRet == -1) {
	        // handle the error and possibly exit
	    } else {
	        TranslateMessage(&msg); 
	        DispatchMessage(&msg); 
	    }
	}
	CloseHandle(hAppMutex);	//ミューテックス破棄
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	SECURITY_ATTRIBUTES security;	// TODO なくてもいいっぽい　スレッド用セキュリティ属性
	static SYSTEMTIME systemtime;	//システム時刻取得用

	static HWND hwndTime;

	TCHAR szTimeBuffer[16] = TEXT("");
	static unsigned short bufferMaster,bufferSlave = 0;
	static HMENU hMenu = 0, hSubMenu = 0;	//タスクトレイ　ポップアップメニューハンドル
	DWORD threadID;		//スレッドID
	NOTIFYICONDATA nIcon;	//タスクトレイ アイコン追加用構造体
	POINT cursorPoint;	//ポップアップメニュー処理用カーソル位置

	switch(message) {
		case WM_CREATE :
			InitializeCriticalSection(&political_section);

			security.nLength =				sizeof(SECURITY_ATTRIBUTES);
			security.lpSecurityDescriptor =	NULL;
			security.bInheritHandle =		TRUE;
			CreateThread(&security, 0, BeepThread, NULL, 0, &threadID);
			/*	タイマー作成	*/
			SetTimer(hwnd, ID_TIMER, TIMER_INTERVAL, NULL);
			GetLocalTime(&systemtime);
			bufferSlave = systemtime.wSecond;

			hMenu		= LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));//タスクトレイ右クリック時のメニューをロード
			hSubMenu	= GetSubMenu(hMenu, 0);

			//タスクトレイ アイコン追加 構造体設定
			nIcon.cbSize	= sizeof(NOTIFYICONDATA);
			nIcon.uID		= NOTIFYICON_ID;
			nIcon.hWnd		= hwnd;
			nIcon.hIcon	= hAppIcon;
			nIcon.uFlags	= NIF_ICON | NIF_MESSAGE | NIF_TIP;
			nIcon.uCallbackMessage = WM_TRAYICONMESSAGE;
			_tcscpy_s(nIcon.szTip, sizeof(nIcon.szTip), szAppName);

			Shell_NotifyIcon(NIM_ADD, &nIcon);	//タスクトレイにアイコン追加
			return 0;
		case WM_COMMAND:
			//ポップアップメニュー選択処理
			switch(LOWORD(wParam)) {
				case ID_BEEPCLOCK_OPENWINDOW:	//ウィンドウを開く
					ShowWindow(hwnd, SW_SHOW);
					break;
				case ID_BEEPCLOCK_CONFIG:		//設定
					break;
				case ID_BEEPCLOCK_QUIT:			//終了
					DestroyWindow(hwnd);			//ウィンドウを廃棄
					//終了処理は case WM_DESTROY:でやる
					break;
			}
			return 0;
		case WM_TIMER:
			GetLocalTime(&systemtime);
			bufferMaster = bufferSlave;
			bufferSlave = systemtime.wSecond;

			if(bufferMaster != bufferSlave) {
				_stprintf_s(szTimeBuffer, sizeof(szTimeBuffer), TEXT("%02u:%02u:%02u"), systemtime.wHour, systemtime.wMinute, systemtime.wSecond);
				SetWindowText(hwnd, szTimeBuffer);
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);

				switch(systemtime.wMinute) {
					case 0:
						if(systemtime.wSecond == 0)
							playBeep(880,800);
						break;
					case 59:
						//if(systemtime.wSecond >= 55 && systemtime.wSecond <=56)	//China
						if(systemtime.wSecond >= 57 && systemtime.wSecond <= 59 )
							playBeep(440,100);
						break;
					case 30:
						if(systemtime.wSecond == 0)
							playBeep(880,800);
						break;
					default:
						switch(systemtime.wSecond) {
							case 0:
								playBeep(880,800);
								break;
							case 57:
							case 58:
							case 59:
								playBeep(440,100);
								break;
							default:
								//playBeep(1000,100);	//PerSecond
								break;
						}
						break;
				}
			}
			return 0;
		case WM_PAINT :
			draw(hwnd, &systemtime);
			break;
		case WM_TRAYICONMESSAGE:	//タスクトレイアイコン　イベント処理
			switch(lParam) {
				case WM_LBUTTONUP:	//左クリック時
					break;
				case WM_RBUTTONUP:	//右クリック時
					GetCursorPos(&cursorPoint);	//右クリックされた座標を取得
					//取得した座標にポップアップメニューを表示
					TrackPopupMenu(hSubMenu, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, cursorPoint.x, cursorPoint.y, 0, hwnd, NULL);
					break;
				case WM_LBUTTONDBLCLK: //左ダブルクリック時
					break;
			}
			return 0;
		case WM_CLOSE:
			ShowWindow(hwnd, SW_HIDE);
			return 0;
		case WM_DESTROY :
			/*	終了処理	*/

			/* タスクトレイからアイコン削除 */
			nIcon.cbSize = sizeof(NOTIFYICONDATA);
			nIcon.hWnd = hwnd;
			nIcon.uFlags = 0;
			nIcon.uID = NOTIFYICON_ID;
			Shell_NotifyIcon(NIM_DELETE, &nIcon);

			DestroyMenu(hMenu);	//ポップアップメニューとサブメニュー(hSubMenu)を廃棄
			
			DeleteCriticalSection(&political_section);	//クリティカルセクション削除
			KillTimer(hwnd, ID_TIMER);					//タイマー削除
			PostQuitMessage(EXIT_SUCCESS);				//終了メッセージを投げる
			return 0;
		default:
			break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

DWORD WINAPI BeepThread(LPVOID lpvoid) {
	BOOL flag;
	DWORD freq, duration;
	while(TRUE) {
		EnterCriticalSection(&political_section);
			flag = beepflag;
			freq = beepfreq;
			duration = beepDuration;		
			beepflag = FALSE;
		LeaveCriticalSection(&political_section);

		if(flag == TRUE) {
			Beep(freq, duration);
		}
		Sleep(1);
	}
	return 0;
}

void playBeep(DWORD freq,DWORD duration) {
	beepfreq = freq;
	beepDuration = duration;
	beepflag = TRUE;
}
