#include <iostream>
#include <SFML/Network.hpp>
#include <iostream>

#include "Player.h"
#include "MovePacket.h"
#include "ServerMessage.h"
#include "DeadGroupPacket.h"
#include "Goban.h"

int main()
{
    golc::Goban goban(19, 19);

    bool consecutivePass = false;
    bool scoringPhase = false;
    Stone scoringAcceptedBy = empty;

    sf::TcpListener listener;
    unsigned short portNum = 3003;

    std::cout << "Public Adress: " << sf::IpAddress::getPublicAddress() << std::endl;
    std::cout << "Local Adress: " << sf::IpAddress::getLocalAddress() << std::endl;

    // Listen and block execution until we get a connection
    bool portBinded = true;
    do
    {
        portBinded = true;
        if (listener.listen(portNum) != sf::Socket::Done)
        {
            std::cerr << "Couldn't bind to port : " << portNum << "\nPlease choose a new port:\n";
            std::cin >> portNum;
            portBinded = false;
        }
    } while (portBinded == false);

    std::cout << "Binded Port : " << portNum << "\n";
    
    // Accept clients
    Player players[2];
    listener.accept(players[0].clientConnection);
    players[0].colour = black;

    // TODO tell first client to wait for new connection

    listener.accept(players[1].clientConnection);
    players[1].colour = white;

    // Send start game message to both clients indicating who's white and who's black
    sf::Packet packet;
    ServerMessage serverMessage(gameStarted, 0);
    packet << serverMessage;
    players[0].clientConnection.send(packet);
    packet.clear();
    serverMessage.additionalInfo = 1;
    packet << serverMessage;
    players[1].clientConnection.send(packet);

    // PLAY GAME
    Stone toPlay = black;
    bool gameOver = false;

    bool sendToOther = true;
    while (!gameOver)
    {
        // Selector logic to receive data from the clients
        sf::SocketSelector selector;
        for (auto& player : players)
        {
            selector.add(player.clientConnection);
        }

        if (selector.wait(sf::seconds(6000.0f)))
        {
            for (auto& curPlayer : players)
            {
                if (curPlayer.colour != toPlay)
                {
                    // Player sent move at incorrect time
                }
                else if (selector.isReady(curPlayer.clientConnection))
                {
                    // Receive data from client
                    packet.clear();
                    curPlayer.clientConnection.receive(packet);

                    switch (GetPacketType(packet))
                    {
                    case PacketType::serverMessage: break;
                    case move:
	                    {
                            if (scoringPhase)
                                break;

                        MovePacket move;
                        packet >> move;
                        bool validMove = true;

                        switch (move.moveType)
                        {
                        case stonePlacement:
                        {
                            sendToOther = true;

                            // Play the move
                            if (goban.PlayStone(move.x, move.y, toPlay))
                            {
                                consecutivePass = false;
                                validMove = true;
                            }
                            else
                            {
                                validMove = false;

                                // TODO Ask for redo if the move isn't valid
                                std::cout << "Invalid Move." << std::endl;
                            }

                            break;
                        }
                        case pass:
                        {
                            // Pass is always valid

                            if (consecutivePass)
                            {
                                //Game ends
                                packet.clear();
                                serverMessage.msgType = scoring;

                                // Pass the turn if game resumes
                                if (toPlay == black)
                                    toPlay = white;
                                else
                                    toPlay = black;
                                serverMessage.additionalInfo = toPlay;
                                packet << serverMessage;

                                // Send to all
                                for (auto& player : players)
                                {
                                    player.clientConnection.send(packet);
                                }

                                scoringPhase = true;
                                goban.GenerateGroupIds();

                                // Don't use normal sending
                                sendToOther = false;
                            }

                            consecutivePass = true;

                            break;
                        }
                        case abandon:
                        {
                            // Abandonning is always valid but ends the game
                            gameOver = true;
                            sendToOther = true;
                            break;
                        }
                        default:
                            validMove = false;
                            break;
                        }

                        // Transmit the move
                        // TODO ask for redo if move is invalid
                        if (validMove && sendToOther)
                        {
                            packet.clear();
                            packet << move;
                            for (auto& [clientConnection, colour] : players)
                            {
                                if (colour != curPlayer.colour)
                                {
                                    clientConnection.send(packet);
                                }
                            }

                            // Print the goban on the server-side
                            std::cout << goban.ToString() << std::endl;

                            // Change player to play
                            if (toPlay == black)
                                toPlay = white;
                            else
                                toPlay = black;
                        }
                        break;
	                    }
                    case deadGroup:
	                    {
                        if (!scoringPhase)
                            break;

						DeadGroupPacket deadPacket;
                        packet >> deadPacket;

                        bool validPacket = false;

                        switch (deadPacket.step)
                        {
                        case continueScoring:
                            // Check if packet is valid
                            for (const auto& [fst, snd] : goban.GroupsById())
                            {
                                if (fst == deadPacket.groupId)
                                {
                                    validPacket = true;
                                    break;
                                }
                            }
                            break;
                        case resumePlay:
                            scoringAcceptedBy = empty;
                            scoringPhase = false;
                            validPacket = true;
                            break;
                        case acceptScoring:
                            if (scoringAcceptedBy == empty)
                            {
                                scoringAcceptedBy = curPlayer.colour;
                            }
                            else if(scoringAcceptedBy != curPlayer.colour)
                            {
                                // If both players accept the dead groups : scoring and incidentally the game, is done
                                packet.clear();
                                ServerMessage message(scoringEnded);
                                packet << message;
                                for (auto& player : players)
                                {
                                    player.clientConnection.send(packet);
                                }
                                return EXIT_SUCCESS;
                            }
                            validPacket = true;
                            break;
                        default: break;
                        }

                        // Resend packet
                        if (validPacket)
                        {
                            packet.clear();
                            packet << deadPacket;

	                        for (auto& player : players)
	                        {
		                        if (player.colour != curPlayer.colour)
		                        {
                                    player.clientConnection.send(packet);
		                        }
	                        }
                        }

		                break;
	                    }
                    default: break;
                    }
                }
            }
        }
    }

    std::cout << "Server timed out" << std::endl;

    return EXIT_SUCCESS;
}