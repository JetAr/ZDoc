#include "stdafx.h"
#include "ProtocolManager.h"

#include "Connection.h"
#include "DeserializeData.h"
#include "SerializeData.h"
#include "MessageFactory.h"
#include "Protocol.h"
#include "ProtocolContext.h"

namespace ProtocolFramework
{

	ProtocolManager::ProtocolManager(void)
	{
	}

	ProtocolManager::~ProtocolManager(void)
	{
	}


	NetworkSerializeResult::Type ProtocolManager::serializeOutgoinBytes( Connection& connection, Buffer& outgoingBytes, Buffer& outputBuffer, SerializeData& serializeData )
	{
		RecyclableBuffer tmpBuffer;

		Buffer *pInput = &outgoingBytes;
		Buffer *pOutput = &tmpBuffer;

		Protocol* pProtocol = serializeData.pProtocol;
		if (!pProtocol)
		{
			return outputBuffer.append(outgoingBytes) ? NetworkSerializeResult::Success : NetworkSerializeResult::Retry;
		}

		while (pProtocol)
		{
			Buffer& out = pProtocol->getLowerProtocol() ? (*pOutput) : outputBuffer;
			EncodeResult::Type encodeResult = pProtocol->encodeContent(connection.getProtocolContext(pProtocol), *pInput, out);
			if (encodeResult != EncodeResult::Success)
			{
				if (encodeResult == EncodeResult::InsufficientBufferSpace && (pProtocol->getLowerProtocol() != NULL))
					return NetworkSerializeResult::Failure; //Buffer overflow at an intermediate protocol layer is not allowable.

				return NetworkSerializeResult::convertEncodingFailure(encodeResult);
			}

			pProtocol = pProtocol->getLowerProtocol();

			Buffer* p = pInput;
			pInput = pOutput;
			pOutput = p;

			pOutput->clearBytes();
		}

		return NetworkSerializeResult::Success;
	}

	NetworkSerializeResult::Type ProtocolManager::serializeOutgoingPacket( Connection& connection, OutgoingPacket& outgoingMsg, Buffer& outputBuffer, SerializeData& serializeData )
	{
		MessageFactory& msgFactory = connection.getMessageFactory();

		// First encode the message:
		RecyclableBuffer packetBuffer;
		SerializeResult::Type ret = msgFactory.serializeMessage(outgoingMsg, packetBuffer);
		if (ret != SerializeResult::Success)
			return NetworkSerializeResult::convertSerializeFailure(ret);

		return serializeOutgoinBytes(connection, packetBuffer, outputBuffer, serializeData);
	}

	NetworkDeserializeResult::Type ProtocolManager::tryDeserializeIncomingPacket( Connection& connection, DeserializeData& deserializeData )
	{
		MessageFactory& msgFactory = connection.getMessageFactory();
		RecyclableBuffer tmpOutputBuffer;
		
		while (true)
		{
			Protocol* pCurrentProtocol = deserializeData.pProtocol;
			ProtocolContext& currentContext = connection.getProtocolContext(pCurrentProtocol);
			Protocol* pUpperProtocol = pCurrentProtocol->getUpperProtocol();

			Buffer& outputBuffer = pUpperProtocol ? tmpOutputBuffer : deserializeData.protocolBytes;

			//Give chance to protocol to call startSession before processing incoming data.
			if (!currentContext.isInitialized() && !currentContext.isInitializationStarted())
			{
				currentContext.setInitializationStarted();
				pCurrentProtocol->startSession(currentContext, deserializeData.protocolBytes);
				//
				if (deserializeData.protocolBytes.hasBytes())
				{
					return NetworkDeserializeResult::ProtocolBytes;
				}
			}
			
			DecodeResult::Type decodeResult = pCurrentProtocol->tryDecode(currentContext, outputBuffer);

			if (decodeResult == DecodeResult::WantMoreData)
			{
				if (pUpperProtocol)
				{
					deserializeData.pProtocol = pUpperProtocol;
					continue;
				}
				else
				{
					return NetworkDeserializeResult::WantMoreData;
				}				
			}
			else if (decodeResult == DecodeResult::NoContent)
			{
				continue;
			}
			else if (decodeResult == DecodeResult::ProtocolBytes)
			{
				if (pUpperProtocol)
				{
					deserializeData.protocolBytes.append(tmpOutputBuffer);
				}
				
				//
				return NetworkDeserializeResult::ProtocolBytes;
			}
			else if(decodeResult == DecodeResult::Close)
			{
				if (pUpperProtocol)
				{
					deserializeData.protocolBytes.append(tmpOutputBuffer);
				}

				return NetworkDeserializeResult::Close;
			}
			else if (decodeResult == DecodeResult::Content)
			{
				if (!currentContext.isInitialized())
				{
					return NetworkDeserializeResult::Initializationfailure;
				}

				if (!pUpperProtocol)
				{
					//De-serialize and return message.
					DeserializeResult::Type deserializeResult = msgFactory.deserializeMessage(outputBuffer, deserializeData.pMessage, deserializeData.nRoutingService);
					if (deserializeResult == DeserializeResult::DiscardContent)
					{
						continue;
					}
					else if (deserializeResult == DeserializeResult::Failure)
					{
						return NetworkDeserializeResult::Failure;
					}
					else if(deserializeResult == DeserializeResult::Success)
					{
						return NetworkDeserializeResult::Content;
					}
					else
					{
						//Should not come here.
					}
				}

				//Give content data to upper protocol:
				if (!pUpperProtocol->readData(connection.getProtocolContext(pUpperProtocol), outputBuffer))
				{
					return NetworkDeserializeResult::Failure;
				}
			}
			else //decodeResult == DecodeResult::Failure
			{
				return NetworkDeserializeResult::Failure; //
			}
		}
	}

	bool ProtocolManager::processIncomingData( Connection& connection, Buffer& incomingBytes )
	{
		Protocol* pLowestProtocol = connection.getProtocol()->getLowestProtocol();

		return pLowestProtocol->readData(connection.getProtocolContext(pLowestProtocol), incomingBytes);
	}

}
