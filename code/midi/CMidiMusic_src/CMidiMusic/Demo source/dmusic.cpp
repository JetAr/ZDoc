/*
Module : dmusic.cpp
Purpose: Defines the code implementation for the CMidiMusic class
Created: CJP / 17-02-2001
History: CJP / 21-10-2001

	1. Fixed problems related to midi files and others formats

	2. Fixed bugs with MIDI port selection

	3. Fixed many problems discovered with the demo aplication

	CJP / 28-12-2010

	1. Fixed bug when using DirectMusic performance

	2. Solved memory leaks

Copyright (c) 2001 by C. Jiménez de Parga
All rights reserved.
*/

#include "stdafx.h"
#include "dmusic.h"



// The constructor, member variables initialisation

CMidiMusic::CMidiMusic()
{
    m_pMusic			= NULL;
    m_pMusic8			= NULL;
    m_pLoader			= NULL;
    m_pPerformance		= NULL;
    m_pSegment			= NULL;
    m_pMusicPort		= NULL;
    m_pSegmentState		= NULL;
    m_pSegmentState8	= NULL;
    m_p3DAudioPath		= NULL;
    m_pDSB				= NULL;
    // Initialize COM
    CoInitialize(NULL);
}


// Intialisation of midi music class.
// Creates instances of the most important interfaces

HRESULT CMidiMusic::Initialize(BOOL b3DPosition)
{
    HRESULT hr;


    if (FAILED(hr=CoCreateInstance(CLSID_DirectMusicLoader,NULL,
                                   CLSCTX_INPROC,IID_IDirectMusicLoader8,
                                   (void**)&m_pLoader))) return hr;

    if (FAILED(hr=CoCreateInstance(CLSID_DirectMusicPerformance,NULL,
                                   CLSCTX_INPROC,IID_IDirectMusicPerformance8,
                                   (void**)&m_pPerformance))) return hr;


    if (b3DPosition)
    {

        // Initialize audio for 3D environment with the appropriate performance
        // function and the standard audiopath
        if (FAILED(hr=m_pPerformance->InitAudio(&m_pMusic,NULL,NULL,
                                                DMUS_APATH_DYNAMIC_STEREO,
                                                64,DMUS_AUDIOF_ALL,NULL))) return hr;

        if (FAILED(hr=m_pPerformance->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D,
                      64,TRUE,&m_p3DAudioPath))) return hr;

        // Creates the 3D directsound buffer
        if (FAILED(hr=m_p3DAudioPath->GetObjectInPath(DMUS_PCHANNEL_ALL,
                      DMUS_PATH_BUFFER,0,GUID_NULL,0,
                      IID_IDirectSound3DBuffer,
                      (LPVOID*) &m_pDSB))) return hr;
    }
    else
        // Initialisation for a not 3D environment
        if (FAILED(hr=m_pPerformance->Init(&m_pMusic,NULL,NULL))) return hr;


    // Invoke the new interface pointer for IDirectMusic8
    if (FAILED(hr=m_pMusic->QueryInterface(IID_IDirectMusic8,(LPVOID*)&m_pMusic8))) return hr;
    // Release the old interface, not needed any more
    SAFE_RELEASE(m_pMusic);

    m_b3DPosition=b3DPosition;

    return S_OK;
}


// Port enumeration function

