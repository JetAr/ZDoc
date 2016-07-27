/********************************************************************
	File :			Protocol.h
	Creation date :	3/5/2014

	License :			Copyright 2010-2014 Ahmed Charfeddine, http://www.pushframework.com

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
#ifndef Protocol__INCLUDED
#define Protocol__INCLUDED

#pragma once
#include "ProtocolFramework.h"
#include "ErrorCodes.h"
#include "AbstractPool.h"

namespace ProtocolFramework
{
	class ProtocolContext;
	class Buffer;

	class PROTOCOLFRAMEWORK_API Protocol : public AbstractPool
	{

	public:	
		Protocol(void);
		virtual ~Protocol(void);

		//
		void addLowerProtocolLayer(Protocol* pProtocol);
	
		//
		virtual ProtocolContext* createNewProtocolContext();
		virtual void startSession(ProtocolContext& context, Buffer& outgoingBytes) = 0;
		virtual bool readData(ProtocolContext& context, Buffer& incomingBytes) = 0;
		virtual DecodeResult::Type tryDecode(ProtocolContext& context, Buffer& outputBuffer) = 0;
		virtual EncodeResult::Type encodeContent(ProtocolContext& context, Buffer& inputBuffer, Buffer& outputBuffer) = 0;
		virtual unsigned int getRequiredOutputSize(unsigned int maxInputSize);
		virtual unsigned int getRequiredRecyclableBuffers(unsigned int nMaxConnections, unsigned int nMaxConcurrentCalls);

	public:
		Protocol* getLowerProtocol();
		Protocol* getUpperProtocol();
		Protocol* getLowestProtocol();

	private:
		Protocol *pUpperProtocol, *pLowerProtocol; 
		virtual void deleteImpl(ObjectPtr obj);
		virtual ObjectPtr createImpl();
		virtual void recycleObject(ObjectPtr p);

	};
}
#endif // Protocol__INCLUDED