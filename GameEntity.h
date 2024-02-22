#pragma once
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h"
#include "Transforms.h"
#include "BufferStructs.h"
#include "Camera.h"
class GameEntity
{
private:
	std::shared_ptr<Mesh> mesh;
	Transforms object;
public: 
	// Constructor
	GameEntity(std::shared_ptr<Mesh> mesh);

	std::shared_ptr<Mesh> GetMesh();
	Transforms* GetTransform();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> buffer, std::shared_ptr<Camera> cam);

};

