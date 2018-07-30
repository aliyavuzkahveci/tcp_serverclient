#include "Tester_TCPServer.h"

#include <algorithm>

#include <AsyncTCPServer_Factory.h>

using namespace AsyncTCP;

namespace TCPTest
{
	Tester_TCPServer::Tester_TCPServer()
	{
	}


	Tester_TCPServer::~Tester_TCPServer()
	{
		mTCPServer.reset();
	}

	void Tester_TCPServer::startServerListener(short portNumber)
	{
		boost::mutex::scoped_lock lock(m_guard);

		try
		{
			if (mTCPServer.get()) return;

			mTCPServer = AsyncTCPServer_Factory::getInstance()->createAsyncTCPServer(shared_from_this(), portNumber);
		}
		catch (const std::exception& ex)
		{
			std::cout << "Tester_TCPServer::startServerListener() -> " << ex.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "Tester_TCPServer::startServerListener() -> Unknown Error Occured!" << std::endl;
		}
	}

	void Tester_TCPServer::closeServerListener()
	{
		boost::mutex::scoped_lock lock(m_guard);

		if (mTCPServer.get())
			ASYNCTCP_IOSERVICE->post(boost::bind(&AsyncTCPServer::close, mTCPServer));
	}

	void Tester_TCPServer::broadcastMessage(const std::string& message)
	{
		mTCPServer->broadcast((unsigned char*)message.c_str(), message.length());
	}

	void Tester_TCPServer::onMessageReceived(SocketMessage_Ptr msg, const AsyncTCPSession_Ptr& session)
	{
		boost::mutex::scoped_lock lock(m_receive_guard);

		std::string receivedMessage((char*)msg->message(), msg->messageLength());
		std::cout << "Tester_TCPServer::onMessageReceived() -> "
			<< "FROM [" << session->ipPortStr() << "] "
			<< "MESSAGE: " << receivedMessage << std::endl;

		//revert the received message and send back to the client!!!
		std::reverse(receivedMessage.begin(), receivedMessage.end());

		mTCPServer->write((const unsigned char*)receivedMessage.c_str(), receivedMessage.length(), session->uniqueID());
	}

	void Tester_TCPServer::afterClientConnect(const AsyncTCPSession_Ptr& session)
	{
		boost::mutex::scoped_lock lock(m_guard);

		std::cout << "Tester_TCPServer::afterClientConnect() -> "
			<< "Peer joined: ["
			<< session->ipPortStr()
			<< ", UID= "
			<< session->uniqueID()
			<< "]" << std::endl;
	}

	void Tester_TCPServer::beforeClientDisconnect(const AsyncTCPSession_Ptr& session)
	{
		boost::mutex::scoped_lock lock(m_guard);

		std::cout << "Tester_TCPServer::beforeClientDisconnect() -> "
			<< "Peer is leaving: ["
			<< session->ipPortStr()
			<< ", UID= "
			<< session->uniqueID()
			<< "]" << std::endl;
	}

}
