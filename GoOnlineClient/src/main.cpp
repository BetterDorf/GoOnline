#include <SFML/network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

#include "MovePacket.h"
#include "ServerMessage.h"
#include "DeadGroupPacket.h"
#include "Goban.h"
#include "GoGraphics.h"
#include "ClientGameStates.h"
#include "Button.h"

int main()
{
    // Create go related variables
    golc::Goban goban(19, 19);
    
    Stone playerColour;
    Stone otherColour;

    Stone turnToPlay = black;
    bool gameOver = false;
    bool hasAcceptedDead = false;
    std::pair<double, double> scores;
    bool won = false;
    bool concedeWin = false;

    // Network logic
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

    sf::Clock deltaClock;
    ClientState clientState = login;
    std::string ip;
    unsigned short port;

    sf::Packet inPacket;
    sf::Packet outPacket;

    bool isSending = false;

    char ipBuffer[30] = "localhost";
    char portBuffer[20] = "3003";

    // Visuals and Sound
    sf::Font font;
    font.loadFromFile("data/MontereyFLF.ttf");
    gog::GoGraphics gobanVisuals(19, font);

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
    auto buttonPass = Button(sf::Vector2f(100, 50), "Pass", font, sf::Color(247, 222, 165, 255), sf::Color::Black);
    buttonPass.setPosition(sf::Vector2f(850, 400));
    auto buttonConcede = Button(sf::Vector2f(150, 50), "concede", font, sf::Color(247, 222, 165, 255), sf::Color::Black);
    buttonConcede.setPosition(sf::Vector2f(1000, 400));

    auto buttonAccept = Button(sf::Vector2f(200, 50), "Accept Groups", font, sf::Color::Red, sf::Color::White);
    buttonPass.setPosition(sf::Vector2f(850, 400));
    auto buttonResume = Button(sf::Vector2f(200, 50), "Resume Play", font, sf::Color::Red, sf::Color::White);
    buttonConcede.setPosition(sf::Vector2f(850, 550));

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
                }
                else
                {
                    playerColour = white;
                    otherColour = black;
                }

                gobanVisuals.SetPlayerColour(playerColour);
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
                //  Game is done and no message is sending
                clientState = done;
                break;
            }

            if (turnToPlay != playerColour)
            {
                // Other player's turn

                // Receive data
                sf::Socket::Status status = socket.receive(inPacket);
                if (status != sf::Socket::Done)
                {
                    break;
                }

                switch (GetPacketType(inPacket))
                {
                case serverMessage:
                {
                    ServerMessage message;
                    inPacket >> message;

                    // TODO logic to cancel moves if server disapprove

                    switch (message.msgType)
                    {
                    case scoring:
                    {
                        clientState = scoringPhase;
                        turnToPlay = static_cast<Stone>(message.additionalInfo);
                        goban.GenerateGroupIds();
                        break;
                    }
                    default:
                        break;
                    }

                	break;
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
                        concedeWin = true;
                        clientState = done;
                        break;
                    }
                    }

                    gobanVisuals.UpdateMove(goban);
                    turnToPlay = playerColour;

                    break;
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
                        gobanVisuals.UpdateMove(goban);
                        outPacket.clear();
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
                        won = false;
                        concedeWin = true;
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
        case scoringPhase:
		    {
		        window.draw(gobanVisuals);
                buttonAccept.SetButtonColor(hasAcceptedDead ? sf::Color::Green : sf::Color::Red);
                window.draw(buttonAccept);
                window.draw(buttonResume);

                if (socket.receive(inPacket) == sf::Socket::Done)
                {
                    // Read data from server
                    switch (GetPacketType(inPacket))
                    {
                    case serverMessage:
	                    {
                        ServerMessage message;
                        inPacket >> message;

                        if (message.msgType == scoringEnded)
                        {
                            clientState = done;

                            // Score the game
                            gobanVisuals.ApplyDeadGroupsToBoard(goban);
                            scores = goban.ScoreBoard();

                            double playerScore, otherScore;
                            if (playerColour == black)
                            {
                                playerScore = scores.first;
                                otherScore = scores.second;
                            }
                            else
                            {
                                playerScore = scores.second;
                                otherScore = scores.first;
                            }
                            if (playerScore > otherScore)
                            {
                                won = true;
                            }
                            else
                            {
                                won = false;
                            }

                            concedeWin = false;
                            gameOver = true;
                            clientState = done;
                        }
                    		break;
	                    }
                    case deadGroup: 
						{
                        DeadGroupPacket deadPacket;
                        inPacket >> deadPacket;

                        switch (deadPacket.step)
                        {
                        case continueScoring:
	                        {
                        	// apply the given dead group
                            gobanVisuals.UpdateDeadGroup(goban, deadPacket.groupId);
                            hasAcceptedDead = false;
		                        break;
	                        }
                        case resumePlay:
	                        {
                            gobanVisuals.ResetDeadGroups();
                            clientState = playing;
                            hasAcceptedDead = false;
		                        break;
	                        }
                        default: break;
                        }
							break;
						}
                    default: break;
                    }

                    inPacket.clear();
                }

                if (isSending)
                {
                    if (socket.send(outPacket) == sf::Socket::Done)
                    {
                        isSending = false;
                        outPacket.clear();
                    }

                    break;
                }

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                gobanVisuals.UpdateMouse(mousePos);

                DeadGroupPacket deadPacket;
                bool validPacket = false;

                if (gobanVisuals.HasMouseSelection() && mousePressed && !hasAcceptedDead)
                {
                    sf::Vector2i selection = gobanVisuals.getMouseSelection();
                    golc::Coord selectionCoord(selection.y, selection.x);

                    int id = 0;
                    bool found = false;

                    // Check selection to see if it's in a group
                    auto groups = goban.GroupsById();

                    for (auto& group : groups)
                    {
	                    if (std::ranges::find(group.second, selectionCoord) != group.second.end())
	                    {
                            found = true;
                            id = group.first;
                            break;
	                    }
                    }

                    if (!found)
                        break;

                    deadPacket.step = continueScoring;
                    deadPacket.groupId = id;
                    validPacket = true;

                    // Show the change locally
                    gobanVisuals.UpdateDeadGroup(goban, id);
                }
                else
                {
	                // Do Button logic
                    if (buttonAccept.Contains(mousePos) && mousePressed && !hasAcceptedDead)
                    {
                        deadPacket.step = acceptScoring;
                        hasAcceptedDead = true;
                        validPacket = true;
                    }
                    else if (buttonResume.Contains(mousePos) && mousePressed)
                    {
                        deadPacket.step = resumePlay;
                        hasAcceptedDead = false;
                        validPacket = true;
                    }
                }

                if (validPacket)
                {
                    outPacket.clear();
                    outPacket << deadPacket;

                    isSending = true;
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