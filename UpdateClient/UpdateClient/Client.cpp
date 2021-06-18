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

// Define global variable for listen socket so it can be closed at any time
SOCKET mySocket = INVALID_SOCKET;

// Used to open the data file for reading.
// Declare an ifstream object and pass it as a parameter to this function.
// The function will attempt to open the data file.
// If the file cannot be opened, an error message will display, 
// cleanup will be performed, and the program will exit.
void openInputFile(ifstream& dataFile);

// Used to open a file for writing.
// Declare an ofstream object and pass it as a parameter to this function.
// The function will attempt to open the named file.
// If the file cannot be opened, an error message will display, 
// cleanup will be performed, and the program will exit.
void openOutputFile(ofstream& dataFile);

// Function to close the listening socket and 
// perform DLL cleanup (WSACleanup)
void cleanup();

// Retrieve the version number from the local data file
int getLocalVersion();

// Read data from the local data file
void readData(int& num1, int& num2);


int main()
{
	WSADATA 	wsaData;
	SOCKADDR_IN	serverAddr;
	int 		iResult;   // stores the initial send, accept, and recv calls
	int 		serverIn;  // stores the server's 
	int			num1;
	int			num2;
	int			sum;
	int			localVersion;
	int 		versionNum;


	/*********************************************************/
	// Add code here to
	// 1) make sure that we are using the current version of the data file
	// 2) update the data file if it is out of data
	/*********************************************************/
	localVersion = getLocalVersion();

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		cerr << "WSAStartup failed with error: " << iResult << endl;
		WSACleanup();
	}

	// bind to a local newtwork address to the socket 
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, IPADDR, &serverAddr.sin_addr);

	// Create the socket.
	mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //
	if (mySocket == INVALID_SOCKET)
	{
		cerr << "Socket failed with error: " << WSAGetLastError() << endl;
		cleanup();
		return 1;
	}
	else
	{
		//cout << "Socket created successfully: " << mySocket << endl;
	}
	cout << "Checking for update..." << endl;
	while (mySocket != INVALID_SOCKET)
	{
		// connect to server
		iResult = connect(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)); //
		if (iResult == SOCKET_ERROR)
		{
			cerr << "Connection failure!" << WSAGetLastError() << endl;
			closesocket(mySocket);
			return 1;
		}
		else
		{
			//cout << "Successfully connected to the socket server: " << iResult << endl;
		}

		// sends a request for the current version number
		iResult = send(mySocket, (char*)&QUERY, sizeof(int), 0);
		if (iResult == SOCKET_ERROR)
		{
			cerr << "Sending failed" << WSAGetLastError() << endl;
			closesocket(mySocket);
			continue;
		}
		else
		{
			//cout << "Successfully sent query to server. Data sent: " << iResult << endl;
		}

		// Get version number from the server.
		iResult = recv(mySocket, (char*)&versionNum, sizeof(int), 0);
		if (iResult == 0)
		{
			cout << "Connection closed";
			closesocket(mySocket);
			return 1;
		}
		else if (iResult < 0)
		{
			cerr << "Receive failed" << WSAGetLastError() << endl;
			closesocket(mySocket);
			return 1;
		}
		else
		{
			//cout << "Successfully received version number: " << versionNum << endl;
			closesocket(mySocket);
		}

		// compares the server's version number to client's version
		if (versionNum != localVersion)
		{
			mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //
			if (mySocket == INVALID_SOCKET)
			{
				cerr << "Socket failed with error: " << WSAGetLastError() << endl;
				cleanup();
				return 1;
			}
			else
			{
				//cout << "Socket created successfully: " << mySocket << endl;
			}

			iResult = connect(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
			if (iResult == SOCKET_ERROR)
			{
				cerr << "Connection failure!" << WSAGetLastError() << endl;
				closesocket(mySocket);
				return 1;
			}
			else
			{
				//cout << "Successfully connected to the socket server: " << iResult << endl;
			}

			iResult = send(mySocket, (char*)&UPDATE, sizeof(int), 0);
			if (iResult == SOCKET_ERROR)
			{
				cout << "Sending update failed! " << endl;
				cleanup();
				continue;
			}
			else
			{
				//cout << "Successfully sent update to server. " << endl;
			}

			ofstream dataFile;
			openOutputFile(dataFile);
			// TODO recieve the three integers from server and then write to file 
			cout << "Downloading Update..." << endl;

			// Ignore version
			iResult = recv(mySocket, (char*)&versionNum, sizeof(int), 0);
			if (iResult < 0) {
				cout << "Something bad happened... " << endl << WSAGetLastError() << endl;

			}
			else if (iResult == 0) {
				cout << "Socket was closed for some reason..." << endl;
			}
			else {
				//cout << "Data read successfully..." << endl;
				//cout << "Read " << iResult << " bytes of data." << endl;
				//cout << "Data in question: " << versionNum << endl;
				
			}
			// Num1
			iResult = recv(mySocket, (char*)&num1, sizeof(int), 0);
			if (iResult < 0) {
				cout << "An error occured... " << endl << WSAGetLastError() << endl;
				
			}
			else if (iResult == 0) {
				cout << "Socket was closed..." << endl;
			}
			else {
				//cout << "Data read successfully..." << endl;
				//cout << "Read " << iResult << " bytes of data." << endl;
				//cout << "Data in question: " << num1 << endl;
				
			}
			// Num2
			iResult = recv(mySocket, (char*)&num2, sizeof(int), 0);
			if (iResult < 0) {
				cout << "An Error occured... " << endl << WSAGetLastError() << endl;
				
			}
			else if (iResult == 0) {
				cout << "Socket was closed..." << endl;
			}
			else {
				//cout << "Data read successfully..." << endl;
				//cout << "Read " << iResult << " bytes of data." << endl;
				//cout << "Data in question: " << num2 << endl;
				
			}
			dataFile << versionNum << "\n" << num1 << "\n" << num2;
			/*iResult = recv(mySocket, (char*)&integer2, sizeof(int), 0);
			iResult = recv(mySocket, (char*)&integer3, sizeof(int), 0);*/

			


			cout << "Update Finished" << endl;
			dataFile.close();
			cleanup();
		}
		else
		{
			cout << "No update found \n" << endl;
			cleanup();
		}

	}

	// Main program purpose starts here: read two numbers from the data file and calculate the sum
	/**** DO NOT CHANGE THESE LINES! ****/
	localVersion = getLocalVersion();
	cout << "Sum Calculator Version " << localVersion << "\n\n";

	readData(num1, num2);
	sum = num1 + num2;
	cout << "The sum of " << num1 << " and " << num2 << " is " << sum << endl;

	return 0;
	/**** DO NOT CHANGE THESE LINES! ****/
}



void cleanup()
{
	cout << "Cleanup is occuring right now..." << endl;
	if (mySocket != INVALID_SOCKET)
	{
		closesocket(mySocket);
		mySocket = INVALID_SOCKET;
	}

	WSACleanup();
	Sleep(100);
	cout << "Cleanup finished\n";
	Sleep(1200);
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



void openOutputFile(ofstream& dataFile)
{
	dataFile.open(FILENAME);
	if (dataFile.fail())
	{
		cerr << "Data file cannot be opened\n";
		cleanup();
		exit(1);
	}
}


void readData(int& num1, int& num2)
{
	ifstream dataFile;
	openInputFile(dataFile);

	int tmp;
	dataFile >> tmp;
	dataFile >> num1;
	dataFile >> num2;

	dataFile.close();
}