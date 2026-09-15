#pragma once

#include "d3dUtil.h"


template<typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
        : mIsConstantBuffer(isConstantBuffer)
    {
        mElementByteSize = sizeof(T);

        // ConstantBufferの中身は256バイトの倍数である必要がある．
        if (isConstantBuffer)
            mElementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));

        
        ThrowIfFailed(
            device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&mUploadBuffer)
            )
        );

        mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData));
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ~UploadBuffer()
    {
        if (mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);
        
        mMappedData = nullptr;
    }

    ID3D12Resource* Resource() const
    {
        return mUploadBuffer.Get();
    }

    void CopyDate(int elementIndex, const T& data)
    {
        memcpy(&mMappedData[elementIndex * mElementByteSize], &data, mElementByteSize);
    }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};
