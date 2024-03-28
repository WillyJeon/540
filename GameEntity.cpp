#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : mesh(mesh), material(material)
{
	
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return std::shared_ptr<Mesh>(mesh);
}

Transforms* GameEntity::GetTransform()
{
    return &object;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> cam)
{
	material->PrepareMaterial(&object, cam);

	/*BufferStruct bsData;
	bsData.colorTint = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	bsData.worldMatrix = object.GetWorldMatrix();
	bsData.projection = cam->GetProjectionMatrix();
	bsData.view = cam->GetViewMatrix();*/
	


	/*D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &bsData, sizeof(bsData));
	context->Unmap(buffer.Get(), 0);*/

	//context->VSSetConstantBuffers(0, 1, vs.GetAddressOf());

	/*material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();*/

	mesh->Draw(context);
}

void GameEntity::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}


