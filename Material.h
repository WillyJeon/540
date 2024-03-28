#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> 
#include "SimpleShader.h"
#include "Camera.h"

#include <vector>
#include <memory>
#include <unordered_map>

using namespace DirectX;
using namespace std;

class Material
{
private:
	XMFLOAT3 colorTint;
	shared_ptr<SimpleVertexShader> vs;
	shared_ptr<SimplePixelShader> ps;
	float roughness;
	unordered_map<string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	unordered_map<string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
	XMFLOAT2 uvScale;
	XMFLOAT2 uvOffset;
	float shiny;

public:
	Material(XMFLOAT3 colorTint, shared_ptr<SimpleVertexShader> vs, shared_ptr<SimplePixelShader> ps, float roughness, float shiny);
	~Material();

	XMFLOAT3 GetColorTint();
	shared_ptr<SimpleVertexShader> GetVertexShader();
	shared_ptr<SimplePixelShader> GetPixelShader();
	float GetRoughness();


	void SetColorTint(XMFLOAT3 colorTint);
	void SetVertexShader(shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(shared_ptr<SimplePixelShader> pixelShader);
	void SetRoughness(float roughness);

	void PrepareMaterial(Transforms* transform, shared_ptr<Camera> cam);
	//void SetTime(float time);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureSRV(string name);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSampler(string name);

	void SetTextureSRV(string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV);
	void SetSampler(string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
};



