#include <iostream>
#include <cstring>

#include <thread>
#include <chrono>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>


std::queue<int> clientQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;




void handleClient(int clientSocket){
    auto start = std::chrono::steady_clock::now();
    // 6. Read the HTTP request
    char buffer[4096] = {};
    read(clientSocket, buffer, sizeof(buffer));

    std::cout << "Request:\n" << buffer << "\n";

    // 7. Create an HTTP response
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

    // 8. Send it back
    std::this_thread::sleep_for(std::chrono::seconds(5));
    send(clientSocket, response, strlen(response), 0);
    auto end = std::chrono::steady_clock::now();

    auto elapsed = end -  start;
    std::cout<< "Total time: " << elapsed.count()<< std::endl;
    // 9. Close sockets
    close(clientSocket);

}
void worker(){
    while(true){
        std::unique_lock<std::mutex> lock(queueMutex);
        while(clientQueue.size() == 0){
            queueCondition.wait(lock);
        }
        int Client = clientQueue.front();
        clientQueue.pop();
        lock.unlock();
        handleClient(Client);

    }
}

int main() {
    // 1. Create the server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        std::cerr << "Could not create socket\n";
        return 1;
    }

    // 2. Describe the address/port we want to use
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    // 3. Bind the socket to port 8080
    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)) == -1) {

        std::cerr << "Bind failed\n";
        close(serverSocket);
        return 1;
    }

    // 4. Start listening for connections
    listen(serverSocket, 5);

    std::cout << "Server listening on port 8080...\n";

    for(int i=0; i< 4; i++){
        std::thread th(worker);
        th.detach();
    }

    // 5. Accept one client connection
    while(true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        if(clientSocket == -1){
            std::cerr <<"Accept Failed"<< std::endl;
        }
        queueMutex.lock();
        clientQueue.push(clientSocket);
        queueMutex.unlock();
        
        queueCondition.notify_one();

    }
    close(serverSocket);

    return 0;
}