#include <AsioServer.h>
#include <windows.h>
#include <thread>
#include <iostream>

const int Port = 5051;
const int ServerTimeout = 8000;
const int ClientTimeOut = 2000;
const std::string hostName = "127.0.0.1";
const std::string helloMessage = "hello from server";
const size_t helloMessageLength = helloMessage.length() + 1;

void HelloMessage()
{
    std::thread clientThread = std::thread
    (
        []() -> void
    {
        //Wait for server to open ports
        Sleep(ClientTimeOut);
        boost::asio::io_service clientIOService;
        AsioWrapper::AsioSocket clientSocket(clientIOService);
        clientSocket.Connect(hostName, Port);
        char buffer[100];
        clientSocket.Read(buffer, helloMessageLength);
        std::cout << "\nMessage from server:\n" << buffer;

    });
    
    AsioWrapper::ASIOServer server(Port);
    AsioWrapper::AsioSocketPtr socket =  server.AcceptSync(ServerTimeout);

    if (socket->Isopen())
    {
        socket->Write((void*)helloMessage.c_str(), helloMessageLength);
    }
    else
    {
        std::cout << " Client failed to connect in " << ServerTimeout << "ms";
    }

    clientThread.join();
}


void ServerAcceptTimout()
{
 
    std::thread clientThread = std::thread
    (
        []() -> void
    {
        //Wait for server to open ports

        boost::asio::io_service clientIOService;

            for (int i = 0 ; i< 15 ; i++)
            {
                AsioWrapper::AsioSocket clientSocket(clientIOService);
                int sleepTime = rand() % 2000 + 1000;

                std::cout << "\nClient: Sleeping for " << sleepTime <<" ms";
                Sleep(sleepTime);
                clientSocket.Connect(hostName, Port);
                
                    if (clientSocket.Isopen())
                    {
                        std::cout << "\nClient: Connected to server";
                    }
                    else
                    {
                        std::cout << "\nClient: Failed to connect";
                    }
            }
    });

    
    AsioWrapper::ASIOServer server(Port);
    while (true)
    {
        std::cout << "\nServer: Wait for client";
        AsioWrapper::AsioSocketPtr socket = server.AcceptSync(ServerTimeout);
        if (socket->Isopen())
        {
            std::cout << "\nServer: Connection established";
        }
        else
        {
            std::cout << "\nServer: Client failed to connect in " << ServerTimeout << "ms";
        }

    }

}

void MainEntryPoint()
{
    HelloMessage();
    //ServerAcceptTimout();
    std::cout << "\nPress any key to continue...";
    getchar();
}

int main()
{
    
    MainEntryPoint();

    return 0;
}

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{
    MainEntryPoint();
    
    return 0;
}
