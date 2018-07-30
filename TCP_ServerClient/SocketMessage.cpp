#include "SocketMessage.h"

namespace AsyncTCP
{
	SocketMessage::SocketMessage() : 
		m_dataLength(0),
		m_initialized(false)
	{
	}

	SocketMessage::SocketMessage(const unsigned char* data, unsigned int length)
	{
		init(length);
		std::copy(data, data + length, m_data + HEADER_SIZE);
	}

	SocketMessage::SocketMessage(const std::string& messageStr)
	{
		init(messageStr.size());
		std::copy(messageStr.begin(), messageStr.end(), m_data + HEADER_SIZE);
	}

	SocketMessage::SocketMessage(const SocketMessage& rhs)
	{
		init(rhs.dataLength());
		std::copy(rhs.data(), rhs.data() + m_dataLength, m_data + HEADER_SIZE);
	}

	SocketMessage& SocketMessage::operator=(const SocketMessage& rhs)
	{
		if (this != &rhs)
		{
			init(rhs.dataLength());
			std::copy(rhs.data(), rhs.data() + m_dataLength, m_data + HEADER_SIZE);
		}
		return *this;
	}
	
	SocketMessage::~SocketMessage()
	{
		if (m_initialized) //free ONLY IF the array is initialized!
			free(m_data);
	}

	void SocketMessage::init(unsigned int length)
	{
		m_dataLength = length + HEADER_SIZE;
		m_data = (unsigned char*)malloc(m_dataLength * sizeof(unsigned char));
		m_initialized = true;
		generateHeader();
	}

	void SocketMessage::generateHeader()
	{
		std::string header(DEFAULT_HEADER);	//this is default header!

		std::stringstream stream;
		stream << std::hex << (m_dataLength - HEADER_SIZE);
		std::string result(stream.str());	//message length string

		header.replace(header.length() - result.length(), result.length(), result);	//adding the message length to the header

		std::copy(header.begin(), header.end(), m_data);
	}

	bool SocketMessage::validateHeader(const unsigned char* candidateHeader)
	{
		if (candidateHeader[0] == ASCII_0 && candidateHeader[1] == ASCII_1 && candidateHeader[2] == ASCII_0 && candidateHeader[3] == ASCII_0)
		{ //candidateHeader[2] & candidateHeader[3] => reserved as '0'
			unsigned int messageSize;
			std::string msgSizeStr((char*)candidateHeader + HEADER_VERSION_SIZE, HEADER_DATALENGTH_SIZE);
			std::stringstream ss;
			ss << std::hex << msgSizeStr;
			ss >> messageSize;

			if (messageSize <= CUPPS_SOCKET_MESSAGE_MAX_LEN)
			{ //message length should be less than or equal to 0xFFFFFF
				m_dataLength = HEADER_SIZE + messageSize;
				m_data = (unsigned char*)malloc(m_dataLength * sizeof(unsigned char));
				std::copy(candidateHeader, candidateHeader + HEADER_SIZE, m_data);

				return true;
			}
		}
		std::cout << "SocketMessage::validateHeader() -> Received Header is not VALID!" << std::endl;
		return false;
	}

	const unsigned char* SocketMessage::data() const
	{
		return m_data;
	}

	unsigned int SocketMessage::dataLength() const
	{
		return m_dataLength;
	}

	unsigned char* SocketMessage::message()
	{
		return m_data + HEADER_SIZE;
	}

	const unsigned char* SocketMessage::message() const
	{
		return m_data + HEADER_SIZE;
	}

	unsigned int SocketMessage::messageLength() const
	{
		return m_dataLength - HEADER_SIZE;
	}

}
