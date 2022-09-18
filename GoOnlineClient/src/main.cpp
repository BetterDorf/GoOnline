#include <SFML/network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include<string.h>

#include "MovePacket.h"
#include "ServerMessage.h"
#include "Goban.h"
#include "GoGraphics.h"
#include "ClientGameStates.h"
#include "Button.h"

int main()
{
    // Create go related variables
    golc::Goban goban(19, 19);
    sf::Font font;
    font.loadFromFile("data/MontereyFLF.ttf");
    goc::GoGraphics gobanVisuals(19, font);
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
    bool won = false;

    // Declare sounds
    sf::SoundBuffer clackBuffer;
    clackBuffer.loadFromFile("data/GoClack.wav");
    sf::SoundBuffer passBuffer;
    passBuffer.loadFromFile("data/GoPass.wav");
    sf::Sound clackSound;
    clackSound.setBuffer(clackBuffer);
    sf::Sound passSound;
    passSound.setBuffer(passBuffer);

    // Buttons
    Button buttonPass = Button(sf::Vector2f(100, 50), "Pass", font, sf::Color(247, 222, 165, 255), sf::Color::Black);
    buttonPass.setPosition(sf::Vector2f(850, 400));
    Button buttonConcede = Button(sf::Vector2f(150, 50), "concede", font, sf::Color(247, 222, 165, 255), sf::Color::Black);
    buttonConcede.setPosition(sf::Vector2f(1000, 400));

    sf::Text winText;
    winText.setFont(font);
    winText.scale(sf::Vector2f(4, 4));

    while (window.isOpen())
    {
        window.clear();

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
            // Draw board
            window.draw(gobanVisuals);

            // Draw buttons
            window.draw(buttonPass);
            window.draw(buttonConcede);

            if (gameOver && isSending == false)
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

                    // TODO logic to cancel moves if server disapprove
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
                        clackSound.play();
                        break;
                    }
                    case pass:
                    {
                        passSound.play();

                        // Nothing to do, turn will be passed
                        break;
                    }
                    case abandon:
                    {
                        won = true;
                        clientState = done;
                        break;
                    }
                    }

                    gobanVisuals.UpdateMove(goban);
                    turnToPlay = playerColour;

                }
                default:
                    break;
                }

                inPacket.clear();
                std::cout << "Packet received" << std::endl;
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
                        gobanVisuals.UpdateMove(goban);
                        outPacket.clear();
                        std::cout << "Packet Sent" << std::endl;
                    }

                    // No need to read input, just wait on the sending
                    break;
                }

                MovePacket move;
                bool validMove = false;

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                gobanVisuals.UpdateMouse(mousePos);

                if (gobanVisuals.HasMouseSelection() && mousePressed)
                {
                    sf::Vector2i selection = gobanVisuals.getMouseSelection();

                    if (goban.PlayStone(selection.y, selection.x, playerColour))
                    {
                        move.moveType = stonePlacement;

                        move.x = selection.y;
                        move.y = selection.x;

                        validMove = true;

                        clackSound.play();
                    }
                    else
                    {
                        validMove = false;
                    }
                }
                else if (mousePressed)
                {
                    // Buttons

                    if (buttonPass.Contains(mousePos))
                    {
                        move.moveType = pass;

                        validMove = true;

                        passSound.play();
                    }
                    else if (buttonConcede.Contains(mousePos))
                    {
                        move.moveType = abandon;

                        validMove = true;
                        gameOver = true;
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
            // TODO read game state from goban for points win and separate it from a concede win
            window.draw(gobanVisuals);
            winText.setString(won ? "YOU WIN" : "YOU LOSE");
            window.draw(winText);
            break;
        default:
            break;
        }

        ImGui::SFML::Render(window);
       // window.draw(gobanVisuals);
        window.display();
    }

    return EXIT_SUCCESS;
}