#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float3 cameraPosition;
    float roughness;
    float3 ambient;
    float2 uvScale;
    float2 uvOffset;
    float shiny;
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

Texture2D SurfaceTexture : register(t0);
Texture2D SpecularMap : register(t1);

SamplerState BasicSampler : register(s0);



float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    input.normal = normalize(input.normal);
	
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;    
    surfaceColor *= colorTint;
    
	//Ambient
    float3 finalColor = ambient * surfaceColor;
	
    float specularScale = SpecularMap.Sample(BasicSampler, input.uv).r * shiny;
	
    for (int i = 0; i < 5; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);
        // Directional
        switch (light.Type)
        {
            case 0:
                finalColor += DirectionalLight(input.normal, input.worldPosition, roughness, cameraPosition, light, surfaceColor, specularScale);
                break;
            
            case 1:
                finalColor += PointLight(input.normal, input.worldPosition, roughness, cameraPosition, light, surfaceColor, specularScale);
                break;    
            
        }

    
    }
    
        return float4(finalColor, 1);
    
}