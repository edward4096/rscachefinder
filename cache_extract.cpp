// cache_extract.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <commdlg.h>

HANDLE hIn;

CHAR Peek()
{
	CHAR c;
	DWORD r;
	ReadFile(hIn,&c,1,&r,0);
	SetFilePointer(hIn,-1,0,FILE_CURRENT);
	return r==1?c:0;
}

BOOL Read(void*pOut,DWORD nBytes) 
{
	DWORD r;
	ReadFile(hIn,pOut,nBytes,&r,0);
	return r==nBytes;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	OPENFILENAME f;
	ZeroMemory(&f,sizeof f);
	char szFile[260];
	f.lStructSize = sizeof f;
	f.lpstrFile = szFile;
	f.lpstrFile[0] = 0;
	f.nMaxFile = sizeof szFile;
	f.lpstrFilter="Cache data\0*.dat\0";
	f.nFilterIndex=1;
	f.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&f))
	{
		hIn = CreateFile(f.lpstrFile,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (hIn == INVALID_HANDLE_VALUE)
		{
			MessageBox(NULL,"Cannot access file",NULL,MB_OK);
			return 1;
		}
		DWORD pos = 0;
		DWORD index = 0;
		while (TRUE)
		{
			if(Peek()==(char)0xff)
			{
				index += 1;
				CHAR c;
				Read(&c,1);
			}
			else
			{
				WIN32_FIND_DATA d;
				if (!Read(&d,sizeof d))
				{
					break;
				}
				CHAR b[300];
				b[0]='c';
				b[1]='a';
				b[2]='c';
				b[3]='h';
				b[4]='e';
				b[5]='0'+(char)index;
				b[6]=0;
				CreateDirectory(b,0);
				b[6]='\\';
				for (int i=0;d.cFileName[i]&&i<sizeof d.cFileName;i+=1)
				{
					b[7+i]=d.cFileName[i];
					b[8+i]=0;
				}
				HANDLE hOut=CreateFile(b,GENERIC_WRITE|GENERIC_READ,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
				if (hOut == INVALID_HANDLE_VALUE)
				{
					int e=GetLastError();
					MessageBox(NULL,"Cannot access output, maybe the file already exists?",NULL,MB_OK);
					return 1;
				}
				DWORD br;
				CHAR buf[1<<12];
				while (d.nFileSizeHigh)
				{
					d.nFileSizeHigh-=1;
					for (int i=0;i<1<<(32-12);i+=1)
					{
						if (!Read(buf,sizeof buf))
						{
							d.nFileSizeHigh=0;
							d.nFileSizeLow=0;
							break;
						}
						WriteFile(hOut,buf,sizeof buf,&br,0);
					}
				}
				DWORD pos=0;
				while (pos<d.nFileSizeLow)
				{
					DWORD left=d.nFileSizeLow-pos;
					br=left>sizeof buf?sizeof buf:left;
					if (!Read(buf,br))
					{
						break;
					}
					pos+=br;
					WriteFile(hOut,buf,br,&br,0);
				}
				SetFileTime(hOut,&d.ftCreationTime,&d.ftLastAccessTime,&d.ftLastWriteTime);
				CloseHandle(hOut);
			}
		}
		MessageBox(NULL,"All complete.","Extract",MB_OK);
	}
	return 0;
}



