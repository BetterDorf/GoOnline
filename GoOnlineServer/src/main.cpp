#include <iostream>
#include <SFML/Network.hpp>

int main()
{
    
    sf::TcpListener listener;

    // Listen and block execution until we get a connection
    if (listener.listen(52800) != sf::Socket::Done)
    {
        return EXIT_FAILURE;
        // Error
    }

    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Done)
    {
        return EXIT_FAILURE;
        // Error
    }

    std::cout << "Connection received : " << client.getRemoteAddress() << std::endl;

    // Receive data and print it
    do
    {
        char receivedData[100];
        size_t receivedSize;

        if (client.receive(receivedData, sizeof(receivedData), receivedSize) != sf::Socket::Done)
        {
            // Error
            std::cerr << "No idea what happened" << std::endl;
            break;
        }

        // Print the data
        std::cout << receivedData << std::endl;
    } while (true);

    std::cout << "Server terminated" << std::endl;

    return EXIT_SUCCESS;
}