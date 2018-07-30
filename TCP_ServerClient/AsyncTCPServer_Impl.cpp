#include "AsyncTCPServer_Impl.h"

#include <functional>

#include <boost/asio/placeholders.hpp>

namespace AsyncTCP
{
	AsyncTCPServer_Impl::AsyncTCPServer_Impl(AsyncTCPServer_Subscriber_Ptr subscriber, short port) :
		AsyncTCPServer(port),
		m_subscriber(subscriber),
		m_tcpSessionIDCounter(0)
	{
		ASYNCTCP_IOSERVICE->post(boost::bind(&AsyncTCPServer_Impl::start, this));
	}


	AsyncTCPServer_Impl::~AsyncTCPServer_Impl()
	{
		m_sessionMap.clear();
	}

	void AsyncTCPServer_Impl::start()
	{
		AsyncTCPSession_Ptr newSessionPtr = AsyncTCPSession_Ptr(new AsyncTCPSession_Impl(shared_from_this(), ++m_tcpSessionIDCounter));

		m_acceptor.async_accept(newSessionPtr->getSocket(),
			boost::bind(&AsyncTCPServer_Impl::handle_accept, this,
				newSessionPtr,
				boost::asio::placeholders::error));
	}

	void AsyncTCPServer_Impl::close()
	{
		for (auto iter : m_sessionMap)
		{
			std::cout << "AsyncTCPServer_Impl::close() -> closing session:" << iter.second->ipPortStr() << std::endl;
			iter.second->closeSession();
		}
		m_sessionMap.clear();
	}

	void AsyncTCPServer_Impl::write(SocketMessage_Ptr message, unsigned int tcpSessionID) const
	{
		auto iter = m_sessionMap.find(tcpSessionID);
		if (iter == m_sessionMap.end())
			std::cout << "AsyncTCPServer_Impl::write(data) -> NO session found for tcpSessionID " << std::to_string(tcpSessionID) << std::endl;
		else
			iter->second->deliverMessage(message);
	}

	void AsyncTCPServer_Impl::write(const unsigned char* data, unsigned int dataLength, unsigned int tcpSessionID) const
	{
		write(SocketMessage_Ptr(new SocketMessage(data, dataLength)), tcpSessionID);
	}

	void AsyncTCPServer_Impl::broadcast(SocketMessage_Ptr message) const
	{
		std::cout << "AsyncTCPServer_Impl::broadcast" << std::endl;
		for (auto iter : m_sessionMap)
			iter.second->deliverMessage(message);
	}

	void AsyncTCPServer_Impl::broadcast(const unsigned char* data, unsigned int dataLength) const
	{
		broadcast(SocketMessage_Ptr(new SocketMessage(data, dataLength)));
	}

	void AsyncTCPServer_Impl::handle_accept(AsyncTCPSession_Ptr session, const boost::system::error_code& error)
	{
		boost::mutex::scoped_lock lock(m_guard);

		if (!error)
		{
			sessionConnected(session);
		}
		else
		{
			std::cout << "AsyncTCPServer_Impl::handle_accept() -> ACCEPT FAILED for session with id:" << std::to_string(session->uniqueID()) << std::endl;
			session.reset();
		}
	}

	void AsyncTCPServer_Impl::onMessageReceived(SocketMessage_Ptr message, AsyncTCPSession_Ptr session)
	{
		if (m_subscriber.get())
			m_subscriber->onMessageReceived(message, session);
	}

	void AsyncTCPServer_Impl::sessionConnected(AsyncTCPSession_Ptr sessionPtr)
	{
		sessionPtr->getSocket().set_option(boost::asio::socket_base::keep_alive(true));
		sessionPtr->start();
		m_sessionMap.insert(TCPSessionPair(sessionPtr->uniqueID(), sessionPtr));

		if (m_subscriber.get())
			m_subscriber->afterClientConnect(sessionPtr);

		AsyncTCPSession_Ptr newSessionPtr = AsyncTCPSession_Ptr(new AsyncTCPSession_Impl(shared_from_this(), ++m_tcpSessionIDCounter));

		m_acceptor.async_accept(newSessionPtr->getSocket(),
			boost::bind(&AsyncTCPServer_Impl::handle_accept, this,
				newSessionPtr,
				boost::asio::placeholders::error));
	}

