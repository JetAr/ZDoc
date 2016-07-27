/********************************************************************
	File :			IOQueue_Linux.h
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
#ifndef IOQueue_Linux__INCLUDED
#define IOQueue_Linux__INCLUDED


namespace PushFramework
{

class IOQueue
{
public:
    IOQueue();
    ~IOQueue(void);
    bool create();
    void free();
    int getQueuedEvent(epoll_event *pEvents, int nMaxEvents, bool isInputEvents);
    bool addSocketContext(SOCKET socket,void* lpObject);
    void deleteSocketContext(SOCKET socket);
    //
    bool rearmSocketForRead(SOCKET socket,void* lpObject);
    bool rearmSocketforWrite(SOCKET socket,void* lpObject);
private:
    int inPollQueue;
    int outPollQueue;

};
extern IOQueue ioQueue;
}

#endif // IOQueue_Linux__INCLUDED
