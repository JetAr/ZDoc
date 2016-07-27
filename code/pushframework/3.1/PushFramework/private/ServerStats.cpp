/********************************************************************
	File :			ServerStats.cpp
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
#include "StdAfx.h"
#include "ServerStats.h"

#include "../include/ServerOptions.h"

#include "Measure.h"
#include "DistributionMeasure.h"
#include "CumulativeMeasure.h"
#include "DurationMeasure.h"
#include "KeyedAveragedMeasure.h"
#include "KeyedAveragedDistributionMeasure.h"

#include "ServerImpl.h"
#include "XMLPacket.h"
#include "Dispatcher.h"
#include "BroadcastManager.h"
#include "ScopedLock.h"
#include "Utilities.h"


namespace PushFramework
{

ServerStats stats;

ServerStats::ServerStats()
{
    packetCounter = 0;
    ::InitializeCriticalSection(&cs);
    init();
}

ServerStats::~ServerStats(void)
{
	for (measuresMapT::iterator it = measuresMap.begin();
		it != measuresMap.end();
		it ++)
	{
		delete it->second;
	}
    ::DeleteCriticalSection(&cs);
}

void ServerStats::init()
{
    //Create measures.

    //Visitors :
    measuresMap[VisitorsOnline] = new CumulMeasure("online", true);

    measuresMap[VisitorsHitsIn] = new CumulMeasure("hitsIn");
    measuresMap[VisitorsHitsOut] = new CumulMeasure("hitsOut");
    measuresMap[VisitorsSYNs] = new CumulMeasure("syn");

    measuresMap[VisitorsDuration] = new DurationMeasure("vstDur");

    measuresMap[VisitorsBounce] = new CumulMeasure("bounce");

    //Bandwidth :
    measuresMap[BandwidthInbound] = new CumulMeasure("inbound");
    measuresMap[BandwidthOutbound] = new CumulMeasure("outbound");
    measuresMap[BandwidthRejection] = new CumulMeasure("rejection");
    measuresMap[BandwidthOutstanding] = new CumulMeasure("outstanding");

    measuresMap[BandwidthInboundVolPerRequest] = new DistributionMeasure("inbound_by_service");
    measuresMap[BandwidthOutboundVolPerRequest] = new DistributionMeasure("outbound_by_service");

    measuresMap[BandwidthInboundPerConnection] = new KeyedAveragedMeasure("inboundc");
    measuresMap[BandwidthOutboundPerConnection] = new KeyedAveragedMeasure("outboundc");

    //Performance :
    measuresMap[PerformanceRequestVolPerRequest] = new DistributionMeasure("request_by_service");
    measuresMap[PerformanceProcessingTime] = new DurationMeasure("processing");
    measuresMap[PerformanceProcessingTimePerService] = new DistributionMeasure("processing_by_service");

    //Qos Broadcast :
    measuresMap[QoSFillRatePerChannel] = new DistributionMeasure("fillrate_by_queue");
    measuresMap[QoSSendRatePerChannel] = new DistributionMeasure("sendrate_by_queue");
    measuresMap[QoSAvgSendRatePerChannel] = new KeyedAveragedDistributionMeasure("avgsendrate_by_queue");

}

void ServerStats::addToCumul( unsigned int measureId, double value )
{
    if (!options.isProfilingEnabled)
        return;

    ScopedLock lock(cs);

    CumulMeasureArgs args;
    args.dwValue = value;

    measuresMap[measureId]->addObservation(args);
}

void ServerStats::addToDuration( unsigned int measureId, double value )
{
    if (!options.isProfilingEnabled)
        return;

    ScopedLock lock(cs);

    DurationMeasureArgs args;
    args.duration = value;

    measuresMap[measureId]->addObservation(args);
}

void ServerStats::addToDistribution( unsigned int measureId, std::string serviceName, double value )
{
    if (!options.isProfilingEnabled)
        return;

    ScopedLock lock(cs);

    DistributionMeasureArgs args;
    args.serviceName = serviceName;
    args.value = value;

    measuresMap[measureId]->addObservation(args);
}

OutgoingPacket* ServerStats::getPerformancePacket()
{

    ScopedLock lock(cs);

    packetCounter++;


    std::string timestamp  = Utilities::getCurrentTime();

    // = "22:00::23";

    std::stringstream ss;

    ss << std::noskipws;

    //Write header :
    ss << "<root typeId=\"";
    ss << AnalyticsProtocol::MeasuresResponse;
    ss << "\">";
    ss << "<stats>";
    ss << "<id val=\"" << packetCounter << "\"/>";
    ss << "<timestamp val=\"" << timestamp << "\"/>";

    //Visitors :
    ss << measuresMap[VisitorsOnline]->collectAndReset(timestamp);
    ss << measuresMap[VisitorsHitsIn]->collectAndReset(timestamp);
    ss << measuresMap[VisitorsHitsOut]->collectAndReset(timestamp);
    ss << measuresMap[VisitorsSYNs]->collectAndReset(timestamp);
    ss << measuresMap[VisitorsDuration]->collectAndReset(timestamp);
    //ss << "<visitorlist><visitor><time value=\"23:00\"/><ip value=\"192.168.1.1\"/><port value=\"1209\"/></visitor></visitorlist>";
    ss << measuresMap[VisitorsBounce]->collectAndReset(timestamp);

    //Bandwidth :
    ss << measuresMap[BandwidthInbound]->collectAndReset(timestamp);
    ss << measuresMap[BandwidthOutbound]->collectAndReset(timestamp);
    ss << measuresMap[BandwidthRejection]->collectAndReset(timestamp);
    ss << measuresMap[BandwidthOutstanding]->collectAndReset(timestamp);
    ss << measuresMap[BandwidthInboundVolPerRequest]->collectAndReset(timestamp);
    ss << measuresMap[BandwidthOutboundVolPerRequest]->collectAndReset(timestamp);
    ss << measuresMap[BandwidthInboundPerConnection]->collectAndReset(timestamp);
    ss << measuresMap[BandwidthOutboundPerConnection]->collectAndReset(timestamp);

    //Performance :
    ss << measuresMap[PerformanceRequestVolPerRequest]->collectAndReset(timestamp);
    ss << measuresMap[PerformanceProcessingTime]->collectAndReset(timestamp);
    ss << measuresMap[PerformanceProcessingTimePerService]->collectAndReset(timestamp);


    //QoS Broadcast :
    ss << measuresMap[QoSFillRatePerChannel]->collectAndReset(timestamp);
    ss << measuresMap[QoSSendRatePerChannel]->collectAndReset(timestamp);
    ss << measuresMap[QoSAvgSendRatePerChannel]->collectAndReset(timestamp);

    ss << "</stats>";
    ss << "</root>";

    std::string data = ss.str();

    return new XMLPacket(data);
}

void ServerStats::addToKeyedDuration(unsigned int measureId, int key, double value )
{
    if (!options.isProfilingEnabled)
        return;

    ScopedLock lock(cs);
    //
    MKAveragedMeasureArgs args;
    args.dwValue = value;
    args.key = key;

    measuresMap[measureId]->addObservation(args);
}

void ServerStats::addToKeyedDistributionDuration( unsigned int measureId, std::string segmentName,std::string key, double value )
{
    if (!options.isProfilingEnabled)
        return;

    ScopedLock lock(cs);
    //
    KeyedAveragedDistributionMeasureArgs args;
    args.key = key;
    args.serviceName = segmentName;
    args.value = value;

    measuresMap[measureId]->addObservation(args);
}

OutgoingPacket* ServerStats::getInitializationPacket()
{
    ScopedLock lock(cs);

    std::stringstream ss;
    ss << std::noskipws;

    //Write header :
    ss << "<root typeId=\"";
    ss << AnalyticsProtocol::InitializationResponse;
    ss << "\">";
    ss << "<init>";

    ss << "<sampling val=\"" << options.samplingRate << "\"/>";
    ss << "<services>";


    ss << dispatcher.getServiceNames();

    ss << "</services>";



    ss << "<queues>";


    ss << broadcastManager.getQueuesNames();

    ss << "</queues>";


    ss << "</init>";
    ss << "</root>";


    std::string data = ss.str();
    return new XMLPacket(data);
}



}
