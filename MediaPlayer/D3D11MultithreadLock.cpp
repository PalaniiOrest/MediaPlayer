#include "pch.h"
#include "D3D11MultithreadLock.h"

D3D11MultithreadLock::D3D11MultithreadLock(winrt::com_ptr<ID3D11Multithread> multithread)
    : m_multithread(std::move(multithread))
{

}

void D3D11MultithreadLock::lock()
{
    if (m_multithread) {
        m_multithread->Enter();
    }
}

void D3D11MultithreadLock::unlock()
{
    if (m_multithread) {
        m_multithread->Leave();
    }
}