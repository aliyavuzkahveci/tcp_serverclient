#include "AsyncTCPServer_Factory.h"
#include "AsyncTCPServer_Impl.h"

namespace AsyncTCP
{
	AsyncTCPServer_Factory_Ptr AsyncTCPServer_Factory::m_instance = nullptr;

	AsyncTCPServer_Factory_Ptr& AsyncTCPServer_Factory::getInstance()
	{
		if (m_instance == nullptr)
			m_instance = std::unique_ptr<AsyncTCPServer_Factory>(new AsyncTCPServer_Factory());
		return m_instance;
	}

	AsyncTCPServer_Factory::AsyncTCPServer_Factory()
	{
	}


	AsyncTCPServer_Factory::~AsyncTCPServer_Factory()
	{
	}

	AsyncTCPServer_Ptr AsyncTCPServer_Factory::createAsyncTCPServer(
		AsyncTCPServer_Subscriber_Ptr subscriberPtr, short port)
	{
		return AsyncTCPServer_Ptr(new AsyncTCPServer_Impl(subscriberPtr, port));
	}
}
