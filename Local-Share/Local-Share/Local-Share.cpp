#include <iostream>
#include <thread>
#include <string>
#include <filesystem>

#include "Utility.h"
#include "ConsoleCommands.h"
#include "Local-Share.h"
#include "Config.h"
#include "HostManager.h"
#include "SharedEvents.h"

#define C ConsoleCommands::
#define SH SharedEvents::

void OnClientConnect(SocketData* data) {
	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << "client connected to host" << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void OnClientDisconnect(SocketData* data) {
	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << INFO_MESSAGE << "client disconnected from host";
	if (data->HasEntry("reason")) {
		std::cout << ". [reason : " << data->GetEntryValue("reason") << "]";
	}
	std::cout << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void OnDisconnectFromHost(SocketData* data) {
	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << CURRENT_TIME << WARNING_MESSAGE << "you disconnected from host[" << g_ClientConnection->GetServerHost() << ":" << g_ClientConnection->GetServerPort() << "]" << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);
}

void RegisterConsoleCommandHandlers() {
	if (!g_CommandManager) {
		return;
	}

	g_CommandManager->AddCommand(CONSOLE_COMMAND_HELP, C Help);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_EXIT, C Exit);
	g_CommandManager->AddCommand(CONSOLE_COMNAND_WORKINGDIR, C WorkingDir);
	
	g_CommandManager->AddCommand(CONSOLE_COMMAND_ADAPTERS, C Adapters);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_HASWIFIADAPTER, C HasWifiAdapter);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_WIFIADDRESS, C WifiAddress);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_STARTSERVER, C StartServer);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_STOPSERVER, C StopServer);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_SERVERSTATUS, C ServerStatus);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_SERVERPORT, C ServerPort);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECT, C Connect);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_DISCONNECT, C Disconnect);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECTIONSTATUS, C ConnectionStatus);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECTIONIP, C ConnectionIP);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CONNECTIONPORT, C ConnectionPort);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_SAY, C Say);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_SENDFILE, C SendFile);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_SENDDIR, C SendDir);
	g_CommandManager->AddCommand(CONSOLE_COMMAND_CANCELTRANSFER, C CancelTransfer);

	g_CommandManager->AddCommand(CONSOLE_COMMAND_ISBUSY, C IsBusy);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_HELP, COMMAND_DESCRIPTION_HELP);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_EXIT, COMMAND_DESCRIPTION_EXIT);
	g_CommandManager->AddCommandDescription(CONSOLE_COMNAND_WORKINGDIR, COMMAND_DESCRIPTION_WORKINGDIR);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_ADAPTERS, COMMAND_DESCRIPTION_ADAPTERS);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_HASWIFIADAPTER, COMMAND_DESCRIPTION_HASWIFIADAPTER);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_WIFIADDRESS, COMMAND_DESCRIPTION_WIFIADDRESS);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_STARTSERVER, COMMAND_DESCRIPTION_STARTSERVER);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_STOPSERVER, COMMAND_DESCRIPTION_STOPSERVER);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SERVERSTATUS, COMMAND_DESCRIPTION_SERVERSTATUS);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SERVERPORT, COMMAND_DESCRIPTION_SERVERPORT);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECT, COMMAND_DESCRIPTION_CONNECT);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_DISCONNECT, COMMAND_DESCRIPTION_DISCONNECT);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECTIONSTATUS, COMMAND_DESCRIPTION_CONNECTIONSTATUS);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECTIONIP, COMMAND_DESCRIPTION_CONNECTIONIP);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CONNECTIONPORT, COMMAND_DESCRIPTION_CONNECTIONPORT);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SAY, COMMAND_DESCRIPTION_SAY);
	
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SENDFILE, COMMAND_DESCRIPTION_SENDFILE);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_SENDDIR, COMMAND_DESCRIPTION_SENDDIR);
	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_CANCELTRANSFER, COMMAND_DESCRIPTION_CANCELTRANSFER);

	g_CommandManager->AddCommandDescription(CONSOLE_COMMAND_ISBUSY, COMMAND_DESCRIPTION_ISBUSY);
}

void DataShareServerProcessFace() {
	if (!g_HostManager) {
		return;
	}

	while (ApplicationRunning) {
		g_HostManager->ProcessConnection();
	}
}

void ClientConnectionProcessFace() {
	if (!g_ClientConnection) {
		return;
	}

	while (ApplicationRunning) {
		g_ClientConnection->ProcessConnection();
	}
}

void InputProcess() {
	g_CommandManager = new CommandControl();
	if (g_CommandManager) {
		RegisterConsoleCommandHandlers();

		while (ApplicationRunning) {
			std::getline(std::cin, g_CommandManager->GetInput());
			g_CommandManager->BuildCommand();

			g_CommandManager->ExecuteCommand(g_CommandManager->GetCommand());
			g_CommandManager->ClearArguments();
		}
	}else{
		SetConsoleColorMode(ConsoleMode::Output);
		std::cout << CURRENT_TIME << ERROR_MESSAGE << "couldn't allocate memory for command controller" << std::endl;
		SetConsoleColorMode(ConsoleMode::Input);
	}
}

