#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <windows.h>
#include <wrl.h> 
#include <string>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3DCompiler.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include<cassert>
#include <unordered_map>

#include "d3dx12.h"


extern const int gNUmFrameResources;

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}



class d3dUtil {
	
public:
	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// CBは，最小ハードウェア割り当てサイズ(通常は256バイト)の倍数である必要がある．
		// 255を足して，下位2バイト(<256領域)をマスクして最も近い256の倍数に切り上げる．
		return (byteSize + 255) & ~255;
	}

};



class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

	std::wstring ToString() const;

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};

// [SubGeometry]
// 複数のジオメトリをひとつの VertexBuffer / IndexBuffer に保存するための SubGeometry．
// Sphere, Cube, Cylinder など異なるジオメトリをひとつの大きなバッファで表現できる
struct SubmeshGeometry
{

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
	
	DirectX::BoundingBox Bounds;
	
};

// [meshGeometry]
struct MeshGeometry
{
	
	std::string Name;

	// システムメモリのコピー?．Blobを使う． vertex/index バッファはジェネリックになるため．
	// 正しい型を指定するのは利用者の責任．
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	// データについての情報
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// MeshGeometryには複数のジオメトリ情報がまとめて入っている．
	// このコンテナを使って SubmeshGeometry  を個別に描画できるうにする．
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexBufferByteSize;
		vbv.SizeInBytes = VertexBufferByteSize;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;
		return ibv;
	}

	// GPUへのアップロードが終わったらリリースできる．
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)														\
{																				\
	HRESULT hr__ = (x);															\
	std::wstring wfn = AnsiToWString(__FILE__);									\
	if (FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); }			\
}																				
#endif

