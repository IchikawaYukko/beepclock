#ifndef __BEEPCLOCK_H__
#define __BEEPCLOCK_H__

/*	タイマーID	*/
#define ID_TIMER	1
/*	タイマー割り込み間隔	*/
#define TIMER_INTERVAL	50

#define NOTIFYICON_ID		1;	//タスクトレイ アイコンID
#define WM_TRAYICONMESSAGE WM_USER + 1	//タスクトレイアイコンイベント

/*	クリティカルセクション	*/
CRITICAL_SECTION political_section;

/*	スレッド間通信用変数	*/
BOOL beepflag = FALSE;
DWORD beepfreq = 0,beepDuration = 0;

/*	プロトタイプ宣言	*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI BeepThread(LPVOID lpvoid);
void playBeep(DWORD freq,DWORD duration);

TCHAR szAppName[]		= TEXT("BeepClock");	//アプリケーション名
TCHAR szAppVersion[]	= TEXT("1.2");			//バージョン番号
HICON hAppIcon;								//アプリケーションアイコン（ウインドウ左上とタスクトレイで使用）
TCHAR test[64];

#endif //__BEEPCLOCK_H__