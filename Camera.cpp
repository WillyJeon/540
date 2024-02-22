#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT3 position, float aspectRatio,float fov, float nearClipPlane, float farClipPlane)
{
	transform.SetPosition(position.x, position.y, position.z);
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
	movementSpeed = 1;
	mouseLookSpeed = 1;
	this->position = position;
	fieldOfView = fov;
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

Transforms* Camera::GetTransform()
{
	return &transform;
}

float Camera::GetFieldOfView()
{
	return fieldOfView;
}




void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, 0.1, 1000));
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 fwd = transform.GetForward();
	position = transform.GetPosition();
	
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&fwd), DirectX::XMVectorSet(0,1,0,0)));

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
	if (input.KeyDown('D')) {
		// Go Left
		transform.MoveRelative(-speed, 0, 0);
	}
	if (input.KeyDown('A')) {
		// Go Right
		transform.MoveRelative(speed, 0, 0);
	}

	if (input.KeyDown(VK_SPACE)) {
		// Move up
		transform.MoveRelative(0, -speed, 0);
	}
	if (input.KeyDown('X')) {
		// Move down
		transform.MoveRelative(0, speed, 0);
	}

	if (input.MouseLeftDown())
	{
		float cursorMovementX = mouseLookSpeed * dt * input.GetMouseXDelta();
		float cursorMovementY = mouseLookSpeed * dt * input.GetMouseYDelta();
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
