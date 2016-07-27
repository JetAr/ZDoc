/********************************************************************
	File :			ListenerImpl_Win.h
	Creation date :	2012/01/29

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
#ifndef ListenerImpl_Win__INCLUDED
#define ListenerImpl_Win__INCLUDED


namespace PushFramework
{
class Listener;

class ListenerImpl
{
public:
    ListenerImpl(Listener* pFacade);
    virtual ~ListenerImpl();
    bool startListening();
    void stopListening();
private:
    Listener* pFacade;
private:
    SOCKET					hSocket; //socket handle.
    WSAEVENT				hSocketEvent;//used for WSAEventSelect.
    HANDLE					hThread;//Handle to the listening thread.
    static DWORD WINAPI threadProc(LPVOID lpVoid);
    void doListen();
    bool acceptConnectionRequest();
    HANDLE	hKillEvent;//kill event.

};

}

#endif // ListenerImpl_Win__INCLUDED
