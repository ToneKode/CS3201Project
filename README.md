# NOTE: 
The source codes of repository serves merely as a refrence for CityU CS students. It shouldn't be copied or used in any scnearios. Should CityU not allow the existance of this repo, please email chewtony54@gmail.com and I will remove it as soon as possible.
# CS3201 Project - TodoTrackerClient

## Description
This is code of the CS3201 Computer Network assigment, where its a client program of a online TODO tracker system. 


## How to Compile and Run
# NOTE: This program works only on the Windows operating system.

### Prerequisites
- **Windows Operating System** (required for Winsock2 API)
- **GCC Compiler** or **MinGW-w64** installed and configured
  - You can install MinGW-w64 from [https://www.mingw-w64.org/](https://www.mingw-w64.org/)
  - Or use a package manager like MSYS2 or install through an IDE like Code::Blocks or Dev-C++
  - Ensure `g++` is available in your system PATH
- **C++11**

### Compilation
To compile the TodoTrackerClient program:

```cmd
g++ -std=c++11 TodoTrackerClient.cpp -lws2_32 -o TodoTrackerClient.exe
```

Or using PowerShell:
```powershell
g++ -std=c++11 TodoTrackerClient.cpp -lws2_32 -o TodoTrackerClient.exe
```

### Running
To run the client program, you need to provide the server's IP address and port number (The defaults are IP: 127.0.0.1 PORT: 18222):

```cmd
TodoTrackerClient.exe <IP_address> <port_number>
```

Example:
```cmd
TodoTrackerClient.exe 127.0.0.1 18222
```
Or using PowerShell:
```powershell
.\TodoTrackerClient.exe 127.0.0.1 18222
```

## Files

```cmd
    .
├── TodoTrackerClient.cpp          # The client
├── report.pdf                     # The Project Report
├── README.md
```

