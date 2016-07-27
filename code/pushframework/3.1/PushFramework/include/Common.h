/********************************************************************
	File :			global.h
	Creation date :	2012/02/01

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
#ifndef global__INCLUDED
#define global__INCLUDED

#pragma once

#include "PushFramework.h"

namespace PushFramework
{

	namespace SendResult
	{
		enum Type
		{
			OK,
			Retry,
			NotOK
		};
	}

	namespace DisconnectionReason
	{
		enum Type
		{
			PeerClosure =0 ,
			InactiveClient,
			ForceableClosure,
			RequestedClosure,
			UnknownFailure,
		};
	}

	namespace Login
	{
		enum Type
		{
			RefuseAndWait = 0,
			RefuseAndClose,
			AcceptClient,
			AcceptClientAndRouteRequest
		};

		bool IsSucceeded(Type type);
	}

	class ConnectionContext;
	/*PUSHFRAMEWORK_API*/ struct LoginData
	{
		LoginData();
		~LoginData();
		IncomingPacket* pRequest;
		OutgoingPacket* pResponse;
		ConnectionContext* connectionContext;
		char clientKey[256];
	};

}


#endif // global__INCLUDED
