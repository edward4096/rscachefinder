
#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <shellapi.h>
#include <shlobj.h>
#include "resource.h"


//globals
INT nDirCheck=0;
HANDLE hOut=INVALID_HANDLE_VALUE;
CHAR strDisk[2];
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
BOOL StrContain(LPSTR a, LPSTR c)
{
	for(int i=0;a[i];i+=1)
	{
		BOOL bMatch = TRUE;
		for (int j=0;c[j];j+=1)
		{
			if (a[i+j]!=c[j])
			{
				bMatch=FALSE;
				break;
			}
		}
		if (bMatch)
		{
			return TRUE;
		}
	}
	return FALSE;
}
#pragma function(memset)
void *memset(void *pDest, int iValue, size_t nBytes)
{
	for (int i=0;i<nBytes;i+=1)
	{
		((CHAR*)pDest)[i]=iValue;
	}
	return pDest;
}

// files to always archive
BOOL IsCacheFile(LPSTR str)
{
	if (StrEq(str,"1jfds")) return TRUE;
	if (StrEq(str,"94jfj")) return TRUE;
	if (StrEq(str,"a2155")) return TRUE;
	if (StrEq(str,"cht3f")) return TRUE;
	if (StrEq(str,"code.dat")) return TRUE;
	if (StrEq(str,"g34zx")) return TRUE;
	if (StrEq(str,"jingle0.mid")) return TRUE;
	if (StrEq(str,"jingle1.mid")) return TRUE;
	if (StrEq(str,"jingle2.mid")) return TRUE;
	if (StrEq(str,"jingle3.mid")) return TRUE;
	if (StrEq(str,"jingle4.mid")) return TRUE;
	if (StrEq(str,"k23lk")) return TRUE;
	if (StrEq(str,"k4o2n")) return TRUE;
	if (StrEq(str,"lam3n")) return TRUE;
	if (StrEq(str,"mn24j")) return TRUE;
	if (StrEq(str,"plam3")) return TRUE;
	if (StrEq(str,"shared_game_unpacker.dat")) return TRUE;
	if (StrEq(str,"sound0.wav")) return TRUE;
	if (StrEq(str,"sound1.wav")) return TRUE;
	if (StrEq(str,"sound2.wav")) return TRUE;
	if (StrEq(str,"sound3.wav")) return TRUE;
	if (StrEq(str,"sound4.wav")) return TRUE;
	if (StrEq(str,"uid.dat")) return TRUE;
	if (StrEq(str,"worldmap.dat")) return TRUE;
	if (StrEq(str,"zck35")) return TRUE;
	if (StrEq(str,"zko34")) return TRUE;
	if (StrEq(str,"zl3kp")) return TRUE;
	if (StrEq(str,"zn12n")) return TRUE;
	if (StrEq(str,"24623168")) return TRUE;
	if (StrEq(str,"37966926")) return TRUE;
	if (StrEq(str,"236861982")) return TRUE;
	if (StrEq(str,"929793776")) return TRUE;
	if (StrEq(str,"60085811638")) return TRUE;
	if (StrEq(str,"1913169001452")) return TRUE;
	if (StrEq(str,"32993056653417")) return TRUE;
	if (StrEq(str,"3305336302107891869")) return TRUE;
	if (StrPrefix(str,"main_file_cache.")) return TRUE;
	if (StrPrefix(str,"loader")&&(StrSuffix(str,".jar")||StrSuffix(str,".cab")||StrSuffix(str,".zip"))) return TRUE;
	if (StrPrefix(str,"mapview")&&(StrSuffix(str,".jar")||StrSuffix(str,".cab")||StrSuffix(str,".zip"))) return TRUE;
	if (StrPrefix(str,"runescape")&&(StrSuffix(str,".jar")||StrSuffix(str,".cab")||StrSuffix(str,".zip"))) return TRUE;
	if (StrPrefix(str,"jag")&&StrSuffix(str,".dll")) return TRUE;
	if (StrSuffix(str,".jag")) return TRUE;
	if (StrSuffix(str,".mem")) return TRUE;
	if (StrContain(str,"mudclient")) return TRUE;
	return FALSE;
}
// directories to always archive
BOOL IsCacheDir(LPSTR str)
{
	if (StrSuffix(str,"\\.jagex_cache_32")) return TRUE;
	if (StrSuffix(str,"\\.file_store_32")) return TRUE;
	if (StrSuffix(str,"\\jagexcache")) return TRUE;
	if (StrSuffix(str,"\\classic")) return TRUE;
	if (StrSuffix(str,"\\loginapplet")) return TRUE;
	if (StrSuffix(str,"\\rsmap")) return TRUE;
	if (StrSuffix(str,"\\runescape")) return TRUE;
	if (StrSuffix(str,"\\oldschool\\LIVE")) return TRUE;
	if (StrSuffix(str,"\\runescape\\LIVE")) return TRUE;
	if (StrSuffix(str,"\\runescape\\LIVE_BETA")) return TRUE;
	return FALSE;
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
		r=IsCacheFile(d.cFileName);
	}while(FindNextFile(h,&d)&&!r);
	FindClose(h);
	return r;
}
void LookIn(LPSTR strDir,BOOL bIsCacheDir)
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
		if (!bIsCacheDir&&!IsCacheFile(d.cFileName))
		{
			continue;
		}
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
			BOOL bIsCacheDir=IsCacheDir(strBuf);
			StrCat(strBuf,"\\*");
			LookIn(strBuf,bIsCacheDir);
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
	strBuf[0]=(strDisk[0]>='a'&&strDisk[0]<='z')||(strDisk[0]>='A'&&strDisk[0]<='Z')?strDisk[0]:'c';
	strBuf[1]=':';
	strBuf[2]='\\';
	strBuf[3]='*';
	strBuf[4]=0;
	ScanDir(strBuf);
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
			SetWindowText(GetDlgItem(hDlg,IDC_DISK),"C");
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
				GetWindowText(GetDlgItem(hDlg,IDC_DISK),strDisk,sizeof strDisk);
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
				SetWindowText(GetDlgItem(hDlg,IDC_STATUS),"Looking for caches...");
				EnableWindow(GetDlgItem(hDlg,IDOK),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_DISK),FALSE);
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
					CHAR strBuf[100]="Looking for caches... Folders checked:        ";
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
			break;
	}
    return FALSE;
}
extern "C" void APIENTRY WinMainCRTStartup()
{
	DialogBox(GetModuleHandle(NULL), (LPCTSTR)IDD_ABOUTBOX, NULL, (DLGPROC)MainDialogProc);
	ExitProcess(0);
}

