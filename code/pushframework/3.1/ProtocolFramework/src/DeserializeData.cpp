#include "stdafx.h"
#include "DeserializeData.h"

namespace ProtocolFramework
{
	//

	DeserializeData::DeserializeData( Protocol* pProtocol )
	{
		this->pProtocol = pProtocol;
		clear();
	}

	DeserializeData::~DeserializeData( void )
	{
		//
	}

	void DeserializeData::clear()
	{
		protocolBytes.clearBytes();
		pMessage = NULL;
		nRoutingService = 0;
	}

}
