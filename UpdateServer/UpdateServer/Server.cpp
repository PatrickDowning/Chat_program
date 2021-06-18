#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Make sure build environment links to Winsock library file
#pragma comment(lib, "Ws2_32.lib")

// Define default global constants
const char FILENAME[] = "data.txt";
const char IPADDR[] = "127.0.0.1";
const int  PORT = 5000;
const int  QUERY = 1;
const int  UPDATE = 2;
const int  CACHEREQUESTS = 5;

// Define global variable for listen socket so it can be
// closed by the shutdown handler
static SOCKET listenSocket = INVALID_SOCKET;

// Event handler to process Ctrl-C and Window close events
static BOOL WINAPI Handler(DWORD cntrlEvent);

// Used to open the data file for reading.
// Declare an ifstream object and pass it as a parameter to this function.
// The function will attempt to open the data file.
// If the file cannot be opened, an error message will display, 
// cleanup will be performed, and the program will exit.
void openInputFile(ifstream& dataFile);

// Function to close the listening socket and 
// perform DLL cleanup (WSACleanup)
void cleanup();

// Retrieve the version number from the local data file
int getLocalVersion();

int main()
{
	WSADATA		wsaData;
	SOCKET		clientSocket = INVALID_SOCKET;
	SOCKADDR_IN	serverAddr;		// Structure to hold server address information
	int			localVersion;
	int			iResult;		// Resulting code from socket functions
	int         clientIn;		// Client's request number
	int		    counter = 0; 		// Keeps track of the number of requests

	// Set the handler function to handle shutdown events
	if (!SetConsoleCtrlHandler(Handler, TRUE))
	{
		cout << "Error setting server exit handler\n";
		return 1;
	}

	localVersion = getLocalVersion();

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		cerr << "WSAStartup failed with error: " << iResult << endl;
		return 1;
	}

	// socket 
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSocket == INVALID_SOCKET)
	{
		cerr << "Socket failed with error:  " << WSAGetLastError() << endl;
		cleanup();
		return 1;
	}




	// bind to a local newtwork address to the socket
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, IPADDR, &serverAddr.sin_addr);


	iResult = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (iResult == SOCKET_ERROR)
	{
		cerr << "Bind has Failed" << WSAGetLastError() << endl;
		cleanup();
		return 1;
	}

	// listen
	iResult = listen(listenSocket, 1);
	if (iResult == SOCKET_ERROR)
	{
		cerr << "Listen failed with error: " << WSAGetLastError() << endl;
		cleanup();
		return 1;
	}

	cout << "Update server\n";
	cout << "Current data file version: v" << localVersion << "\n";
	cout << "Running on port number " << PORT << "\n\n";


	



	while (listenSocket != INVALID_SOCKET)
	{
		cout << "\n Waiting for connections... \n" << endl;
		
		
		clientSocket = accept(listenSocket, NULL, NULL);
		//cout << clientSocket << endl;
		if (clientSocket == INVALID_SOCKET)
		{
			cerr << "Acception failure" << WSAGetLastError() << endl;
			closesocket(listenSocket);
			continue;
		}

		cout << "Connection received" << endl;
		iResult = recv(clientSocket, (char*)&clientIn, sizeof(int), 0);

		if (iResult == 0)
		{
			cout << "Connection closed";
			closesocket(listenSocket);
			continue;
		}
		else if (iResult < 0)
		{
			cerr << "Recv failed" << WSAGetLastError() << endl;
			closesocket(listenSocket);
			continue;
		}
		
		if (clientIn == QUERY)
		{
			
			cout << "	Request for current version number: v" << localVersion << endl;
			cout << "	Connection closed " << endl;
			counter++;
			cout << "Total requests handled : " << counter << endl;
			iResult = send(clientSocket, (char*)&localVersion, sizeof(int), 0);
	
			if (iResult == INVALID_SOCKET)
			{
				cerr << "Sending failed" << WSAGetLastError() << endl;
				closesocket(listenSocket);
				continue;
			}
			closesocket(clientSocket);
		}
		else
		{
			// UPDATE
			cout << "The client requested an Update." << endl;
			ifstream dataFile;
			openInputFile(dataFile);

			for (int i = 0; i < 3 && iResult != INVALID_SOCKET; i++)
			{
				dataFile >> clientIn;

				iResult = send(clientSocket, (char*)&clientIn, sizeof(int), 0);

				if (iResult == INVALID_SOCKET)
				{
					cerr << "Sending failed" << WSAGetLastError() << endl;

				}
			}

			dataFile.close();
			closesocket(clientSocket);
			counter++;
			cout << "Connection Closed" << endl;
			// print message about how many requests have been handled
			cout << "Total requests handled : " << counter << endl;
			// update the localVersion variable again after server handles 5 clients
			if (counter % 5 == 0)
			{
				localVersion = getLocalVersion();
			}

		}
	}

	cleanup();
	return 0;
}



BOOL WINAPI Handler(DWORD cntrlEvent)
{
	cleanup();
	return FALSE;
}



void cleanup()
{
	if (listenSocket != INVALID_SOCKET)
	{
		closesocket(listenSocket);
		listenSocket = INVALID_SOCKET;
	}

	WSACleanup();
	Sleep(100);
	//cout << "Cleanup finished\n";
	Sleep(1200);
}



void openInputFile(ifstream& dataFile)
{
	dataFile.open(FILENAME);
	if (dataFile.fail())
	{
		cerr << "Data file cannot be opened\n";
		cleanup();
		exit(1);
	}
}



int getLocalVersion()
{
	ifstream dataFile;
	openInputFile(dataFile);

	int version;
	dataFile >> version;
	dataFile.close();

	return version;
}