HRESULT CMidiMusic::PortEnumeration(DWORD dwIndex,LPINFOPORT lpInfoPort)
{
    HRESULT hr;
    DMUS_PORTCAPS portinf;

    // Set to 0 the DMUS_PORTCAPS structure
    ZeroMemory(&portinf,sizeof(portinf));
    portinf.dwSize=sizeof(DMUS_PORTCAPS);

    //Call the DirectMusic8 member function to enumerate systems ports
    if (FAILED(hr=m_pMusic8->EnumPort(dwIndex,&portinf))) return hr;


    // Converts port description to char string
    WideCharToMultiByte(CP_ACP,0,portinf.wszDescription,-1,
                        lpInfoPort->szPortDescription,
                        sizeof(lpInfoPort->szPortDescription)/
                        sizeof(lpInfoPort->szPortDescription[0]),0,0);


    // Copy the GUID of DMUS_PORTCAP structure to CMidiMusic port structure
    CopyMemory(&(lpInfoPort->guidSynthGUID),&portinf.guidPort,sizeof(GUID));

    lpInfoPort->dwClass					=portinf.dwClass;
    lpInfoPort->dwEffectFlags			=portinf.dwEffectFlags;
    lpInfoPort->dwFlags	 				=portinf.dwFlags;
    lpInfoPort->dwMaxAudioChannels	    =portinf.dwMaxAudioChannels;
    lpInfoPort->dwMaxChannelGroups		=portinf.dwMaxChannelGroups;
    lpInfoPort->dwMaxVoices				=portinf.dwMaxVoices;
    lpInfoPort->dwType					=portinf.dwType;

    return hr;
}


// Function to select a MIDI port

HRESULT CMidiMusic::SelectPort(LPINFOPORT InfoPort)
{
    HRESULT hr;
    DMUS_PORTPARAMS dmus;

    // Sets to 0 the port capabilities structure
    ZeroMemory(&dmus,sizeof(DMUS_PORTPARAMS));
    // Sets the params for this port
    dmus.dwSize=sizeof(DMUS_PORTPARAMS);
    dmus.dwValidParams=DMUS_PORTPARAMS_CHANNELGROUPS;
    dmus.dwChannelGroups=1;

    // If the port allows an audiopath
    if (InfoPort->dwFlags & DMUS_PC_AUDIOPATH)
        dmus.dwFeatures=DMUS_PORT_FEATURE_AUDIOPATH;


    // The midi port is created here
    if (FAILED(hr=m_pMusic8->CreatePort(InfoPort->guidSynthGUID,&dmus,
                                        &m_pMusicPort,NULL))) return hr;

    // We have to activate it
    if (FAILED(hr=m_pMusicPort->Activate(TRUE))) return hr;


    // In case we aren't in a 3D mode
    if (!m_b3DPosition)
    {
        // Add the port to the performance
        if (FAILED(hr=m_pPerformance->AddPort(m_pMusicPort))) return hr;
        // Assigns a block of 16 performance channels to the performance
        if (FAILED(hr=m_pPerformance->AssignPChannelBlock(0,m_pMusicPort,1)))
            return hr;
    }

    return S_OK;
}

// Function to load a midi, segment or wave file into a segment

HRESULT CMidiMusic::LoadMidiFromFile(LPCSTR szMidi)
{
    WCHAR wstrMidi[256];
    HRESULT hr;

    // If exists a segment before, then release it
    if (m_pSegment)
        SAFE_RELEASE(m_pSegment);

    // Converts ANSI (8-bits) to the UNICODE (16-bit) string
    MultiByteToWideChar(CP_ACP,0,szMidi,-1,wstrMidi,256);

    // Then load it into the segment
    if (FAILED(hr=m_pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment,
                  IID_IDirectMusicSegment8,wstrMidi,
                  (LPVOID*) &m_pSegment))) return hr;


    // In case it is a midi file mark it is a standard midi file

    if(FAILED(hr=m_pSegment->SetParam(GUID_StandardMIDIFile,
                                      0xFFFFFFFF,0,0,NULL)))
        return hr;


    // Finally, download band data to the performance
    m_pSegment->Download(m_pPerformance);

    return S_OK;
}

