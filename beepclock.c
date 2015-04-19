/************************************************************************************
 *	Hardware Beep Clock ver 1.2
 *	�t�@�C�����F beepclock.c
 *	�t�@�C���̓��e�Fbeepclock ���C���v���O����
 *	�쐬�ҁFfuwuyuan/�s����q
 *	�C���F
 *
 *	�v���W�F�N�g�ݒ�F
 *	C/C++
 *		�R�[�h����
 *			�����^�C�����C�u�����F�}���`�X���b�h (/MT)
 *			��MSVCR80.dll��ˑ���
 *	�S��
 *		�����Z�b�g
 *			�}���`�o�C�g�������g�p����
 *			��Win98�Ή�
 *
 *	���C�Z���X/Licensing:
 *	���̃t�@�C���́A����҂ł��� fuwuyuan/�s����q �ɂ����MIT License�̂��ƌ��J����Ă��܂��B
 *
 *	Copyright (c) 2015 fuwuyuan/�s����q
 *	This software is released under the MIT License, see LICENSE.txt.
 *	http://opensource.org/licenses/mit-license.php
************************************************************************************/
// TODO �ݒ�_�C�A���O�����
// TODO �����\�����ۑΉ�	getTimeFormat()
// TODO beep�����T�E���h�J�[�h����o����悤�ɂ���
// TODO �o�[�W��������\��

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
	HANDLE hAppMutex;		//��d�N���h�~�p�~���[�e�b�N�X�n���h��
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(wndclass));

	hAppIcon = LoadIcon(NULL, IDI_ASTERISK);	//�A�v���P�[�V�����̃A�C�R�������[�h;

	//�Q�d�N���h�~����
	hAppMutex = CreateMutex(NULL, TRUE, TEXT("beepclock mutex"));
	if(hAppMutex == NULL) {
		MessageBox(NULL, TEXT("�~���[�e�b�N�X�擾�Ɏ��s���܂����B"), TEXT("�G���["), MB_ICONERROR);
		return EXIT_FAILURE;
	} else if( GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("�Q�d�N���͂ł��܂���B"), TEXT("�G���["), MB_ICONERROR);
		return EXIT_FAILURE;		
	}

	/*	�E�C���h�E�N���X�̒�`	*/
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

	/*	�E�C���h�E�N���X�̓o�^	*/
	if (!RegisterClass (&wndclass)) {
		_stprintf_s(test, sizeof(test), TEXT("%d"),GetLastError());
		MessageBox(NULL, test,
					szAppName, MB_ICONERROR);	//�o�^���s�Ȃ�G���[��ʒm
		//MessageBox(NULL, TEXT("Program requires WindowsNT!"),
//					szAppName, MB_ICONERROR);
		return EXIT_FAILURE;
	}
	/*	���C���E�B���h�E�̍쐬	*/
	hwnd = CreateWindow(szAppName, TEXT("BeepClock"),
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,CW_USEDEFAULT,
						210 + 8,80 + 34,
						NULL, NULL, hInstance, NULL);
	//�^�X�N�g���C�Ɋi�[�����̂Ŕ�\��
	//ShowWindow(hwnd, iCmdShow);	//�쐬�����E�B���h�E�̕\��

	/*	���b�Z�[�W���[�v	*/
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0) { 
	    if (bRet == -1) {
	        // handle the error and possibly exit
	    } else {
	        TranslateMessage(&msg); 
	        DispatchMessage(&msg); 
	    }
	}
	CloseHandle(hAppMutex);	//�~���[�e�b�N�X�j��
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	SECURITY_ATTRIBUTES security;	// TODO �Ȃ��Ă��������ۂ��@�X���b�h�p�Z�L�����e�B����
	static SYSTEMTIME systemtime;	//�V�X�e�������擾�p

	static HWND hwndTime;

	TCHAR szTimeBuffer[16] = TEXT("");
	static unsigned short bufferMaster,bufferSlave = 0;
	static HMENU hMenu = 0, hSubMenu = 0;	//�^�X�N�g���C�@�|�b�v�A�b�v���j���[�n���h��
	DWORD threadID;		//�X���b�hID
	NOTIFYICONDATA nIcon;	//�^�X�N�g���C �A�C�R���ǉ��p�\����
	POINT cursorPoint;	//�|�b�v�A�b�v���j���[�����p�J�[�\���ʒu

	switch(message) {
		case WM_CREATE :
			InitializeCriticalSection(&political_section);

			security.nLength =				sizeof(SECURITY_ATTRIBUTES);
			security.lpSecurityDescriptor =	NULL;
			security.bInheritHandle =		TRUE;
			CreateThread(&security, 0, BeepThread, NULL, 0, &threadID);
			/*	�^�C�}�[�쐬	*/
			SetTimer(hwnd, ID_TIMER, TIMER_INTERVAL, NULL);
			GetLocalTime(&systemtime);
			bufferSlave = systemtime.wSecond;

			hMenu		= LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));//�^�X�N�g���C�E�N���b�N���̃��j���[�����[�h
			hSubMenu	= GetSubMenu(hMenu, 0);

			//�^�X�N�g���C �A�C�R���ǉ� �\���̐ݒ�
			nIcon.cbSize	= sizeof(NOTIFYICONDATA);
			nIcon.uID		= NOTIFYICON_ID;
			nIcon.hWnd		= hwnd;
			nIcon.hIcon	= hAppIcon;
			nIcon.uFlags	= NIF_ICON | NIF_MESSAGE | NIF_TIP;
			nIcon.uCallbackMessage = WM_TRAYICONMESSAGE;
			_tcscpy_s(nIcon.szTip, sizeof(nIcon.szTip), szAppName);

			Shell_NotifyIcon(NIM_ADD, &nIcon);	//�^�X�N�g���C�ɃA�C�R���ǉ�
			return 0;
		case WM_COMMAND:
			//�|�b�v�A�b�v���j���[�I������
			switch(LOWORD(wParam)) {
				case ID_BEEPCLOCK_OPENWINDOW:	//�E�B���h�E���J��
					ShowWindow(hwnd, SW_SHOW);
					break;
				case ID_BEEPCLOCK_CONFIG:		//�ݒ�
					break;
				case ID_BEEPCLOCK_QUIT:			//�I��
					DestroyWindow(hwnd);			//�E�B���h�E��p��
					//�I�������� case WM_DESTROY:�ł��
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
		case WM_TRAYICONMESSAGE:	//�^�X�N�g���C�A�C�R���@�C�x���g����
			switch(lParam) {
				case WM_LBUTTONUP:	//���N���b�N��
					break;
				case WM_RBUTTONUP:	//�E�N���b�N��
					GetCursorPos(&cursorPoint);	//�E�N���b�N���ꂽ���W���擾
					//�擾�������W�Ƀ|�b�v�A�b�v���j���[��\��
					TrackPopupMenu(hSubMenu, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, cursorPoint.x, cursorPoint.y, 0, hwnd, NULL);
					break;
				case WM_LBUTTONDBLCLK: //���_�u���N���b�N��
					break;
			}
			return 0;
		case WM_CLOSE:
			ShowWindow(hwnd, SW_HIDE);
			return 0;
		case WM_DESTROY :
			/*	�I������	*/

			/* �^�X�N�g���C����A�C�R���폜 */
			nIcon.cbSize = sizeof(NOTIFYICONDATA);
			nIcon.hWnd = hwnd;
			nIcon.uFlags = 0;
			nIcon.uID = NOTIFYICON_ID;
			Shell_NotifyIcon(NIM_DELETE, &nIcon);

			DestroyMenu(hMenu);	//�|�b�v�A�b�v���j���[�ƃT�u���j���[(hSubMenu)��p��
			
			DeleteCriticalSection(&political_section);	//�N���e�B�J���Z�N�V�����폜
			KillTimer(hwnd, ID_TIMER);					//�^�C�}�[�폜
			PostQuitMessage(EXIT_SUCCESS);				//�I�����b�Z�[�W�𓊂���
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
