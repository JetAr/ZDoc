/********************************************************************
	File :			ServerStats.h
	Creation date :	2010/6/27

	License :			Copyright 2010 Ahmed Charfeddine, http://www.pushframework.com

				   Licensed under the Apache License, Version 2.0 (the "License");
				   you may not use this file except in compliance with the License.
				   You may obtain a copy of the License at

					   http://www.apache.org/licenses/LICENSE-2.0

				   Unless required by applicable law or agreed to in writing, software
				   distributed under the License is distributed on an "AS IS" BASIS,
				   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
				   See the License for the specific language governing permissions and
				   limitations under the License.


*********************************************************************/
#ifndef ServerStats__INCLUDED
#define ServerStats__INCLUDED

#pragma once


namespace PushFramework
{


class ServerImpl;
class Measure;

class ServerStats
{
public:
    ServerStats();
    ~ServerStats();
    typedef enum Measures
    {
        VisitorsOnline = 1, // Inc at Reactor::ProcessFirstPacket (when client is new) Dec at ClientFactoryImpl::disposeClient
        VisitorsHitsIn,//Inc at Reactor::ProcessFirstPacket (whether client is new or not)
        VisitorsHitsOut, // ClientFactoryImpl::disposeClient
        VisitorsSYNs,	// inc at CAcceptor::handleAcceptedSocket
        VisitorsDuration, //inc duration at CClientFactoryImpl::disposeClient
        VisitorsBounce,
        BandwidthInbound,//CReactor::OnReceiveComplete
        BandwidthOutbound,//CReactor::OnWriteComplete
        BandwidthRejection,//CChannel::SendData
        BandwidthOutstanding,//CChannel::SendData
        BandwidthInboundVolPerRequest, //CReactor::dispatchRequest
        BandwidthOutboundVolPerRequest,	//CChannel::SendData
        BandwidthInboundPerConnection, //CReactor::OnReceiveComplete
        BandwidthOutboundPerConnection, //CReactor::OnWriteComplete
        PerformanceRequestVolPerRequest, //CReactor::dispatchRequest
        PerformanceProcessingTime,	//CReactor::dispatchRequest
        PerformanceProcessingTimePerService,	//CReactor::dispatchRequest
        QoSFillRatePerChannel,
        QoSSendRatePerChannel,
        QoSAvgSendRatePerChannel,

    } Measures;


public:
    void init();
    //
    void addToCumul(unsigned int measureId, double value);
    void addToDuration(unsigned int measureId, double value);
    void addToDistribution(unsigned int measureId, std::string serviceName, double value);
    void addToKeyedDuration(unsigned int measureId, int key, double value);
    void addToKeyedDistributionDuration(unsigned int measureId, std::string segmentName,std::string key, double value);


    OutgoingPacket* getPerformancePacket();
    OutgoingPacket* getInitializationPacket();
private:
    typedef std::map<unsigned int, Measure*> measuresMapT;
    measuresMapT measuresMap;

    int packetCounter;

    CRITICAL_SECTION cs;

};
extern ServerStats stats;
}

#endif // ServerStats__INCLUDED
