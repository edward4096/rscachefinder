
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
void StrCat(LPSTR d, const CHAR*s)
{
	while(*d)d++;
	while(*s)*d++=*s++;
	*d=0;
}
DWORD StrLen(const CHAR*s)
{
	const CHAR*t = s;
	while(*s)s++;
	return s-t;
}
BOOL StrEq(const CHAR*a, const CHAR*b)
{
	int i=0;
	while(a[i])
	{
		if(a[i]!=b[i]){return FALSE;}
		else {i+=1;}
	}
	return a[i]==b[i];
}
BOOL StrSuffix(const CHAR*a, const CHAR*suffix)
{
	return StrLen(a)>StrLen(suffix)&&StrEq(a+StrLen(a)-StrLen(suffix),suffix);
}
BOOL StrPrefix(const CHAR*a, const CHAR*prefix)
{
	int i=0;
	while(prefix[i])
	{
		if(a[i]!=prefix[i]){return FALSE;}
		else {i+=1;}
	}
	return TRUE;
}
BOOL StrContain(const CHAR*a, const CHAR*c)
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
	for (size_t i=0;i<nBytes;i+=1)
	{
		((CHAR*)pDest)[i]=iValue;
	}
	return pDest;
}

// files to always archive
BOOL IsCacheFile(LPSTR str)
{
	if (StrPrefix(str,"1jfds")) return TRUE;
	if (StrPrefix(str,"94jfj")) return TRUE;
	if (StrPrefix(str,"a2155")) return TRUE;
	if (StrPrefix(str,"cht3f")) return TRUE;
	if (StrEq(str,"code.dat")) return TRUE;
	if (StrPrefix(str,"g34zx")) return TRUE;
	if (StrEq(str,"jingle0.mid")) return TRUE;
	if (StrEq(str,"jingle1.mid")) return TRUE;
	if (StrEq(str,"jingle2.mid")) return TRUE;
	if (StrEq(str,"jingle3.mid")) return TRUE;
	if (StrEq(str,"jingle4.mid")) return TRUE;
	if (StrPrefix(str,"k23lk")) return TRUE;
	if (StrPrefix(str,"k4o2n")) return TRUE;
	if (StrPrefix(str,"lam3n")) return TRUE;
	if (StrPrefix(str,"mn24j")) return TRUE;
	if (StrPrefix(str,"plam3")) return TRUE;
	if (StrEq(str,"shared_game_unpacker.dat")) return TRUE;
	if (StrEq(str,"worldmap.dat")) return TRUE;
	if (StrPrefix(str,"zck35")) return TRUE;
	if (StrPrefix(str,"zko34")) return TRUE;
	if (StrPrefix(str,"zl3kp")) return TRUE;
	if (StrPrefix(str,"zn12n")) return TRUE;
	if (StrPrefix(str,"24623168")) return TRUE;
	if (StrPrefix(str,"37966926")) return TRUE;
	if (StrPrefix(str,"236861982")) return TRUE;
	if (StrPrefix(str,"929793776")) return TRUE;
	if (StrPrefix(str,"60085811638")) return TRUE;
	if (StrPrefix(str,"1913169001452")) return TRUE;
	if (StrPrefix(str,"32993056653417")) return TRUE;
	if (StrPrefix(str,"3305336302107891869")) return TRUE;
	if (StrPrefix(str,"main_file_cache.")) return TRUE;
	if (StrPrefix(str,"loader")&&(StrSuffix(str,".jar")||StrSuffix(str,".cab")||StrSuffix(str,".zip"))) return TRUE;
	if (StrPrefix(str,"mapview")&&(StrSuffix(str,".jar")||StrSuffix(str,".cab")||StrSuffix(str,".zip"))) return TRUE;
	if (StrPrefix(str,"runescape")&&(StrSuffix(str,".jar")||StrSuffix(str,".cab")||StrSuffix(str,".zip"))) return TRUE;
	if (StrPrefix(str,"loginapplet")&&(StrSuffix(str,".jar")||StrSuffix(str,".cab")||StrSuffix(str,".zip"))) return TRUE;
	if (StrPrefix(str,"jag")&&StrSuffix(str,".dll")) return TRUE;
	if (StrSuffix(str,".jag")) return TRUE;
	if (StrSuffix(str,".mem")&&(StrPrefix(str,"entity")||StrPrefix(str,"land")||StrPrefix(str,"maps")||StrPrefix(str,"sounds"))) return TRUE;
	if (StrContain(str,"mudclient")) return TRUE;
	if (StrContain(str,".jag-")) return TRUE;
	if (StrContain(str,".mem-")) return TRUE;
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
	if (StrSuffix(str,"\\cache-93423-17382-59373-28323")) return TRUE;
	return FALSE;
}

