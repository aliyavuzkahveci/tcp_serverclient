#include "AsyncTCPServer.h"

namespace AsyncTCP
{
	AsyncTCPServer::AsyncTCPServer(short port) :
		m_acceptor(*ASYNCTCP_IOSERVICE.get())
	{
		tcp::endpoint ep(tcp::v4(), port);
		m_acceptor.open(ep.protocol());
		m_acceptor.set_option(tcp::acceptor::reuse_address(true));
		m_acceptor.bind(ep);
		m_acceptor.listen();
	}


	AsyncTCPServer::~AsyncTCPServer()
	{
		m_acceptor.close();
	}

	AsyncTCPSession::AsyncTCPSession(unsigned int uniqueID) :
		m_uniqueID(uniqueID),
		m_socket(*ASYNCTCP_IOSERVICE.get())
	{
	}

	AsyncTCPSession::~AsyncTCPSession()
	{
	}

	std::string AsyncTCPSession::ipPortStr() const
	{
		return m_socket.remote_endpoint().address().to_string() + "::" + std::to_string(m_socket.remote_endpoint().port());
		//return boost::lexical_cast<std::string>(m_socket.remote_endpoint());
	}
}
