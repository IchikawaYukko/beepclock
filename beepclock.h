#ifndef __BEEPCLOCK_H__
#define __BEEPCLOCK_H__

/*	�^�C�}�[ID	*/
#define ID_TIMER	1
/*	�^�C�}�[���荞�݊Ԋu	*/
#define TIMER_INTERVAL	50

#define NOTIFYICON_ID		1;	//�^�X�N�g���C �A�C�R��ID
#define WM_TRAYICONMESSAGE WM_USER + 1	//�^�X�N�g���C�A�C�R���C�x���g

/*	�N���e�B�J���Z�N�V����	*/
CRITICAL_SECTION political_section;

/*	�X���b�h�ԒʐM�p�ϐ�	*/
BOOL beepflag = FALSE;
DWORD beepfreq = 0,beepDuration = 0;

/*	�v���g�^�C�v�錾	*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI BeepThread(LPVOID lpvoid);
void playBeep(DWORD freq,DWORD duration);

TCHAR szAppName[]		= TEXT("BeepClock");	//�A�v���P�[�V������
TCHAR szAppVersion[]	= TEXT("1.2");			//�o�[�W�����ԍ�
HICON hAppIcon;								//�A�v���P�[�V�����A�C�R���i�E�C���h�E����ƃ^�X�N�g���C�Ŏg�p�j
TCHAR test[64];

#endif //__BEEPCLOCK_H__