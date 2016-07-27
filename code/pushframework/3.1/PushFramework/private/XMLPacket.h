/*
 * File:   XMLPacket.h
 * Author: ahmed
 *
 * Created on 12 juin 2011, 19:32
 */

#ifndef XMLPACKET_H
#define	XMLPACKET_H

#include "xmlParser.h"


namespace AnalyticsProtocol
{

enum
{
    LoginRequest = 1,
    LoginResponse,

    ConsoleCommandRequest,
    ConsoleCommandResponse,

    LiveSubscriptionRequest,
    InitializationResponse,
    MeasuresResponse,
    VisitorInResponse,
    VisitorOutResponse,


    SessionsRequest,
    SessionsResponse,
    StatsRequest,
    StatsResponseBegin,
    StatsResponse,
    StatsResponseEnd,
    GeoStatsRequest,
    GeoStatsResponse,

    LogoutRequest,

};
}

namespace PushFramework
{
class XMLPacket : public IncomingPacket, public OutgoingPacket
{
public:
    XMLPacket(int typeId);
    XMLPacket();

    int getTypeId();

    XMLPacket(const std::string& data);
    virtual ~XMLPacket();

    XMLNode& getRoot();

    bool Encode();

    bool Decode(char* stringBuffer);

    const char* getArgumentAsText(const char* argName);
    int getArgumentAsInt(const char* argName);
    bool getArgumentAsBool(const char* argName);
    void setArgumentAsText(const char* argName, const char* val);
    void setArgumentAsInt(const char* argName, int val);
    void setArgumentAsBool(const char* argName, bool val);

    std::string& getData();

private:
    XMLNode xmlRoot;
    std::string data;
    bool isXmlFormat;
};

}

#endif	/* XMLPACKET_H */