// Function to load MIDI or WAVE file from a resource
HRESULT CMidiMusic::LoadMidiFromResource(TCHAR *strResource,TCHAR *strResourceType)
{
    HRESULT hr;
    HRSRC hres=NULL;
    void* pMem=NULL;
    DWORD dwSize=0;
    DMUS_OBJECTDESC objdesc;


    SAFE_RELEASE(m_pSegment);

    // Find the resource
    hres=FindResource(NULL,strResource,strResourceType);
    if(hres==NULL)
        return E_FAIL;

    // Load the resource
    pMem=(void*)LoadResource(NULL,hres);
    if(NULL==pMem)
        return E_FAIL;

    // Store the size of the resource
    dwSize=SizeofResource(NULL,hres);

    // Set up our object description
    ZeroMemory(&objdesc,sizeof(DMUS_OBJECTDESC));
    objdesc.dwSize=sizeof(DMUS_OBJECTDESC);
    objdesc.dwValidData=DMUS_OBJ_MEMORY | DMUS_OBJ_CLASS;
    objdesc.guidClass=CLSID_DirectMusicSegment;
    objdesc.llMemLength=(LONGLONG)dwSize;
    objdesc.pbMemData=(BYTE*)pMem;

    if (FAILED(hr=m_pLoader->GetObject(&objdesc,
                                       IID_IDirectMusicSegment8,
                                       (void**)&m_pSegment)))
    {
        if(hr==DMUS_E_LOADER_FAILEDOPEN) return hr;
    }


    // Specify a standard midi file
    if(FAILED(hr=m_pSegment->SetParam(GUID_StandardMIDIFile,
                                      0xFFFFFFFF,0,0,NULL)))
        return hr;



    // Download band data to the performance

    m_pSegment->Download(m_pPerformance);

    return S_OK;
}

// Function to play a segment

HRESULT CMidiMusic::Play()
{
    HRESULT hr;

    // Plays a segment and stores the segment state
    if (FAILED(hr=m_pPerformance->PlaySegmentEx(
                      m_pSegment,
                      NULL,
                      NULL,
                      0,
                      0,
                      &m_pSegmentState,
                      NULL,
                      m_p3DAudioPath
                  ))) return hr;


    // Gets the new interface for SegmentState
    if (FAILED(hr=m_pSegmentState->QueryInterface(IID_IDirectMusicSegmentState8,
                  (LPVOID*)&m_pSegmentState8))) return hr;


    // Release the references to SegmentState
    SAFE_RELEASE(m_pSegmentState);

    return hr;
}

// Functions to pause a segment

HRESULT CMidiMusic::Pause()
{
    MUSIC_TIME          mtNow;
    HRESULT hr;

    // Stops the segment playback
    if (FAILED(hr=m_pPerformance->Stop(
                      NULL,
                      NULL,
                      0,
                      0))) return hr;

    // Get the position before the pause
    if (FAILED(hr=m_pSegmentState8->GetSeek(&mtNow))) return hr;

    // Set restart point
    if (FAILED(hr=m_pSegment->SetStartPoint(mtNow)))  return hr;

    return S_OK;

}

// Function to resume after the pause, any sound must be listened

HRESULT CMidiMusic::Resume()
{
    HRESULT hr;

    if (IsPlaying()==S_FALSE)
    {
        hr=Play();
        return hr;
    }
    return S_FALSE;
}


// Funtion to find out when the music is actually played

HRESULT CMidiMusic::IsPlaying()
{
    HRESULT hr;

    if (FAILED(hr=m_pPerformance->IsPlaying(m_pSegment,NULL))) return hr;

    return hr;

}


// Function to repeat a segment

HRESULT CMidiMusic::SetRepeat(BOOL bRepeat)
{
    HRESULT hr;

    if (bRepeat==TRUE)
        hr=m_pSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
    else
        hr=m_pSegment->SetRepeats(0);

    return hr;
}


// Function to stop a segment

HRESULT CMidiMusic::Stop()
{
    HRESULT hr;

    if (FAILED(hr=m_pSegment->SetStartPoint(0))) return hr;

    if (FAILED(hr=m_pPerformance->Stop(NULL,NULL,0,0))) return hr;

    return S_FALSE;
}

// Function to get the length in ticks of a segment

HRESULT CMidiMusic::GetLength(MUSIC_TIME *mtMusicTime)
{
    HRESULT hr;

    if (FAILED(hr=m_pSegment->GetLength(mtMusicTime))) return hr;

    return hr;

}

