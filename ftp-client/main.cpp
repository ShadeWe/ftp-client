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

char ip_address[15];					// contains the ip address of a domain (xxx.xxx.xxx.xxx)

bool isConnectionEstablished = false;	// true when the ip address was obtained, user has logged in and so on.
bool isDataSpecified = false;			// true when all the fields were specified by a user using /set

string getResponseCode(char * buffer) {

	string response;

	for (int i = 0; i < 3; i++) {
		response.push_back(buffer[i]);
	}

	return response;
}

void writeLine(string text, int color) 
{
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | color));
	cout << text << endl;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | WHITE));
}

int getDataPort(char * buffer) {

	string message = buffer;
	string values[2];

	int writting = 1;
	for (int i = message.find(')') - 1; true; i--) {

		if (message[i] != ',') {
			values[writting] = message[i] + values[writting];
		}
		else {
			writting--;

			if (writting < 0) {
				break;
			}
		}

	}

	return stoi(values[0]) * 256 + stoi(values[1]);

}

void write(string text, int color) 
{
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | color));
	cout << text;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | WHITE));
}

int processCommand(string command) {

	if (command == "/commands") {

		write("/commands ", OCEANIC);
		writeLine("shows all the commands of this program you can use", LIGHTWHITE);

		write("/set ", OCEANIC);
		writeLine("\t  set the parameters of an FTP server to connect to", LIGHTWHITE);

		write("/connect ", OCEANIC);
		write(" connect to an ftp server. That works only after all the required \n \t  parameters were specified (using ", LIGHTWHITE);
		write("/set", OCEANIC);
		writeLine(")", LIGHTWHITE);

		cout << endl;

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
		write("Check if you've set everything properly and type ", WHITE);
		writeLine("'/connect'", OCEANIC);
		cout << endl;

		isDataSpecified = true;

		return 0;
	}
	if (command == "/connect") {

		if (isDataSpecified == false) {
			write("[hint] ", YELLOW);
			cout << "please, type /set and specify the parameters of a server" << endl << endl;
			return 0;
		}

		write("[login in] ", DARKPURPLE);
		cout << "checking hostname ... ";

		hostent * host;   

		host = gethostbyname(hostname.c_str());

		if (host == NULL) {
			cout << endl << endl;
			write("[error] ", RED);
			writeLine("something went wrong while getting the IP address...\ncheck if the hostname has been written properly and try again", LIGHTWHITE);
			cout << endl;
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

		char buffer[1024];

		recv(serverSocket, buffer, 1024, 0);
		string responseCode = getResponseCode(buffer);

		if (responseCode != "220") {
			cout << "the server responded with a code different from 220";
			return 0;
		}

		writeLine("okay", LIGHTGREEN);

		string login = "USER " + username + "\r\n";
		string pass = "PASS " + password + "\r\n"; 

		write("[login in] ", DARKPURPLE);
		cout << "checking username ... ";

		send(serverSocket, login.c_str(), login.length(), 0);
		recv(serverSocket, buffer, 1024, 0);

		if (getResponseCode(buffer) == "331") {
			writeLine("okay", LIGHTGREEN);
		}
		else {
			writeLine("wrong", LIGHTRED);
			cout << endl;
			return 0;
		}
		
		write("[login in] ", DARKPURPLE);
		cout << "checking password ... ";

		send(serverSocket, pass.c_str(), pass.length(), 0);
		recv(serverSocket, buffer, 1024, 0);

		if (getResponseCode(buffer) == "230") {
			writeLine("okay", LIGHTGREEN);
			cout << endl;
		}
		else {
			writeLine("wrong", LIGHTRED);
			cout << endl;
			return 0;
		}

		send(serverSocket, "PASV\r\n", 6, 0);	// entering passive mode
		recv(serverSocket, buffer, 1024, 0);
		int port = getDataPort(buffer);			// contains the port we have to connect to in order to get data from the server

		cout << "You have logged on, you are able to continue now" << endl << endl;

		isConnectionEstablished = true;
		
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

	write("Welcome to the FTP-client, type in ", WHITE);
	write("'/commands'", OCEANIC);
	writeLine(" to see what commands you can use", WHITE);
	cout << endl;

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
