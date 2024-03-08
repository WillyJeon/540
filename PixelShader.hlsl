#include "ShaderIncludes.hlsli"
#define MAX_SPECULAR_EXPONENT 256.0F;
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light {
	int Type;
	float3 Direction;
	float Range;
	float3 Position;
	float Intensity;
	float3 Color;
	float SpotFalloff;
	float3 Padding;

};
cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float3 cameraPosition;
    float roughness;
    float3 ambient;
	Light lights[5];
	
};


// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for


// --------------------------------------------------------

float Diffuse(float3 normal, float3 direction)
{
    return saturate(dot(normal, direction));
}


float Specular(float3 normal, float3 direction, float roughness, float3 cam)
{
    float3 ref = reflect(-direction, normal);
	
    return roughness == 1 ? 0.0f : pow(max(dot(cam, ref), 0), (1 - roughness) * 256);
}


float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}


float3 DirectionalLight(float3 normal, float3 position, float roughness, float3 cam, Light light, float3 color)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCamera = normalize(cam - position);
    
    return (Diffuse(normal, toLight) * color + Specular(normal, toLight, roughness, toCamera)) * light.Intensity * light.Color;
}

float3 PointLight(float3 normal, float3 position, float roughness, float3 cam, Light light, float3 color)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCamera = normalize(cam - position);
    
    return (Diffuse(normal, toLight) * color + Specular(normal, toLight, roughness, toCamera)) * Attenuate(light, position) * light.Intensity * light.Color;

}

float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    input.normal = normalize(input.normal);
	
	
	//Ambient
    float3 finalColor = ambient * colorTint;
	
	
    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        // Directional
        switch (light.Type)
        {
            case LIGHT_TYPE_DIRECTIONAL: 
                finalColor += DirectionalLight(input.normal, input.worldPosition, roughness, cameraPosition, light, colorTint);
                break;
            
            case LIGHT_TYPE_POINT:
                finalColor += PointLight(input.normal, input.worldPosition, roughness, cameraPosition, light, colorTint);
                break;
            
            case LIGHT_TYPE_SPOT:
                break;
            
            
        }

    
	
    
        return float4(finalColor, 1);
    }
}