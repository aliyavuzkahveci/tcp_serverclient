#ifndef _ASYNC_TCP_UTIL_H
#define _ASYNC_TCP_UTIL_H

/*
@author  Ali Yavuz Kahveci aliyavuzkahveci@gmail.com
* @version 1.0
* @since   17-07-2018
* @Purpose: implements utility structs and constant expressions
*/

namespace AsyncTCP
{
#if defined(_WIN32)
#   define DECLSPEC_EXPORT __declspec(dllexport)
#   define DECLSPEC_IMPORT __declspec(dllimport)
	//
	//  HAS_DECLSPEC_IMPORT_EXPORT defined only for compilers with distinct
	//  declspec for IMPORT and EXPORT
#   define HAS_DECLSPEC_IMPORT_EXPORT
#elif defined(__GNUC__)
#   define DECLSPEC_EXPORT __attribute__((visibility ("default")))
#   define DECLSPEC_IMPORT __attribute__((visibility ("default")))
#elif defined(__SUNPRO_CC)
#   define DECLSPEC_EXPORT __global
#   define DECLSPEC_IMPORT /**/
#else
#   define DECLSPEC_EXPORT /**/
#   define DECLSPEC_IMPORT /**/
#endif

#ifndef ASYNCTCP_DLL
#   ifdef ASYNCTCP_DLL_IMPORTS
#       define ASYNCTCP_DLL ASYNCTCP_DECLSPEC_IMPORT
#   elif defined(ASYNCTCP_STATIC_LIBS)
#       define ASYNCTCP_DLL /**/
#   else
#       define ASYNCTCP_DLL DECLSPEC_EXPORT
#   endif
#endif

#define ASCII_0 0x30
#define ASCII_1 0x31

#define CUPPS_SOCKET_MESSAGE_MAX_LEN 0xFFFFFF
#define DEFAULT_HEADER "0100000000"
#define HEADER_SIZE 10
#define HEADER_VERSION_SIZE 4
#define HEADER_DATALENGTH_SIZE 6

	enum TCPClientSocketStatusType
	{
		SS_ConnectionSuccess = 0,
		SS_WriteSuccess = 1,
		SS_ConnectionError = 2,
		SS_ReadError = 3,
		SS_WriteError = 4,
		SS_ReadTimeout = 5,
		SS_WriteTimeout = 6
	};

	static std::ostream& operator<<(std::ostream& outStream, TCPClientSocketStatusType status)
	{
		switch (status)
		{
		case SS_ConnectionSuccess:
			outStream << "Connection success.";
			break;
		case SS_WriteSuccess:
			outStream << "Write success.";
			break;
		case SS_ConnectionError:
			outStream << "Connection error.";
			break;
		case SS_ReadError:
			outStream << "Read error.";
			break;
		case SS_WriteError:
			outStream << "Write error.";
			break;
		case SS_ReadTimeout:
			outStream << "Read timeout.";
			break;
		case SS_WriteTimeout:
			outStream << "Write timeout.";
			break;
		default:
			outStream << "Unknown.";
			break;
		}
		return outStream;
	}

}

#endif
