// cache_extract.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <commdlg.h>
#include <stdio.h>

HANDLE hIn;

BYTE Peek()
{
	BYTE c;
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
		DWORD signature;
		BOOL bNewVersion=Peek()==0xfe;
		if (bNewVersion)
		{
			if (!Read(&signature,sizeof signature) || signature!=0x435352fe)
			{
				MessageBox(NULL,"Bad signature. This isn't a cache file!",NULL,MB_OK);
				return 1;
			}
		}
		DWORD pos = 0;
		DWORD index = 0;
		BOOL bGetDirPath=TRUE;
		CHAR strDir[300];
		CHAR *strAppend="";
		while (TRUE)
		{
			if (bGetDirPath&&bNewVersion)
			{
				bGetDirPath=FALSE;
				DWORD nLen;
				if (!Read(&nLen,sizeof nLen)||nLen>=sizeof strDir||!Read(strDir,nLen))
				{
					break;
				}
				strDir[nLen]=0;
				for (int i=0;strDir[i];i+=1)
				{
					if (strDir[i]=='\\')
					{
						strDir[i]=' ';
						strAppend=&strDir[i];
					}
					else if((strDir[i]>='A'&&strDir[i]<='Z')
						||(strDir[i]>='a'&&strDir[i]<='z')
						||(strDir[i]>='0'&&strDir[i]<='9')
						||strDir[i]=='_'||strDir[i]==' ')
					{
						//okay
					}
					else
					{
						strDir[i]='_'; //replace char not allowed in path
					}
				}
			}
			if(Peek()==0xff)
			{
				index += 1;
				CHAR c;
				Read(&c,1);
				bGetDirPath=TRUE;
			}
			else
			{
				WIN32_FIND_DATA d;
				if (!Read(&d,sizeof d))
				{
					break;
				}
				CHAR b[1000];
				sprintf(b,"cache%d%s",index,strAppend);
				CreateDirectory(b,0);
				strcat(b,"\\");
				d.cFileName[sizeof(d.cFileName)-1]=0;//make sure it ends with NUL
				strcat(b,d.cFileName);
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



