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
	XMFLOAT4 colorTint;
	shared_ptr<SimpleVertexShader> vertexShader;
	shared_ptr<SimplePixelShader> pixelShader;

public:
	Material(XMFLOAT4 colorTint, shared_ptr<SimpleVertexShader> vertexShader, shared_ptr<SimplePixelShader> pixelShader);
	~Material();

	XMFLOAT4 GetColorTint();
	shared_ptr<SimpleVertexShader> GetVertexShader();
	shared_ptr<SimplePixelShader> GetPixelShader();


	void SetColorTint(XMFLOAT4 colorTint);
	void SetVertexShader(shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(shared_ptr<SimplePixelShader> pixelShader);
	void SetTime(float time);
	

};

