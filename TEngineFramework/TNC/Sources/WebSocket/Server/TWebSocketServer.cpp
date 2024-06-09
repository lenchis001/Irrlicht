#include "TWebSocketServer.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using namespace TNC::WebSocket::Server;

void TWebSocketServer::start(int port)
{
	_server = boost::make_shared<server>();

	_server->set_access_channels(websocketpp::log::alevel::all);
	_server->clear_access_channels(websocketpp::log::alevel::frame_payload);

	_server->init_asio();

	_server->set_message_handler([&](websocketpp::connection_hdl client, message_ptr message) {
		if (_dataHandler) {
			auto apiKey = _server->get_con_from_hdl(client)->get_request_header("apiKey");

			_dataHandler(message->get_payload(), apiKey);
		}
		});
	_server->set_open_handler([&](websocketpp::connection_hdl client) {
		if (_clientConnectedHandler) {
			auto apiKey = _server->get_con_from_hdl(client)->get_request_header("apiKey");

			_clientConnectedHandler([&](const std::string& data) {
				_server->send(client, data, websocketpp::frame::opcode::value::text);
				}, apiKey);
		}
		_clients.insert(client);
		});
	_server->set_close_handler([&](websocketpp::connection_hdl client) {
		_clients.erase(client);
		});

	_server->listen(port);

	_server->start_accept();

	_socketThread = boost::make_shared<boost::thread>([&]() {
		_server->run();
		});
}

void TWebSocketServer::stop()
{
	_clients.clear();
	_server->stop();
}

void TWebSocketServer::send(const std::string& data)
{
	for (auto& it : _clients) {
		_server->send(it, data, websocketpp::frame::opcode::value::text);
	}
}

void TWebSocketServer::setDataHandler(WebSocketDataHandler handler)
{
	_dataHandler = handler;
}

void TWebSocketServer::setClientConnectedHandler(ClientConnectedHandler handler)
{
	_clientConnectedHandler = handler;
}