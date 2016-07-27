/********************************************************************
	File :			Listener.h
	Creation date :	2010/6/27

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
#ifndef Listener__INCLUDED
#define Listener__INCLUDED

#pragma once

#include "../include/ListenerOptions.h"

namespace PushFramework
{

class ListenerImpl;
class Listener
{
    friend class ListenerImpl;
public:
    Listener();
    virtual ~Listener(void);
    void setListeningPort(unsigned int uPort);
    bool startListening();
    void stopListening();
	void setOptions(ListenerOptions& listenerOptions);
	Protocol* getProtocol();
private:
    ListenerImpl* pImpl;
    unsigned int uPort;
protected:
    ListenerOptions listenerOptions;
    virtual bool handleAcceptedSocket(SOCKET clientSocket, SOCKADDR_IN address) = 0;

};

}

#endif // Listener__INCLUDED
