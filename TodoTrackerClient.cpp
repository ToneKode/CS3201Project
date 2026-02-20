#include <iostream>
#include <string>
#include <winsock2.h>
#include <algorithm>
#include <ws2tcpip.h>
#include <limits>

#pragma comment(lib, "ws2_32.lib")

class Client;
// Command Interface for ADD REMOVE MARK LIST QUIT
class command
{
public:
    virtual ~command() {}
    virtual void execute(Client &client) = 0;
};

// Client class
class Client
{
private:
    SOCKET clientSocket;
    bool connected;

public:
    Client();
    ~Client();
    bool connectToServer(const std::string &ip, unsigned short port);
    SOCKET getSocket() const { return clientSocket; }
    void closeConnection();
    bool isConnected() const { return connected; }
    bool recvLine(std::string &out);
};

Client::Client(){
    clientSocket = INVALID_SOCKET;
    connected = false;

    // wsadata startup
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        std::cerr << "WSAStartup failed" << std::endl;
        exit(1);
    }
}

Client::~Client(){
    // socket close
    closeConnection();
    WSACleanup();
}

bool Client::connectToServer(const std::string &ip, unsigned short port){
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Error checking: failed init of socket in client constructor
    if (clientSocket == INVALID_SOCKET){
        std::cerr << "Socket creation failed! " << std::endl;
        return false;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    // Error checking: ip check
    if (inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr) != 1){
        std::cerr << "Invalid IP address format" << std::endl;
        closesocket(clientSocket);
        return false;
    }
    // Error checking: socket connection error
    if (connect(clientSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR){
        std::cerr << "Connection failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return false;
    }
    connected = true;
    return true;
}

// close connection with server
void Client::closeConnection(){
    if (clientSocket != INVALID_SOCKET){
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
    connected = false;
}

// helper function that uses recv by reading a line char by char
bool Client::recvLine(std::string &out){
    char c;
    out.clear();
    while (true){
        int ret = recv(clientSocket, &c, 1, 0);
        if (ret <= 0)
            return false;
        if (c == '\n')
            break;
        out.push_back(c);
    }
    return true;
}

// ADD REMOVE MARK LIST QUIT command

// Add Command
class addCommand : public command
{
public:
    void execute(Client &client) override;
};

void addCommand::execute(Client &client){
    // sends the ADD string
    std::string cmd = "ADD\n";
    send(client.getSocket(), cmd.c_str(), cmd.size(), 0);

    std::string line;
    while (true){
        std::cout << "Client: " << std::flush;
        std::getline(std::cin, line);
        // send the # string
        if (line == "#"){
            std::string term = "#\n";
            send(client.getSocket(), term.c_str(), term.size(), 0);
            break;
        }
        // send the line string
        line += "\n";
        send(client.getSocket(), line.c_str(), line.size(), 0);
    }
    // recv response
    std::string response;
    if (client.recvLine(response))
        std::cout << "Server: " << response << std::endl;
    else
        std::cerr << "No response from server.\n";
}

// Remove Command
class removeCommand : public command
{
public:
    void execute(Client &client) override;
};

void removeCommand::execute(Client &client){
    //send the REMOVE string
    std::string cmd = "REMOVE\n";
    send(client.getSocket(), cmd.c_str(), cmd.size(), 0);
    std::string line;

    while (true){
        std::cout << "Client: " << std::flush;
        std::getline(std::cin, line);
        //send the # string
        if (line == "#"){
            std::string term = "#\n";
            send(client.getSocket(), term.c_str(), term.size(), 0);
            break;
        }
        //send the ids to be removed
        line += "\n";
        send(client.getSocket(), line.c_str(), line.size(), 0);
    }
    //recv from server
    std::string response;
    if (client.recvLine(response))
        std::cout << "Server: " << response << std::endl;
    else
        std::cerr << "No response from server.\n";
}

// Mark Command
class markCommand : public command
{
public:
    void execute(Client &client) override;
};

void markCommand::execute(Client &client){
    //send the MARK string
    std::string cmd = "MARK\n";
    send(client.getSocket(), cmd.c_str(), cmd.size(), 0);
    std::string line;

    while (true){
        std::cout << "Client: " << std::flush;
        std::getline(std::cin, line);
        //send the # string
        if (line == "#"){
            std::string term = "#\n";
            send(client.getSocket(), term.c_str(), term.size(), 0);
            break;
        }
        //send the ids to be marked
        line += "\n";
        send(client.getSocket(), line.c_str(), line.size(), 0);
    }
    //recv server reponse
    std::string response;
    if (client.recvLine(response))
        std::cout << "Server: " << response << std::endl;
    else
        std::cerr << "No response from server.\n";
}

// List Command
class listCommand : public command
{
public:
    void execute(Client &client) override;
};

void listCommand::execute(Client &client){
    //send the LIST string
    std::string msg = "LIST\n";
    send(client.getSocket(), msg.c_str(), msg.size(), 0);
    //recv server reponse
    std::cout << "Server:" << std::endl;
    std::string response;
    while (true){
        if (!client.recvLine(response)){
            std::cerr << "Connection closed or error while receiving list.\n";
            break;
        }
        if (response == "#")
            break;
        std::cout << response << std::endl;
    }
}

// Quit Command
class quitCommand : public command
{
public:
    void execute(Client &client) override;
};

void quitCommand::execute(Client &client){
    //send the QUIT string
    std::string msg = "QUIT\n";
    send(client.getSocket(), msg.c_str(), msg.size(), 0);
    //recv server reponse
    std::string response;
    if (client.recvLine(response))
        std::cout << "Server: " << response << std::endl;
    else
        std::cerr << "Failed to receive acknowledgment from server.\n";
}

// Command Factory Class 
class commandFactory
{
public:
    static command *createCommand(const std::string &cmd);
};

command *commandFactory::createCommand(const std::string &cmd){
    std::string upper = cmd;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (upper == "ADD"){
        return new addCommand();
    }
    if (upper == "REMOVE"){
        return new removeCommand();
    }
    if (upper == "MARK"){
        return new markCommand();
    }
    if (upper == "LIST"){
        return new listCommand();
    }
    if (upper == "QUIT"){
        return new quitCommand();
    }

    return nullptr;
}

// Main
int main(int argc, char *argv[]){
    // Error checking: input in command prompt
    if (argc != 3){
        std::cerr << "Usage: " << argv[0] << " <IP_address> <port_number>\n";
        return 1;
    }

    // Make a client object AND connects client to server
    Client client;
    if (!client.connectToServer(argv[1], static_cast<unsigned short>(atoi(argv[2])))){
        return 1;
    }

    std::string cmd;
    std::cout << "Client: " << std::flush;

    while (true){
        if (!std::getline(std::cin, cmd))
            break;

        // trim trailing and leading spaces
        cmd.erase(0, cmd.find_first_not_of(" \t\r\n"));
        cmd.erase(cmd.find_last_not_of(" \t\r\n") + 1);

        if (cmd.empty()){
            std::cout << "Client: " << std::flush;
            continue;
        }

        // make a command factory
        command *Command = commandFactory::createCommand(cmd);
        if (Command){
            // send the command to the factory to determine which command to execute
            Command->execute(client);
            // QUIT: first end the prompt loop
            if (cmd == "QUIT" || cmd == "quit"){
                delete Command;
                break;
            }
            delete Command;
        }
        else{
            // Error checking: for all inputs not ADD REMOVE MARK LIST QUIT
            std::cout << "Server: Error - Command not understood.\n";
        }
        std::cout << "Client: " << std::flush;
    }

    // QUIT: then close the socket
    client.closeConnection();
    return 0;
}