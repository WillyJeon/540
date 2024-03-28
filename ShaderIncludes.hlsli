#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__

#define MAX_SPECULAR_EXPONENT 256.0F;
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
// ALL of your code pieces (structs, functions, etc.) go here!
struct Light
{
    int Type;
    float3 Direction;
    float Range;
    float3 Position;
    float Intensity;
    float3 Color;
    float SpotFalloff;
    float3 Padding;

};

struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float2 uv : TEXCOORD;
};

float Diffuse(float3 normal, float3 direction)
{
    return saturate(dot(normal, direction));
}


float Specular(float3 normal, float3 direction, float roughness, float3 cam, float3 position)
{
    float3 refl = reflect(-direction, normal);
    float3 V = normalize(cam - position);
    float specExponent = (1.0 - roughness) * MAX_SPECULAR_EXPONENT;
	
    return pow(saturate(dot(refl, V)), specExponent);
}


float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}


float3 DirectionalLight(float3 normal, float3 position, float roughness, float3 cam, Light light, float3 color, float specular)
{
    float3 toLight = normalize(-light.Direction);    
    float spec = Specular(normal, toLight, roughness, cam, position) * specular;   
    
    return (Diffuse(normal, toLight) * color + spec) * light.Intensity * light.Color;
}

float3 PointLight(float3 normal, float3 position, float roughness, float3 cam, Light light, float3 color, float specular)
{
    float3 toLight = normalize(light.Position - position);
    float spec = Specular(normal, toLight, roughness, cam, position) * specular;
    
    return (Diffuse(normal, toLight) * color +spec) * Attenuate(light, position) * light.Intensity * light.Color;

}


#endif