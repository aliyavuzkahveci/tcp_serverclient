#ifndef _ASYNC_SOCKET_MESSAGE_H
#define _ASYNC_SOCKET_MESSAGE_H

/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   17-07-2018
* @Purpose: Class definition holding the streamed data from the TCP/IP socket!
*/

#include <string>
#include <memory>
#include <deque>
#include <iostream>
#include <sstream>

#include "AsyncTCPUtil.h"

namespace AsyncTCP
{
	class ASYNCTCP_DLL SocketMessage
	{
	public:
		SocketMessage();
		SocketMessage(const unsigned char* data, unsigned int length = CUPPS_SOCKET_MESSAGE_MAX_LEN);
		SocketMessage(const std::string& messageStr);
		SocketMessage(const SocketMessage& rhs);
		SocketMessage& operator=(const SocketMessage& rhs);

		virtual ~SocketMessage();

		const unsigned char* data() const; //returns data (header + message)
		unsigned int dataLength() const;

		unsigned char* message(); //used when receiving message from TCP socket!
		const unsigned char* message() const; //returns message only!
		unsigned int messageLength() const;

		bool validateHeader(const unsigned char*);

	private:
		//allocates memory to store the socket message!
		void init(unsigned int length);

		void generateHeader();

		bool m_initialized;
		unsigned char* m_data;
		unsigned int m_dataLength;
	};
	using SocketMessage_Ptr = std::shared_ptr<SocketMessage>;
	using SocketMessage_queue = std::deque<SocketMessage_Ptr>;

}

#endif
