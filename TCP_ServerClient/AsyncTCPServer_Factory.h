#ifndef _ASYNC_TCP_SERVER_FACTORY_H
#define _ASYNC_TCP_SERVER_FACTORY_H

/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   17-07-2018
* @Purpose: factory class to create asynchronous TCP Server for communicating with the Client
*/

#include <memory>

#include "AsyncTCPServer.h"

namespace AsyncTCP
{
	class AsyncTCPServer_Factory;
	using AsyncTCPServer_Factory_Ptr = std::unique_ptr<AsyncTCPServer_Factory>;

	class ASYNCTCP_DLL AsyncTCPServer_Factory final
	{
	public:
		static AsyncTCPServer_Factory_Ptr& getInstance();

		virtual ~AsyncTCPServer_Factory();

		/*1st param -> Async TCP Subscriber pointer, 2nd param -> portNummber*/
		AsyncTCPServer_Ptr createAsyncTCPServer(AsyncTCPServer_Subscriber_Ptr, short);

	private:
		AsyncTCPServer_Factory();

		/*to protect the Singleton class from being copied*/
		AsyncTCPServer_Factory(const AsyncTCPServer_Factory&) = delete;
		AsyncTCPServer_Factory& operator=(const AsyncTCPServer_Factory&) = delete;
		AsyncTCPServer_Factory(AsyncTCPServer_Factory&&) = delete;
		//AsyncTCPServer_Factory& operator=(AsyncTCPServer_Factory&) = delete; //Warning C4522 multiple assignment operators specified
		/*to protect the Singleton class from being copied*/

		static AsyncTCPServer_Factory_Ptr m_instance;

	};

}

#endif
