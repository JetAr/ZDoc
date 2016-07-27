/********************************************************************
	File :			Overlap.h
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
#ifndef Overlap__INCLUDED
#define Overlap__INCLUDED

#pragma once



namespace PushFramework
{


enum IOType
{
    IOInitialize=1,
    IORead,
    IOWrite,
};


#ifdef Plateform_Windows
class OVERLAPPEDPLUS
{
public:
    OVERLAPPED			m_ol;
    IOType				m_ioType;

    OVERLAPPEDPLUS(IOType ioType)
    {
        ZeroMemory(this, sizeof(OVERLAPPEDPLUS));
        m_ioType = ioType;
    }
};
#endif

}
#endif // Overlap__INCLUDED
