// cache_extract.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

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
		HANDLE hIn = CreateFile(f.lpstrFile,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (hIn == INVALID_HANDLE_VALUE)
		{
			MessageBox(NULL,"Cannot access file",NULL,MB_OK);
			return 1;
		}
		DWORD size = GetFileSize(hIn,NULL);
		char*buf=(char*)HeapAlloc(GetProcessHeap(),0,size);
		ReadFile(hIn,buf,size,&size,0);
		CloseHandle(hIn);
		DWORD pos = 0;
		DWORD index = 0;
		while (pos<size)
		{
			if(buf[pos]==(char)0xff)
			{
				index += 1;
				pos += 1;
			}
			else
			{
				
				CHAR b[300];
				b[0]='c';
				b[1]='a';
				b[2]='c';
				b[3]='h';
				b[4]='e';
				b[5]='0'+(char)index;
				b[6]=0;
				CreateDirectory(b,0);
				WIN32_FIND_DATA d;
				if (pos+sizeof d>size)
				{
					break;
				}
				b[6]='\\';
				CopyMemory(&d,&buf[pos],sizeof d);
				pos+=sizeof d;
				if (pos+d.nFileSizeLow>size)
				{
					break;
				}
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
				WriteFile(hOut,&buf[pos],d.nFileSizeLow,&br,0);
				SetFileTime(hOut,&d.ftCreationTime,&d.ftLastAccessTime,&d.ftLastWriteTime);
				CloseHandle(hOut);
				pos+=d.nFileSizeLow;
			}
		}
		MessageBox(NULL,"All complete.","Extract",MB_OK);
	}
	return 0;
}



