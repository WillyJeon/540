#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> 
#include "SimpleShader.h"
#include "Camera.h"

#include <vector>
#include <memory>

using namespace DirectX;
using namespace std;

class Material
{
private:
	XMFLOAT3 colorTint;
	shared_ptr<SimpleVertexShader> vertexShader;
	shared_ptr<SimplePixelShader> pixelShader;
	float roughness;

public:
	Material(XMFLOAT3 colorTint, shared_ptr<SimpleVertexShader> vertexShader, shared_ptr<SimplePixelShader> pixelShader, float roughness);
	~Material();

	XMFLOAT3 GetColorTint();
	shared_ptr<SimpleVertexShader> GetVertexShader();
	shared_ptr<SimplePixelShader> GetPixelShader();
	float GetRoughness();


	void SetColorTint(XMFLOAT3 colorTint);
	void SetVertexShader(shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(shared_ptr<SimplePixelShader> pixelShader);
	void SetRoughness(float roughness);
	//void SetTime(float time);
	

};

