#include "StdAfx.h"
#include "../include/Common.h"


bool PushFramework::Login::IsSucceeded( Type type )
{
	return type == AcceptClientAndRouteRequest || type == AcceptClient;
}
PushFramework::LoginData::LoginData()
{
	pRequest = NULL;
	pResponse = NULL;
	connectionContext = NULL;
}

PushFramework::LoginData::~LoginData()
{
	//
}
