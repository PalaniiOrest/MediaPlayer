#pragma once
#include <d3d11.h>
#include <mutex>

class D3D11MultithreadLock {
public:
    explicit D3D11MultithreadLock(winrt::com_ptr<ID3D11Multithread> multithread);

    void lock();

    void unlock();

private:
    winrt::com_ptr<ID3D11Multithread> m_multithread;
};
