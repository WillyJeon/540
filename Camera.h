#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include "Transforms.h"
#include "Input.h"

class Camera
{
private:
	Transforms transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	float fieldOfView;
	float nearClipPlane;
	float farClipPlane;
	float movementSpeed;
	float mouseLookSpeed;

	DirectX::XMFLOAT3 position;
	float aspectRatio;

public:
	Camera(DirectX::XMFLOAT3 position, float aspectRatio,float fov, float nearClipPlane, float farClipPlane);
	~Camera();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	Transforms* GetTransform();
	float GetFieldOfView();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);
};

