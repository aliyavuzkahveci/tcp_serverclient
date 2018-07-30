#ifndef _ASYNC_TCP_GLOBALS_H
#define _ASYNC_TCP_GLOBALS_H

/*@author  Ali Yavuz Kahveci aliyavuz.kahveci@tav.aero
* @version 1.0
* @since   18-07-2018
* @Purpose: header file for accessing GLOBAL objects (ex: IO service)
*/

#include <memory>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "AsyncTCPUtil.h"

namespace AsyncTCP
{
	class AsyncTCP_Globals;
	using AsyncTCP_Globals_Ptr = std::unique_ptr<AsyncTCP_Globals>;

	class ASYNCTCP_DLL AsyncTCP_Globals final
	{
	public:
		static AsyncTCP_Globals_Ptr& getInstance();

		virtual ~AsyncTCP_Globals();

		//1st param -> io_service ptr
		void init(std::shared_ptr<boost::asio::io_service> ioServicePtr);

		void terminate();

		std::shared_ptr<boost::asio::io_service> getIOService();

	private:
		AsyncTCP_Globals();

		/*to protect the Singleton class from being copied*/
		AsyncTCP_Globals(const AsyncTCP_Globals&) = delete;
		AsyncTCP_Globals& operator=(const AsyncTCP_Globals&) = delete;
		AsyncTCP_Globals(AsyncTCP_Globals&&) = delete;
		//AsyncTCP_Globals& operator=(AsyncTCP_Globals&) = delete; //Warning C4522 multiple assignment operators specified
		/*to protect the Singleton class from being copied*/

		static AsyncTCP_Globals_Ptr m_instance;
		
		std::shared_ptr<boost::asio::io_service> m_ioService;
		std::shared_ptr<boost::thread_group> m_threadGroup;
	};
}

#define ASYNCTCP_IOSERVICE AsyncTCP::AsyncTCP_Globals::getInstance()->getIOService()

#endif
