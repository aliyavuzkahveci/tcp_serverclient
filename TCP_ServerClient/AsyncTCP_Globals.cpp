#include "AsyncTCP_Globals.h"

#include <iostream>

namespace AsyncTCP
{
	AsyncTCP_Globals_Ptr AsyncTCP_Globals::m_instance = nullptr;

	AsyncTCP_Globals_Ptr& AsyncTCP_Globals::getInstance()
	{
		if (m_instance == nullptr)
			m_instance = std::unique_ptr<AsyncTCP_Globals>(new AsyncTCP_Globals());
		return m_instance;
	}

	AsyncTCP_Globals::AsyncTCP_Globals()
	{
	}

	AsyncTCP_Globals::~AsyncTCP_Globals()
	{
	}

	void AsyncTCP_Globals::init(std::shared_ptr<boost::asio::io_service> ioService)
	{
		m_ioService = ioService;
		m_threadGroup = std::shared_ptr<boost::thread_group>(new boost::thread_group());
		boost::asio::io_service::work work(*m_ioService.get());

		for (int i = 0; i < 25; ++i) //creating 25 threads by default!
			m_threadGroup->create_thread(boost::bind(&boost::asio::io_service::run, m_ioService.get()));
	}

	void AsyncTCP_Globals::terminate()
	{
		m_ioService->stop();
		std::cout << "IO Service STOPPED!" << std::endl;

		m_threadGroup->join_all();
		std::cout << "IO Service Threads are terminated!" << std::endl;
	}

	std::shared_ptr<boost::asio::io_service> AsyncTCP_Globals::getIOService()
	{
		return m_ioService;
	}
}
