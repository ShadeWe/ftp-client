#pragma once

using namespace std;

class Server
{

private:

	string hostname;
	string username;
	string password;
	string port;

	SOCKET serverSocket;		  // the first socket we connect to and which we send commands through
	SOCKET dataSocket;			  // the socket we get data from

	char ip_address[15];		  // the ip address the connection has been established to
	bool isConnectionEstablished; // true when all the required data were specified correctly and the connection has been established.

public:
	// getters
	string GetHostname();
	bool GetConnectionState();
	
	string RetrieveMessage(char * buffer, int size);
	int ConnectToDataport();
	int ObtainServerSettings();
	int RetrieveDataPort(string message);
	char * RetrieveIPaddress(string domain);
	int RetrieveResponseCode(string buffer);
	int Connect();
	int SendFTPcommand(string command);

};