// Function to get the playback position of a segment

HRESULT CMidiMusic::GetSeek(MUSIC_TIME *mtMusicTime)
{
    HRESULT hr;

    if (FAILED(hr=m_pSegmentState8->GetSeek(mtMusicTime))) return hr;

    return hr;

}


// Function to move the pointer to a specific position

HRESULT CMidiMusic::Seek(MUSIC_TIME mtMusicTime)
{
    HRESULT hr;

    if (FAILED(hr=m_pSegment->SetStartPoint(mtMusicTime))) return hr;

    return S_OK;

}



// This function does the same of GetSeek

HRESULT CMidiMusic::GetTicks(MUSIC_TIME *mtMusicTime)
{
    HRESULT hr;

    hr=GetSeek(mtMusicTime);

    return hr;
}


//Function to get the reference time

HRESULT CMidiMusic::GetReferenceTime(REFERENCE_TIME *rtReferenceTime)
{

    MUSIC_TIME mtMusicTime;

    m_pSegmentState8->GetStartPoint(&mtMusicTime);

    m_pPerformance->MusicToReferenceTime(mtMusicTime,rtReferenceTime);

    return S_OK;
}



// Function to set the 3D audio position
// DS3D_IMMEDIATE set the parameters immediately

HRESULT CMidiMusic::SetPosition(D3DVALUE x,D3DVALUE y,D3DVALUE z)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetPosition(x,y,z,DS3D_IMMEDIATE);
    return hr;
}



// Function to get the 3D audio position

HRESULT CMidiMusic::GetPosition(D3DVALUE *x,D3DVALUE *y,D3DVALUE *z)
{
    HRESULT hr;
    D3DVECTOR d3dVect;
    hr=S_FALSE;
    if (m_b3DPosition)
    {
        hr=m_pDSB->GetPosition(&d3dVect);
        if (x) *x=d3dVect.x;
        if (y) *y=d3dVect.y;
        if (z) *z=d3dVect.z;
    }

    return hr;
}


// Function to set the audio velocity

HRESULT CMidiMusic::SetVelocity(D3DVALUE x,D3DVALUE y,D3DVALUE z)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetVelocity(x,y,z,DS3D_IMMEDIATE);
    return hr;
}


// Function to set the 3D audio mode

HRESULT CMidiMusic::SetMode(DWORD dwMode)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetMode(dwMode,DS3D_IMMEDIATE);
    return hr;
}

// Function to set the maximum distance of audio range

HRESULT CMidiMusic::SetMaxDistance(D3DVALUE flMaxDistance)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetMaxDistance(flMaxDistance,DS3D_IMMEDIATE);
    return hr;
}


// Function set the minimum distance of audio range

HRESULT CMidiMusic::SetMinDistance(D3DVALUE flMinDistance)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetMinDistance(flMinDistance,DS3D_IMMEDIATE);
    return hr;
}


//Function to set the audio cone orientation

HRESULT CMidiMusic::SetConeOrientation(D3DVALUE x,D3DVALUE y,D3DVALUE z)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetConeOrientation(x,y,z,DS3D_IMMEDIATE);
    return hr;
}


//Function to get the audio cone orientation

HRESULT CMidiMusic::GetConeOrientation(D3DVALUE *x,D3DVALUE *y,D3DVALUE *z)
{
    HRESULT hr;
    D3DVECTOR d3dVect;
    hr=S_FALSE;
    if (m_b3DPosition)
    {
        hr=m_pDSB->GetConeOrientation(&d3dVect);
        if (x) *x=d3dVect.x;
        if (y) *y=d3dVect.y;
        if (z) *z=d3dVect.z;
    }
    return hr;
}

// Function to set the audio cone angles

HRESULT CMidiMusic::SetConeAngles(DWORD dwInsideConeAngle,DWORD dwOutsideConeAngle)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetConeAngles(dwInsideConeAngle,dwOutsideConeAngle,DS3D_IMMEDIATE);
    return hr;
}