	void AsyncTCPServer_Impl::sessionDisconnected(AsyncTCPSession_Ptr sessionPtr)
	{
		if (m_subscriber.get())
			m_subscriber->beforeClientDisconnect(sessionPtr);

		if (sessionPtr.get())
			sessionPtr->closeSession();

		m_sessionMap.erase(sessionPtr->uniqueID());
	}


	AsyncTCPSession_Impl::AsyncTCPSession_Impl(AsyncTCPServer_Ptr server, unsigned int uniqueID) :
		AsyncTCPSession(uniqueID),
		m_isShuttingDown(false)
	{
		this->m_serverPtr = server;
	}

	AsyncTCPSession_Impl::~AsyncTCPSession_Impl()
	{
	}

	void AsyncTCPSession_Impl::start()
	{
		handle_read_header();
	}

	void AsyncTCPSession_Impl::closeSession()
	{
		m_isShuttingDown = true;
		m_socket.shutdown(tcp::socket::shutdown_both);
		m_socket.close();
	}

	void AsyncTCPSession_Impl::deliverMessage(SocketMessage_Ptr socketMsg)
	{
		m_writeMsgQueue.push_back(socketMsg);
		if (m_writeMsgQueue.size() == 1) //was empty before pushing this message! So, no write process was in progress!!!
			handle_write();
	}

	void AsyncTCPSession_Impl::handle_read_header()
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
				std::cout << "AsyncTCPSession_Impl::handle_read_header() -> ERROR while READING HEADER from socket! Error: " << error.message() << std::endl;
				AsyncTCPServer_Impl_Ptr serverImplPtr = std::dynamic_pointer_cast<AsyncTCPServer_Impl>(m_serverPtr);
				if (m_isShuttingDown) return;

				if (serverImplPtr != nullptr && serverImplPtr.get())
					serverImplPtr->sessionDisconnected(shared_from_this());
			}
		});
	}

	void AsyncTCPSession_Impl::handle_read_body()
	{
		boost::asio::async_read(m_socket, boost::asio::buffer(m_receivedMsg->message(), m_receivedMsg->messageLength()), boost::asio::transfer_exactly(m_receivedMsg->messageLength()),
			[this](boost::system::error_code error, unsigned int bytes_transferred)
		{
			boost::mutex::scoped_lock lock(m_guard);

			AsyncTCPServer_Impl_Ptr serverImplPtr = std::dynamic_pointer_cast<AsyncTCPServer_Impl>(m_serverPtr);
			if (!error && bytes_transferred == m_receivedMsg->messageLength())
			{
				if (serverImplPtr != nullptr && serverImplPtr.get())
					serverImplPtr->onMessageReceived(m_receivedMsg, shared_from_this());

				m_receivedMsg.reset();
				handle_read_header();
			}
			else
			{
				std::cout << "AsyncTCPSession_Impl::handle_read_body() -> ERROR while READING HEADER from socket! Error: " << error.message() << std::endl;
				if (m_isShuttingDown) return;

				if (serverImplPtr != nullptr && serverImplPtr.get())
					serverImplPtr->sessionDisconnected(shared_from_this());
			}
		});
	}

	void AsyncTCPSession_Impl::handle_write()
	{
		boost::asio::async_write(m_socket,
			boost::asio::buffer(m_writeMsgQueue.front()->data(), m_writeMsgQueue.front()->dataLength()),
			[this](boost::system::error_code error, unsigned int bytes_transferred)
		{
			if (!error && m_writeMsgQueue.front()->dataLength() == bytes_transferred)
			{
				m_writeMsgQueue.pop_front();
				if (!m_writeMsgQueue.empty())
					handle_write();
			}
			else
			{
				std::cout << "AsyncTCPSession_Impl::handle_write() -> ERROR while WRITING to socket! Error: " << error.message() << std::endl;
				if (m_isShuttingDown) return;

				AsyncTCPServer_Impl_Ptr serverImplPtr = std::dynamic_pointer_cast<AsyncTCPServer_Impl>(m_serverPtr);
				if (serverImplPtr != nullptr && serverImplPtr.get())
					serverImplPtr->sessionDisconnected(shared_from_this());
			}
		});
	}

}


