#include <iostream>
#include <SFML/network.hpp>

int main()
{
    sf::TcpSocket socket;
    if (socket.connect("172.16.1.54", 52800) != sf::Socket::Done)
    {
       // Error
    }

    do
    {
        // Send data
        char dataToSend[100];

        std::cin >> dataToSend;

        // Push it to the server
        if (socket.send(dataToSend, sizeof(dataToSend)) != sf::Socket::Done)
        {
            // Error
        }
    } while (true);

   
}