#define TRANSMITTANCE_SAMPLE_STEP_COUNT							5													// Number of energy sample
#define MAX_SAMPLE_STEP_DISTANCE								200000.0											// When the ray-marching distance exceeds this number, it stops.

#define PI														3.1415926535897932384626433832795
#define PI2														6.283185307179586476925286766559
#define ONE_OVER_FOURPI											0.07957747154594767
#define THREE_OVER_16PI											0.05968310365946075091333141126469

#define USE_Ray_Distance										1
#define ALLOW_CONTROL_CLOUD										1
#define STEREO_INSTANCED										0

#define FLOAT16_MAX												65500.0f

#define LOW_FREQ_LOD											1.0f
#define HIGH_FREQ_LOD											0.0f

struct DataPerEye
{
	float4x4			m_WorldToProjMat;			// Matrix for converting World to Projected Space for the first eye
	float4x4			m_ProjToWorldMat;			// Matrix for converting Projected Space to World Matrix for the first eye
	float4x4			m_ViewToWorldMat;			// Matrix for converting View Space to World for the first eye
	float4x4			m_PrevWorldToProjMat;		// Matrix for converting Previous Projected Space to World for the first eye (it is used for reprojection)
	float4x4			m_LightToProjMat;			// Matrix for converting Light to Projected Space Matrix for the first eye
	float4				cameraPosition;
};

struct DataPerLayer
{
	float					CloudsLayerStart;
	float					EarthRadiusAddCloudsLayerStart;
	float					EarthRadiusAddCloudsLayerStart2;
	float					EarthRadiusAddCloudsLayerEnd;
	float					EarthRadiusAddCloudsLayerEnd2;
	float					LayerThickness;

	//Cloud
	float					CloudDensity;						// The overall density of clouds. Using bigger value makes more dense clouds, but it also makes ray-marching artifact worse.
	float					CloudCoverage;						// The overall coverage of clouds. Using bigger value makes more parts of the sky be covered by clouds. (But, it does not make clouds more dense)
	float					CloudType;							// Add this value to control the overall clouds' type. 0.0 is for Stratus, 0.5 is for Stratocumulus, and 1.0 is for Cumulus.

	float					CloudTopOffset;						// Intensity of skewing clouds along the wind direction.

	//Modeling
	float					CloudSize;							// Overall size of the clouds. Using bigger value generates larger chunks of clouds.

	float					BaseShapeTiling;					// Control the base shape of the clouds. Using bigger value makes smaller chunks of base clouds.
	float					DetailShapeTiling;					// Control the detail shape of the clouds. Using bigger value makes smaller chunks of detail clouds.

	float					DetailStrenth;						// Intensify the detail of the clouds. It is possible to lose whole shape of the clouds if the user uses too high value of it.
	float					CurlTextureTiling;					// Control the curl size of the clouds. Using bigger value makes smaller curl shapes.

	float					CurlStrenth;						// Intensify the curl effect.
	float					AnvilBias;							// Using lower value makes anvil shape.
	float					PadA;
	float					PadB;
	float					PadC;

	float4					WindDirection;
	float4					StandardPosition;					// The current center location for applying wind

	float					WeatherTextureSize;					// Control the size of Weather map, bigger value makes the world to be covered by larger clouds pattern.
	float					WeatherTextureOffsetX;

	float					WeatherTextureOffsetZ;
	float					RotationPivotOffsetX;

	float					RotationPivotOffsetZ;
	float					RotationAngle;

	float					RisingVaporScale;
	float					RisingVaporUpDirection;
	float					RisingVaporIntensity;
	float					PadD;
	float					PadE;
	float					PadF;
};

struct volumetricCloud
{
    struct VolumetricCloudsCB
    {
        uint					m_JitterX;						    // the X offset of Re-projection
        uint					m_JitterY;					    	// the Y offset of Re-projection
        uint					MIN_ITERATION_COUNT;		    	// Minimum iteration number of ray-marching
        uint					MAX_ITERATION_COUNT;			    // Maximum iteration number of ray-marching

        DataPerEye		        m_DataPerEye[2];
        DataPerLayer	        m_DataPerLayer[2];

        float4				    m_StepSize;							// Cap of the step size X: min, Y: max
        float4			    	TimeAndScreenSize;				    // X: EplasedTime, Y: RealTime, Z: FullWidth, W: FullHeight
        float4			    	lightDirection;
        float4			    	lightColorAndIntensity;

        float4			    	EarthCenter;
        float					EarthRadius;

        float					m_MaxSampleDistance;

        float					m_CorrectU;							// m_JitterX / FullWidth
        float					m_CorrectV;							// m_JitterX / FullHeight			

        //Lighting
        float					BackgroundBlendFactor;	            // Blend clouds with the background, more background will be shown if this value is close to 0.0
        float					Contrast;							// Contrast of the clouds' color 
        float					Eccentricity;						// The bright highlights around the sun that the user needs at sunset
        float					CloudBrightness;					// The brightness for clouds

        float					Precipitation;
        float					SilverliningIntensity;		        // Intensity of silver-lining
        float					SilverliningSpread;				    // Using bigger value spreads more silver-lining, but the intesity of it
        float					Random00;							// Random seed for the first ray-marching offset

        float					CameraFarClip;
        uint					EnabledDepthCulling;
        uint					EnabledLodDepthCulling;
        uint					DepthMapWidth;

        uint					DepthMapHeight;
        // VolumetricClouds' Light shaft
        uint					GodNumSamples;						// Number of godray samples

        float					GodrayMaxBrightness;
        float					GodrayExposure;						// Intensity of godray

        float					GodrayDecay;						// Using smaller value, the godray brightness applied to each iteration is reduced. The level of reduction is also reduced per iteration.
        float					GodrayDensity;						// The distance between each interation.
        float					GodrayWeight;						// Using smaller value, the godray brightness applied to each iteration is reduced. The level of reduction is not changed.
        float					m_UseRandomSeed;

        float					Test00;
        float					Test01;
        float					Test02;
        float					Test03;
    };

    texture3d<float> highFreqNoiseTexture;
    texture3d<float> lowFreqNoiseTexture;
    texture2d<float> curlNoiseTexture;
    texture2d<float> weatherTexture;
    texture2d<float> depthTexture;
    texture2d<float> LowResCloudTexture;
    texture2d<float> g_PrevFrameTexture;
    texture2d<float> g_LinearDepthTexture;

    sampler g_LinearClampSampler;
    sampler g_LinearWrapSampler;
    sampler g_PointClampSampler;
    sampler g_LinearBorderSampler;

    struct Uniforms_VolumetricCloudsCBuffer
    {
        VolumetricCloudsCB g_VolumetricClouds;
    };

