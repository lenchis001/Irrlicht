#pragma once

#include "map"
#include "string"

#include "Mixins/SingletonMixin.h"
#include "THttpClient/THttpClient.h"
#include "TWebSocket/TWebSocket.h"

namespace TNC {
class TNCFacade : public THttpClient, public SingletonMixin<TNCFacade> {

    friend SingletonMixin;

    std::map<std::wstring, boost::shared_ptr<TWebSocket>> _connections;

public:
    boost::shared_ptr<TWebSocket> getConnection(std::wstring connectionName);

    void createConnection(std::wstring connectionName, std::wstring address);
    void removeConnection(std::wstring connectionName);
};
}