#ifndef IWEB_SOCKET_CLIENT
#define IWEB_SOCKET_CLIENT

#include "string"
#include "map"

#define BOOST_THREAD_VERSION 4

#include <boost/thread/future.hpp>
#include <boost/function.hpp>

namespace TNC::WebSocket::Client {

	class IWebSocketClient {
	public:
		virtual boost::future<void> connect(const std::string& host, const std::map<std::string, std::string>& headers) = 0;

		virtual void send(const std::string& data) = 0;

		virtual void setDataHandler(boost::function<void(const std::string&)> handler) = 0;

		virtual void disconnect() = 0;
	};

}

#endif // IWEB_SOCKET_CLIENT