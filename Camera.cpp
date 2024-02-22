#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT3 position, float aspectRatio, float nearClipPlane, float farClipPlane)
{
	transform.SetPosition(position.x, position.y, position.z);
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
	movementSpeed = 1;
	this->farClipPlane = farClipPlane;
	this->nearClipPlane = nearClipPlane;
	
}

Camera::~Camera()
{

}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearClipPlane, farClipPlane));
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 fwd = transform.GetForward();
	DirectX::XMFLOAT3 pos = transform.GetPosition();
	
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&fwd), DirectX::XMVectorSet(0,1,0,0)));

}

void Camera::Update(float dt)
{
	float speed = dt * 1;
	Input& input = Input::GetInstance();
	if (input.KeyDown('W')) {
		// Go Forward
		transform.MoveRelative(0, 0, speed);
	}
	if (input.KeyDown('S')) {
		// Go Backward
		transform.MoveRelative(0, 0, -speed);
	}
	if (input.KeyDown('A')) {
		// Go Left
		transform.MoveRelative(-speed, 0, 0);
	}
	if (input.KeyDown('D')) {
		// Go Right
		transform.MoveRelative(speed, 0, 0);
	}

	if (input.KeyDown(VK_SPACE)) {
		// Move up
		transform.MoveRelative(0, speed, 0);
	}
	if (input.KeyDown('X')) {
		// Move down
		transform.MoveRelative(0, -speed, 0);
	}

	if (input.MouseLeftDown())
	{
		int cursorMovementX = input.GetMouseXDelta();
		int cursorMovementY = input.GetMouseYDelta();
		/* Other mouse movement code here */
		
		transform.Rotate(cursorMovementY, cursorMovementX, 0);
		DirectX::XMFLOAT3 rot = transform.GetPitchYawRoll();
		if (rot.x > DirectX::XM_PIDIV2) {
			rot.x = DirectX::XM_PIDIV2;
		}
		if (rot.x < -DirectX::XM_PIDIV2) {
			rot.x = -DirectX::XM_PIDIV2;
		}
		transform.SetRotation(rot.x, rot.y, rot.z);
	}
	

	UpdateViewMatrix();
}