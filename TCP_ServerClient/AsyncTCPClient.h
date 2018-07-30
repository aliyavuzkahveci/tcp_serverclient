#ifndef _ASYNC_TCP_CLIENT_H
#define _ASYNC_TCP_CLIENT_H

/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   17-07-2018
* @Purpose: classes for TCP Client functionalities with client subsriber base class
*/

#include <memory>

#include <boost/asio.hpp>

#include "AsyncTCP_Globals.h"
#include "SocketMessage.h"
#include "AsyncTCPUtil.h"

using boost::asio::ip::tcp;

namespace AsyncTCP
{
	class AsyncTCPClient_Subscriber;
	using AsyncTCPClient_Subscriber_Ptr = std::shared_ptr<AsyncTCPClient_Subscriber>;

	class ASYNCTCP_DLL AsyncTCPClient
	{
	public:
		AsyncTCPClient(AsyncTCPClient_Subscriber_Ptr subscriber, const std::string& ipAddress, const std::string& port);

		virtual ~AsyncTCPClient();

		bool isConnected() { return m_isConnected; };
		bool isReconnecting() { return m_isReconnecting; };

		void close();

		void write(SocketMessage_Ptr);
		void write(const std::string& message);

	private:
		void try_to_reconnect();

		void handle_connect();

		void handle_read_header();
		void handle_read_body();

		void handle_write();

		boost::mutex m_guard;

		AsyncTCPClient_Subscriber_Ptr m_subscriber;
		tcp::socket m_socket;
		tcp::resolver m_resolver;

		SocketMessage_Ptr m_receivedMsg;
		SocketMessage_queue m_writeMsgQueue;

		unsigned char m_candidateHeader[HEADER_SIZE];
		std::string m_ipAddress;
		std::string m_port;
		bool m_isConnected;
		bool m_isReconnecting;
		bool m_isShuttingDown;

		/*to protect the class from being copied*/
		AsyncTCPClient(const AsyncTCPClient&) = delete;
		AsyncTCPClient& operator=(const AsyncTCPClient&) = delete;
		AsyncTCPClient(AsyncTCPClient&&) = delete;
		//AsyncTCPClient& operator=(AsyncTCPClient&) = delete; //Warning C4522 multiple assignment operators specified
		/*to protect the class from being copied*/
	};
	using AsyncTCPClient_Ptr = std::shared_ptr<AsyncTCPClient>;

	class ASYNCTCP_DLL AsyncTCPClient_Subscriber : public std::enable_shared_from_this<AsyncTCPClient_Subscriber>
	{
	public:
		virtual void onMessageReceived(SocketMessage_Ptr msg) = 0;
		virtual void onStatusChanged(TCPClientSocketStatusType status) = 0;
	};
	using AsyncTCPClient_Subscriber_Ptr = std::shared_ptr<AsyncTCPClient_Subscriber>;
}

#endif
