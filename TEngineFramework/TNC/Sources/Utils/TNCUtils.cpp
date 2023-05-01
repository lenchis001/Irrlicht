#include "TNCUtils.h"

#include <algorithm>

namespace TUtils {
Uri TUtils::Uri::Parse(const std::wstring& uri)
{
    Uri result;

    typedef std::wstring::const_iterator iterator_t;

    if (uri.length() == 0)
        return result;

    iterator_t uriEnd = uri.end();

    // get query start
    iterator_t queryStart = std::find(uri.begin(), uriEnd, L'?');

    // protocol
    iterator_t protocolStart = uri.begin();
    iterator_t protocolEnd = std::find(protocolStart, uriEnd, L':'); //"://");

    if (protocolEnd != uriEnd) {
        std::wstring prot = &*(protocolEnd);
        if ((prot.length() > 3) && (prot.substr(0, 3) == L"://")) {
            result.Protocol = std::wstring(protocolStart, protocolEnd);
            protocolEnd += 3; //      ://
        } else
            protocolEnd = uri.begin(); // no protocol
    } else
        protocolEnd = uri.begin(); // no protocol

    // host
    iterator_t hostStart = protocolEnd;
    iterator_t pathStart = std::find(hostStart, uriEnd, L'/'); // get pathStart

    iterator_t hostEnd = std::find(protocolEnd,
        (pathStart != uriEnd) ? pathStart : queryStart,
        L':'); // check for port

    result.Host = std::wstring(hostStart, hostEnd);

    // port
    if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == L':')) // we have a port
    {
        hostEnd++;
        iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
        result.Port = std::wstring(hostEnd, portEnd);
    }
    if (result.Port.empty())
        result.Port = L"80";

    // path
    if (pathStart != uriEnd) {
        result.Path = std::wstring(pathStart, queryStart);
    }
    if (result.Path.empty())
        result.Path = L"/";

    // query
    if (queryStart != uriEnd)
        result.QueryString = std::wstring(queryStart, uri.end());

    return result;

} // Parse

}