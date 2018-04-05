#include <string>
#include <iostream>
#include <winsock.h>
#include <fstream>
#include <iomanip>
#include "Server.h"
#include "coloredText.h"

string Server::RetrieveMessage(char * buffer, int size) {
	string buf = buffer;
	return buf.substr(0, size);
}

int Server::ObtainServerSettings() {
	Write("   hostname >> ", DARKPURPLE);
	getline(cin, hostname);
	if (cin.eof()) return -1;

	Write("   username >> ", DARKPURPLE);
	getline(cin, username);
	if (cin.eof()) return -1;

	Write("   password >> ", DARKPURPLE);
	getline(cin, password);
	if (cin.eof()) return -1;

	Write("   port(21) >> ", DARKPURPLE);
	getline(cin, port);
	if (cin.eof()) return -1;

	return 0;
}

int Server::RetrieveDataPort(string message) {

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

char * Server::RetrieveIPaddress(string domain) {
	hostent * host;
	host = gethostbyname(domain.c_str());

	if (host != NULL) {
		sprintf_s(ip_address, "%s", inet_ntoa(*(in_addr*)host->h_addr_list[0]));
		return ip_address;
	}
	else {
		return 0;
	}
}

int Server::RetrieveResponseCode(string buffer) {
	try {
		return stoi(buffer.substr(0, 3));
	}
	catch (invalid_argument) {}
}

int Server::Connect() {

	cout << endl;
	Write("[login in] ", DARKPURPLE);
	cout << "verifying the hostname and port ... ";

	char buffer[1024];

	if (RetrieveIPaddress(hostname) == 0) {
		WriteLine("\t\t\twrong\n", LIGHTRED);
		return -1;
	}

	// the address of the server the user is connecting to
	SOCKADDR_IN address;
	address.sin_addr.s_addr = inet_addr(ip_address);
	address.sin_port = htons(stoi(port));
	address.sin_family = AF_INET;

	// creating a socket and binding the address to this socket.
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	bind(serverSocket, (SOCKADDR*)&address, sizeof(address));

	// connecting to the socket and immediate reading from it
	connect(serverSocket, (SOCKADDR*)&address, sizeof(address));
	recv(serverSocket, buffer, 1024, 0);

	if (RetrieveResponseCode(buffer) != 220) {
		WriteLine("\t\t\twrong", LIGHTRED); 
		return -1;
	}
	else {
		WriteLine("\t\t\tokay", LIGHTGREEN);
	}

	string login = "USER " + username + "\r\n";
	string pass = "PASS " + password + "\r\n";

	Write("[login in] ", DARKPURPLE);
	cout << "verifying the username ... ";

	send(serverSocket, login.c_str(), login.length(), 0);
	recv(serverSocket, buffer, 1024, 0);

	if (RetrieveResponseCode(buffer) != 331) {
		WriteLine("\t\t\t\twrong \n", LIGHTRED);
		return -1;
	}
	else {
		WriteLine("\t\t\t\tokay", LIGHTGREEN);
	}

	Write("[login in] ", DARKPURPLE);
	cout << "verifying the password ... ";

	send(serverSocket, pass.c_str(), pass.length(), 0);
	recv(serverSocket, buffer, 1024, 0);

	if (RetrieveResponseCode(buffer) != 230) {
		WriteLine("\t\t\t\twrong\n", LIGHTRED);
		return -1;
	}
	else {
		WriteLine("\t\t\t\tokay", LIGHTGREEN);
	}

	cout << "\n You have logged on, you are able to continue now" << endl << endl;

	isConnectionEstablished = true;

	return 0;

}

int Server::ConnectToDataport() {

	char buffer[1024];

	send(serverSocket, "PASV\r\n", 6, 0);				// entering passive mode
	int size = recv(serverSocket, buffer, 1024, 0);
	
	cout << RetrieveMessage(buffer, size) << endl;
	int dataPort;

	if (RetrieveResponseCode(buffer) == 227) {
		dataPort = RetrieveDataPort(buffer);			// contains the port we have to connect to in order to get data from the server
	}
	else {
		cout << "an error occured, I can't download this file :(" << endl;
		return -1;
	}

	SOCKADDR_IN dataSocketAddress;
	dataSocketAddress.sin_addr.s_addr = inet_addr(ip_address);
	dataSocketAddress.sin_port = htons(dataPort);
	dataSocketAddress.sin_family = AF_INET;

	// creating a socket and binding the address to this socket.
	dataSocket = socket(AF_INET, SOCK_STREAM, 0);
	bind(dataSocket, (SOCKADDR*)&dataSocketAddress, sizeof(dataSocketAddress));

	int err_code = connect(dataSocket, (SOCKADDR*)&dataSocketAddress, sizeof(dataSocketAddress));

	if (err_code == SOCKET_ERROR) {
		cout << "an error occured while connecting to the server";
		return 0;
	}

	memset(buffer, 0, 1024);
}

int Server::SendFTPcommand(string command) {

	if (command.substr(0, 3) == "put") {

		char message[200];

		ConnectToDataport();

		string filename = command.substr(4);
		string ftpCommand = "STOR " + filename + "\r\n";

		ifstream fin(filename, ios::binary);

		// getting the size of the file
		fin.seekg(0, fin.end);
		int length = fin.tellg();
		fin.seekg(0, fin.beg);

		char * buffer = new char [length];

		fin.read(buffer, length);
		fin.close();

		int size = 0;

		send(serverSocket, ftpCommand.c_str(), ftpCommand.length(), 0);

		size = recv(serverSocket, message, 200, 0);
		cout << RetrieveMessage(message, size) << endl;

		send(dataSocket, buffer, length, 0);

		cout << "The file has been uploaded :)\n\n";

		delete [] buffer;

		return 0;
	}

	if (command.substr(0, 3) == "bin") {

		char buffer[200];
		Write(" Entering the binary mode ... ", WHITE);
		send(serverSocket, "TYPE I\r\n", 8, 0);
		recv(serverSocket, buffer, 100, 0);
		
		if (RetrieveResponseCode(buffer) == 200) {
			WriteLine("\t\t\t\t\tsuccess!\n", LIGHTGREEN);
			this->currentMode = "binary";
		}
		else {
			WriteLine("\t\t\t\t\failure!\n", LIGHTRED);
		}

		return 0;
	}

	if (command.substr(0, 3) == "ascii") {

		char buffer[200];
		Write(" Entering the ascii mode ... ", WHITE);
		send(serverSocket, "TYPE A\r\n", 8, 0);
		recv(serverSocket, buffer, 100, 0);

		if (RetrieveResponseCode(buffer) == 200) {
			WriteLine("\t\t\t\t\tsuccess!\n", LIGHTGREEN);
			this->currentMode = "ascii";
		}
		else {
			WriteLine("\t\t\t\t\failure!\n", LIGHTRED);
		}

		return 0;
	}

	if (command.substr(0, 6) == "delete") {

		char message[200];

		string filename = command.substr(7);
		string ftpCommand = "DELE " + filename + "\r\n";

		send(serverSocket, ftpCommand.c_str(), ftpCommand.length(), 0);
		int length = recv(serverSocket, message, 200, 0);
		cout << RetrieveMessage(message, length) << endl;

		return 0;

	}

	if (command.substr(0, 2) == "cd") {

		char message[200];

		string pathname = command.substr(3);
		string ftpCommand = "CWD " + pathname + "\r\n";

		send(serverSocket, ftpCommand.c_str(), ftpCommand.length(), 0);
		int length = recv(serverSocket, message, 200, 0);
		cout << RetrieveMessage(message, length) << endl;

		return 0;
	}

	if (command.substr(0, 3) == "get") {

		char buffer[10000];
		char message[200];

		ConnectToDataport();

		string filename = command.substr(4);
		string ftpCommand = "RETR " + filename + "\r\n";

		send(serverSocket, ftpCommand.c_str(), ftpCommand.length(), 0);
		int length = recv(serverSocket, message, 200, 0);
		cout << RetrieveMessage(message, length) << endl;

		int iResult = 0;

		ofstream fout(filename, ios_base::binary);

		do {
			
			iResult = recv(dataSocket, buffer, 10000, 0);
			if (iResult > 0) {
				
				// writting to the file
				fout.write(buffer, iResult);
				memset(buffer, 0, 10000);

			}
			else if (iResult == 0) {
				Write("[get] ", LIGHTWHITE);
				printf("Connection closed\n\n");
			}
			else {
				Write("[get] ", LIGHTWHITE);
				printf("recv failed: %d\n\n", WSAGetLastError());
				return 0;
			}

		} while (iResult > 0);

		length = recv(serverSocket, message, 200, 0);
		cout << RetrieveMessage(message, length) << endl;

		fout.close();

		return 0;
	}

	if (command == "noop") {

		char message[200];

		int size;
		send(serverSocket, "NOOP\r\n", 6, 0);
		size = recv(serverSocket, message, 126, 0);

		return 0;
	}

	if (command == "bye" || command == "quit") {

		if (this->isConnectionEstablished != true) {
			return 0;
		}

		char message[200];
		send(serverSocket, "QUIT\r\n", 6, 0);
		int size = recv(serverSocket, message, 200, 0);
		cout << RetrieveMessage(message, size) << "\n";

		this->isConnectionEstablished = false;

		return 0;
	}

	if (command == "ls") {

		char buffer[2048];
		char message[126];

		int size;

		// if the data connection's mode is binary, change it to the ASCII one during executing this command.
		bool changeBack = false;
		if (this->currentMode == "binary") {
			SendFTPcommand("ascii");
			changeBack = true;
		}

		ConnectToDataport();

		send(serverSocket, "LIST\r\n", 6, 0);

		size = recv(serverSocket, message, 126, 0);
		cout << RetrieveMessage(message, size) << endl;

		do {
			size = recv(dataSocket, buffer, 2048, 0);
			cout << RetrieveMessage(buffer, size) << endl;
		} while (size > 0);


		size = recv(serverSocket, message, 80, 0);
		cout << RetrieveMessage(message, size) << endl;

		memset(buffer, 0, 2048);

		// change the mode back to the binary one if it was binary before executing the 'ls' command.
		if (changeBack == true) {
			SendFTPcommand("bin");
		}

		return 0;
	}

	return 0;
}

bool Server::GetConnectionState() {
	return this->isConnectionEstablished;
}
string Server::GetHostname() {
	return this->hostname;
}