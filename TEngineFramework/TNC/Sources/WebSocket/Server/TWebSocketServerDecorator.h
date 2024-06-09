#ifndef TWEB_SOCKET_SERVER_DECORATOR
#define TWEB_SOCKET_SERVER_DECORATOR

#include "boost/smart_ptr.hpp"

#include "IWebSocketServer.h"

namespace TNC::WebSocket::Server {

	class TWebSocketServerDecorator : public IWebSocketServer
	{
	public:
		TWebSocketServerDecorator();

		void start(int port) override;
		void stop() override;
		void send(const std::string& data) override;
		void setDataHandler(WebSocketDataHandler handler) override;
		void setClientConnectedHandler(ClientConnectedHandler handler) override;
	private:
		boost::shared_ptr<IWebSocketServer> _webSocketServer;
	};

}

#endif // TWEB_SOCKET_SERVER_DECORATOR