#include <iostream>
#include <winsock.h>
#include <Windows.h>
#include <string>
#include "colorDefines.h"

using namespace std;

SOCKET serverSocket;		// contains the socket a user is connecting to.

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// these values are set by a user and are needed to connect to a server.
string hostname;
string username;
string password;
string port;

char ip_address[15];		// contains the ip address of a domain

bool isConnectionEstablished = false;

string getResponseCode(char * buffer) {

	string response;

	for (int i = 0; i < 3; i++) {
		response.push_back(buffer[i]);
	}

	return response;
}

void writeLine(string text, int color) {

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | color));
	cout << text << endl;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));

}

void write(string text, int color) {

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | color));
	cout << text;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));

}

int processCommand(string command) {

	if (command == "/gethelp") {
		cout << "To connect to an FTP server, type in '/connect' and follow instructions" << endl << endl;
		return 0;
	}
	if (command == "/set") {

		write("   hostname >> ", DARKPURPLE);
		getline(cin, hostname);
		if (cin.eof()) return 0;

		write("   username >> ", DARKPURPLE);
		getline(cin, username);
		if (cin.eof()) return 0;

		write("   password >> ", DARKPURPLE);
		getline(cin, password);
		if (cin.eof()) return 0;

		write("   port(21) >> ", DARKPURPLE);
		getline(cin, port);
		if (cin.eof()) return 0;


		cout << endl;
		writeLine("Check if you've set everything properly and type '/connect'", LIGHTWHITE);
		cout << endl;

		return 0;
	}
	if (command == "/connect") {
		cout << "connecting... ";

		hostent * host;   

		host = gethostbyname(hostname.c_str());

		if (host == NULL) {
			writeLine("something went wrong while getting the IP address...\ncheck if the hostname has been written properly and try again", RED);
			return 0;
		}
		
		sprintf_s(ip_address, "%s", inet_ntoa(*(in_addr*)host->h_addr_list[0]));

		// the address of the server a user is connecting to
		SOCKADDR_IN address;
		address.sin_addr.s_addr = inet_addr(ip_address);
		address.sin_port = htons(stoi(port));
		address.sin_family = AF_INET;

		// creating a socket and binding the address to this socket.
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		bind(serverSocket, (SOCKADDR*)&address, sizeof(address));

		// connecting to the socket
		connect(serverSocket, (SOCKADDR*)&address, sizeof(address));

		writeLine("done!", LIGHTGREEN);

		char buffer[1024];

		recv(serverSocket, buffer, 1024, 0);
		string responseCode = getResponseCode(buffer);

		if (responseCode == "220") {
			isConnectionEstablished = true;
		}
		
		cout << endl;

		return 0;
	}

}

int main()
{

	// here comes winsock startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);

	if (WSAStartup(DllVersion, &wsaData) != 0) {
		cout << "an error occured during winsock startup ... " << endl;
		return 0;
	}

	// speedtest.tele2.net / ftp.hq.nasa.gov

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
	cout << "Welcome to the FTP-client, type in";
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 3));
	cout << " '/gethelp' ";
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
	cout << "to see what you can do" << endl << endl;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

	string command;

	while (true) {

		if (isConnectionEstablished == false)
			write(" command >> ", OCEANIC);
		else
			write(" " + hostname + " >> ", OCEANIC);


		getline(cin, command);
		cout << endl;

		processCommand(command);


		cin.clear();
	}
	
	system("PAUSE");
	return 0;
}
