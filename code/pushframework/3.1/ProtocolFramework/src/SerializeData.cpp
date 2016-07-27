#include "stdafx.h"
#include "SerializeData.h"

namespace ProtocolFramework
{
	SerializeData::SerializeData(Protocol* pProtocol)
	{
		this->nEncodingBytes = 0;
		this->nSerializationBytes = 0;
		this->pProtocol = pProtocol;
	}


	SerializeData::~SerializeData(void)
	{
	}

}
