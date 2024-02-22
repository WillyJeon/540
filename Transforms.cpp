#include "Transforms.h"
using namespace DirectX;

Transforms::Transforms() :
	translation(0, 0, 0),
	pitchYawRoll(0, 0, 0),
	scale(1, 1, 1)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
}

DirectX::XMFLOAT3 Transforms::GetPosition()
{
	return translation;
}

DirectX::XMFLOAT3 Transforms::GetPitchYawRoll()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transforms::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transforms::GetWorldMatrix()
{
	UpdateWorldMatrix();
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transforms::GetWorldInverseTransposeMatrix()
{
	UpdateWorldMatrix();
	return worldInverseTransposeMatrix;
}

DirectX::XMFLOAT3 Transforms::GetRight()
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&pitchYawRoll));
	DirectX::XMVECTOR rot = DirectX::XMVector3Rotate(DirectX::XMVectorSet(1, 0, 0, 0), quat);
	//XMStoreFloat3(&translation, XMLoadFloat3(&translation) + dir);
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, rot);
	return dir;
}

DirectX::XMFLOAT3 Transforms::GetUp()
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&pitchYawRoll));
	DirectX::XMVECTOR rot = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 1, 0, 0), quat);
	//XMStoreFloat3(&translation, XMLoadFloat3(&translation) + dir);
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, rot);
	return dir;
}

DirectX::XMFLOAT3 Transforms::GetForward()
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&pitchYawRoll));
	DirectX::XMVECTOR rot = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, 1, 0), quat);
	//XMStoreFloat3(&translation, XMLoadFloat3(&translation) + dir);
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, rot);
	return dir;
}

void Transforms::SetPosition(float x, float y, float z)
{
	translation.x = x;
	translation.y = y;
	translation.z = z;
}

void Transforms::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
}

void Transforms::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Transforms::MoveAbsolute(float x, float y, float z)
{
	translation.x += x;
	translation.y += y;
	translation.z += z;
}

void Transforms::MoveRelative(float x, float y, float z)
{
	DirectX::XMVECTOR move = XMVectorSet(x, y, z, 0);
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&pitchYawRoll));
	DirectX::XMVECTOR dir = DirectX::XMVector3Rotate(move, quat);
	XMStoreFloat3(&translation, XMLoadFloat3(&translation) + dir);
}

void Transforms::Rotate(float p, float y, float r)
{
	pitchYawRoll.x += p;
	pitchYawRoll.y += y;
	pitchYawRoll.z += r;
}

void Transforms::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

void Transforms::Scale(DirectX::XMFLOAT3 scale)
{
	scale = scale;
}

void Transforms::UpdateWorldMatrix()
{
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&translation));
	XMMATRIX r = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMMATRIX s = XMMatrixScalingFromVector(XMLoadFloat3(&scale));


	XMMATRIX worldMat = s * r * t;
	XMStoreFloat4x4(&worldMatrix, worldMat);
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(worldMat)));
}