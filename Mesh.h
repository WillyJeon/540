#pragma once

#include <d3d11.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <fstream>
#include <vector>
using namespace DirectX;

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	void HelperBuffers(Vertex vertices[], unsigned int indices[], int numberIndices, int numberVertices, Microsoft::WRL::ComPtr<ID3D11Device> device);

	
public:
	Mesh(Vertex vertices[], unsigned int indices[], int numberIndices, int numberVertices, Microsoft::WRL::ComPtr<ID3D11Device> device);
	Mesh(const char* obj, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Mesh();

	int numberIndices;


	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	int GetIndexCount();
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

