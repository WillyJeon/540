#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
class Transform
{
public:
	Transform();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	void MoveAbsolute(float x, float y, float z);
	//void MoveRelative(float x, float y, float z);
	void Rotate(float p, float y, float r);
	void Scale(float x, float y, float z);
private:
	DirectX::XMFLOAT3 translation;
	DirectX::XMFLOAT3 pitchYawRoll;
	DirectX::XMFLOAT3 scale;
	
	DirectX::XMFLOAT4X4 worldMatrix;

	void UpdateWorldMatrix();
};

