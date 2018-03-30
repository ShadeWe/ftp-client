#include <string>
#include <iostream>
#include <winsock.h>
#include <fstream>
#include <iomanip>
#include "Server.h"
#include "coloredText.h"

int Server::ObtainServerSettings() {
	Write("   hostname >> ", DARKPURPLE);
	getline(cin, hostname);
	if (cin.eof()) return 0;

	Write("   username >> ", DARKPURPLE);
	getline(cin, username);
	if (cin.eof()) return 0;

	Write("   password >> ", DARKPURPLE);
	getline(cin, password);
	if (cin.eof()) return 0;

	Write("   port(21) >> ", DARKPURPLE);
	getline(cin, port);
	if (cin.eof()) return 0;
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
	return stoi(buffer.substr(0, 3));
}

int Server::Connect() {

	cout << endl;
	Write("[login in] ", DARKPURPLE);
	cout << "verifying the hostname and port ... ";

	char buffer[1024];

	if (RetrieveIPaddress(hostname) == 0) {
		WriteLine("an error occured while getting the ip address", LIGHTRED);
		return 0;
	}

	// the address of the server a user is connecting to
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
		WriteLine("wrong", LIGHTRED); 
		return 0;
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
		WriteLine("wrong", LIGHTRED);
		return 0;
	}
	else {
		WriteLine("\t\t\t\tokay", LIGHTGREEN);
	}

	Write("[login in] ", DARKPURPLE);
	cout << "verifying the password ... ";

	send(serverSocket, pass.c_str(), pass.length(), 0);
	recv(serverSocket, buffer, 1024, 0);

	if (RetrieveResponseCode(buffer) != 230) {
		WriteLine("wrong", LIGHTRED);
		return 0;
	}
	else {
		WriteLine("\t\t\t\tokay", LIGHTGREEN);
	}

	send(serverSocket, "PASV\r\n", 6, 0);	// entering passive mode
	recv(serverSocket, buffer, 1024, 0);

	int dataPort = RetrieveDataPort(buffer);			// contains the port we have to connect to in order to get data from the server

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

	cout << "\n You have logged on, you are able to continue now" << endl << endl;

	isConnectionEstablished = true;

}

int Server::SendFTPcommand(string command) {

	char buffer[30000];

	if (command.substr(0, 3) == "bin") {

		send(serverSocket, "TYPE I\r\n", 8, 0);
		recv(serverSocket, buffer, 100, 0);
		
		if (RetrieveResponseCode(buffer) == 200) {
			cout << "entered binary mode" << endl;
		}

		memset(buffer, 0, 100);
		return 0;
	}

	if (command.substr(0, 3) == "get") {

		string filename = command.substr(4);
		string ftpCommand = "RETR " + filename + "\r\n";

		send(serverSocket, ftpCommand.c_str(), ftpCommand.length(), 0);

		int iResult;
		int size = 0;

		do {
			
			iResult = recv(dataSocket, buffer + size, 30000 - size, 0);
			if (iResult > 0) {
				Write("[get] ", LIGHTWHITE);
				printf("Bytes received: %d\n", iResult);
				size = size + iResult;
			}
			else if (iResult == 0) {
				Write("[get] ", LIGHTWHITE);
				printf("Connection closed\n\n");
			}
			else {
				Write("[get] ", LIGHTWHITE);
				printf("recv failed: %d\n", WSAGetLastError());
			}

		} while (iResult > 0);

		ofstream fout(filename, ios_base::binary);
		fout.write(buffer, size);
		fout.close();

		memset(buffer, 0, 30000);
		return 0;
	}

	if (command == "ls") {

		send(serverSocket, "LIST\r\n", 6, 0);
		int size = recv(dataSocket, buffer, 2048, 0);

		string processedString(buffer);
		processedString = processedString.substr(0, size);
		cout << processedString << endl;

		memset(buffer, 0, 2048);
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