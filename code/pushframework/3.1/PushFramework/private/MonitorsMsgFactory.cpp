#include "StdAfx.h"
#include "MonitorsMsgFactory.h"
#include "XMLPacket.h"

namespace PushFramework
{
	MonitorsMsgFactory theMonitorsMsgFactory;

	MonitorsMsgFactory::MonitorsMsgFactory(void)
	{
	}

	MonitorsMsgFactory::~MonitorsMsgFactory(void)
	{
	}

	void MonitorsMsgFactory::disposeOutgoingPacket( OutgoingPacket* pPacket )
	{
		XMLPacket* pXMLMessage = (XMLPacket*) pPacket;
		delete pXMLMessage;
	}

	void MonitorsMsgFactory::disposeIncomingPacket( IncomingPacket* pPacket )
	{
		XMLPacket* pXMLMessage = (XMLPacket*) pPacket;
		delete pXMLMessage;
	}

	SerializeResult::Type MonitorsMsgFactory::serializeMessage( OutgoingPacket& outgoingPacket, Buffer& buffer )
	{
		XMLPacket& response = (XMLPacket&) outgoingPacket;
		if (!response.Encode())
		{
			return SerializeResult::Failure;
		}

		if (!buffer.append(response.getData().c_str(), response.getData().length()))
		{
			return SerializeResult::InsufficientBufferSpace;
		}

		return  SerializeResult::Success;
	}

	bool MonitorsMsgFactory::preSerializeMessage( OutgoingPacket& outgoingPacket )
	{
		XMLPacket& response = (XMLPacket&) outgoingPacket;
		return response.Encode();
	}

	DeserializeResult::Type MonitorsMsgFactory::deserializeMessage( Buffer& contentBytes, IncomingPacket*& pMessage, int& nRoutingService )
	{
		XMLPacket* pRequest = new XMLPacket();
		if (!pRequest->Decode(contentBytes.getBuffer()))
		{
			delete pRequest;
			return DeserializeResult::Failure;
		}

		pMessage = dynamic_cast<IncomingPacket*> (pRequest);
		nRoutingService = pRequest->getTypeId();
		return DeserializeResult::Success;
	}

}