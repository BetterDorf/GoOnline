#include <SFML/network.hpp>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include<string.h>

#include "MovePacket.h"
#include "ServerMessage.h"
#include "Goban.h"
#include "GoGraphics.h"
#include "ClientGameStates.h"

int main()
{
    // Create go related variables
    golc::Goban goban(19, 19);
    goc::GoGraphics gobanVisuals(19, 50);
    Stone playerColour;
    Stone otherColour;

    sf::TcpSocket socket;
    socket.setBlocking(false);

    sf::Packet sendingPacket;
    sf::Packet receivePacket;

    // Create window
    sf::RenderWindow window(sf::VideoMode(640 * 2, 480 * 2), "OnlineGo");
    if (!ImGui::SFML::Init(window))
    {
        std::cout << "Error loading ImGui" << std::endl;
        return EXIT_FAILURE;
    }

    // LoopGame logic
    sf::Clock deltaClock;
    ClientState clientState = login;
    std::string ip;
    unsigned short port;

    sf::Packet inPacket;
    sf::Packet outPacket;

    bool isSending = false;

    char ipBuffer[30] = "localhost";
    char portBuffer[20] = "3003";

    Stone turnToPlay = black;
    bool gameOver = false;

    while (window.isOpen())
    {
        bool mousePressed = false;
        sf::Event event{};
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                sf::Vector2f size = { static_cast<float>(event.size.width), static_cast<float>(event.size.height) };
                sf::Vector2f center = size / 2.0f;
                window.setView(sf::View(center, size));
            }

            mousePressed = event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        }

        // Update ImGui
        ImGui::SFML::Update(window, deltaClock.restart());

        // TODO switch on gamestate to receive and send the correct packets

        switch (clientState)
        {
        case login:
        {
            bool show = true;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::Begin("Login", &show);
            ImGui::TextWrapped("Enter the ip address and the port number of the server.");

            ImGui::InputTextWithHint("IP", "ServerIp", ipBuffer, sizeof(ipBuffer));
            ImGui::InputTextWithHint("Port", "PortNumber", portBuffer, sizeof(portBuffer));

            if (ImGui::Button("Login"))
            {
                clientState = connecting;
                ip = ipBuffer;
                port = static_cast<unsigned short>(std::stoi(portBuffer));
            }

            ImGui::End();
            break;
        }
        case connecting:
        {
            const sf::Socket::Status status = socket.connect(ip, port);

            if (status == sf::Socket::Done || status == sf::Socket::NotReady)
            {
                clientState = waitingForPlayer;
            }
            else if (status == sf::Socket::Disconnected || status == sf::Socket::Error)
            {
                std::cerr << "Couldn't connect\n";
                return EXIT_FAILURE;
            }
            break;
        }
        case waitingForPlayer:
        {
            if (socket.receive(inPacket) != sf::Socket::Done)
            {
                break;
            }

            if (GetPacketType(inPacket) != serverMessage)
            {
                inPacket.clear();
                break;
            }

            ServerMessage message;
            inPacket >> message;

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

                clientState = playing;
            }

            inPacket.clear();
            break;
        }
        case playing:
        {
            if (gameOver)
            {
                clientState = done;
                break;
            }

            if (turnToPlay != playerColour)
            {
                // Other player's turn

                // Receive data
                if (socket.receive(inPacket) != sf::Socket::Done)
                {
                    break;
                }

                switch (GetPacketType(inPacket))
                {
                case serverMessage:
                {
                    ServerMessage message;
                    inPacket >> message;

                    // TODO respond to server
                }
                case move:
                {
                    MovePacket move;
                    inPacket >> move;

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

                inPacket.clear();
            }
            else
            {
                // Player's turn

                // Send info
                if (isSending)
                {
                    if (socket.send(outPacket) == sf::Socket::Done)
                    {
                        // Move was validated and sent
                        isSending = false;
                        turnToPlay = otherColour;
                    }

                    // No need to read input, just wait on the sending
                    break;
                }

                // TODO change visuals
                std::cout << goban.ToString() << std::endl;

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
                    outPacket.clear();
                    outPacket << move;

                    isSending = true;
                }
            }
            break;
        }
        case done:
            break;
        default:
            break;
        }

        window.clear();
        ImGui::SFML::Render(window);
        window.draw(gobanVisuals);
        window.display();
    }

    return EXIT_SUCCESS;
}