void ShowStartupInfo() {
	std::string Undefined = "Undefined";

	std::string workingDirectory;
	GetApplicationDirectory(&workingDirectory);

	NetAdapter wifi;
	GetWifiAdapter(&wifi);

	#define CheckEmpty(str) (str.empty() ? Undefined : str)

	SetConsoleColorMode(ConsoleMode::Output);
	std::cout << "--------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "----" << std::endl;
	std::cout << "---- Server Name       : " << APPLICATION_NAME << std::endl;
	std::cout << "---- Version           : " << GetFilteredVersion(APPLICATION_VERSION) << std::endl;
	std::cout << "---- Working Directory : " << (workingDirectory.empty() ? "." : workingDirectory) << std::endl;
	std::cout << "----" << std::endl;
	std::cout << "---- Has Wifi Adapter : " << ((HasWifiAdapter() && IsValidNetAdapter(wifi)) ? "Yes" : "No") << std::endl;
	std::cout << "---- Wifi Ip Address  : " << CheckEmpty(wifi.ip) << std::endl;
	std::cout << "----" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "use `help` to see commands" << std::endl;
	SetConsoleColorMode(ConsoleMode::Input);

	#undef CheckEmpty

	SetConsoleTitleA(APPLICATION_NAME);
}

/// <summary>

// ProgressBar.cpp

// Header required to help detect window version
/*#include <sdkddkver.h>

// Macro used to reduce namespace pollution
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Reference for various Win32 API functions and 
// structure declarations.
//#include <Windows.h>

// Header needed for unicode adjustment support
#include <tchar.h>

#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_DEFAULTPROGRESSCTRL	401
#define ID_SMOOTHPROGRESSCTRL	402
#define ID_VERTICALPROGRESSCTRL	403

int value = 0;

//
//
// WndProc - Window procedure
//
//
LRESULT
CALLBACK
WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC dc = BeginPaint(hWnd,&ps);
			
			std::string portStr = std::to_string(value);

			TextOut(dc, 0, 0, portStr.c_str(), portStr.length());
		
			EndPaint(hWnd, &ps);
			//std::cout << "painting" << portStr << std::endl;
		}
		break;
	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

//
//
// WinMain - Win32 application entry point.
//
//
int
APIENTRY
CreatePWindows(
	HWND Parent,
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd)
{
	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof(iccex);
	iccex.dwICC = ICC_PROGRESS_CLASS;

	if (!InitCommonControlsEx(&iccex))
		return 1;

	// Setup window class attributes.
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpszClassName = TEXT("PROGRESSBARSAMPLE");
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;

	// Register window and ensure registration success.
	if (!RegisterClassEx(&wcex))
		return 1;
	
	// Create the window.
	HWND hWnd = ::CreateWindowEx(
		0,
		TEXT("PROGRESSBARSAMPLE"),
		TEXT("ProgressBar samples"),
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		640,
		480,
		Parent,
		NULL,
		hInstance,
		NULL);

	// Validate window.
	if (!hWnd)
		return 1;


	HWND hDefaultProgressCtrl,
		hSmoothProgressCtrl,
		hVerticalProgressCtrl;


	// Create default progress bar.
	hDefaultProgressCtrl = ::CreateWindowEx(
		0,
		PROGRESS_CLASS,
		TEXT(""),
		WS_CHILD | WS_VISIBLE,
		20,
		20,
		450,
		30,
		hWnd,
		(HMENU)ID_DEFAULTPROGRESSCTRL,
		hInstance,
		NULL);

	::SendMessage(hDefaultProgressCtrl, PBM_SETPOS, (WPARAM)(INT)40, 0);


	// Create smooth progress bar.
	hSmoothProgressCtrl = ::CreateWindowEx(
		0,
		PROGRESS_CLASS,
		TEXT(""),
		WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
		20,
		60,
		450,
		30,
		hWnd,
		(HMENU)ID_SMOOTHPROGRESSCTRL,
		hInstance,
		NULL);

	::SendMessage(hSmoothProgressCtrl, PBM_SETPOS, (WPARAM)(INT)100, 0);


	// Create vertical progress bar.
	hVerticalProgressCtrl = ::CreateWindowEx(
		0,
		PROGRESS_CLASS,
		TEXT(""),
		WS_CHILD | WS_VISIBLE | PBS_VERTICAL,
		20,
		100,
		30,
		100,
		hWnd,
		NULL,
		hInstance,
		NULL);

	::SendMessage(hVerticalProgressCtrl, PBM_SETPOS, (WPARAM)(INT)40, 0);



	WNDCLASSEX wcex2;
	ZeroMemory(&wcex2, sizeof(wcex2));

	wcex2.cbSize = sizeof(wcex2);
	wcex2.style = CS_HREDRAW | CS_VREDRAW;
	wcex2.lpszClassName = TEXT("TEXUNBUT");
	wcex2.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex2.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcex2.lpfnWndProc = WndProc;
	wcex2.hInstance = hInstance;
	// Register window and ensure registration success.
	if (!RegisterClassEx(&wcex2))
		return 1;

	HWND textWindow = ::CreateWindowEx(
		0,
		TEXT("TEXUNBUT"),
		TEXT("salam chetori?"),
		WS_CHILD | WS_VISIBLE,
		20,
		300,
		50,
		100,
		hWnd,
		NULL,
		hInstance,
		NULL);


	// Display the window.
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	std::thread upd([&]() {
		while (true) {
			RedrawWindow(textWindow, NULL, NULL,
				RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_ALLCHILDREN);
			Sleep(1);
			value++;
		}
	});

	// Main message loop.
	MSG msg;
	while (::GetMessage(&msg, hWnd, 0, 0) > 0)
		::DispatchMessage(&msg);

	// Unregister window class, freeing the memory that was
	// previously allocated for this window.
	::UnregisterClass(wcex.lpszClassName, hInstance);
	::UnregisterClass(wcex2.lpszClassName, hInstance);

	return (int)msg.wParam;
}
*/
/// </summary>
/// <returns></returns>

