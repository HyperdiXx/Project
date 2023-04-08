
static const float PI = 3.14159265359;
static const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

// NDF function
// Disney's alpha = roughness ^ 2
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 brdf(in float3 L, in float3 V, in float3 N, in float3 Albedo, in float3 Radiance, in float R, in float M, in float3 FRef)
{
    const float3 H = normalize(L + V);
    const float NdotL = max(0.0, dot(N, L));
    const float NdotH = max(0.0, dot(N, H));
    const float NdotV = max(0.0, dot(N, V));

    // Calculate Fresnel term for direct lighting. 
    const float3 F = fresnelSchlick(FRef, max(0.0, dot(H, L)));
    // Calculate normal distribution for specular BRDF.
    const float D = ndfGGX(NdotH, R);
    // Calculate geometric attenuation for specular BRDF.
    const float G = gaSchlickGGX(NdotL, NdotV, R);

    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), M);

    float3 diffuseBRDF = kd * Albedo;
    float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * NdotL * NdotV);


    return float3((diffuseBRDF + specularBRDF) * Radiance * NdotL);
}

float3 ibl()
{
    return float3(1.0, 1.0, 1.0);
}