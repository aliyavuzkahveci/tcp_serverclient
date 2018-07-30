#ifndef _ASYNC_TCP_SERVER_IMPL_H
#define _ASYNC_TCP_SERVER_IMPL_H

/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   17-07-2018
* @Purpose: implementation class for TCP Server
*/

#include <map>

#include "AsyncTCPServer.h"

namespace AsyncTCP
{
	using TCPSessionMap = std::map<unsigned int, AsyncTCPSession_Ptr>; //<tcpSessionID, AsyncTCPSession_Ptr>
	using TCPSessionPair = std::pair<unsigned int, AsyncTCPSession_Ptr>;

	class AsyncTCPServer_Impl : public AsyncTCPServer, public std::enable_shared_from_this<AsyncTCPServer_Impl>
	{
		friend class AsyncTCPSession_Impl;
	public:
		AsyncTCPServer_Impl(AsyncTCPServer_Subscriber_Ptr subscriber, short port);

		virtual ~AsyncTCPServer_Impl();

	private:
		void start();
		void close();

		void write(SocketMessage_Ptr, unsigned int) const;

		void write(const unsigned char* data, unsigned int dataLength, unsigned int peerID) const;

		void broadcast(SocketMessage_Ptr) const;

		void broadcast(const unsigned char* data, unsigned int dataLength) const;

		void onMessageReceived(SocketMessage_Ptr, AsyncTCPSession_Ptr);

		void handle_accept(AsyncTCPSession_Ptr session, const boost::system::error_code& error);

		void sessionConnected(AsyncTCPSession_Ptr sessionPtr);
		void sessionDisconnected(AsyncTCPSession_Ptr sessionPtr);

		AsyncTCPServer_Subscriber_Ptr m_subscriber;
		unsigned int m_tcpSessionIDCounter;
		TCPSessionMap m_sessionMap;
		boost::mutex m_guard;
	};
	using AsyncTCPServer_Impl_Ptr = std::shared_ptr<AsyncTCPServer_Impl>;

	class AsyncTCPSession_Impl : public AsyncTCPSession, public std::enable_shared_from_this<AsyncTCPSession_Impl>
	{
	public:
		AsyncTCPSession_Impl(AsyncTCPServer_Ptr server, unsigned int uniqueID);

		virtual ~AsyncTCPSession_Impl();

	private:
		void start();
		void closeSession();

		void deliverMessage(SocketMessage_Ptr);

		void handle_read_header();
		void handle_read_body();

		void handle_write();

		unsigned char m_candidateHeader[HEADER_SIZE];
		SocketMessage_Ptr m_receivedMsg;
		boost::mutex m_guard;
		SocketMessage_queue m_writeMsgQueue;
		bool m_isShuttingDown;

	};
	using AsyncTCPSession_Impl_Ptr = std::shared_ptr<AsyncTCPSession_Impl>;
}

#endif
