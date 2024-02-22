#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <d3d11.h>

struct BufferStruct{
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 view;

};