///////////////////////////////////////////////////////////////////////////////
//
// File           : $Workfile: IOBuffer.cpp $
// Version        : $Revision: 21 $
// Function       :
//
// Author         : $Author: Len $
// Date           : $Date: 13/08/02 9:46 $
//
// Notes          :
//
// Modifications  :
//
// $Log: /Web Articles/SocketServers/EchoServer/JetByteTools/Win32Tools/IOBuffer.cpp $
//
// 21    13/08/02 9:46 Len
// Throw an exception if we try to release a buffer that already has a 0
// reference count - not doing this can lead to very hard to find bugs...
//
// 20    9/08/02 13:04 Len
// Lint issues.
//
// 19    6/08/02 11:51 Len
// All calls to Output() are now wrapped in a DEBUG_ONLY() macro so they
// vanish completely from release builds.
//
// 18    22/07/02 18:09 Len
// Added GetBufferSize() to the allocator.
//
// 17    15/07/02 9:54 Len
// Support 0 byte reads.
//
// 16    9/07/02 18:31 Len
// Bug fixes and code cleaning.
//
// 15    6/07/02 13:49 Len
// Buffers now contain a sequence number.
// New nested helper class: InOrderBufferList. This provides a way of
// ensuring that buffers are processed in the order defined by their
// sequence numbers and provides a temporary store for buffers that arrive
// out of sequence.
//
// 14    7/06/02 14:14 Len
// We now derive from OVERLAPPED rather than contain an instance. This
// means we can remove the explicit conversion functions.
//
// 13    29/05/02 12:05 Len
// More lint issues.
//
// 12    29/05/02 11:16 Len
// Lint issues.
//
// 11    28/05/02 11:01 Len
// FromOverlapped() now returns 0 if the pointer passed in is 0.
//
// 10    28/05/02 10:02 Len
// Removed unrequired include
//
// 9     26/05/02 15:11 Len
// Factored out common 'user data' code into a mixin base class.
// Use NodeList for the list implementations.
//
// 8     21/05/02 11:35 Len
// User data can now be stored/retrieved as either an unsigned long or a
// void *.
//
// 7     20/05/02 23:17 Len
// Updated copyright and disclaimers.
//
// 6     20/05/02 8:07 Len
// Refactored to remove the concept of the io operation that the buffer is
// being used for and replace it with a more abstract 'user data' concept.
// The knowledge of what the buffer is used for is now in the SocketServer
// code and users are free to add their own user data if they use the
// buffers outside of the SocketServer.
// General code cleanup.
//
// 5     14/05/02 14:37 Len
// Lint cleanup.
//
// 4     14/05/02 13:49 Len
// Our optimised SplitData() method is inappropriate now that we're
// reference counting the buffers. We were allocating a new buffer and
// putting the extra data into it. We should have been putting the data we
// wanted to process into the new buffer and then moving the remaining
// data in the existing buffer to close the gap. The new code results in
// one more memory move.
//
// 3     13/05/02 13:45 Len
// Removed all knowledge of sockets from the iobuffer.
// Added the concept of 'max free buffers' to the allocator. This allows
// us to shrink the buffer pool rather than just growing it. IOBuffers are
// now reference counted.
//
// 2     10/05/02 19:25 Len
// Lint options and code cleaning.
//
// 1     9/05/02 18:47 Len
//
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1997 - 2002 JetByte Limited.
//
// JetByte Limited grants you ("Licensee") a non-exclusive, royalty free,
// licence to use, modify and redistribute this software in source and binary
// code form, provided that i) this copyright notice and licence appear on all
// copies of the software; and ii) Licensee does not utilize the software in a
// manner which is disparaging to JetByte Limited.
//
// This software is provided "as is" without a warranty of any kind. All
// express or implied conditions, representations and warranties, including
// any implied warranty of merchantability, fitness for a particular purpose
// or non-infringement, are hereby excluded. JetByte Limited and its licensors
// shall not be liable for any damages suffered by licensee as a result of
// using, modifying or distributing the software or its derivatives. In no
// event will JetByte Limited be liable for any lost revenue, profit or data,
// or for direct, indirect, special, consequential, incidental or punitive
// damages, however caused and regardless of the theory of liability, arising
// out of the use of or inability to use software, even if JetByte Limited
// has been advised of the possibility of such damages.
//
// This software is not designed or intended for use in on-line control of
// aircraft, air traffic, aircraft navigation or aircraft communications; or in
// the design, construction, operation or maintenance of any nuclear
// facility. Licensee represents and warrants that it will not use or
// redistribute the Software for such purposes.
//
///////////////////////////////////////////////////////////////////////////////

