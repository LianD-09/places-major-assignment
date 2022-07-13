# Places-major-assignment
This is our major assignment for Network Programming subject.

## General
* Use Window Socket to perform connection between a simple model network including Server and Clients.
* Features: Logging in/out, signing in, finding places, adding friend, sharing location... 

## Requirement
* Window 7/8/10/11 OS
* C++
* WinSock 2.2
* Visual Studio 2015

## Development Example
Start with forking the upstream repository https://github.com/LianD-09/places-major-assignment, and clone your personal fork in your workspace. (replace `LianD-09` with your own github username). Also add the `upstream` repository (using `https` protocol)

```bash
git clone git@github.com:LianD-09/places-major-assignment.git
cd places-major-assignment
git remote add upstream https://github.com/LianD-09/places-major-assignment
```
Run your server first
```bash
cd /Places-major-assignment/Debug
./Server.exe <port>
```

Run client
```bash
cd /Places-major-assignment/Debug
./Client.exe <server_IP> <port>
```

## Prepare
* Add header Winsock.h library to source code.
* Add pre-processor declaration: #pragma comment(lib, "Ws2_32.lib")

## Authors
* [LianD-09](https://github.com/LianD-09)
* [Makise-Chris](https://github.com/Makise-Chris)
* [nghiaNT2310](https://github.com/nghiaNT2310)
