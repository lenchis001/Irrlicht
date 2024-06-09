#ifndef IWEBSOCKER_SERVER
#define IWEBSOCKER_SERVER

#include "string"

#include "boost/function.hpp"

namespace TNC::WebSocket::Server {

	typedef boost::function<void(const std::string&, const std::string&)> WebSocketDataHandler;
	typedef boost::function<void(boost::function<void(const std::string&)>, const std::string&)> ClientConnectedHandler;

	class IWebSocketServer {
	public:
		virtual void start(int port) = 0;
		virtual void stop() = 0;
		virtual void send(const std::string& data) = 0;
		virtual void setDataHandler(WebSocketDataHandler handler) = 0;
		virtual void setClientConnectedHandler(ClientConnectedHandler handler) = 0;
	};

}




#endif // IWEBSOCKER_SERVER