/*
* Copyright (c) 2018-2019 Confetti Interactive Inc.
*
* This is a part of Ephemeris.
* This file(code) is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License (https://creativecommons.org/licenses/by-nc/4.0/legalcode) Based on a work at https://github.com/ConfettiFX/The-Forge.
* You may not use the material for commercial purposes.
*
*/

#include <metal_stdlib>
using namespace metal;


struct VSInput
{
  float4 Position			[[attribute(0)]];
  float2 Tex				[[attribute(1)]];
  float4 ParticlePositions	[[attribute(2)]];
  float4 ParticleColors		[[attribute(3)]];
  float4 ParticleInfo		[[attribute(4)]];
};

struct Uniforms_StarUniform
{
	float4x4 RotMat;
	float4x4 ViewProjMat;
	float4 LightDirection;
	float4 Dx;
	float4 Dy;
};

struct PsIn
{
	float4 position [[position]];
	float3 texCoord;
	float2 screenCoord;
	float3 color;
};

PsIn PushVertex(float4x4 rotMat, float4x4 viewProjMat, float3 pos, float3 color, float3 dx, float3 dy, float2 vOffset)
{
	PsIn output;
	float4 position = rotMat * float4(pos.x, pos.y, pos.z, 1.0);
	output.position = viewProjMat * float4(position.xyz + dx*vOffset.x + dy*vOffset.y, 1.0);

	output.texCoord.z = output.position.z;
	
	output.position.z = output.position.w;
	output.position /= output.position.w;
		
	output.screenCoord = float2( (output.position.x + 1.0) * 0.5, (1.0 - output.position.y) * 0.5);
			
	output.texCoord.xy = vOffset;
	output.color = color;
	return output;
}


vertex PsIn stageMain(uint VertexID [[vertex_id]], VSInput input [[stage_in]], uint InstanceID [[instance_id]], constant Uniforms_StarUniform &StarUniform [[buffer(5)]])
{
	PsIn Out;

	float4 starInfo = input.ParticleColors;
	float particleSize  = starInfo.y;
	float3 Width        = StarUniform.Dx.xyz * particleSize;
	float3 Height       = StarUniform.Dy.xyz * particleSize;

	float3 Color = input.ParticleColors.rgb * input.ParticleColors.a;
	float time = StarUniform.LightDirection.a;
	float timeSeed = starInfo.z;
	float timeScale = starInfo.a;	

	float blink = (sin(timeSeed + time * timeScale ) + 1.0f) * 0.5f;
	Color *= blink;
		
	if(VertexID == 0)
	{
		Out = PushVertex(StarUniform.RotMat, StarUniform.ViewProjMat, input.ParticlePositions.xyz, Color, Width, Height, float2(-1.0, -1.0));
	}
	else if(VertexID == 1)
	{
		Out = PushVertex(StarUniform.RotMat, StarUniform.ViewProjMat, input.ParticlePositions.xyz, Color, Width, Height, float2(1.0, -1.0));
	}
	else if(VertexID == 2)
	{
		Out = PushVertex(StarUniform.RotMat, StarUniform.ViewProjMat, input.ParticlePositions.xyz, Color, Width, Height, float2(-1.0, 1.0));
	}
	else
	{
		Out = PushVertex(StarUniform.RotMat, StarUniform.ViewProjMat, input.ParticlePositions.xyz, Color, Width, Height, float2(1.0, 1.0));
	}
     
	return Out;
}
