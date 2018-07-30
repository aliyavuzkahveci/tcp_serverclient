#include "Tester_TCPClient.h"

namespace TCPTest
{
	Tester_TCPClient::Tester_TCPClient()
	{
	}


	Tester_TCPClient::~Tester_TCPClient()
	{
	}

	void Tester_TCPClient::startConnection(const std::string& ipAddress, const std::string& port)
	{
		std::cout << "Tester_TCPClient::startConnection() -> IP Address:" << ipAddress << " Port Number:" << port << std::endl;
		m_tcpClient = AsyncTCPClient_Ptr(new AsyncTCPClient(shared_from_this(), ipAddress, port));
	}

	AsyncTCPClient_Ptr Tester_TCPClient::getClient()
	{
		return m_tcpClient;
	}

	void Tester_TCPClient::onMessageReceived(SocketMessage_Ptr msg)
	{
		std::cout << "RECEIVED MESSAGE [" << msg->messageLength() << "]" << std::endl;
		std::cout << std::string((char*)msg->message(), msg->messageLength()) << std::endl;
	}

	void Tester_TCPClient::onStatusChanged(TCPClientSocketStatusType status)
	{
		std::cout << "Tester_TCPClient::onStatusChanged() -> New Status:" << status << std::endl;
	}
}
