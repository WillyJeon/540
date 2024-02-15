#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh) : mesh(mesh)
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

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> buffer)
{
	BufferStruct bsData;
	bsData.colorTint = DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
	bsData.worldMatrix = object.GetWorldMatrix();


	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &bsData, sizeof(bsData));
	context->Unmap(buffer.Get(), 0);

	context->VSSetConstantBuffers(0, 1, buffer.GetAddressOf());

	mesh->Draw(context);
}
