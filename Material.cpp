#include "Material.h"

Material::Material(XMFLOAT4 colorTint, shared_ptr<SimpleVertexShader> vertexShader, shared_ptr<SimplePixelShader> pixelShader, float roughness)
{
    this->colorTint = colorTint;
    this->vertexShader = vertexShader;
    this->pixelShader = pixelShader;
    this->roughness = roughness;
}

Material::~Material()
{
}

XMFLOAT4 Material::GetColorTint()
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

void Material::SetColorTint(XMFLOAT4 colorTint)
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