#include "IOBuffer.h"

#include "Exception.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Exposing low access member
//lint -esym(1536, CIOBuffer::m_overlapped)
//lint -esym(1536, CIOBuffer::m_wsabuf)
//
// Default constructor implicitly called
//lint -esym(1926, Allocator::m_criticalSection)
//lint -esym(1926, Allocator::m_freeList)
//lint -esym(1926, Allocator::m_activeList)
//lint -esym(1926, InOrderBufferList::m_list)
//
// Symbol did not appear in the constructor initializer list
//lint -esym(1928, Node)
//lint -esym(1928, COpaqueUserData)
//
// Member not defined
//lint -esym(1526, CIOBuffer::CIOBuffer)
//lint -esym(1526, CIOBuffer::operator=)
//lint -esym(1526, Allocator::Allocator)
//lint -esym(1526, Allocator::operator=)
//
//lint -esym(534, InterlockedIncrement)   ignoring return value
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools
{
namespace Win32
{

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer
///////////////////////////////////////////////////////////////////////////////

CIOBuffer::CIOBuffer(
    Allocator &allocator,
    size_t size)
    :  m_operation(0),
       m_sequenceNumber(0),
       m_allocator(allocator),
       m_ref(1),
       m_size(size),
       m_used(0)
{
    memset(this, 0, sizeof(OVERLAPPED));

    Empty();
}

WSABUF *CIOBuffer::GetWSABUF() const
{
    return const_cast<WSABUF*>(&m_wsabuf);
}

size_t CIOBuffer::GetUsed() const
{
    return m_used;
}

size_t CIOBuffer::GetSize() const
{
    return m_size;
}

const BYTE *CIOBuffer::GetBuffer() const
{
    return &m_buffer[0];
}

void CIOBuffer::Empty()
{
    m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsabuf.len = m_size;

    m_used = 0;
}

void *CIOBuffer::operator new(size_t objectSize, size_t bufferSize)
{
    void *pMem = new char[objectSize + bufferSize];

    return pMem;
}

void CIOBuffer::operator delete(void *pObject, size_t /* bufferSize*/)
{
    delete [] pObject;
}

void CIOBuffer::Use(
    size_t dataUsed)
{
    m_used += dataUsed;
}

CIOBuffer *CIOBuffer::SplitBuffer(
    size_t bytesToRemove)
{
    CIOBuffer *pNewBuffer = m_allocator.Allocate();

    pNewBuffer->AddData(m_buffer, bytesToRemove);

    m_used -= bytesToRemove;

    memmove(m_buffer, m_buffer + bytesToRemove, m_used);

    return pNewBuffer;
}

CIOBuffer *CIOBuffer::AllocateNewBuffer() const
{
    return m_allocator.Allocate();
}

void CIOBuffer::ConsumeAndRemove(
    size_t bytesToRemove)
{
    m_used -= bytesToRemove;

    memmove(m_buffer, m_buffer + bytesToRemove, m_used);
}

void CIOBuffer::SetupZeroByteRead()
{
    m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsabuf.len = 0;
}

void CIOBuffer::SetupRead()
{
    if (m_used == 0)
    {
        m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
        m_wsabuf.len = m_size;
    }
    else
    {
        m_wsabuf.buf = reinterpret_cast<char*>(m_buffer) + m_used;
        m_wsabuf.len = m_size - m_used;
    }
}

void CIOBuffer::SetupWrite()
{
    m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
    m_wsabuf.len = m_used;

    m_used = 0;
}

void CIOBuffer::AddData(
    const char * const pData,
    size_t dataLength)
{
    if (dataLength > m_size - m_used)
    {
        throw CException(_T("CIOBuffer::AddData"), _T("Not enough space in buffer"));
    }

    memcpy(m_buffer + m_used, pData, dataLength);

    m_used += dataLength;
}

void CIOBuffer::AddData(
    const BYTE * const pData,
    size_t dataLength)
{
    AddData(reinterpret_cast<const char*>(pData), dataLength);
}

void CIOBuffer::AddData(
    BYTE data)
{
    AddData(&data, 1);
}

//z 计数
void CIOBuffer::AddRef()
{
    ::InterlockedIncrement(&m_ref);
}

void CIOBuffer::Release()
{
    if (m_ref == 0)
    {
        // Error! double release
        throw CException(_T("CIOBuffer::Release()"), _T("m_ref is already 0"));
    }

    if (0 == ::InterlockedDecrement(&m_ref))
    {
        m_sequenceNumber = 0;
        m_operation = 0;
        m_used = 0;

        m_allocator.Release(this);
    }
}

size_t CIOBuffer::GetOperation() const
{
    return m_operation;
}

void CIOBuffer::SetOperation(
    size_t operation)
{
    m_operation = operation;
}

size_t CIOBuffer::GetSequenceNumber() const
{
    return m_sequenceNumber;
}

void CIOBuffer::SetSequenceNumber(
    size_t sequenceNumber)
{
    m_sequenceNumber = sequenceNumber;
}

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::Allocator
///////////////////////////////////////////////////////////////////////////////

CIOBuffer::Allocator::Allocator(
    size_t bufferSize,
    size_t maxFreeBuffers)
    :  m_bufferSize(bufferSize),
       m_maxFreeBuffers(maxFreeBuffers)
{
    // TODO share this code with the socket pool
}

CIOBuffer::Allocator::~Allocator()
{
    try
    {
        Flush();
    }
    catch(...)
    {
    }
}

CIOBuffer *CIOBuffer::Allocator::Allocate()
{
    CCriticalSection::Owner lock(m_criticalSection);

    CIOBuffer *pBuffer = 0;

    if (!m_freeList.Empty())
    {
		//z 从 freelist 中获取一个 node
        pBuffer = m_freeList.PopNode();

		pBuffer->AddRef();
    }
    else
    {
        pBuffer = new(m_bufferSize)CIOBuffer(*this, m_bufferSize);

        if (!pBuffer)
        {
            throw CException(_T("CIOBuffer::Allocator::Allocate()"),_T("Out of memory"));
        }

        //lint -e{1933} call to unqualified virtual function
        OnBufferCreated();
    }

    m_activeList.PushNode(pBuffer);

    //lint -e{1933} call to unqualified virtual function
    OnBufferAllocated();

    return pBuffer;
}

void CIOBuffer::Allocator::Release(
    CIOBuffer *pBuffer)
{
    if (!pBuffer)
    {
        throw CException(_T("CIOBuffer::Allocator::Release()"), _T("pBuffer is null"));
    }

    CCriticalSection::Owner lock(m_criticalSection);

    //lint -e{1933} call to unqualified virtual function
    OnBufferReleased();

    // unlink from the in use list

    pBuffer->RemoveFromList();

    if (m_maxFreeBuffers == 0 ||
            m_freeList.Count() < m_maxFreeBuffers)
    {
        pBuffer->Empty();

        // add to the free list

        m_freeList.PushNode(pBuffer);
    }
    else
    {
        DestroyBuffer(pBuffer);
    }
}

void CIOBuffer::Allocator::Flush()
{
    CCriticalSection::Owner lock(m_criticalSection);

    while (!m_activeList.Empty())
    {
        //lint -e{1933} call to unqualified virtual function
        OnBufferReleased();

        DestroyBuffer(m_activeList.PopNode());
    }

    while (!m_freeList.Empty())
    {
        DestroyBuffer(m_freeList.PopNode());
    }
}

void CIOBuffer::Allocator::DestroyBuffer(CIOBuffer *pBuffer)
{
    delete pBuffer;

    //lint -e{1933} call to unqualified virtual function
    OnBufferDestroyed();
}

size_t CIOBuffer::Allocator::GetBufferSize() const
{
    return m_bufferSize;
}

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::InOrderBufferList
///////////////////////////////////////////////////////////////////////////////

CIOBuffer::InOrderBufferList::InOrderBufferList(
    CCriticalSection &criticalSection)
    :  m_next(0),
       m_criticalSection(criticalSection)
{
}

void CIOBuffer::InOrderBufferList::AddBuffer(
    CIOBuffer *pBuffer)
{
    CCriticalSection::Owner lock(m_criticalSection);

    std::pair<BufferSequence::iterator, bool> result = m_list.insert(BufferSequence::value_type(pBuffer->GetSequenceNumber(), pBuffer));

    if (!result.second)
    {
        DEBUG_ONLY(Output(_T("UNEXPECTED, element already in map!")));
    }
}

CIOBuffer *CIOBuffer::InOrderBufferList::ProcessAndGetNext()
{
    CCriticalSection::Owner lock(m_criticalSection);

    m_next++;

    CIOBuffer *pNext = 0;

    BufferSequence::iterator it;

    it = m_list.begin();

    if (it != m_list.end())
    {
        if (it->first == m_next)
        {
            pNext = it->second;

            //lint -e{534} Ignoring return value of function
            m_list.erase(it);
        }
        else
        {
            DEBUG_ONLY(Output(ToString(this) + _T(" PAGN Got buffer  : ") + ToString(it->first) + _T("Want buffer : ") + ToString(m_next)));
        }
    }

    return pNext;
}

CIOBuffer *CIOBuffer::InOrderBufferList::GetNext()
{
    CCriticalSection::Owner lock(m_criticalSection);

    CIOBuffer *pNext = 0;

    BufferSequence::iterator it;

    it = m_list.begin();

    if (it != m_list.end())
    {
        if (it->first == m_next)
        {
            pNext = it->second;

            //lint -e{534} Ignoring return value of function
            m_list.erase(it);
        }
        else
        {
            DEBUG_ONLY(Output(ToString(this) + _T(" GN0 Got buffer  : ") + ToString(it->first) + _T("Want buffer : ") + ToString(m_next)));
        }
    }

    return pNext;
}

// TODO resolve reference counting issues, the list should take a reference...

CIOBuffer *CIOBuffer::InOrderBufferList::GetNext(
    CIOBuffer *pBuffer)
{
    CCriticalSection::Owner lock(m_criticalSection);

    if (m_next == pBuffer->GetSequenceNumber())
    {
        return pBuffer;
    }

    std::pair<BufferSequence::iterator, bool> result = m_list.insert(BufferSequence::value_type(pBuffer->GetSequenceNumber(), pBuffer));

    if (!result.second)
    {
        DEBUG_ONLY(Output(_T("UNEXPECTED, element already in map!")));
    }

    CIOBuffer *pNext = 0;

    BufferSequence::iterator it;

    it = m_list.begin();

    if (it != m_list.end())
    {
        if (it->first == m_next)
        {
            pNext = it->second;

            //lint -e{534} Ignoring return value of function
            m_list.erase(it);
        }
        else
        {
            DEBUG_ONLY(Output(ToString(this) + _T(" GN1 Got buffer  : ") + ToString(it->first) + _T("Want buffer : ") + ToString(m_next)));
        }
    }

    return pNext;
}

void CIOBuffer::InOrderBufferList::ProcessBuffer()
{
    CCriticalSection::Owner lock(m_criticalSection);

    DEBUG_ONLY(Output(ToString(this) + _T(" Processed : ") + ToString(m_next)));

    m_next++;
}

void CIOBuffer::InOrderBufferList::Reset()
{
    m_next = 0;

    if (!m_list.empty())
    {
        DEBUG_ONLY(Output(_T("List not empty when reset !")));
        // worry!

        // TODO throw?
    }
}

bool CIOBuffer::InOrderBufferList::Empty() const
{
    return m_list.empty();
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Win32
} // End of namespace JetByteTools

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////
