#include "TNCFacade.h"

boost::shared_ptr<TNC::TWebSocket> TNC::TNCFacade::getConnection(std::wstring connectionName)
{
    return _connections.at(connectionName);
}

void TNC::TNCFacade::createConnection(std::wstring connectionName, std::wstring address)
{
    boost::shared_ptr<TWebSocket> handler(new TWebSocket(address));

    _connections.insert({ connectionName, handler });
}

void TNC::TNCFacade::removeConnection(std::wstring connectionName)
{
    _connections.erase(connectionName);
}
