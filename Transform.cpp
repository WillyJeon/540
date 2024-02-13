#include "Transform.h"

using namespace DirectX;

Transform::Transform():
	translation(0, 0, 0),
	pitchYawRoll(0, 0, 0),
	scale(1,1,1)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return DirectX::XMFLOAT4X4();
}

void Transform::SetPosition(float x, float y, float z)
{
	translation.x = x;
	translation.y = y;
	translation.z = z;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	translation.x += x;
	translation.y += y;
	translation.z += z;
}

//void Transform::MoveRelative(float x, float y, float z)
//{
//	translation.x += x;
//	translation.y += y;
//	translation.z += z;
//}

void Transform::Rotate(float p, float y, float r)
{
	pitchYawRoll.x += p;
	pitchYawRoll.y += y;
	pitchYawRoll.z += r;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

void Transform::UpdateWorldMatrix()
{
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&translation));
	XMMATRIX r = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMMATRIX s = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
	

	XMMATRIX worldMat = s * r * t;
	XMStoreFloat4x4(&worldMatrix, worldMat);
}
