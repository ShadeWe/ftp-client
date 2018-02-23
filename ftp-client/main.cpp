#include <iostream>
#include <winsock.h>
#include <Windows.h>
#include <string>

using namespace std;

void processCustomCommands(string command) {

	if (command == "/gethelp") {
		cout << "here's the list of commands you can do:" << endl;
		cout << "HELP PASV PORT LIST PASS USER" << endl;
	}

}

string cutMessage(char * buffer) {

	string response;

	for (int i = 0; true; i++) {
		if (buffer[i] != '\r') response.push_back(buffer[i]);
		else break;
	}

	return response;
}

int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// here comes winsock startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);

	if (WSAStartup(DllVersion, &wsaData) != 0) {
		cout << "an error occured during winsock startup ... " << endl;
		return 0;
	}


	hostent * host;     // contains the ip address of a domain
	string hostname;    // the hostname typed in by the user


						// speedtest.tele2.net / ftp.hq.nasa.gov

	while (true) {

		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 3));
		cout << "enter a hostname: ";
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));

		cin >> hostname;
		host = gethostbyname(hostname.c_str());

		if (host == NULL) {
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 4));
			cout << "something went wrong while getting the IP address...\ncheck if the hostname has been written properly and try again" << endl;
			SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
		}
		else {
			break;
		}

	}

	char ip_address[15];
	sprintf_s(ip_address, "%s", inet_ntoa(*(in_addr*)host->h_addr_list[0]));


	cout << endl << "the ip address of the domain specified: ";

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 13));
	cout << ip_address << endl << endl;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

	// the address of the server a user is connecting to
	SOCKADDR_IN address;
	address.sin_addr.s_addr = inet_addr(ip_address);
	address.sin_port = htons(21);
	address.sin_family = AF_INET;

	// creating a socket and binding the address to this socket.
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	bind(serverSocket, (SOCKADDR*)&address, sizeof(address));

	// connecting to the socket
	connect(serverSocket, (SOCKADDR*)&address, sizeof(address));

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
	cout << "connection has been established, type in";
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 3));
	cout << " '/gethelp' ";
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));
	cout << "to see what you can do" << endl << endl;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));

	char buffer[1024];	// contains everything the server sends back.
	string command;		// the command sent by a user.

	cin.ignore();

	recv(serverSocket, buffer, 1024, 0);
	string response = cutMessage(buffer);	// cuttin' the message from all this stuff containing in the buffer

	cout << "the server responded with the code ";
	cout << response[0] << response[1] << response[2] << endl;
	cout << "you can send messages now." << endl << endl;


	while (true) {

		// getting a command from a user.
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 3));
		cout << "command to server: ";
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));

		getline(cin, command);

		if (command[0] == '/') {
			processCustomCommands(command);
			continue;
		}
		else {

			command.push_back('\r');
			command.push_back('\n');

			const char * c = command.c_str();

			send(serverSocket, c, command.length(), 0);
		}

		recv(serverSocket, buffer, 1024, 0);
		string response = cutMessage(buffer);

		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 3));
		cout << "server's response: ";
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));

		cout << response << endl << endl;

	}

	system("PAUSE");
	return 0;
}
