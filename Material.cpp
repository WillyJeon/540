#include "Material.h"

Material::Material(XMFLOAT3 colorTint, shared_ptr<SimpleVertexShader> vs, shared_ptr<SimplePixelShader> ps, float roughness, float shiny)
    : colorTint(colorTint), vs(vs), ps(ps), roughness(roughness), shiny(shiny)
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
    return vs;
}

shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return ps;
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
    this->vs = vertexShader;
}

void Material::SetPixelShader(shared_ptr<SimplePixelShader> pixelShader)
{
    this->ps = pixelShader;
}

void Material::SetRoughness(float roughness)
{
    this->roughness = roughness;
}

void Material::PrepareMaterial(Transforms* object, shared_ptr<Camera> cam)
{
    
    vs->SetShader();
    ps->SetShader();

    vs->SetMatrix4x4("worldMatrix", object->GetWorldMatrix()); // match variable
    vs->SetMatrix4x4("view", cam->GetViewMatrix()); // names in your
    vs->SetMatrix4x4("projection", cam->GetProjectionMatrix()); // shader’s cbuffer!
    vs->SetMatrix4x4("worldInverseTranspose", object->GetWorldInverseTransposeMatrix());

    ps->SetFloat3("colorTint", colorTint); // Strings here MUST
    ps->SetFloat("roughness", roughness);
    ps->SetFloat3("cameraPosition", cam->GetTransform()->GetPosition());
    ps->SetFloat2("uvScale", uvScale);
    ps->SetFloat2("uvOffset", uvOffset);
    ps->SetFloat("shiny", shiny);



    vs->CopyAllBufferData();

    ps->CopyAllBufferData();

    for (auto& t : textureSRVs) { ps->SetShaderResourceView(t.first.c_str(), t.second); }
    for (auto& s : samplers) { ps->SetSamplerState(s.first.c_str(), s.second); }
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV(string name)
{
    return Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>();
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::GetSampler(string name)
{
    return Microsoft::WRL::ComPtr<ID3D11SamplerState>();
}

void Material::SetTextureSRV(string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV)
{
    textureSRVs.insert({ name, textureSRV });
}

void Material::SetSampler(string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers.insert({ name, sampler });
}