#include "FolderQueue.h"

int main() {

	/*std::thread winThread([]() {
		HWND ConsoleWindow = GetConsoleWindow();

		CreatePWindows(ConsoleWindow, NULL, NULL, NULL, NULL);
	});*/

	/*std::string path;
	GetApplicationDirectory(&path);
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		std::string name;
		const wchar_t* ePath = entry.path().c_str();

		const size_t pathLength = wcslen(ePath);
		
		char* pathPtr = new char[pathLength + 1];

		wcstombs_s(NULL, pathPtr, pathLength + 1, ePath, pathLength + 1);
		
		std::string fPath(pathPtr, pathLength);

		size_t start = 0;
		FindNextPos(&fPath, "\\", start, &start);
		name = fPath.substr(start + 1);

		std::cout << "path is : " << name << std::endl;
	}*/

	/*std::string dir;
	GetApplicationDirectory(&dir);

	FolderQueue queue("", true, dir);

	size_t size = 0;

	std::string name;
	while (queue.Next(&name)) {
		if (!name.empty()) {
			size += 1;
			std::cout << "name is : " << name << std::endl;
			Sleep(1500);
		}
	}

	std::cout << "queue size is : " << size << std::endl;
	*/
	g_HostManager = new HostManager();
	g_HostManager->SetServerHost(ANYMACHINE);
	g_HostManager->SetServerPort(DEFAULT_PORT);

	EventManager& hostEvents = g_HostManager->GetEventManager();

	hostEvents.AddHandler(SOCKET_CLIENT_CONNECT_MESSAGE_TYPE, OnClientConnect);
	hostEvents.AddHandler(SOCKET_CLIENT_DISCONNECT_MESSAGE_TYPE, OnClientDisconnect);

	hostEvents.AddHandler(SHARED_EVENTS_TYPE_ONSAY, SH OnSay);

	hostEvents.AddHandler(SHARED_EVENTS_TYPE_ONCREATEFILE, SH OnCreateFile);
	hostEvents.AddHandler(SHARED_EVENTS_TYPE_ONCREATEDIRECTORY, SH OnCreateDirectory);

	hostEvents.AddHandler(SHARED_EVENTS_TYPE_ONRECIEVEFILEDATA, SH OnRecieveFileData);
	hostEvents.AddHandler(SHARED_EVENTS_TYPE_ONGOTCHUNK, SH OnGotChunk);

	hostEvents.AddHandler(SHARED_EVENTS_TYPE_ONREQUESTNEXTFILE, SH OnRequestNextFile);

	g_ClientConnection = new ClientConnection();
	g_ClientConnection->SetServerHost(LOCALHOST);
	g_ClientConnection->SetServerPort(DEFAULT_PORT);

	EventManager& clientEvents = g_ClientConnection->GetEventManager();

	clientEvents.AddHandler(CLIENT_DISCONNECT_MESSAGE_TYPE, OnDisconnectFromHost);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_ONSAY, SH OnSay);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_ONCREATEFILE, SH OnCreateFile);
	clientEvents.AddHandler(SHARED_EVENTS_TYPE_ONCREATEDIRECTORY, SH OnCreateDirectory);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_ONRECIEVEFILEDATA, SH OnRecieveFileData);
	clientEvents.AddHandler(SHARED_EVENTS_TYPE_ONGOTCHUNK, SH OnGotChunk);

	clientEvents.AddHandler(SHARED_EVENTS_TYPE_ONREQUESTNEXTFILE, SH OnRequestNextFile);

	DataShareServerProcess = DataShareServerProcessFace;
	ClientConnectionProcess = ClientConnectionProcessFace;

	ShowStartupInfo();

	std::thread InputThread(InputProcess);

	InputThread.join();

	if (g_ServerThread) {
		TerminateThread(g_ServerThread->native_handle(), 1);
		g_ServerThread->detach();
		g_ServerThread = NULL;
	}
	if (g_ClientThread) {
		TerminateThread(g_ClientThread->native_handle(), 1);
		g_ClientThread->detach();
		g_ClientThread = NULL;
	}

	HostManager::CleanUp();

	return 0;
}