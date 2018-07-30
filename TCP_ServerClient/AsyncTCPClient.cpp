#include "AsyncTCPClient.h"

#include <thread>
#include <chrono>

namespace AsyncTCP
{
	AsyncTCPClient::AsyncTCPClient(AsyncTCPClient_Subscriber_Ptr subscriber, const std::string& ipAddress, const std::string& port) :
		m_socket(*ASYNCTCP_IOSERVICE.get()),
		m_resolver(*ASYNCTCP_IOSERVICE.get()),
		m_subscriber(subscriber),
		m_ipAddress(ipAddress),
		m_port(port),
		m_isConnected(false),
		m_isReconnecting(false),
		m_isShuttingDown(false)
	{
		handle_connect();
	}

	AsyncTCPClient::~AsyncTCPClient()
	{
	}

	void AsyncTCPClient::close()
	{
		m_isShuttingDown = true;
		if (m_isConnected)
			m_socket.shutdown(tcp::socket::shutdown_both);
		m_socket.close();
	}

	void AsyncTCPClient::write(SocketMessage_Ptr socketMsg)
	{
		m_writeMsgQueue.push_back(socketMsg);
		if (m_writeMsgQueue.size() == 1) //was empty before pushing this message! So, no write process was in progress!!!
			handle_write();
	}

	void AsyncTCPClient::write(const std::string& message)
	{
		this->write(SocketMessage_Ptr(new SocketMessage(message)));
	}

	void AsyncTCPClient::try_to_reconnect()
	{
		m_isConnected = false;
		m_isReconnecting = true;
		std::this_thread::sleep_for(std::chrono::seconds(5)); //wait for the server to become started!

		if (m_isShuttingDown) return;

		std::cout << "AsyncTCPClient::try_to_reconnect() -> TRY TO RE-CONNECT..." << std::endl;
		ASYNCTCP_IOSERVICE->post(boost::bind(&AsyncTCPClient::handle_connect,this));
	}

	void AsyncTCPClient::handle_connect()
	{
		m_isReconnecting = false;
		tcp::resolver::iterator ep_iter = m_resolver.resolve({ m_ipAddress, m_port });

		boost::asio::async_connect(m_socket, ep_iter,
			[this](boost::system::error_code error, tcp::resolver::iterator)
		{
			if (!error)
			{
				m_isConnected = true;
				m_subscriber->onStatusChanged(TCPClientSocketStatusType::SS_ConnectionSuccess);
				handle_read_header();
			}
			else
			{
				std::cout << "AsyncTCPClient::handle_connect() -> Could NOT connected to the SERVER! Error: " << error.message() << std::endl;
				m_subscriber->onStatusChanged(TCPClientSocketStatusType::SS_ConnectionError);

				if (!m_isShuttingDown)
					try_to_reconnect();
			}
		});
	}

	void AsyncTCPClient::handle_read_header()
	{
		boost::asio::async_read(m_socket, boost::asio::buffer(m_candidateHeader, HEADER_SIZE), boost::asio::transfer_exactly(HEADER_SIZE),
			[this](boost::system::error_code error, unsigned int bytes_transferred)
		{
			boost::mutex::scoped_lock lock(m_guard);

			m_receivedMsg = SocketMessage_Ptr(new SocketMessage());
			if (!error && bytes_transferred == HEADER_SIZE && m_receivedMsg->validateHeader(m_candidateHeader))
			{
				handle_read_body();
			}
			else
			{
				std::cout << "AsyncTCPClient::handle_read_header() -> ERROR while READING HEADER from socket! Error: " << error.message() << std::endl;
				m_subscriber->onStatusChanged(TCPClientSocketStatusType::SS_ReadError);

				if (!m_isShuttingDown)
					try_to_reconnect();
			}
		});
	}

	void AsyncTCPClient::handle_read_body()
	{
		boost::asio::async_read(m_socket, boost::asio::buffer(m_receivedMsg->message(), m_receivedMsg->messageLength()), boost::asio::transfer_exactly(m_receivedMsg->messageLength()),
			[this](boost::system::error_code error, unsigned int bytes_transferred)
		{
			boost::mutex::scoped_lock lock(m_guard);

			if (!error && bytes_transferred == m_receivedMsg->messageLength())
			{
				m_subscriber->onMessageReceived(m_receivedMsg);

				m_receivedMsg.reset();
				handle_read_header();
			}
			else
			{
				std::cout << "AsyncTCPClient::handle_read_body() -> ERROR while READING BODY from socket! Error: " << error.message() << std::endl;
				m_subscriber->onStatusChanged(TCPClientSocketStatusType::SS_ReadError);

				if (!m_isShuttingDown)
					try_to_reconnect();
			}
		});
	}

	void AsyncTCPClient::handle_write()
	{
		boost::asio::async_write(m_socket,
			boost::asio::buffer(m_writeMsgQueue.front()->data(), m_writeMsgQueue.front()->dataLength()),
			[this](boost::system::error_code error, unsigned int bytes_transferred)
		{
			if (!error && m_writeMsgQueue.front()->dataLength() == bytes_transferred)
			{
				m_subscriber->onStatusChanged(TCPClientSocketStatusType::SS_WriteSuccess);

				m_writeMsgQueue.pop_front();
				if (!m_writeMsgQueue.empty())
				{
					handle_write();
				}
			}
			else
			{
				std::cout << "AsyncTCPClient::handle_write() -> ERROR while WRITING to socket! Error: " << error.message() << std::endl;
				m_subscriber->onStatusChanged(TCPClientSocketStatusType::SS_WriteError);

				if (!m_isShuttingDown)
					try_to_reconnect();
			}
		});
	}

}
