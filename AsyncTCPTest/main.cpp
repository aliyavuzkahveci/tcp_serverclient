// AsyncTCPTest.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include <AsyncTCP_Globals.h>
#include "Tester_TCPServer.h"
#include "Tester_TCPClient.h"

constexpr auto HELP = "-h";
constexpr auto SERVER = "-server";
constexpr auto CLIENT = "-client";
constexpr auto LOCALHOST = "localhost";

constexpr auto MIN_PORT = 0;
constexpr auto MAX_PORT = 65535;

constexpr auto UC_Q = 0x51;
constexpr auto LC_Q = 0x71;

using namespace TCPTest;

int main(int argc, char* argv[])
{
	int returnValue = 0;
	bool isServer = true;
	int portNumber;

	if (argc == 2 && std::strcmp(argv[1], HELP) == 0)
	{
		returnValue = 1; //HELP requested!
	}
	else if (argc == 3 && std::strcmp(argv[1], SERVER) == 0)
	{	//AsyncTCPTest -server ~port~
		isServer = true;
		portNumber = std::atoi(argv[2]);
		if (portNumber <= MIN_PORT || portNumber >= MAX_PORT)
		{
			std::cout << "Port numbers MUST be in the range of " << MIN_PORT << " - " << MAX_PORT << std::endl;
			returnValue = -1;
		}
	}
	else if (argc == 4 && std::strcmp(argv[1], CLIENT) == 0)
	{	//AsyncTCPTest -client ~ipAddress~ ~port~
		isServer = false;
		//ipAddress -> argv[2]
		portNumber = std::atoi(argv[3]);

		boost::system::error_code ec;
		boost::asio::ip::address_v4::from_string(argv[2], ec);
		if (std::strcmp(argv[2], LOCALHOST) != 0 && ec)
		{
			std::cout << "Ip Address format is wrong! Error message: " << ec.message() << std::endl;
			returnValue = -2;
		}
		else if (portNumber <= MIN_PORT || portNumber >= MAX_PORT)
		{
			std::cout << "Port numbers MUST be in the range of " << MIN_PORT << " - " << MAX_PORT << std::endl;
			returnValue = -2;
		}
	}
	else
	{
		std::cout << "You have entered wrong inputs..." << std::endl;
		returnValue = -3;
	}

	if (returnValue != 0)
	{
		std::cout << "There 2 formats for the correct execution of the program:" << std::endl;
		std::cout << "	1. AsyncTCPTest -server ~port~" << std::endl;
		std::cout << "	2. AsyncTCPTest -client ~ipAddress~ ~port~" << std::endl;
	}
	else //Starting the execution of the real program!
	{
		/*initialization of boost io service*/
		std::shared_ptr<boost::asio::io_service> ioServicePtr =
			std::shared_ptr<boost::asio::io_service>(new boost::asio::io_service()); //must be defined in main!

		boost::asio::io_service::work work(*ioServicePtr.get()); //must be defined in main!

		AsyncTCP::AsyncTCP_Globals::getInstance()->init(ioServicePtr);
		/*initialization of boost io service*/

		std::string messageText;

		if (isServer) //starting TCP Server
		{
			Tester_TCPServer_Ptr serverPtr = Tester_TCPServer_Ptr(new Tester_TCPServer());
			serverPtr->startServerListener(portNumber);

			std::cout
				<< "Please write \"Q\" to quit application..." << std::endl
				<< "Message: " << std::endl;

			while (std::getline(std::cin, messageText))
			{
				if (messageText.length() == 1 && (messageText.at(0) == UC_Q || messageText.at(0) == LC_Q))
					break;

				serverPtr->broadcastMessage(messageText);

				std::cout
					<< "Please write \"Q\" to quit application..." << std::endl
					<< "Message: " << std::endl;
			}

			serverPtr->closeServerListener();
			serverPtr.reset();
		}
		else //starting TCP Client connection!
		{
			Tester_TCPClient_Ptr clientPtr = Tester_TCPClient_Ptr(new Tester_TCPClient());
			clientPtr->startConnection(std::string(argv[2]), std::string(argv[3]));

			std::cout
				<< "Please write \"Q\" to quit application..." << std::endl
				<< "Message: " << std::endl;

			while (std::getline(std::cin, messageText))
			{
				if (messageText.length() == 1 && (messageText.at(0) == UC_Q || messageText.at(0) == LC_Q))
					break;

				if (clientPtr->getClient()->isConnected())
				{
					clientPtr->getClient()->write(messageText);

					std::cout
						<< "Please write \"Q\" to quit application..." << std::endl
						<< "Message: " << std::endl;
				}
			}

			clientPtr->getClient()->close();
			clientPtr.reset();
		}

		AsyncTCP::AsyncTCP_Globals::getInstance()->terminate();
	}

    return returnValue;
}

