/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   18-07-2018
* @Purpose: tester class to use the Async TCP Client object
*/

#include <AsyncTCPClient.h>

using namespace AsyncTCP;

namespace TCPTest
{
	class Tester_TCPClient : public AsyncTCPClient_Subscriber
	{
	public:
		Tester_TCPClient();
		virtual ~Tester_TCPClient();

		//1st param -> destination Ip Address, 2nd param -> destination port
		void startConnection(const std::string&, const std::string&);

		AsyncTCPClient_Ptr getClient();

	private:
		void onMessageReceived(SocketMessage_Ptr msg) override;

		void onStatusChanged(TCPClientSocketStatusType status) override;

		AsyncTCPClient_Ptr m_tcpClient;
	};
	using Tester_TCPClient_Ptr = std::shared_ptr<Tester_TCPClient>;

}
