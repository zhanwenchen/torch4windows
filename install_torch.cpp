#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <objbase.h>
#include <shlobj.h>
#include <iostream>
#include <map>
#include <tchar.h>
#include <Esent.h>
#include <WinNT.h>
// #include <zip.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Advapi32.lib")



#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

using namespace std;


TCHAR DEFAULT_DIRECTORY[10] = L"C:\\torch\\";

std::map<std::string, std::string> URL_DICT = {
    { "Git", "https://git-scm.com/download/win" },
    { "CMake", "https://cmake.org/files/v3.7/cmake-3.7.2-win64-x64.zip" }
};

TCHAR* getDirectory() {
  TCHAR directory[MAX_PATH];
  BROWSEINFO bInfo;
  bInfo.hwndOwner = NULL;
  bInfo.pidlRoot = NULL;
  bInfo.pszDisplayName = directory; // Address of a buffer to receive the display name of the folder selected by the user
  bInfo.lpszTitle = L"Select a folder to install"; // Title of the dialog
  bInfo.ulFlags = 0 ;
  bInfo.lpfn = NULL;
  bInfo.lParam = 0;
  bInfo.iImage = -1;

  LPITEMIDLIST lpItem = SHBrowseForFolder( &bInfo);
  if( lpItem != NULL )
  {
    SHGetPathFromIDList(lpItem, directory);
    return directory;
  } else {
    return DEFAULT_DIRECTORY;
  }
}


void startProcess(int argc, TCHAR* argv[])
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if( argc != 2 )
    {
        // printf(L"Usage: %s [cmdline]\n", argv[0]);
        return;
    }

    // Start the child process.
    if( !CreateProcess( NULL,   // No module name (use command line)
        argv[1],        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf("CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

void download(TCHAR url[], TCHAR saveName[], TCHAR name[]) {
	cout << "downloading " << name << endl;
	HRESULT hr = URLDownloadToFile (NULL, url, saveName, 0, NULL );
	cout << "Done!" << endl;
	system("PAUSE");
}

bool EnumInstalledSoftware(void)
{
    HKEY hUninstKey = NULL;
    HKEY hAppKey = NULL;
    LPCTSTR sAppKeyName;
    LPCTSTR sSubKey;
    LPCTSTR sDisplayName;
    LPCTSTR sRoot = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    long lResult = ERROR_SUCCESS;
    DWORD dwType = KEY_ALL_ACCESS;
    DWORD dwBufferSize = 0;

    //Open the "Uninstall" key.
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, sRoot, 0, KEY_READ, &hUninstKey) != ERROR_SUCCESS)
    {
        return false;
    }

    for(DWORD dwIndex = 0; lResult == ERROR_SUCCESS; dwIndex++)
    {
        //Enumerate all sub keys...
        dwBufferSize = sizeof(sAppKeyName);
        if((lResult = RegEnumKeyEx(hUninstKey, dwIndex, (LPWSTR)sAppKeyName,
            &dwBufferSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS)
        {
            //Open the sub key.
            wsprintf((LPWSTR)sSubKey, L"%s\\%s", sRoot, sAppKeyName);
            if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSubKey, 0, KEY_READ, &hAppKey) != ERROR_SUCCESS)
            {
                RegCloseKey(hAppKey);
                RegCloseKey(hUninstKey);
                return false;
            }

            //Get the display name value from the application's sub key.
            dwBufferSize = sizeof(sDisplayName);
            if(RegQueryValueEx(hAppKey, L"DisplayName", NULL,
                &dwType, (unsigned char*)sDisplayName, &dwBufferSize) == ERROR_SUCCESS)
            {
                wprintf(L"%s\n", sDisplayName);
            }
            else{
                //Display name value doe not exist, this application was probably uninstalled.
            }

            RegCloseKey(hAppKey);
        }
    }

    RegCloseKey(hUninstKey);

    return true;
}

int main() {
  std::cout << getDirectory() << std::endl;
  startProcess(NULL, NULL);
}