    constant Uniforms_VolumetricCloudsCBuffer & VolumetricCloudsCBuffer;
	
	// Argument buffers
	struct GraphicsArgData
	{
		texture3d<float> highFreqNoiseTexture;
		texture3d<float> lowFreqNoiseTexture;
		texture2d<float> curlNoiseTexture;
		texture2d<float> weatherTexture;
		texture2d<float> depthTexture;
		texture2d<float> LowResCloudTexture;
		sampler g_LinearClampSampler;
		sampler g_LinearWrapSampler;
		sampler g_PointClampSampler;
		sampler g_LinearBorderSampler;
		texture2d<float> g_PrevFrameTexture;
		
		texture2d<float> g_SrcTexture2D;
		texture2d<float> g_SkyBackgroudTexture;
		device float4* TransmittanceColor;
		
		texture2d<float> g_GodrayTexture;
		
		texture2d<float> g_BlurTexture;
		
		texture2d<float> g_PostProcessedTexture;
		texture2d<float> g_PrevVolumetricCloudTexture;
		
		texture2d<float> g_LinearDepthTexture;
	};

	struct GraphicsArgDataPerFrame
	{
		constant Uniforms_VolumetricCloudsCBuffer & VolumetricCloudsCBuffer;
	};
	
	struct ComputeArgData
	{
		texture2d<float> SrcTexture;
		texture2d<float, access::write> SavePrevTexture;
		
		texture2d<float, access::read_write> DstTexture;
		
		texture2d<float> InputTex;
		texture2d<float, access::read_write> OutputTex;
	};
	

    float randomFromScreenUV(float2 uv)
    {
        return fract((sin(dot((uv).xy, float2(12.9898005, 78.23300170))) * (float)(43758.546875)));
    };

    float3 rand[(TRANSMITTANCE_SAMPLE_STEP_COUNT + 1)] = { {0.0, 0.0, 0.0}, {0.612305, (-0.1875), 0.28125}, {0.648437, 0.026367000, (-0.792969)}, {(-0.636719), 0.449219, (-0.539062)}, {(-0.8085940), 0.74804696, 0.456055}, {0.542969, 0.35156300, 0.048462} };
    bool ray_trace_sphere(float3 center, float3 rd, float3 offset, float radius2, thread float(& t1), thread float(& t2))
    {
        float3 p = (center - offset);
        float b = dot(p, rd);
        float c = (dot(p, p) - radius2);
        float f = ((b * b) - c);
        if ((f >= (float)(0.0)))
        {
            float sqrtF = sqrt(f);
            (t1 = ((-b) - sqrtF));
            (t2 = ((-b) + sqrtF));
            if (((t2 <= (float)(0.0)) && (t1 <= (float)(0.0))))
            {
                return false;
            }
            return true;
        }
        else
        {
            (t1 = (float)(0.0));
            (t2 = (float)(0.0));
            return false;
        }
    };

    bool GetStartEndPointForRayMarching(float3 ws_origin, float3 ws_ray, float3 earthCenter,
 float layerStart2, float LayerEnd2, thread float3(& start), thread float3(& end))
    {
        float ot1, ot2, it1, it2;
        (start = (end = float3(0.0, 0.0, 0.0)));

        if (!ray_trace_sphere(ws_origin, ws_ray, earthCenter, LayerEnd2, ot1, ot2))
        {
            return false;
        }
        bool inIntersected = ray_trace_sphere(ws_origin, ws_ray, earthCenter, layerStart2, it1, it2);
        if (inIntersected)
        {
            float branchFactor = saturate(floor((it1 + (float)(1.0))));
            (start = (ws_origin + ((float3)(mix(max(it2, 0.0), max(ot1, 0.0), branchFactor)) * ws_ray)));
            (end = (ws_origin + ((float3)(mix(ot2, it1, branchFactor)) * ws_ray)));
        }
        else
        {
            (end = (ws_origin + ((float3)(ot2) * ws_ray)));
            (start = (ws_origin + ((float3)(max(ot1, 0.0)) * ws_ray)));
        }
        return true;
    };

    float Remap(float original_value, float original_min, float original_max, float new_min, float new_max)
    {
        return (new_min + (((original_value - original_min) / (original_max - original_min)) * (new_max - new_min)));
    };

    float RemapClamped(float original_value, float original_min, float original_max, float new_min, float new_max)
    {
        return (new_min + (saturate(((original_value - original_min) / (original_max - original_min))) * (new_max - new_min)));
    };

    float HenryGreenstein(float g, float cosTheta)
    {
        float g2 = (g * g);
        float numerator = ((float)(1.0) - g2);
        float denominator = pow(abs((((float)(1.0) + g2) - (((float)(2.0) * g) * cosTheta))), 1.5);
        return (((float)(ONE_OVER_FOURPI) * numerator) / denominator);
    };

    float Phase(float g, float g2, float cosTheta, float y)
    {
        return mix(HenryGreenstein(g, cosTheta), HenryGreenstein(g2, cosTheta), y);
    };

   // Get the point projected to the inner atmosphere shell
    float3 getProjectedShellPoint(float earthRadiusAddCloudLayerStart, float3 pt, float3 center)
    {
	    return earthRadiusAddCloudLayerStart * normalize(pt - center) + center;
    }

    float getRelativeHeight(float3 pt, float3 projectedPt, float layer_thickness)
    {
        return saturate((length((pt - projectedPt)) / layer_thickness));
    };

    // Get the relative height accurately
    float getRelativeHeightAccurate(float3 earthCenter, float earthRadiusAddCloudLayerStart,
                                    float3 pt, float3 projectedPt, float layer_thickness)
    {
	    float t = distance(pt, earthCenter);
	    t -= earthRadiusAddCloudLayerStart;
	    return saturate(max(t, 0.0f) / layer_thickness);
    }

    float PackFloat16(float value)
    {
        return value * 0.001f;
    }

    static float UnPackFloat16(float value)
    {
        return value * 1000.0f;
    }

    float getAtmosphereBlendForComposite(float distance)
    {
        float rate = mix(0.75f, 0.4f, saturate(VolumetricCloudsCBuffer.g_VolumetricClouds.m_MaxSampleDistance / MAX_SAMPLE_STEP_DISTANCE));
        float Threshold = VolumetricCloudsCBuffer.g_VolumetricClouds.m_MaxSampleDistance * rate;
        float InvThreshold = VolumetricCloudsCBuffer.g_VolumetricClouds.m_MaxSampleDistance - Threshold;
        return saturate(max(distance * MAX_SAMPLE_STEP_DISTANCE - Threshold, 0.0f) / InvThreshold);
    }

