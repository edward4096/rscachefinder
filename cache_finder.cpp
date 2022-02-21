
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include "resource.h"


//globals
INT nDirCheck=0;
HANDLE hOut=INVALID_HANDLE_VALUE;
BOOL bSuccess=FALSE;



//strings
void StrCat(LPSTR d, LPSTR s)
{
	while(*d)d++;
	while(*s)*d++=*s++;
	*d=0;
}
DWORD StrLen(LPSTR s)
{
	LPSTR t = s;
	while(*s)s++;
	return s-t;
}
BOOL StrEq(LPSTR a, LPSTR b)
{
	int i=0;
	while(a[i])
	{
		if(a[i]!=b[i]){return FALSE;}
		else {i+=1;}
	}
	return a[i]==b[i];
}
BOOL StrSuffix(LPSTR a, LPSTR suffix)
{
	return StrLen(a)>StrLen(suffix)&&StrEq(a+StrLen(a)-StrLen(suffix),suffix);
}
BOOL StrPrefix(LPSTR a, LPSTR prefix)
{
	int i=0;
	while(prefix[i])
	{
		if(a[i]!=prefix[i]){return FALSE;}
		else {i+=1;}
	}
	return TRUE;
}

// cache searching and copying
BOOL HasCache(LPSTR strDir) 
{
	WIN32_FIND_DATA d;
	HANDLE h = FindFirstFile(strDir,&d);
	if (h == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	BOOL r=FALSE;
	do
	{
		if (StrEq(d.cFileName,"1jfds"))r=TRUE;
		if (StrEq(d.cFileName,"94jfj"))r=TRUE;
		if (StrEq(d.cFileName,"a2155"))r=TRUE;
		if (StrEq(d.cFileName,"cht3f"))r=TRUE;
		if (StrEq(d.cFileName,"code.dat"))r=TRUE;
		if (StrEq(d.cFileName,"g34zx"))r=TRUE;
		if (StrEq(d.cFileName,"jingle0.mid"))r=TRUE;
		if (StrEq(d.cFileName,"jingle1.mid"))r=TRUE;
		if (StrEq(d.cFileName,"jingle2.mid"))r=TRUE;
		if (StrEq(d.cFileName,"jingle3.mid"))r=TRUE;
		if (StrEq(d.cFileName,"jingle4.mid"))r=TRUE;
		if (StrEq(d.cFileName,"k23lk"))r=TRUE;
		if (StrEq(d.cFileName,"k4o2n"))r=TRUE;
		if (StrEq(d.cFileName,"lam3n"))r=TRUE;
		if (StrEq(d.cFileName,"mn24j"))r=TRUE;
		if (StrEq(d.cFileName,"plam3"))r=TRUE;
		if (StrEq(d.cFileName,"shared_game_unpacker.dat"))r=TRUE;
		if (StrEq(d.cFileName,"sound0.wav"))r=TRUE;
		if (StrEq(d.cFileName,"sound1.wav"))r=TRUE;
		if (StrEq(d.cFileName,"sound2.wav"))r=TRUE;
		if (StrEq(d.cFileName,"sound3.wav"))r=TRUE;
		if (StrEq(d.cFileName,"sound4.wav"))r=TRUE;
		if (StrEq(d.cFileName,"uid.dat"))r=TRUE;
		if (StrEq(d.cFileName,"worldmap.dat"))r=TRUE;
		if (StrEq(d.cFileName,"zck35"))r=TRUE;
		if (StrEq(d.cFileName,"zko34"))r=TRUE;
		if (StrEq(d.cFileName,"zl3kp"))r=TRUE;
		if (StrEq(d.cFileName,"zn12n"))r=TRUE;
		if (StrPrefix(d.cFileName,"main_file_cache."))r=TRUE;
		if (StrPrefix(d.cFileName,"mudclient"))r=TRUE;
		if (StrPrefix(d.cFileName,"loader"))r=TRUE;
		if (StrPrefix(d.cFileName,"mapview"))r=TRUE;
		if (StrSuffix(d.cFileName,".jag"))r=TRUE;
		if (StrSuffix(d.cFileName,".mem"))r=TRUE;
		
	}while(FindNextFile(h,&d)&&!r);
	FindClose(h);
	return r;
}
void LookIn(LPSTR strDir)
{
	if (!HasCache(strDir))
	{
		return;//do not copy
	}
	CHAR strBuf2[300];
	CHAR copyBuf[1000];
	WIN32_FIND_DATA d;
	HANDLE h = FindFirstFile(strDir,&d);
	DWORD dwWritten;
	DWORD dwRead;
	if (h == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		strBuf2[0]=0;
		StrCat(strBuf2,strDir);
		strBuf2[StrLen(strBuf2)-1]=0;
		StrCat(strBuf2,d.cFileName);
		HANDLE hIn = CreateFile(strBuf2,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (hIn != INVALID_HANDLE_VALUE)
		{
			WriteFile(hOut,&d,sizeof d,&dwWritten,0);
			while (true)
			{
				ReadFile(hIn,copyBuf,sizeof copyBuf,&dwRead,0);
				if (dwRead == 0)
				{
					break;
				}
				else
				{
					WriteFile(hOut,copyBuf,dwRead,&dwWritten,0);
				}
			}
			CloseHandle(hIn);
		}
	}while(FindNextFile(h,&d));
	FindClose(h);
	strBuf2[0]=(char)0xff;
	WriteFile(hOut,strBuf2,1,&dwWritten,0);
	bSuccess=TRUE;
}
void ScanDir(LPSTR strDir)
{
	nDirCheck+=1;
	//OutputDebugString(strDir);
	//OutputDebugString("\r\n");
	WIN32_FIND_DATA d;
	HANDLE h = FindFirstFile(strDir,&d);
	if (h == INVALID_HANDLE_VALUE)
	{
		return;
	}
	CHAR*strBuf=(CHAR*)HeapAlloc(GetProcessHeap(),0,300);
	do
	{
		if ((d.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			&& !StrEq(d.cFileName,".")
			&& !StrEq(d.cFileName,".."))
		{
			strBuf[0]=0;
			StrCat(strBuf,strDir);
			strBuf[StrLen(strBuf)-1]=0;
			StrCat(strBuf,d.cFileName);
			StrCat(strBuf,"\\*");
			LookIn(strBuf);
			ScanDir(strBuf);
		}
	}while(FindNextFile(h,&d));
	FindClose(h);
	HeapFree(GetProcessHeap(),0,strBuf);
}

DWORD CALLBACK Scan(LPVOID lpParam)
{
	
	CHAR strBuf[30];
	HWND hDlg=(HWND)lpParam;
	for (char c='C';c<='Z';c++)
	{
		strBuf[0]=c;
		strBuf[1]=':';
		strBuf[2]='\\';
		strBuf[3]='*';
		strBuf[4]=0;
		ScanDir(strBuf);
	}
	PostMessage(hDlg,WM_APP,0,0);
	return 0;
}

// user intreface
LRESULT CALLBACK MainDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHAR strPath[300];

	switch (message)
	{
		case WM_INITDIALOG:
			if (GetEnvironmentVariable("userprofile",strPath,sizeof strPath))
			{
				StrCat(strPath,"\\Desktop\\RunescapeCaches");
			}
			else
			{
				strPath[0]=0;
				StrCat(strPath,"C:\\RunescapeCaches");
			}
			SetWindowText(GetDlgItem(hDlg,IDC_OUTPATH),strPath);	
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			if (LOWORD(wParam) == IDOK)
			{
				GetWindowText(GetDlgItem(hDlg,IDC_OUTPATH),strPath,sizeof strPath);
				CreateDirectory(strPath,0);
				StrCat(strPath,"\\RSCaches.dat");
				hOut = CreateFile(strPath,GENERIC_READ|GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				if (hOut == INVALID_HANDLE_VALUE)
				{
					int x = GetLastError();
					MessageBox(NULL,"Couldn't create output file",NULL,MB_OK);
					ExitProcess(1);
				}
				SetTimer(hDlg,10/*id*/,1000,0);
				SetWindowText(GetDlgItem(hDlg,IDC_STATUS),"Looking for caches...\r\nThis will take a long time.");
				EnableWindow(GetDlgItem(hDlg,IDOK),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_PICKPATH),FALSE);
				CloseHandle(CreateThread(0,0,Scan,hDlg,0,0));
			}else if (LOWORD(wParam)==IDC_PICKPATH)
			{
				BROWSEINFO bi;
				bi.hwndOwner=hDlg;
				bi.pidlRoot=NULL;
				bi.pszDisplayName=strPath;
				bi.lpszTitle="Select Output Folder";
				bi.ulFlags=BIF_RETURNONLYFSDIRS;
				bi.lpfn=NULL;
				bi.lParam=0;
				bi.iImage=0;
				ITEMIDLIST*pOut=SHBrowseForFolder(&bi);
				if (pOut)
				{
					SHGetPathFromIDList(pOut,strPath);
					CoTaskMemFree(pOut);
					SetWindowText(GetDlgItem(hDlg,IDC_OUTPATH),strPath);
				}
			}
			break;

		case WM_TIMER:
			if (wParam==10) 
			{
				if (nDirCheck)
				{
					CHAR strBuf[100]="Looking for caches...\r\nThis will take a long time.\r\nFolders checked:\r\n        ";
					int nPos=StrLen(strBuf);
					int i=nDirCheck;
					while (i)
					{
						strBuf[nPos-1]=(CHAR)(i%10)+'0';
						nPos-=1;
						i/=10;
					}
					SetWindowText(GetDlgItem(hDlg,IDC_STATUS),strBuf);
				}
				
				return 0;
			}
			break;

		case WM_APP:
			{
				nDirCheck=0;
				KillTimer(hDlg,10/*id*/);
				if (bSuccess==FALSE)
				{
					SetWindowText(GetDlgItem(hDlg,IDC_STATUS),"No caches were found.");
				}
				else
				{
					SetWindowText(GetDlgItem(hDlg,IDC_STATUS),"Caches found and saved!");
					GetWindowText(GetDlgItem(hDlg,IDC_OUTPATH),strPath,sizeof strPath);
					ShellExecute(0,"OPEN",strPath,0,0,SW_SHOW);
				}
				CloseHandle(hOut);
			}
	}
    return FALSE;
}
extern "C" void APIENTRY WinMainCRTStartup()
{
	DialogBox(GetModuleHandle(NULL), (LPCTSTR)IDD_ABOUTBOX, NULL, (DLGPROC)MainDialogProc);
	ExitProcess(0);
}