// Function to get the audio cone angles

HRESULT CMidiMusic::GetConeAngles(LPDWORD dwInsideConeAngle,LPDWORD dwOutsideConeAngle)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->GetConeAngles(dwInsideConeAngle,dwOutsideConeAngle);
    return hr;
}


// Function to set the cone outside volume

HRESULT CMidiMusic::SetConeOutsideVolume(LONG lConeOutsideVolume)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->SetConeOutsideVolume(lConeOutsideVolume,DS3D_IMMEDIATE);
    return hr;
}

// Function to retrieve the cone outside volume

HRESULT CMidiMusic::GetConeOutsideVolume(LPLONG plConeOutsideVolume)
{
    HRESULT hr;
    hr=S_FALSE;
    if (m_b3DPosition)
        hr=m_pDSB->GetConeOutsideVolume(plConeOutsideVolume);
    return hr;
}


// Function to set the master tempo

HRESULT CMidiMusic::SetMasterTempo(float fTempo)
{
    HRESULT hr;

    hr=m_pPerformance->SetGlobalParam(GUID_PerfMasterTempo,
                                      (void*)&fTempo, sizeof(float));
    return hr;
}


// Function to set the audio master volume (only in software synth. mode)
HRESULT CMidiMusic::SetMasterVolume(long nVolume)
{
    HRESULT hr;

    hr=m_pPerformance->SetGlobalParam(GUID_PerfMasterVolume,
                                      (void*)&nVolume,sizeof(long));

    return hr;
}

// Function to set audio effects

HRESULT CMidiMusic::SetEffect(BOOL bActivate,int nEffect)
{
    IKsControl   *pControl;
    KSPROPERTY   ksp;
    DWORD        dwEffects=0;
    ULONG        cb;
    HRESULT      hr;

    // Get the IKsControl interface
    hr=m_pMusicPort->QueryInterface(IID_IKsControl,(void**)&pControl);

    if (SUCCEEDED(hr))
    {
        ZeroMemory(&ksp, sizeof(ksp));


        if (!bActivate)
            dwEffects=0;
        else
        {
            // Select an audio effect
            switch(nEffect)
            {
            case 0x1:
                dwEffects=DMUS_EFFECT_REVERB;
                break;
            case 0x2:
                dwEffects=DMUS_EFFECT_CHORUS;
                break;
            case 0x3:
                dwEffects=DMUS_EFFECT_CHORUS | DMUS_EFFECT_REVERB;
                break;
            }
            //Possible error here if you don't have /Ze compiler option set
        }

        ksp.Set=GUID_DMUS_PROP_Effects;
        ksp.Id=0;
        ksp.Flags=KSPROPERTY_TYPE_SET;


        pControl->KsProperty(&ksp,
                             sizeof(ksp),
                             (LPVOID)&dwEffects,
                             sizeof(dwEffects),
                             &cb);

        SAFE_RELEASE(pControl);

    }

    return hr;

}

// The class destructor
// Release all the interfaces
// Remove ports and closedown the performance
// Uninitialize COM

CMidiMusic::~CMidiMusic()
{
    if (m_pLoader)
    {
        m_pLoader->ReleaseObjectByUnknown(m_pSegment);
        SAFE_RELEASE(m_pLoader);
    }

    SAFE_RELEASE(m_pSegmentState);
    SAFE_RELEASE(m_pSegmentState8);
    SAFE_RELEASE(m_p3DAudioPath);
    SAFE_RELEASE(m_pDSB);
    SAFE_RELEASE(m_pSegment);
    SAFE_RELEASE(m_pMusic);
    SAFE_RELEASE(m_pMusic8);

    if (m_pPerformance)
    {
        m_pPerformance->RemovePort(m_pMusicPort);
        m_pPerformance->CloseDown();
        SAFE_RELEASE(m_pMusicPort);
    }

    SAFE_RELEASE(m_pMusicPort);

    CoUninitialize();
}