	float2 Rotation(float2 pos, float theta)
	{
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);
		return float2(pos.x * cosTheta - pos.y * sinTheta, pos.x * sinTheta + pos.y * cosTheta);
	}

    float GetDensityHeightGradientForPoint(float relativeHeight, float cloudType)
    {
        float cumulus = max(0.0, (RemapClamped(relativeHeight, 0.01, 0.15, 0.0, 1.0) * RemapClamped(relativeHeight, 0.9, 0.95, 1.0, 0.0)));
        float stratocumulus = max(0.0, (RemapClamped(relativeHeight, 0.0, 0.15, 0.0, 1.0) * RemapClamped(relativeHeight, 0.3, 0.65, 1.0, 0.0)));
        float stratus = max(0.0, (RemapClamped(relativeHeight, 0.0, 0.1, 0.0, 1.0) * RemapClamped(relativeHeight, 0.2, 0.3, 1.0, 0.0)));
        float cloudType2 = (cloudType * (float)(2.0));
        float a = mix(stratus, stratocumulus, saturate(cloudType2));
        float b = mix(stratocumulus, cumulus, saturate((cloudType2 - (float)(1.0))));
        return mix(a, b, round(cloudType));
    };

    float SampleDensity(float3 worldPos, float lod, float height_fraction, float3 currentProj, float LayerThickness,
                        float CloudSize, float BaseShapeTiling, float CloudCoverage, float CloudType, float AnvilBias,
                        float CurlStrenth, float CurlTextureTiling,
                        float DetailStrenth,
                        float RisingVaporUpDirection, float RisingVaporScale, float RisingVaporIntensity,
                        float3 cloudTopOffsetWithWindDir, float4 windWithVelocity,
                        float3 biasedCloudPos, float DetailShapeTilingDivCloudSize,
                        float WeatherTextureOffsetX, float WeatherTextureOffsetZ, float WeatherTextureSize,
                        float RotationPivotOffsetX, float RotationPivotOffsetZ,
                        float RotationAngle,
                        bool cheap)
    {
        float3 unwindWorldPos = worldPos;
        (worldPos += ((float3)(height_fraction) * cloudTopOffsetWithWindDir));
        (worldPos += biasedCloudPos);

		float2 weatherMapUV = (unwindWorldPos.xz + windWithVelocity.xy + float2(WeatherTextureOffsetX, WeatherTextureOffsetZ)) / (WeatherTextureSize);

		float2 RotationOffset = float2(RotationPivotOffsetX, RotationPivotOffsetZ);
		weatherMapUV -= RotationOffset;
		weatherMapUV = Rotation(weatherMapUV, RotationAngle);
		weatherMapUV += RotationOffset;

        float4 weatherData = weatherTexture.sample(g_LinearWrapSampler, weatherMapUV, level(1.0));

        float3 worldPosDivCloudSize = (worldPos / (float3)CloudSize);

        float4 low_freq_noises = lowFreqNoiseTexture.sample(g_LinearWrapSampler, (worldPosDivCloudSize * (float3)BaseShapeTiling), level(lod));
        float low_freq_fBm = ((((low_freq_noises).g * (float)(0.625)) + ((low_freq_noises).b * (float)(0.25))) + ((low_freq_noises).a * (float)(0.125)));

        float base_cloud = RemapClamped((low_freq_noises).r, (low_freq_fBm - (float)(1.0)), 1.0, 0.0, 1.0);
        (base_cloud = saturate((base_cloud + CloudCoverage)));

        float cloudType = saturate(((weatherData).g + CloudType));
        float density_height_gradient = GetDensityHeightGradientForPoint(height_fraction, cloudType);
        (base_cloud *= density_height_gradient);
        
        float cloud_coverage = saturate((weatherData).r);
        (cloud_coverage = pow(cloud_coverage, RemapClamped(height_fraction, 0.2, 0.8, 1.0, mix(1.0, 0.5, AnvilBias))));

        float base_cloud_coverage = RemapClamped(base_cloud, cloud_coverage, 1.0, 0.0, 1.0);
        (base_cloud_coverage *= cloud_coverage);
        float final_cloud = base_cloud_coverage;
        if ((!cheap))
        {
            float2 curl_noise = curlNoiseTexture.sample(g_LinearWrapSampler, float2(((worldPosDivCloudSize).xz * (float2)(CurlTextureTiling))), level(0.0)).rg;
            ((worldPos).xz += ((curl_noise * (float2)(((float)(1.0) - height_fraction))) * (float2)(CurlStrenth)));

            float3 high_frequency_noises = highFreqNoiseTexture.sample(g_LinearWrapSampler, float3((worldPos * (float3)(DetailShapeTilingDivCloudSize))), level(lod)).rgb;            
            float high_freq_fBm = ((((high_frequency_noises).r * (float)(0.625)) + ((high_frequency_noises).g * (float)(0.25))) + ((high_frequency_noises).b * (float)(0.125)));

            float height_fraction_new = getRelativeHeight(worldPos, currentProj, LayerThickness);
            float height_freq_noise_modifier = mix(high_freq_fBm, ((float)(1.0) - high_freq_fBm), saturate((height_fraction_new * (float)(10.0))));

            (final_cloud = RemapClamped(base_cloud_coverage, (height_freq_noise_modifier * DetailStrenth), 1.0, 0.0, 1.0));
        }
        return final_cloud;
    };

    float GetLightEnergy(float height_fraction, float dl, float ds_loded, float phase_probability, float cos_angle, float step_size, float contrast)
    {
        float primary_att = exp((-dl));

        float secondary_att = (exp(((-dl) * (float)(0.25))) * (float)(0.7));
        float attenuation_probability = max(primary_att, (secondary_att * (float)(0.25)));

        float depth_probability = mix(((float)(0.05) + pow(ds_loded, RemapClamped(height_fraction, 0.3, 0.85, 0.5, 2.0))), 1.0, saturate(dl));
        float vertical_probability = pow(RemapClamped(height_fraction, 0.07, 0.14, 0.1, 1.0), 0.8);
        float in_scatter_probability = (vertical_probability * depth_probability);
        float light_energy = (attenuation_probability + (in_scatter_probability * phase_probability));
        (light_energy = pow(abs(light_energy), contrast));
        return light_energy;
    };

    float SampleEnergy( float3 rayPos, float3 magLightDirection, float height_fraction, float3 currentProj,
                        float3 earthCenter,
                        float earthRadiusAddCloudLayerStart,
                        float layerThickness,
                        float CloudSize, float BaseShapeTiling, float CloudCoverage, float CloudType, float AnvilBias,
                        float CurlStrenth, float CurlTextureTiling,
                        float DetailStrenth,
                        float RisingVaporUpDirection, float RisingVaporScale, float RisingVaporIntensity,
                        float3 cloudTopOffsetWithWindDir, float4 windWithVelocity,
                        float3 biasedCloudPos, float DetailShapeTilingDivCloudSize,
                        float WeatherTextureOffsetX, float WeatherTextureOffsetZ, float WeatherTextureSize,
                        float RotationPivotOffsetX, float RotationPivotOffsetZ,
                        float RotationAngle,
                        float ds_loded, float stepSize, float cosTheta, float mipBias)
    {
        float totalSample = 0;
        float mipmapOffset = mipBias;
        float step = 0.5;

        for (int i = 0; (i < TRANSMITTANCE_SAMPLE_STEP_COUNT); (i++))
        {
            float3 rand3 = rand[i];
            float3 direction = (magLightDirection + normalize(rand3));
            (direction = normalize(direction));
            float3 samplePoint = (rayPos + ((float3)((step * stepSize)) * direction));
            
            float3 currentProj_new = getProjectedShellPoint(earthRadiusAddCloudLayerStart, samplePoint, earthCenter);
		    float height_fraction_new = getRelativeHeight(samplePoint, currentProj_new, layerThickness);

            totalSample += SampleDensity(   samplePoint, mipmapOffset, height_fraction_new, currentProj_new, layerThickness,
                                            CloudSize, BaseShapeTiling, CloudCoverage, CloudType, AnvilBias,
                                            CurlStrenth, CurlTextureTiling,
                                            DetailStrenth,
                                            RisingVaporUpDirection, RisingVaporScale, RisingVaporIntensity,
                                            cloudTopOffsetWithWindDir, windWithVelocity,
                                            biasedCloudPos, DetailShapeTilingDivCloudSize,
                                            WeatherTextureOffsetX, WeatherTextureOffsetZ, WeatherTextureSize,
                                            RotationPivotOffsetX, RotationPivotOffsetZ,
                                            RotationAngle,
                                            false);

            (mipmapOffset += (float)(0.5));
            (step += 1.0);
        }

        float hg = max(HenryGreenstein((VolumetricCloudsCBuffer.g_VolumetricClouds).Eccentricity, cosTheta), (saturate(HenryGreenstein((0.99 - (VolumetricCloudsCBuffer.g_VolumetricClouds).SilverliningSpread), cosTheta)))) * (VolumetricCloudsCBuffer.g_VolumetricClouds).SilverliningIntensity;
		float dl = totalSample * VolumetricCloudsCBuffer.g_VolumetricClouds.Precipitation;
		hg = hg / max(dl, 0.05);

        float energy = GetLightEnergy(height_fraction, dl, ds_loded, hg, cosTheta, stepSize, (VolumetricCloudsCBuffer.g_VolumetricClouds).Contrast);
        return energy;
    };

	float GetDensity(float3 startPos, float3 worldPos, float3 dir, float raymarchOffset,
					thread float(& intensity), thread float(& atmosphericBlendFactor), thread float(& depth), float2 uv)
    {
        float3 sampleStart, sampleEnd;

        (depth = (float)(0.0));
        (intensity = (float)(0.0));
        (atmosphericBlendFactor = 0.0);
        
        // If the current view direction is not intersected with cloud's layers
	    if (!GetStartEndPointForRayMarching(startPos, dir, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart2, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerEnd2, sampleStart, sampleEnd))
		    return 0.0;

        float horizon = abs((dir).y);
        uint sample_count = (uint)mix((float)VolumetricCloudsCBuffer.g_VolumetricClouds.MAX_ITERATION_COUNT, (float)VolumetricCloudsCBuffer.g_VolumetricClouds.MIN_ITERATION_COUNT, horizon);
	    float sample_step = mix(VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.y, VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.x, horizon);
        float distCameraToStart = distance(sampleStart, startPos);        

        // Update the distance between hit points against clouds and view position
        depth = distance(sampleEnd, startPos);

        
        atmosphericBlendFactor = distCameraToStart / MAX_SAMPLE_STEP_DISTANCE;

        // Horizontal Culling
    	// The most of cases, we don't need to render the clouds below the horizon
        if (((sampleStart).y + VolumetricCloudsCBuffer.g_VolumetricClouds.Test01 < 0.0))
        {
            (intensity = (float)(0.0));
            return 0.0;
        }

        // Atmosphere Culling		
        // we don't need to render the clouds where the background should be shown 100%
        // How the clouds should be blended depends on the user
        if (distCameraToStart >= VolumetricCloudsCBuffer.g_VolumetricClouds.m_MaxSampleDistance)
        {
            return 1.0;
        }

        float transStepSize = VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.a;

        uint2 texels = (uint2)(float2(VolumetricCloudsCBuffer.g_VolumetricClouds.DepthMapWidth, VolumetricCloudsCBuffer.g_VolumetricClouds.DepthMapHeight) * uv);

        float sceneDepth;

        if (((float)((VolumetricCloudsCBuffer.g_VolumetricClouds).EnabledLodDepthCulling) > 0.5))
        {
            (sceneDepth = (float)(depthTexture.read(texels.xy).r));
            if (sceneDepth < 1.0f)
            {
                return 1.0;
            }
        }

        float alpha = 0.0;
        (intensity = 0.0);
        bool detailedSample = false;
        int missedStepCount = 0;

        float cheapLOD = 1.0f;

        float bigStep = (sample_step * (float)(2.0));

        float3 smallStepMarching = ((float3)(sample_step) * dir);
        float3 bigStepMarching = ((float3)(bigStep) * dir);
        float3 raymarchingDistance = (smallStepMarching * (float3)(raymarchOffset));

        float3 magLightDirection = ((float3)(2.0) * ((VolumetricCloudsCBuffer.g_VolumetricClouds).lightDirection).xyz);

        bool pickedFirstHit = false;

        float cosTheta = dot(dir, ((VolumetricCloudsCBuffer.g_VolumetricClouds).lightDirection).xyz);
        float3 rayPos = sampleStart;

        float4 windWithVelocity = ((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].StandardPosition);
        float3 biasedCloudPos = ((float3)(4.5) * (((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].WindDirection).xyz + float3(0.0, 0.1, 0.0)));
        float3 cloudTopOffsetWithWindDir = ((float3)((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].CloudTopOffset) * ((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].WindDirection).xyz);

        float DetailShapeTilingDivCloudSize = ((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].DetailShapeTiling / (VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].CloudSize);
        float LODbias = 0.0;

        for (uint j = 0; (j < sample_count); (j++))
        {
            (rayPos += raymarchingDistance);

            float3 currentProj = getProjectedShellPoint(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz);
            float height_fraction = getRelativeHeightAccurate(VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, currentProj, (VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].LayerThickness);
            if ((!detailedSample))
            {
                float sampleResult = SampleDensity( rayPos, cheapLOD, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                                                    cloudTopOffsetWithWindDir, windWithVelocity,
                                                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                                                    true);

                if ((sampleResult > 0.0))
                {
                    (detailedSample = true);
                    (raymarchingDistance = (-bigStepMarching));
                    (missedStepCount = 0);
                    continue;
                }
                else
                {
                    (raymarchingDistance = bigStepMarching);
                }
            }
            else
            {
                // Get the density from current rayPos
                float sampleResult = SampleDensity( rayPos, LODbias, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                                                    cloudTopOffsetWithWindDir, windWithVelocity,
                                                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                                                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                                                    false);

                if (sampleResult == 0.0)
                {
                    (missedStepCount++);
                    if ((missedStepCount > 10))
                    {
                        (detailedSample = false);
                    }
                }
                else
                {
                    if ((!pickedFirstHit))
                    {
                        (depth = distance(rayPos, startPos));
                        (pickedFirstHit = true);
                    }
                    
                    // If it hit the clouds, get the light enery from current rayPos and accumulate it
                    float sampledAlpha = sampleResult * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudDensity;
                    float sampledEnergy = SampleEnergy(rayPos, magLightDirection, height_fraction, currentProj,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                        cloudTopOffsetWithWindDir, windWithVelocity,
                        biasedCloudPos, DetailShapeTilingDivCloudSize,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                        sampleResult, transStepSize, cosTheta, 0.0f);

                    float oneMinusAlpha = ((float)(1.0) - alpha);
                    (sampledAlpha *= oneMinusAlpha);
                    (intensity += (sampledAlpha * sampledEnergy));
                    (alpha += sampledAlpha);
                    if ((alpha >= 1.0))
                    {
                        (intensity /= alpha);
                        (depth = PackFloat16(depth));
                        return 1.0;
                    }
                }
                (raymarchingDistance = smallStepMarching);
            }
        }
        (depth = PackFloat16(depth));
        return alpha;
    };

    float GetDensityWithComparingDepth( float3 startPos, float3 worldPos, float3 dir, float raymarchOffset,
	                                    thread float(& intensity), thread float(& atmosphericBlendFactor), thread float(& depth), float2 uv)
    {
        float3 sampleStart, sampleEnd;
        (depth = (float)(0.0));
        (intensity = (float)(0.0));
        (atmosphericBlendFactor = 0.0);
       
       // If the current view direction is not intersected with cloud's layers	
	    if (!GetStartEndPointForRayMarching(startPos, dir, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart2, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerEnd2, sampleStart, sampleEnd))
		    return 0.0;

        float horizon = abs((dir).y);

        uint sample_count = (uint)mix((float)VolumetricCloudsCBuffer.g_VolumetricClouds.MAX_ITERATION_COUNT, (float)VolumetricCloudsCBuffer.g_VolumetricClouds.MIN_ITERATION_COUNT, horizon);
	    float sample_step = mix(VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.y, VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.x, horizon);

        (depth = distance(sampleEnd, startPos));
        float distCameraToStart = distance(sampleStart, startPos);
        
        atmosphericBlendFactor = distCameraToStart / MAX_SAMPLE_STEP_DISTANCE;

        float sceneDepth = depthTexture.sample(g_LinearClampSampler, uv, level(0)).r;
        if (sceneDepth < 1.0f)
        {
          atmosphericBlendFactor = 1.0f;
        }

        float linearDepth = mix(50.0f, 100000000.0f, sceneDepth);

        if (((sampleStart).y < 0.0))
        {
            (intensity = (float)(0.0));
            return 0.0;
        }

        // Atmosphere Culling		
        // we don't need to render the clouds where the background should be shown 100%
        // How the clouds should be blended depends on the user 
        if (distCameraToStart >= VolumetricCloudsCBuffer.g_VolumetricClouds.m_MaxSampleDistance)
        {
            return 1.0;
        }

        float transStepSize = VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.a;

        //float sceneDepth = depthTexture.read(texels.xy).r;
        float alpha = 0.0;
        (intensity = 0.0);
        bool detailedSample = false;
        int missedStepCount = 0;

        float cheapLOD = 1.0f;
        float bigStep = (sample_step * (float)(2.0));

        float3 smallStepMarching = ((float3)(sample_step) * dir);
        float3 bigStepMarching = ((float3)(bigStep) * dir);

        float3 raymarchingDistance = (smallStepMarching * (float3)(raymarchOffset));

        float3 magLightDirection = ((float3)(2.0) * ((VolumetricCloudsCBuffer.g_VolumetricClouds).lightDirection).xyz);

        bool pickedFirstHit = false;

        float cosTheta = dot(dir, ((VolumetricCloudsCBuffer.g_VolumetricClouds).lightDirection).xyz);
        float3 rayPos = sampleStart;

		float4 windWithVelocity = ((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].StandardPosition);
        float3 biasedCloudPos = ((float3)(4.5) * (((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].WindDirection).xyz + float3(0.0, 0.1, 0.0)));
        float3 cloudTopOffsetWithWindDir = ((float3)((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].CloudTopOffset) * ((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].WindDirection).xyz);

        float DetailShapeTilingDivCloudSize = ((VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].DetailShapeTiling / (VolumetricCloudsCBuffer.g_VolumetricClouds).m_DataPerLayer[0].CloudSize);

        float LODbias = 0.0;
       
        for (uint j = 0; (j < sample_count); (j++))
        {
            (rayPos += raymarchingDistance);
            if ((linearDepth < distance(startPos, rayPos)))
            {
                break;
            }

            float3 currentProj = getProjectedShellPoint(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz);
		    float height_fraction = getRelativeHeightAccurate(VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness);

            if ((!detailedSample))
            {
                // Get the density from current rayPos
                float sampleResult = SampleDensity(rayPos, cheapLOD, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir, windWithVelocity,
                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                    true);

                if ((sampleResult > 0.0))
                {
                    (detailedSample = true);
                    (raymarchingDistance = (-bigStepMarching));
                    (missedStepCount = 0);
                    continue;
                }
                else
                {
                    (raymarchingDistance = bigStepMarching);
                }
            }
            else
            {
                // Get the density from current rayPos
                float sampleResult = SampleDensity(rayPos, LODbias, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir, windWithVelocity,
                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                    false);

                if (sampleResult == 0.0)
                {
                    (missedStepCount++);
                    if ((missedStepCount > 10))
                    {
                        (detailedSample = false);
                    }
                }
                else
                {
                    if ((!pickedFirstHit))
                    {
                        (depth = distance(rayPos, startPos));
                        (pickedFirstHit = true);
                    }
                    // If it hit the clouds, get the light enery from current rayPos and accumulate it
                    float sampledAlpha = sampleResult * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudDensity;
                    float sampledEnergy = SampleEnergy(
                        rayPos, magLightDirection, height_fraction, currentProj,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                        cloudTopOffsetWithWindDir, windWithVelocity,
                        biasedCloudPos, DetailShapeTilingDivCloudSize,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                        VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                        sampleResult, transStepSize, cosTheta, 0.0f);

                    float oneMinusAlpha = ((float)(1.0) - alpha);
                    (sampledAlpha *= oneMinusAlpha);
                    (intensity += (sampledAlpha * sampledEnergy));
                    (alpha += sampledAlpha);
                    if ((alpha >= 1.0))
                    {
                        (intensity /= alpha);
                        (depth = PackFloat16(depth));
                        return 1.0;
                    }
                }
                (raymarchingDistance = smallStepMarching);
            }
        }

        (depth = PackFloat16(depth));
        return alpha;
    };

    // Get the final density, light intensirt, atmophereBlendFactor, and distance between hit points against clouds and view position
    float GetDensity_Double_Layers(float3 startPos, float3 worldPos, float3 dir, float raymarchOffset,
        thread float(& intensity), thread float(& atmosphericBlendFactor), thread float(& depth), float2 uv)
    {
        float3 sampleStart, sampleEnd;

        depth = 0.0;
        intensity = 0.0;
        atmosphericBlendFactor = 0.0f;

        // If the current view direction is not intersected with cloud's layers
        if (!GetStartEndPointForRayMarching(startPos, dir, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
            min(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart2, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart2),
            max(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerEnd2, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerEnd2), sampleStart, sampleEnd))
            return 0.0;

        // Determine the sample count and its step size along the cosTheta of view direction with Up vector 
        float horizon = abs(dir.y);

        uint sample_count = (uint)mix((float)VolumetricCloudsCBuffer.g_VolumetricClouds.MAX_ITERATION_COUNT, (float)VolumetricCloudsCBuffer.g_VolumetricClouds.MIN_ITERATION_COUNT, horizon);
        float sample_step = mix((float)VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.y, (float)VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.x, horizon);

        // Update the distance between hit points against clouds and view position
        depth = distance(startPos, sampleEnd);

        float distCameraToStart = distance(sampleStart, startPos);
        atmosphericBlendFactor = distCameraToStart / MAX_SAMPLE_STEP_DISTANCE;

        // Horizontal Culling		
        // The most of cases, we don't need to render the clouds below the horizon
        if (sampleStart.y < 0.0f)
        {
            intensity = 0.0;
            return 0.0;
        }

        // Atmosphere Culling		
        // we don't need to render the clouds where the background should be shown 100%
        // How the clouds should be blended depends on the user 
        if (distCameraToStart >= VolumetricCloudsCBuffer.g_VolumetricClouds.m_MaxSampleDistance)
        {
            return 1.0;
        }

        float transStepSize = VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.a;

         // Depth Culling
        // Get texel coordinates for Depth culling
        uint2 texels = (uint2)(float2(VolumetricCloudsCBuffer.g_VolumetricClouds.DepthMapWidth, VolumetricCloudsCBuffer.g_VolumetricClouds.DepthMapHeight) * uv);

        //Get the lodded depth, if it is not using, use far distance instead to pass the culling
        float sceneDepth;

        if ((float)VolumetricCloudsCBuffer.g_VolumetricClouds.EnabledLodDepthCulling > 0.5f)
        {
            (sceneDepth = (float)(depthTexture.read(texels.xy).r));

            if (sceneDepth < 1.0f)
            {
                return 0.0;
            }
        }

        // Prepare to do raymarching
        float alpha = 0.0f;
        intensity = 0.0f;
        bool detailedSample = false; // start with cheap raymarching
        int missedStepCount = 0;
        

        float cheapLOD = 1.0f;
        float bigStep = sample_step * 2.0f; //pow(2.0f, cheapLOD);

        float3 smallStepMarching = sample_step * dir;
        float3 bigStepMarching = bigStep * dir;

        // To prevent raymarching artifact, use a random value
        float3 raymarchingDistance = smallStepMarching * raymarchOffset;

        float3 magLightDirection = 2.0 * VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.xyz;

        bool pickedFirstHit = false;

        float cosTheta = dot(dir, VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.xyz);
        float3 rayPos = sampleStart;

        float4 windWithVelocity = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].StandardPosition;
        float4 windWithVelocity_2nd = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].StandardPosition;

        float3 biasedCloudPos = 4.5f * (VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WindDirection.xyz + float3(0.0, 0.1, 0.0));
        float3 biasedCloudPos_2nd = 4.5f * (VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WindDirection.xyz + float3(0.0, 0.1, 0.0));

        float3 cloudTopOffsetWithWindDir = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudTopOffset * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WindDirection.xyz;
        float3 cloudTopOffsetWithWindDir_2nd = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudTopOffset * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WindDirection.xyz;

        float DetailShapeTilingDivCloudSize = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailShapeTiling / VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize;
        float DetailShapeTilingDivCloudSize_2nd = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailShapeTiling / VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize;

        float LODbias = 0.0f;
        
        for (uint j = 0; j < sample_count; j++)
        {
            rayPos += raymarchingDistance;

            float3 currentProj = getProjectedShellPoint(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz);
            float height_fraction = getRelativeHeightAccurate(VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness);

            float3 currentProj_2nd = getProjectedShellPoint(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart, rayPos, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz);
            float height_fraction_2nd = getRelativeHeightAccurate(VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart, rayPos, currentProj_2nd, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness);

            if (!detailedSample)
            {
                // Get the density from current rayPos
                float sampleResult = SampleDensity(
                    rayPos, cheapLOD, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir, windWithVelocity,
                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                    true);

                float sampleResult_2nd = SampleDensity(
                    rayPos, cheapLOD, height_fraction_2nd, currentProj_2nd, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir_2nd, windWithVelocity_2nd,
                    biasedCloudPos_2nd, DetailShapeTilingDivCloudSize_2nd,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationAngle,
                    true);

                if (sampleResult > 0.0f || sampleResult_2nd > 0.0f)
                {
                    // If it hit the clouds, change to the expensive raymarching
                    detailedSample = true;

                    raymarchingDistance = -bigStepMarching;
                    missedStepCount = 0;
                    continue;
                }
                else
                    raymarchingDistance = bigStepMarching;
            }
            else
            {
                // Get the density from current rayPos
                float sampleResult = SampleDensity(
                    rayPos, LODbias, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir, windWithVelocity,
                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                    false);

                float sampleResult_2nd = SampleDensity(
                    rayPos, LODbias, height_fraction_2nd, currentProj_2nd, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir_2nd, windWithVelocity_2nd,
                    biasedCloudPos_2nd, DetailShapeTilingDivCloudSize_2nd,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationAngle,
                    false);

                if (sampleResult == 0.0 && sampleResult_2nd == 0.0)
                {
                    missedStepCount++;

                    // If expensive raymarching failed more 10 times, go back to cheap raymarching
                    if (missedStepCount > 10)
                        detailedSample = false;
                }
                else
                {
                    // Get the first hit position against clouds to use it for reprojection
                    if (!pickedFirstHit)
                    {
                        depth = distance(rayPos, startPos);
                        pickedFirstHit = true;
                    }

                    if (sampleResult > 0.0)
                    {
                        // If it hit the clouds, get the light enery from current rayPos and accumulate it
                        float sampledAlpha = sampleResult * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudDensity;
                        float sampledEnergy = SampleEnergy(
                            rayPos, magLightDirection, height_fraction, currentProj,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                            cloudTopOffsetWithWindDir, windWithVelocity,
                            biasedCloudPos, DetailShapeTilingDivCloudSize,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                            sampleResult, transStepSize, cosTheta, 0.0f);

                        float oneMinusAlpha = 1.0 - alpha;
                        sampledAlpha *= oneMinusAlpha;

                        intensity += sampledAlpha * sampledEnergy;
                        alpha += sampledAlpha;
                    }

                    if (sampleResult_2nd > 0.0)
                    {
                        // If it hit the clouds, get the light enery from current rayPos and accumulate it
                        float sampledAlpha = sampleResult_2nd * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudDensity;
                        float sampledEnergy = SampleEnergy(
                            rayPos, magLightDirection, height_fraction_2nd, currentProj_2nd,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].AnvilBias,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlTextureTiling,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailStrenth,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporIntensity,
                            cloudTopOffsetWithWindDir_2nd, windWithVelocity_2nd,
                            biasedCloudPos_2nd, DetailShapeTilingDivCloudSize_2nd,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureSize,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetZ,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationAngle,
                            sampleResult_2nd, transStepSize, cosTheta, 0.0f);

                        float oneMinusAlpha = 1.0 - alpha;
                        sampledAlpha *= oneMinusAlpha;

                        intensity += sampledAlpha * sampledEnergy;
                        alpha += sampledAlpha;
                    }

                    if (alpha >= 1.0f)
                    {
                        intensity /= alpha;
                        depth = PackFloat16(depth);
                        return 1.0f;
                    }
                }

                raymarchingDistance = smallStepMarching;
            }
        }

        depth = PackFloat16(depth);
        return alpha;
    };

    // Get the final density, light intensirt, atmophereBlendFactor, and distance between hit points against clouds and view position
    float GetDensity_Double_Layers_WithComparingDepth(float3 startPos, float3 worldPos, float3 dir, float raymarchOffset,
        thread float(& intensity), thread float(& atmosphericBlendFactor), thread float(& depth), float2 uv)
    {
        float3 sampleStart, sampleEnd;

        depth = 0.0;
        intensity = 0.0;
        atmosphericBlendFactor = 0.0f;

        // If the current view direction is not intersected with cloud's layers
        if (!GetStartEndPointForRayMarching(startPos, dir, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
            min(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart2, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart2),
            max(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerEnd2, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerEnd2), sampleStart, sampleEnd))
            return 0.0;

        // Determine the sample count and its step size along the cosTheta of view direction with Up vector 
        float horizon = abs(dir.y);

        uint sample_count = (uint)mix((float)VolumetricCloudsCBuffer.g_VolumetricClouds.MAX_ITERATION_COUNT, (float)VolumetricCloudsCBuffer.g_VolumetricClouds.MIN_ITERATION_COUNT, horizon);
        float sample_step = mix((float)VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.y, (float)VolumetricCloudsCBuffer.g_VolumetricClouds.m_StepSize.x, horizon);

        // Update the distance between hit points against clouds and view position
        depth = distance(startPos, sampleEnd);

        float distCameraToStart = distance(sampleStart, startPos);
        atmosphericBlendFactor = distCameraToStart / MAX_SAMPLE_STEP_DISTANCE;

        float sceneDepth = depthTexture.sample(g_LinearClampSampler, uv, level(0)).r;
        if (sceneDepth < 1.0f)
        {
            atmosphericBlendFactor = 1.0f;
        }

        float linearDepth = mix(50.0f, 100000000.0f, sceneDepth);

        // Horizontal Culling		
        // The most of cases, we don't need to render the clouds below the horizon
        if (sampleStart.y < 0.0f)
        {
            intensity = 0.0;
            return 0.0;
        }

        // Atmosphere Culling		
        // we don't need to render the clouds where the background should be shown 100%
        // How the clouds should be blended depends on the user 
        if (distCameraToStart >= VolumetricCloudsCBuffer.g_VolumetricClouds.m_MaxSampleDistance)
        {
            return 1.0;
        }

        float transStepSize = VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.a;

        // Depth Culling
        // Get texel coordinates for Depth culling
       

        // Prepare to do raymarching
        float alpha = 0.0f;
        intensity = 0.0f;
        bool detailedSample = false; // start with cheap raymarching
        int missedStepCount = 0;

        float cheapLOD = 1.0f;
        float bigStep = sample_step * 2.0f; //pow(2.0f, cheapLOD);

        float3 smallStepMarching = sample_step * dir;
        float3 bigStepMarching = bigStep * dir;

        // To prevent raymarching artifact, use a random value
        float3 raymarchingDistance = smallStepMarching * raymarchOffset;

        float3 magLightDirection = 2.0 * VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.xyz;

        bool pickedFirstHit = false;

        float cosTheta = dot(dir, VolumetricCloudsCBuffer.g_VolumetricClouds.lightDirection.xyz);
        float3 rayPos = sampleStart;

        float4 windWithVelocity = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].StandardPosition;
        float4 windWithVelocity_2nd = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].StandardPosition;

        float3 biasedCloudPos = 4.5f * (VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WindDirection.xyz + float3(0.0, 0.1, 0.0));
        float3 biasedCloudPos_2nd = 4.5f * (VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WindDirection.xyz + float3(0.0, 0.1, 0.0));

        float3 cloudTopOffsetWithWindDir = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudTopOffset * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WindDirection.xyz;
        float3 cloudTopOffsetWithWindDir_2nd = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudTopOffset * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WindDirection.xyz;

        float DetailShapeTilingDivCloudSize = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailShapeTiling / VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize;
        float DetailShapeTilingDivCloudSize_2nd = VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailShapeTiling / VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize;

        float LODbias = 0.0f;
     
        for (uint j = 0; j < sample_count; j++)
        {
            rayPos += raymarchingDistance;

            if (linearDepth < distance(startPos, rayPos))
            {
                break;
            }

            float3 currentProj = getProjectedShellPoint(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz);
            float height_fraction = getRelativeHeightAccurate(VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart, rayPos, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness);

            float3 currentProj_2nd = getProjectedShellPoint(VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart, rayPos, VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz);
            float height_fraction_2nd = getRelativeHeightAccurate(VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart, rayPos, currentProj_2nd, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness);

            if (!detailedSample)
            {
                // Get the density from current rayPos
                float sampleResult = SampleDensity(
                    rayPos, cheapLOD, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir, windWithVelocity,
                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                    true);

                float sampleResult_2nd = SampleDensity(
                    rayPos, cheapLOD, height_fraction_2nd, currentProj_2nd, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir_2nd, windWithVelocity_2nd,
                    biasedCloudPos_2nd, DetailShapeTilingDivCloudSize_2nd,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationAngle,
                    true);

                if (sampleResult > 0.0f || sampleResult_2nd > 0.0f)
                {
                    // If it hit the clouds, change to the expensive raymarching
                    detailedSample = true;

                    raymarchingDistance = -bigStepMarching;
                    missedStepCount = 0;
                    continue;
                }
                else
                    raymarchingDistance = bigStepMarching;
            }
            else
            {
                // Get the density from current rayPos
                float sampleResult = SampleDensity(
                    rayPos, LODbias, height_fraction, currentProj, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir, windWithVelocity,
                    biasedCloudPos, DetailShapeTilingDivCloudSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                    false);

                float sampleResult_2nd = SampleDensity(
                    rayPos, LODbias, height_fraction_2nd, currentProj_2nd, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].AnvilBias,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlTextureTiling,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailStrenth,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporIntensity,
                    cloudTopOffsetWithWindDir_2nd, windWithVelocity_2nd,
                    biasedCloudPos_2nd, DetailShapeTilingDivCloudSize_2nd,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureSize,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetZ,
                    VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationAngle,
                    false);

                if (sampleResult == 0.0 && sampleResult_2nd == 0.0)
                {
                    missedStepCount++;

                    // If expensive raymarching failed more 10 times, go back to cheap raymarching
                    if (missedStepCount > 10)
                        detailedSample = false;
                }
                else
                {
                    // Get the first hit position against clouds to use it for reprojection
                    if (!pickedFirstHit)
                    {
                        depth = distance(rayPos, startPos);
                        pickedFirstHit = true;
                    }

                    if (sampleResult > 0.0)
                    {
                        // If it hit the clouds, get the light enery from current rayPos and accumulate it
                        float sampledAlpha = sampleResult * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudDensity;
                        float sampledEnergy = SampleEnergy(
                            rayPos, magLightDirection, height_fraction, currentProj,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].EarthRadiusAddCloudsLayerStart,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].LayerThickness,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].AnvilBias,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].CurlTextureTiling,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].DetailStrenth,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RisingVaporIntensity,
                            cloudTopOffsetWithWindDir, windWithVelocity,
                            biasedCloudPos, DetailShapeTilingDivCloudSize,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].WeatherTextureSize,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationPivotOffsetZ,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[0].RotationAngle,
                            sampleResult, transStepSize, cosTheta, 0.0f);

                        float oneMinusAlpha = 1.0 - alpha;
                        sampledAlpha *= oneMinusAlpha;

                        intensity += sampledAlpha * sampledEnergy;
                        alpha += sampledAlpha;
                    }

                    if (sampleResult_2nd > 0.0)
                    {
                        // If it hit the clouds, get the light enery from current rayPos and accumulate it
                        float sampledAlpha = sampleResult_2nd * VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudDensity;
                        float sampledEnergy = SampleEnergy(
                            rayPos, magLightDirection, height_fraction_2nd, currentProj_2nd,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.EarthCenter.xyz,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].EarthRadiusAddCloudsLayerStart,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].LayerThickness,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudSize, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].BaseShapeTiling, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudCoverage, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CloudType, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].AnvilBias,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlStrenth, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].CurlTextureTiling,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].DetailStrenth,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporUpDirection, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporScale, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RisingVaporIntensity,
                            cloudTopOffsetWithWindDir_2nd, windWithVelocity_2nd,
                            biasedCloudPos_2nd, DetailShapeTilingDivCloudSize_2nd,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureOffsetZ, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].WeatherTextureSize,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetX, VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationPivotOffsetZ,
                            VolumetricCloudsCBuffer.g_VolumetricClouds.m_DataPerLayer[1].RotationAngle,
                            sampleResult_2nd, transStepSize, cosTheta, 0.0f);

                        float oneMinusAlpha = 1.0 - alpha;
                        sampledAlpha *= oneMinusAlpha;

                        intensity += sampledAlpha * sampledEnergy;
                        alpha += sampledAlpha;
                    }

                    if (alpha >= 1.0f)
                    {
                        intensity /= alpha;
                        depth = PackFloat16(depth);
                        return 1.0f;
                    }
                }

                raymarchingDistance = smallStepMarching;
            }
        }

        depth = PackFloat16(depth);
        return alpha;
    };

    // Sample previous information and return the result that projected texture coordinates are out bounded or not
    float4 SamplePrev(float2 prevUV, thread float(& outOfBound))
    {
        prevUV.xy = float2((prevUV.x + 1.0) * 0.5, (1.0 - prevUV.y) * 0.5);
        outOfBound = step(0.0, max(max(-prevUV.x, -prevUV.y), max(prevUV.x, prevUV.y) - 1.0));

        return g_PrevFrameTexture.sample(g_LinearClampSampler, prevUV.xy, level(0));
    };

    // Correct current texture coordinates and get the result of Volumetric clouds's rendering 
    float4 SampleCurrent(float2 uv, float2 _Jitter)
    {
        uv = uv - (_Jitter - 1.5) * (1.0 / VolumetricCloudsCBuffer.g_VolumetricClouds.TimeAndScreenSize.zw);
        return LowResCloudTexture.sample(g_PointClampSampler, uv, level(0));
    };

    // Check whether current texture coordinates should be updated or not
    float ShouldbeUpdated(float2 uv, float2 jitter)
    {
        float2 texelRelativePos = fmod(uv * VolumetricCloudsCBuffer.g_VolumetricClouds.TimeAndScreenSize.zw, 4.0);
        texelRelativePos -= jitter;
        float2 valid = saturate(2.0 * (float2(0.5, 0.5) - abs(texelRelativePos - float2(0.5, 0.5))));
        return valid.x * valid.y;
    };
};
