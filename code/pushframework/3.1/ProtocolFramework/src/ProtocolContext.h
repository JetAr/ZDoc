#ifndef ProtocolContext__INCLUDED
#define ProtocolContext__INCLUDED

#pragma once
#include "ProtocolFramework.h"
#include "RecyclableBuffer.h"

namespace ProtocolFramework
{
	class PROTOCOLFRAMEWORK_API ProtocolContext
	{
	public:
		ProtocolContext(void);
		virtual ~ProtocolContext(void);
		
		Buffer& getDataBuffer();
		bool isInitialized();
		bool isInitializationStarted();
		void setInitializationStarted();
		void setInitialized();
		
		void recycleIntern();
	private:
		RecyclableBuffer receivedData;
		short state;

	protected:
		virtual void recycle();
	};

}
#endif // ProtocolContext__INCLUDED