// errors handling helpers
void Error(const CHAR*strErr,BOOL bUseLastError)
{
	CHAR strBuf[200];
	LPSTR strMessage;
	if (bUseLastError&&FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,(LPSTR)&strMessage,0,0))
	{
		strBuf[0]=0;
		StrCat(strBuf,strErr);
		StrCat(strBuf,": ");
		if (StrLen(strBuf)+StrLen(strMessage)<sizeof strBuf-1)
		{
			StrCat(strBuf,strMessage);
			MessageBox(NULL,strBuf,NULL,MB_OK);
		}
		else
		{
			MessageBox(NULL,strErr,NULL,MB_OK);
		}
	}
	else
	{
		MessageBox(NULL,strErr,NULL,MB_OK);
	}
	ExitProcess(1);
}
void Write(const LPVOID pBuf, DWORD nBytes)
{
	DWORD dwWritten;
	if (!WriteFile(hOut,pBuf,nBytes,&dwWritten,0)||dwWritten!=nBytes)
	{
		Error("Couldn't write output file",TRUE);
	}
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
	CHAR copyBuf[1000];
	WIN32_FIND_DATA d;
	HANDLE h = FindFirstFile(strDir,&d);
	DWORD dwRead;
	if (h == INVALID_HANDLE_VALUE)
	{
		return;
	}
	OutputDebugString(strDir);
	OutputDebugString("\r\n");
	DWORD nStrDirLen=StrLen(strDir)-2;
	Write(&nStrDirLen,sizeof nStrDirLen);
	Write(strDir,nStrDirLen);
	do
	{
		if (!bIsCacheDir&&!IsCacheFile(d.cFileName))
		{
			continue;
		}
		CHAR*strBuf2=(CHAR*)HeapAlloc(GetProcessHeap(),0,StrLen(strDir)+StrLen(d.cFileName)+10);
		if (!strBuf2)
		{
			Error("Out of memory",TRUE);
		}
		strBuf2[0]=0;
		StrCat(strBuf2,strDir);
		strBuf2[StrLen(strBuf2)-1]=0;
		StrCat(strBuf2,d.cFileName);
		HANDLE hIn = CreateFile(strBuf2,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		HeapFree(GetProcessHeap(),0,strBuf2);
		if (hIn != INVALID_HANDLE_VALUE)
		{
			d.nFileSizeLow=GetFileSize(hIn,&d.nFileSizeHigh);
			Write(&d,sizeof d);
			while (true)
			{
				if (!ReadFile(hIn,copyBuf,sizeof copyBuf,&dwRead,0))
				{
					Error("Couldn't copy file into archive",TRUE);
				}
				if (dwRead == 0)
				{
					break;
				}
				else
				{
					Write(copyBuf,dwRead);
				}
			}
			CloseHandle(hIn);
		}
	}while(FindNextFile(h,&d));
	FindClose(h);
	BYTE b=0xff;
	Write(&b,1);
	bSuccess=TRUE;
}
void ScanDir(LPSTR strDir)
{
	nDirCheck+=1;
	WIN32_FIND_DATA d;
	HANDLE h = FindFirstFile(strDir,&d);
	if (h == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if ((d.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			&& !StrEq(d.cFileName,".")
			&& !StrEq(d.cFileName,".."))
		{
			CHAR*strBuf=(CHAR*)HeapAlloc(GetProcessHeap(),0,StrLen(strDir)+StrLen(d.cFileName)+10);
			if (!strBuf)
			{
				Error("Out of memory",FALSE);
			}
			strBuf[0]=0;
			StrCat(strBuf,strDir);
			strBuf[StrLen(strBuf)-1]=0;
			StrCat(strBuf,d.cFileName);
			BOOL bIsCacheDir=IsCacheDir(strBuf);
			StrCat(strBuf,"\\*");
			LookIn(strBuf,bIsCacheDir);
			ScanDir(strBuf);
			HeapFree(GetProcessHeap(),0,strBuf);
		}
	}while(FindNextFile(h,&d));
	FindClose(h);
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

// user interface
LRESULT CALLBACK MainDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHAR strPath[300];
	BOOL gotDefPath;

	switch (message)
	{
		case WM_INITDIALOG:
			gotDefPath=FALSE;
			if (GetModuleFileName(NULL,strPath,sizeof strPath))
			{
				for (int i=StrLen(strPath);i>0;i--)
				{
					if (strPath[i]=='\\')
					{
						strPath[i]=0;
						break;
					}
				}
				const CHAR*strAppend="\\RunescapeCaches";
				if (StrLen(strPath)+StrLen(strAppend)<sizeof strPath-1)
				{
					gotDefPath=TRUE;
					StrCat(strPath,strAppend);
				}
			}
			if (gotDefPath&&!CreateDirectory(strPath,NULL)) {
				// the directory already exists or is not writable, don't use it
				gotDefPath=FALSE;
			}
			if (!gotDefPath&&GetEnvironmentVariable("userprofile",strPath,sizeof strPath))
			{
				const CHAR*strAppend="\\Desktop\\RunescapeCaches";
				if (StrLen(strPath)+StrLen(strAppend)<sizeof strPath-1)
				{
					gotDefPath=TRUE;
					StrCat(strPath,strAppend);
				}
			}
			if (!gotDefPath)
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
				const CHAR*strAppend="\\RSCaches.dat";
				if (StrLen(strPath)+StrLen(strAppend)>=sizeof strPath-1)
				{
					Error("Output path too long",FALSE);
				}
				StrCat(strPath,strAppend);
				hOut = CreateFile(strPath,GENERIC_READ|GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				if (hOut == INVALID_HANDLE_VALUE)
				{
					Error("Couldn't create output file",TRUE);
				}
				DWORD signature=0x435352fe;
				Write(&signature,sizeof signature);
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
					BOOL bOkay = SHGetPathFromIDList(pOut,strPath);
					CoTaskMemFree(pOut);
					if (bOkay)
					{
						SetWindowText(GetDlgItem(hDlg,IDC_OUTPATH),strPath);
					}
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

