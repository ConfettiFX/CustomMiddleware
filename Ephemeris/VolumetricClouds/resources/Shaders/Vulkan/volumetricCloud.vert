/*
* Copyright (c) 2018-2019 Confetti Interactive Inc.
*
* This is a part of Ephemeris.
* This file(code) is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License (https://creativecommons.org/licenses/by-nc/4.0/legalcode) Based on a work at https://github.com/ConfettiFX/The-Forge.
* You may not use the material for commercial purposes.
*
*/

#version 450 core
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_samplerless_texture_functions : enable

#include "volumetricCloud.h"

layout(location = 0) in vec4 TEXCOORD0;
layout(location = 0) out vec2 vertOutput_TEXCOORD;
layout(location = 1) out vec2 vertOutput_TEXCOORD1;

struct VSInput
{
  vec4 m_ProjectionExtents;
  uint VertexID;
};
struct VSOutput
{
  vec4 Position;
  vec2 TexCoord;
  vec2 VSray;
};
VSOutput HLSLmain(VSInput input0)
{
  VSOutput Out;
  vec4 position;
  ((position).x = float(((((input0).VertexID == uint(2))) ? (float(3.0)) : (float((-1.0))))));
  ((position).y = float(((((input0).VertexID == uint(0))) ? (float((-3.0))) : (float(1.0)))));
  ((position).zw = vec2(1.0));
  ((Out).Position = position);
  ((Out).TexCoord = (((position).xy * vec2(0.5, (-0.5))) + vec2(0.5)));
  ((Out).VSray = (((position).xy * ((input0).m_ProjectionExtents).xy) + ((input0).m_ProjectionExtents).zw));
  return Out;
}
void main()
{
  VSInput input0;
  input0.m_ProjectionExtents = TEXCOORD0;
  input0.VertexID = gl_VertexIndex;
  VSOutput result = HLSLmain(input0);
  gl_Position = result.Position;
  vertOutput_TEXCOORD = result.TexCoord;
  vertOutput_TEXCOORD1 = result.VSray;
}
