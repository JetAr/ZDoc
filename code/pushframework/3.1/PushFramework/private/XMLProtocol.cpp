/********************************************************************
	File :			XMLProtocol.cpp
	Creation date :	2012/01/29

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
#include "XMLProtocol.h"


#include "XMLPacket.h"

namespace PushFramework
{

XMLProtocol::XMLProtocol(void)
{
}

XMLProtocol::~XMLProtocol(void)
{
}

EncodeResult::Type XMLProtocol::encodeContent( ProtocolContext& context, Buffer& inputBuffer, Buffer& outputBuffer )
{
	//Just append an end character:
	unsigned int requiredBytes = inputBuffer.getDataSize() + 1;
	if (requiredBytes < outputBuffer.getRemainingSize())
	{
		return EncodeResult::InsufficientBufferSpace;
	}
	outputBuffer.append(inputBuffer);
	char endChar = 0;
	outputBuffer.append(&endChar, 1);

	return EncodeResult::Success;
}

DecodeResult::Type XMLProtocol::tryDecode( ProtocolContext& context, Buffer& outputBuffer )
{
	Buffer& inputBuffer = context.getDataBuffer();

	for (unsigned int offset  = 0; offset < inputBuffer.getDataSize(); offset++)
	{
		char ch = inputBuffer.getAt(offset);
		if (ch=='\0')
		{
			return outputBuffer.append(inputBuffer.getBuffer(), offset+1) ? DecodeResult::Content : DecodeResult::Failure;
		}
	}
	return DecodeResult::WantMoreData;
}


ProtocolContext* XMLProtocol::createNewProtocolContext()
{
	return new ProtocolContext();
}

void XMLProtocol::startSession( ProtocolContext& context, Buffer& outgoingBytes )
{
	context.setInitialized();
}

bool XMLProtocol::readData( ProtocolContext& context, Buffer& incomingBytes )
{
	return context.getDataBuffer().append(incomingBytes);
}




}
