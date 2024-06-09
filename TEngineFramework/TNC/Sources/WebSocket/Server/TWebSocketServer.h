#ifndef TWEB_SOCKET_SERVER
#define TWEB_SOCKET_SERVER

#include <set>

#include "IWebSocketServer.h"

#include "boost/smart_ptr.hpp"
#include "boost/thread.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace TNC::WebSocket::Server {

	typedef websocketpp::server<websocketpp::config::asio> server;
	typedef server::message_ptr message_ptr;

	class TWebSocketServer : public IWebSocketServer
	{
	public:
		void start(int port) override;
		void stop() override;
		void send(const std::string& data) override;
		void setDataHandler(WebSocketDataHandler handler) override;
		void setClientConnectedHandler(ClientConnectedHandler handler) override;
	private:
		boost::shared_ptr<server> _server;
		std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> _clients;
		boost::shared_ptr<boost::thread> _socketThread;
		WebSocketDataHandler _dataHandler;
		ClientConnectedHandler _clientConnectedHandler;
	};

}

#endif // TWEB_SOCKET_SERVER