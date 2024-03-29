#pragma once

#include <d3d11.h>
#include "Vertex.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	int numberIndices;

public:
	Mesh(Vertex vertices[], unsigned int indices[], int numberIndices, int numberVertices, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

