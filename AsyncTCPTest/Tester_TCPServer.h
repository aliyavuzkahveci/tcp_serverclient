/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   18-07-2018
* @Purpose: tester class to use the Async TCP Server object
*/

#include <memory>

#include <AsyncTCPServer.h>

using namespace AsyncTCP;

namespace TCPTest
{
	class Tester_TCPServer : public AsyncTCPServer_Subscriber
	{
	public:
		Tester_TCPServer();
		
		virtual ~Tester_TCPServer();

		void startServerListener(short portNumber);

		void closeServerListener();

		void broadcastMessage(const std::string& message);

	private:
		/*methods of interface AsyncTCPServer_Subscriber*/
		void onMessageReceived(SocketMessage_Ptr msg, const AsyncTCPSession_Ptr& session);

		void afterClientConnect(const AsyncTCPSession_Ptr& session);

		void beforeClientDisconnect(const AsyncTCPSession_Ptr& session);
		/*methods of interface AsyncTCPServer_Subscriber*/

		AsyncTCPServer_Ptr mTCPServer;
		boost::mutex m_guard;
		boost::mutex m_receive_guard;

	};
	using Tester_TCPServer_Ptr = std::shared_ptr<Tester_TCPServer>;
}
