// Nim.h
//
// This header file establishes some constants that should be used in the Nim project for COMP 311
#include <WinSock2.h>
#include <string>

static char NIM_UDPPORT[] = "29333";            // Port number used by TicTacToe servers

#define NIM_QUERY  "Who?"
#define NIM_NAME     "Name="
#define NIM_CHALLENGE "Player="
#define NIM_ACCEPT "YES"
#define NIM_DECLINE "NO"
#define NIM_CONFIRM "GREAT!"

const int v4AddressSize = 16;
const int portNumberSize = 10;
const int MAX_INTERFACES = 20;
const int WAIT_TIME = 60;
const int MAX_SEND_BUF = 2048;
const int MAX_CHAT_SIZE = MAX_SEND_BUF - 2;
const int MAX_RECV_BUF = 2048;
const int MAX_SERVERS = 100;
const int CLIENT = 1;
const int SERVER = 2;
const int NO_WINNER = 0;
const int ABORT = -1;


static char board[10];

struct ServerStruct {
    std::string name;
    std::string host;
    std::string port;
};


SOCKET connectsock(const char *, const char *, const char *);

SOCKET passivesock(const char *, const char *);

int UDP_recv(SOCKET, char *, int, char *, char *);

int UDP_send(SOCKET, char *, int, const char *, const char *);

int wait(SOCKET, int, int);

char *timestamp();

int getServers(SOCKET, char *, char *, ServerStruct[]);

int playNIM(SOCKET, std::string, std::string, std::string, int);

int serverMain(int, char *argv[], std::string);

int clientMain(int, char *argv[], std::string);

int getIPAddressInfo(char *, char *);

char *getBroadcastAddress();
