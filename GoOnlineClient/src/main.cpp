#include <SFML/network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include<string.h>

#include "MovePacket.h"
#include "ServerMessage.h"
#include "Goban.h"

int main()
{
    // Create go related variables
    Goban goban(19, 19);
    Stone playerColour;
    Stone otherColour;

    // Ask to get an ip from the user
    std::cout << "Ip du serveur : " << std::endl;

    std::string serverIp;
    std::cin >> serverIp;

    sf::TcpSocket socket;
    if (socket.connect(serverIp, 52800) != sf::Socket::Done)
    {
       // Couldn't connect
        std::cout << "Couldn't connect" << std::endl;
        return EXIT_FAILURE;
    }

    // Wait for game started message
    while (true)
    {
        sf::Packet packet;
        socket.receive(packet);

        if (GetPacketType(packet) == serverMessage)
        {
            ServerMessage message;
            packet >> message;

            if (message.msgType == gameStarted)
            {
                if (message.additionalInfo == 0)
                {
                    playerColour = black;
                    otherColour = white;
                    std::cout << "You play black" << std::endl;
                }
                else
                {
                    playerColour = white;
                    otherColour = black;
                    std::cout << "You play white" << std::endl;
                }

                break;
            }
        }
    }

    // Create the application window
    sf::RenderWindow window(sf::VideoMode(800, 600), "OnlineGO");

    // Once game is started we can play
    Stone turnToPlay = black;
    bool gameOver = false;
    while (!gameOver)
    {
        sf::Packet packet;
        if (turnToPlay != playerColour)
        {
            // Other player's turn

            // Receive data
            packet.clear();
            socket.receive(packet);
            switch (GetPacketType(packet))
            {
            case serverMessage:
            {
                ServerMessage message;
                packet >> message;

                // TODO respond to server
            }
            case move:
            {
                MovePacket move;
                packet >> move;

                switch (move.moveType)
                {
                case stonePlacement:
                {
                    goban.PlayStone(move.x, move.y, otherColour);
                    break;
                }
                case pass:
                {
                    // Nothing to do, turn will be passed
                    break;
                }
                case abandon:
                {
                    std::cout << "You win by forfeit!" << std::endl;
                    return EXIT_SUCCESS;
                }
                }

                turnToPlay = playerColour;
            }
            default:
                break;
            }
        }
        else
        {
            // Player's turn
            std::cout << goban.ToString() << std::endl;

            bool moveSent = false;
            do
            {
                std::cout << "Your turn to play.\ntype X,Y to play\tpass to pass\tabandon to forfeit" << std::endl;
                std::string input;

                //Reset buffer
                std::cin.clear();
                //std::cin.ignore(std::numeric_limits<std::streamsize>::max());
                std::cin >> input;

                MovePacket move;
                bool validMove = true;
                if (input == "pass")
                {
                    move.moveType = pass;
                }
                else if (input == "abandon")
                {
                    move.moveType = abandon;
                    gameOver = true;
                    std::cout << "You lose by forfeit" << std::endl;
                }
                else
                {
                    // Try to read input as coordinates
                    // find pos of comma
                    int i = 0;
                    bool found = false;
                    for (; i < input.length(); i++)
                    {
                        if (input[i] == ',')
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        validMove = false;
                    }

                    try
                    {
                        // Invert x and y
                        int y = stoi(input.substr(0, i)) - 1;
                        int x = stoi(input.substr(i + 1, input.length() - 1)) - 1;

                        if (goban.PlayStone(x, y, playerColour))
                        {
                            move.moveType = stonePlacement;
                            move.x = x;
                            move.y = y;
                        }
                        else
                        {
                            validMove = false;
                        }
                    }
                    catch (const std::exception&)
                    {
                        validMove = false;
                    }
                }

                if (validMove)
                {
                    // Send the valid move
                    packet.clear();
                    packet << move;
                    socket.send(packet);

                    moveSent = true;

                    std::cout << goban.ToString() << std::endl;
                }
            } while (!moveSent);

            turnToPlay = otherColour;
        }
    }

    return EXIT_SUCCESS;
}