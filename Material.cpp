#include "Material.h"

Material::Material(XMFLOAT3 colorTint, shared_ptr<SimpleVertexShader> vertexShader, shared_ptr<SimplePixelShader> pixelShader, float roughness)
    : colorTint(colorTint), vertexShader(vertexShader), pixelShader(pixelShader), roughness(roughness)
{
}

Material::~Material()
{
}

XMFLOAT3 Material::GetColorTint()
{
    return colorTint;
}

shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

float Material::GetRoughness()
{
    return roughness;
}

void Material::SetColorTint(XMFLOAT3 colorTint)
{
    this->colorTint = colorTint;
}

void Material::SetVertexShader(shared_ptr<SimpleVertexShader> vertexShader)
{
    this->vertexShader = vertexShader;
}

void Material::SetPixelShader(shared_ptr<SimplePixelShader> pixelShader)
{
    this->pixelShader = pixelShader;
}

void Material::SetRoughness(float roughness)
{
    this->roughness = roughness;
}

