/********************************************************************
	File :			DemuxImpl_Win.h
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
#ifndef DEMUXIMPL_H_
#define DEMUXIMPL_H_

namespace PushFramework
{

class Demultiplexor;
class DemuxImpl
{
public:
    DemuxImpl(Demultiplexor* pFacade);
    virtual ~DemuxImpl();
    bool start();
    void stop();
private:
    Demultiplexor* pFacade;
private:
	vector<HANDLE> workersThreadsVect;
    HANDLE g_hShutdownEvent;
    static DWORD WINAPI KerIOCPWorkerProc (LPVOID WorkContext);
    void proc();
};

}
#endif /* DEMUXIMPL_H_ */
