#ifndef Buffer__INCLUDED
#define Buffer__INCLUDED

#pragma once

#include "ProtocolFramework.h"


namespace ProtocolFramework
{
	// This class holds a line of memory and keeps track of its data. 
	class PROTOCOLFRAMEWORK_API Buffer
	{
	public:
		Buffer(void);

		virtual ~Buffer(void);

		void allocate( unsigned int _size );

		void assign(char* _buf, unsigned int _maxSize, unsigned int size = 0);

		void assign(Buffer& srcBuffer);

		void assign(Buffer& srcBuffer, unsigned int size);

		unsigned int getRemainingSize();

		bool append(const char* _buf, unsigned int _size);

		bool append(const char c);

		unsigned int getDataSize();

		unsigned int getCapacity();

		char* getBuffer(int offset = 0);

		char* getPosition();

		char getAt(int offset);

		void growSize(unsigned int growBy);

		void pop(unsigned int _size);

		bool append(Buffer& srcBuffer);

		void clearBytes();

		bool isEmpty();

		bool hasBytes();

		void setPosition(unsigned int size);

		bool isFull();

	protected:
		char* buf;
		unsigned int size;
		unsigned int maxSize;
		bool ownsBuffer;
	};

}
#endif // Buffer__INCLUDED
