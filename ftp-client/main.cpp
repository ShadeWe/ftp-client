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
		Write("\t  list all the files from the remove host\n", LIGHTWHITE);

		Write("  bin  ", OCEANIC);
		Write("\t  enter the binary mode (needed to download binary files)\n", LIGHTWHITE);

		Write("  delete", OCEANIC);
		Write("  delete a particular file from the remote host\n", LIGHTWHITE);

		Write("  cd  ", OCEANIC);
		Write("\t  change the current directory\n", LIGHTWHITE);

		Write("  get  ", OCEANIC);
		Write("\t  download a file from the remote host\n", LIGHTWHITE);

		cout << endl;

		return 0;
	}
	
	if (command == "/connect") {

		if (server.ObtainServerSettings() != -1) {
			if (server.Connect() == -1) {
				cout << " the data were specified incorrectly, try again using ";
				WriteLine("/connect\n", OCEANIC);
			}

		}
		else
			cout << "\n the data are not complete ... \n\n";

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

		// reading a command from the user
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
