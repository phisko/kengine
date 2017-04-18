#include <iomanip>
#include <iostream>
#include <sstream>
#include <regex>
#include "ProtocolModule.hpp"

#include "chop.hpp"

static bool readLine(std::istream &s, std::string &ret)
{
    if (!s || s.peek() == -1)
        return false;

    ret.clear();

    char c;
    while (s && s.peek() != -1)
    {
        c = (char)s.get();
        if (c == '\r' && s.peek() == '\n')
        {
            s.get();
            return true;
        }
        ret.append(1, c);
    }

    return true;
}

void ProtocolModule::handle(const kia::packets::IncomingMessage &packet) const noexcept
{
    /*
     Request       = Request-Line              ; Section 5.1
                        *(( general-header        ; Section 4.5
                         | request-header         ; Section 5.3
                         | entity-header ) CRLF)  ; Section 7.1
                        CRLF
                        [ message-body ]          ; Section 4.3
     */

    kia::packets::HttpRequest request;

    request.clientFd = packet.clientFd;

    std::stringstream stream(packet.msg);
    std::string line;

    // First line indicates method, request-URI and HTTP version
    if (!readLine(stream, line))
    {
        send(std::move(request));
        return;
    }

    getRequestLine(request, line);

    // Get header fields into the HttpRequest
    while (readLine(stream, line) && putils::chop(line).length())
        addHeader(request, line);

    // Rest of message is the body
    while (readLine(stream, line))
    {
        getParams(request, line);
        request.body += line + "\n";
    }

    request.body = putils::chop(request.body);

    send(std::move(request));
}

void ProtocolModule::getParams(kia::packets::HttpRequest &request, const std::string &line) const noexcept
{
    static const std::regex reg("^(.*)=(.*)$");

    std::smatch m;
    if (std::regex_match(line, m, reg))
        request.params[m[1]] = m[2];
}

// Blatantly stolen from StackOverflow
static std::string urlDecode(const std::string &sSrc)
{
    static const char HEX2DEC[256] =
            {
                    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
                    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,

                    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

                    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

                    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
                    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
            };

    // Note from RFC1630: "Sequences which start with a percent
    // sign but are not followed by two hexadecimal characters
    // (0-9, A-F) are reserved for future extension"

    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    const unsigned char * const SRC_END = pSrc + SRC_LEN;
    // last decodable '%'
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;

    char * const pStart = new char[SRC_LEN];
    char * pEnd = pStart;

    while (pSrc < SRC_LAST_DEC)
    {
        if (*pSrc == '%')
        {
            char dec1, dec2;
            if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
            {
                *pEnd++ = (dec1 << 4) + dec2;
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = *pSrc++;
    }

    // the last 2- chars
    while (pSrc < SRC_END)
        *pEnd++ = *pSrc++;

    std::string sResult(pStart, pEnd);
    delete [] pStart;
    return sResult;
}

void ProtocolModule::getRequestLine(kia::packets::HttpRequest &request, const std::string &line) const noexcept
{
    // From RFC 2616: Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
    static const std::regex reg("^([^\\s]+)\\s+([^\\s]+)\\s+([^\\s]+)$");

    std::smatch m;
    if (std::regex_match(line, m, reg) == true)
    {
        request.method = m[1];
        request.uri = urlDecode(m[2]);
        request.httpVersion = m[3];
    }
}

void ProtocolModule::addHeader(kia::packets::HttpRequest &request, const std::string &line) const noexcept
{
    // message-header = field-name ":" [ field-value ]
    static const std::regex reg("^([^\\:]*)\\: (.*)$");

    std::smatch m;
    if (std::regex_match(line, m, reg) == true)
    {
        auto key = m[1];
        auto value = m[2];

        request.headers.emplace(key, value);
    }
}

void ProtocolModule::handle(const kia::packets::HttpResponse &response) const noexcept
{
    /*
      Response      = Status-Line               ; Section 6.1
                       *(( general-header        ; Section 4.5
                        | response-header        ; Section 6.2
                        | entity-header ) CRLF)  ; Section 7.1
                       CRLF
                       [ message-body ]          ; Section 7.2
     */

    std::stringstream   output;

    // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
    output << response.httpVersion << " "
           << response.statusCode << " "
           << response.reasonPhrase << "\r\n";

    // message-header = field-name ":" [ field-value ]
    for (auto &pair : response.headers)
        output << pair.first << ":" << pair.second << "\r\n";
    output << "\r\n";

    output << response.body;
    output << "\r\n";

    send(kia::packets::OutgoingMessage{ response.clientFd, output.str() });
}
