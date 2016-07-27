#include "StdAfx.h"
#include "IOBuffer.h"


namespace PushFramework
{


	IOBuffer::IOBuffer( IOType type )
		:RecyclableBuffer(RecyclableBuffer::Socket)
	{
		pOverlap = new OVERLAPPEDPLUS(type);
		m_wsaBuffer.buf = getBuffer(0);
		m_wsaBuffer.len = getCapacity();
	}

	IOBuffer::~IOBuffer( void )
	{
		delete pOverlap;
	}

}
