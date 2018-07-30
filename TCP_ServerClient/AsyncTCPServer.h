#ifndef _ASYNC_TCP_SERVER_H
#define _ASYNC_TCP_SERVER_H

/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   17-07-2018
* @Purpose: base classes for TCP Server & Server-Client session representations and functions
*/

#include <boost/asio.hpp>

#include "AsyncTCP_Globals.h"
#include "SocketMessage.h"

using boost::asio::ip::tcp;

namespace AsyncTCP
{
	class AsyncTCPServer_Subscriber;
	using AsyncTCPServer_Subscriber_Ptr = std::shared_ptr<AsyncTCPServer_Subscriber>;

	class ASYNCTCP_DLL AsyncTCPServer
	{
	public:
		AsyncTCPServer(short port);

		virtual ~AsyncTCPServer();

		virtual void start() = 0;
		virtual void close() = 0;

		//1st param -> message to be sent, 2nd param -> the receiver client's peerID
		virtual void write(SocketMessage_Ptr, unsigned int) const = 0;

		//1st param -> data pointer to be sent, 2nd param -> size of data, 3rd param -> the receiver client's peerID
		virtual void write(const unsigned char* data, unsigned int dataLength, unsigned int tcpSessionID) const = 0;

		//1st param -> message to be broadcasted
		virtual void broadcast(SocketMessage_Ptr) const = 0;

		//1st param -> data pointer to be broadcasted, 2nd param -> size of data
		virtual void broadcast(const unsigned char* data, unsigned int dataLength) const = 0;

	protected:
		tcp::acceptor m_acceptor;
		bool m_bExiting;

	private:
		/*to protect the class from being copied*/
		AsyncTCPServer(const AsyncTCPServer&) = delete;
		AsyncTCPServer& operator=(const AsyncTCPServer&) = delete;
		AsyncTCPServer(AsyncTCPServer&&) = delete;
		//AsyncTCPServer& operator=(AsyncTCPServer&) = delete; //Warning C4522 multiple assignment operators specified
		/*to protect the class from being copied*/

	};
	using AsyncTCPServer_Ptr = std::shared_ptr<AsyncTCPServer>;

	class AsyncTCPServer_Impl;

	class ASYNCTCP_DLL AsyncTCPSession
	{
		friend class AsyncTCPServer_Impl;
	public:
		AsyncTCPSession(unsigned int uniqueID);

		virtual ~AsyncTCPSession();

		unsigned int uniqueID() const { return m_uniqueID; };
		std::string ipPortStr() const;

	protected:
		virtual void start() = 0;
		virtual void closeSession() = 0;
		virtual void deliverMessage(SocketMessage_Ptr) = 0;

		tcp::socket& getSocket() { return m_socket; };

		tcp::socket m_socket;
		AsyncTCPServer_Ptr m_serverPtr;

	private:
		unsigned int m_uniqueID;

		/*to protect the class from being copied*/
		AsyncTCPSession(const AsyncTCPSession&) = delete;
		AsyncTCPSession& operator=(const AsyncTCPSession&) = delete;
		AsyncTCPSession(AsyncTCPSession&&) = delete;
		//AsyncTCPSession& operator=(AsyncTCPSession&) = delete; //Warning C4522 multiple assignment operators specified
		/*to protect the class from being copied*/
	};
	using AsyncTCPSession_Ptr = std::shared_ptr<AsyncTCPSession>;

	class ASYNCTCP_DLL AsyncTCPServer_Subscriber : public std::enable_shared_from_this<AsyncTCPServer_Subscriber>
	{
	public:
		virtual void onMessageReceived(SocketMessage_Ptr msg, const AsyncTCPSession_Ptr& session) = 0;

		virtual void afterClientConnect(const AsyncTCPSession_Ptr& session) = 0;

		virtual void beforeClientDisconnect(const AsyncTCPSession_Ptr& session) = 0;

	};
	using AsyncTCPServer_Subscriber_Ptr = std::shared_ptr<AsyncTCPServer_Subscriber>;
}

#endif
