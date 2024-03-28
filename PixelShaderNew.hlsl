#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float time;
    float2 resolution;
	
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




float random(float s)
{
    return frac(sin(s) * 43758.5453123);
}


float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    //float4 mask = time * random(float2(2, 1));
	
    float2 xy = (2.0f * input.uv.xy * resolution.xy/2) / resolution.y;
	
    float3 center = float3(0.5, 0, 0.5);
    
    float3 pp = 0;
    
    float somethin = 50;
	
    const float count = 100.0f;
	
    for (int i = 0; i < count; i++)
    {
        float angle = sin(time * 3.141592 * 0.01f) - random(i) * 3.141592 * 2;
        
        float rand = sqrt(random(angle)) * 0.5f;
        
        float2 temp = float2(center.x + cos(angle) * rand, center.z + sin(angle) * rand);
        
        float dist = distance(input.uv, temp);
        somethin = min(somethin, dist);
        
        if (somethin == dist)
        {
            pp.xy = temp;
            pp.z = 1 - (i / count * xy.x * xy.y);
        }

    }
    
    float3 shade = float3(0.1f, 0.1f, 0.1f) * (1.0f - max(0.0f, dot(pp, center)));
    
	//distance(input.uv / 2, 2);
	
    
        return 1 - float4(pp + shade, 1.0f);
}