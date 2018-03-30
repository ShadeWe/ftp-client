#include <iostream>
#include <winsock.h>
#include <Windows.h>
#include <string>

#include "Server.h"
#include "coloredText.h"

Server server;

int processCommand(string command) {

	if (command == "/commands") {

		Write("-----------------------------", LIGHTWHITE);
		Write(" environment commands ", OCEANIC);
		Write("----------------------------- \n", LIGHTWHITE);

		Write("/commands ", OCEANIC);
		WriteLine("shows all the commands of this program you can use", LIGHTWHITE);

		Write("/connect ", OCEANIC);
		Write(" set the parameters and connect to an ftp server\n\n", LIGHTWHITE);
		
		Write("-----------------------------", LIGHTWHITE);
		Write(" interaction commands ", OCEANIC);
		Write("----------------------------- \n", LIGHTWHITE);

		Write("  ls  ", OCEANIC);
		Write("\t  list all the files on the ftp server you're connected to\n", LIGHTWHITE);

		cout << endl;

		return 0;
	}
	
	if (command == "/connect") {

		server.ObtainServerSettings();
		server.Connect();

	}

}

int main() {

	// here comes winsock startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);

	if (WSAStartup(DllVersion, &wsaData) != 0) {
		cout << "an error occured during winsock startup ... " << endl;
		return 0;
	}

	// speedtest.tele2.net / ftp.hq.nasa.gov

	Write("Welcome to the FTP-client, type in ", WHITE);
	Write("'/commands'", OCEANIC);
	WriteLine(" to see what commands you can use\n", WHITE);

	string command;

	while (true) {
		
		if (server.GetConnectionState() == false) {
			Write(" command >> ", OCEANIC);
		}
		else {
			Write(" " + server.GetHostname() + " >> ", OCEANIC);
		}

		getline(cin, command);
		cout << endl;

		if (server.GetConnectionState() == false)
			processCommand(command);
		else
			server.SendFTPcommand(command);


		cin.clear();
	}
	
	system("PAUSE");
	return 0;
}
