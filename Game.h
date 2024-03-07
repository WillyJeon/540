#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"

#include <vector>
#include <memory>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:
	bool show = false;
	int number = 0;
	float color[4];
	int hand;
	int cpHand;
	float tint[4];
	float offset[3];
	float move;
	std::vector<std::shared_ptr<Camera>> cam;

	std::shared_ptr<Camera > activeCam;
	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	void Helper(float deltaTime);
	void BuildUI();
	void EntitiesUI(std::vector<std::shared_ptr<GameEntity>> entities);
	void CamerasUI();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> pixelShaderNew;
	std::shared_ptr<SimpleVertexShader> vertexShader;

	std::vector<std::shared_ptr<Mesh>> meshes;

	std::vector<std::shared_ptr<Material>> materials;

	std::vector<std::shared_ptr<GameEntity>> entities;
};

