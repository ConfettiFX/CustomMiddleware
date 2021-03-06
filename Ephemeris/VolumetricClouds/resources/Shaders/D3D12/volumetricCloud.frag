/*
* Copyright (c) 2018-2019 Confetti Interactive Inc.
*
* This is a part of Ephemeris.
* This file(code) is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License (https://creativecommons.org/licenses/by-nc/4.0/legalcode) Based on a work at https://github.com/ConfettiFX/The-Forge.
* You may not use the material for commercial purposes.
*
*/

#include "volumetricCloud.h"

struct PSIn {
	float4 m_Position : SV_POSITION;
    float2 m_Tex0 : TEXCOORD;
	float2 VSray: TEXCOORD1;
};

float4 main(in PSIn input) : SV_TARGET
{
	float2 db_uvs = input.m_Tex0;

	float2 ScreenUV = input.m_Tex0;

	ScreenUV.x += g_VolumetricClouds.m_CorrectU;
	ScreenUV.y += g_VolumetricClouds.m_CorrectV;

	float3 ScreenNDC;
	ScreenNDC.x = ScreenUV.x * 2.0 - 1.0;
	ScreenNDC.y = (1.0 - ScreenUV.y) * 2.0 - 1.0;

	float3 projectedPosition = float3(ScreenNDC.xy, 0.0);
	float4 worldPos = mul(g_VolumetricClouds.m_DataPerEye[0].m_ProjToWorldMat, float4(projectedPosition, 1.0));
	float4 CameraPosition = g_VolumetricClouds.m_DataPerEye[0].cameraPosition;
	worldPos /= worldPos.w;


	float3 viewDir = normalize(worldPos.xyz - CameraPosition.xyz);

	float intensity;
	float atmosphereBlendFactor;
	float depth;
	
	float randomSeed = lerp(0.0f, g_VolumetricClouds.Random00, g_VolumetricClouds.m_UseRandomSeed);
	float dentisy = GetDensity(CameraPosition.xyz, worldPos.xyz, viewDir, randomSeed,
    intensity, atmosphereBlendFactor, depth, db_uvs);
		   
	return float4(intensity, atmosphereBlendFactor, depth, dentisy);
}