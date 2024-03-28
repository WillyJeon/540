#pragma once
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h"
#include "Transforms.h"
#include "Camera.h"
#include "Material.h"


class GameEntity
{
private:
	std::shared_ptr<Mesh> mesh;
	Transforms object;
	std::shared_ptr<Material> material;


public:
	// Constructor
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

	std::shared_ptr<Mesh> GetMesh();
	Transforms* GetTransform();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> cam);
	void SetMaterial(std::shared_ptr<Material> material);
	std::shared_ptr<Material> GetMaterial();


};
