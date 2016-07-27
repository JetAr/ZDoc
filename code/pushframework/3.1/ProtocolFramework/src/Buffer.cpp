#include "stdafx.h"
#include "Buffer.h"

namespace ProtocolFramework
{

	Buffer::Buffer( void )
	{
		buf = NULL;
		maxSize = 0;
		size = 0;
		ownsBuffer = false;
	}

	Buffer::~Buffer( void )
	{
		if (ownsBuffer)
		{
			delete [] buf;
			buf = NULL;
		}
	}

	void Buffer::assign( char* _buf, unsigned int _maxSize, unsigned int _size /*= 0*/ )
	{
		buf = _buf;
		size = _size;
		maxSize = _maxSize;
		ownsBuffer = false;
	}

	void Buffer::assign( Buffer& srcBuffer )
	{
		assign(srcBuffer.getBuffer(), srcBuffer.getCapacity(), srcBuffer.getDataSize());
	}

	void Buffer::assign( Buffer& srcBuffer, unsigned int size )
	{
		assign(srcBuffer.getBuffer(), srcBuffer.getCapacity(), size);
	}

	unsigned int Buffer::getRemainingSize()
	{
		return maxSize - size;
	}

	bool Buffer::append( const char* _buf, unsigned int _size )
	{
		if(_size == 0)
			return true;

		if (getRemainingSize() < _size)
			return false;

		::memcpy(buf + size, _buf, _size);

		size += _size;
		return true;
	}

	bool Buffer::append( Buffer& srcBuffer )
	{
		return append(srcBuffer.getBuffer(), srcBuffer.getDataSize());
	}

	bool Buffer::append( const char c )
	{
		return append(&c, 1);
	}

	unsigned int Buffer::getDataSize()
	{
		return size;
	}

	char* Buffer::getBuffer(int offset/* = 0*/)
	{
		return buf+offset;
	}

	char Buffer::getAt( int offset )
	{
		return buf[offset];
	}

	void Buffer::growSize( unsigned int growBy )
	{
		size += growBy;
	}

	void Buffer::pop( unsigned int _size )
	{
		if (size < _size)
			return;

		size -= _size;
		::memmove(buf, buf + _size, size);
	}

	void Buffer::allocate( unsigned int _size )
	{
		buf = new char[_size];
		maxSize = _size;
		ownsBuffer = true;
	}

	unsigned int Buffer::getCapacity()
	{
		return maxSize;
	}

	char* Buffer::getPosition()
	{
		return buf + size;
	}

	void Buffer::clearBytes()
	{
		size = 0;
	}

	bool Buffer::isEmpty()
	{
		return size == 0;
	}

	bool Buffer::hasBytes()
	{
		return size > 0;
	}

	void Buffer::setPosition( unsigned int size )
	{
		this->size = size;
	}

	bool Buffer::isFull()
	{
		return size == maxSize;
	}

}


