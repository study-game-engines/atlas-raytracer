#extension GL_EXT_nonuniform_qualifier : enable

#include <terrainMaterial.hsh>
#include <../globals.hsh>
#include <../common/utility.hsh>

layout (location = 0) out vec3 baseColorFS;
layout (location = 1) out vec3 normalFS;
layout (location = 2) out vec3 geometryNormalFS;
layout (location = 3) out vec3 roughnessMetalnessAoFS;
layout (location = 4) out uint materialIdxFS;
layout (location = 5) out vec2 velocityFS;

layout (set = 3, binding = 1) uniform sampler2D normalMap;
layout (set = 3, binding = 2) uniform usampler2D splatMap;
layout (set = 3, binding = 3) uniform sampler2DArray baseColorMaps;
layout (set = 3, binding = 4) uniform sampler2DArray roughnessMaps;
layout (set = 3, binding = 5) uniform sampler2DArray aoMaps;
layout (set = 3, binding = 6) uniform sampler2DArray normalMaps;

layout (set = 3, binding = 8, std140) uniform UBO {
    TerrainMaterial materials[128];
} Materials;

layout (set = 3, binding = 9, std140) uniform UniformBuffer {
    vec4 frustumPlanes[6];

    float heightScale;
    float displacementDistance;

    float tessellationFactor;
    float tessellationSlope;
    float tessellationShift;
    float maxTessellationLevel;
} Uniforms;

layout(push_constant) uniform constants {
    float nodeSideLength;
    float tileScale;
    float patchSize;
    float normalTexelSize;

    float leftLoD;
    float topLoD;
    float rightLoD;
    float bottomLoD;

    vec2 nodeLocation;
} PushConstants;

layout(location=0) in vec2 materialTexCoords;
layout(location=1) in vec2 texCoords;
layout(location=2) in vec3 ndcCurrent;
layout(location=3) in vec3 ndcLast;
// layout(location=4) flat in uvec4 materialIndicesTE;

vec3 SampleBaseColor(vec2 off, uvec4 indices, vec4 tiling) {
    
    vec3 q00 = texture(baseColorMaps, vec3(materialTexCoords / 4.0 * tiling.x, nonuniformEXT(float(indices.x)))).rgb;
    vec3 q10 = indices.y != indices.x ? texture(baseColorMaps, vec3(materialTexCoords / 4.0 * tiling.y, nonuniformEXT(float(indices.y)))).rgb : q00;
    vec3 q01 = indices.z != indices.x ? texture(baseColorMaps, vec3(materialTexCoords / 4.0 * tiling.z, nonuniformEXT(float(indices.z)))).rgb : q00;
    vec3 q11 = indices.w != indices.x ? texture(baseColorMaps, vec3(materialTexCoords / 4.0 * tiling.w, nonuniformEXT(float(indices.w)))).rgb : q00;
    
    // Interpolate samples horizontally
    vec3 h0 = mix(q00, q10, off.x);
    vec3 h1 = mix(q01, q11, off.x);
    
    // Interpolate samples vertically
    return mix(h0, h1, off.y);    
    
}

float SampleRoughness(vec2 off, uvec4 indices, vec4 tiling) {

    float q00 = texture(roughnessMaps, vec3(materialTexCoords / 4.0 * tiling.x, nonuniformEXT(float(indices.x)))).r;
    float q10 = indices.y != indices.x ? texture(roughnessMaps, vec3(materialTexCoords / 4.0 * tiling.y, nonuniformEXT(float(indices.y)))).r : q00;
    float q01 = indices.z != indices.x ? texture(roughnessMaps, vec3(materialTexCoords / 4.0 * tiling.z, nonuniformEXT(float(indices.z)))).r : q00;
    float q11 = indices.w != indices.x ? texture(roughnessMaps, vec3(materialTexCoords / 4.0 * tiling.w, nonuniformEXT(float(indices.w)))).r : q00;
    
    // Interpolate samples horizontally
    float h0 = mix(q00, q10, off.x);
    float h1 = mix(q01, q11, off.x);
    
    // Interpolate samples vertically
    return mix(h0, h1, off.y);    
    
}

float SampleAo(vec2 off, uvec4 indices, vec4 tiling) {
    
    float q00 = texture(aoMaps, vec3(materialTexCoords / 4.0 * tiling.x, nonuniformEXT(float(indices.x)))).r;
    float q10 = indices.y != indices.x ? texture(aoMaps, vec3(materialTexCoords / 4.0 * tiling.y, nonuniformEXT(float(indices.y)))).r : q00;
    float q01 = indices.z != indices.x ? texture(aoMaps, vec3(materialTexCoords / 4.0 * tiling.z, nonuniformEXT(float(indices.z)))).r : q00;
    float q11 = indices.w != indices.x ? texture(aoMaps, vec3(materialTexCoords / 4.0 * tiling.w, nonuniformEXT(float(indices.w)))).r : q00;
    
    // Interpolate samples horizontally
    float h0 = mix(q00, q10, off.x);
    float h1 = mix(q01, q11, off.x);
    
    // Interpolate samples vertically
    return mix(h0, h1, off.y);    
    
}

vec3 SampleNormal(vec2 off, uvec4 indices, vec4 tiling) {

    vec3 q00 = texture(normalMaps, vec3(materialTexCoords / 4.0 * tiling.x, nonuniformEXT(float(indices.x)))).rgb;
    vec3 q10 = indices.y != indices.x ? texture(normalMaps, vec3(materialTexCoords / 4.0 * tiling.y, nonuniformEXT(float(indices.y)))).rgb : q00;
    vec3 q01 = indices.z != indices.x ? texture(normalMaps, vec3(materialTexCoords / 4.0 * tiling.z, nonuniformEXT(float(indices.z)))).rgb : q00;
    vec3 q11 = indices.w != indices.x ? texture(normalMaps, vec3(materialTexCoords / 4.0 * tiling.w, nonuniformEXT(float(indices.w)))).rgb : q00;
    
    // Interpolate samples horizontally
    vec3 h0 = mix(q00, q10, off.x);
    vec3 h1 = mix(q01, q11, off.x);
    
    // Interpolate samples vertically
    return mix(h0, h1, off.y);    
    
}

float Interpolate(float q00, float q10, float q01, float q11, vec2 off) {

    // Interpolate samples horizontally
    float h0 = mix(q00, q10, off.x);
    float h1 = mix(q01, q11, off.x);
    
    // Interpolate samples vertically
    return mix(h0, h1, off.y);    

}

void main() {

    uvec4 indices;
    vec2 coords = materialTexCoords;

    vec2 tex = materialTexCoords / PushConstants.tileScale;
    vec2 off = tex - floor(tex);

    off = vec2(off.x + (0.5 * sin(coords.y)
        + 0.7 * cos(coords.y)) / PushConstants.tileScale,
        off.y + (0.4 * cos(coords.x * 2.0) + 0.6 * cos(coords.x)) / PushConstants.tileScale);
        
    vec2 splatOffset = floor(off);
    off = off - floor(off);

    float texel = 1.0 / (8.0 * PushConstants.patchSize);
    tex = (floor(coords / PushConstants.nodeSideLength / texel) + splatOffset) * texel;
    indices.x = textureLod(splatMap, tex, 0).r;
    indices.y = textureLod(splatMap, tex + vec2(texel, 0.0), 0).r;
    indices.z = textureLod(splatMap, tex + vec2(0.0, texel), 0).r;
    indices.w = textureLod(splatMap, tex + vec2(texel, texel), 0).r;
    
    vec4 tiling = vec4(
        Materials.materials[indices.x].tiling,
        Materials.materials[indices.y].tiling,
        Materials.materials[indices.z].tiling,
        Materials.materials[indices.w].tiling
    );

    baseColorFS = SampleBaseColor(off, indices, tiling);

    float roughness = Interpolate(
        Materials.materials[indices.x].roughness,
        Materials.materials[indices.y].roughness,
        Materials.materials[indices.z].roughness,
        Materials.materials[indices.w].roughness,
        off
    );
    float metalness = Interpolate(
        Materials.materials[indices.x].metalness,
        Materials.materials[indices.y].metalness,
        Materials.materials[indices.z].metalness,
        Materials.materials[indices.w].metalness,
        off
        );
    float ao = Interpolate(
        Materials.materials[indices.x].ao,
        Materials.materials[indices.y].ao,
        Materials.materials[indices.z].ao,
        Materials.materials[indices.w].ao,
        off
        );
    
    materialIdxFS = Materials.materials[indices.x].idx;

    // We should move this to the tesselation evaluation shader
    // so we only have to calculate these normals once. After that 
    // we can pass a TBN matrix to this shader
    tex = vec2(PushConstants.normalTexelSize) + texCoords *
        (1.0 - 3.0 * PushConstants.normalTexelSize)
        + 0.5 * PushConstants.normalTexelSize;
    vec3 norm = 2.0 * texture(normalMap, tex).rgb - 1.0;

    geometryNormalFS = 0.5 * normalize(mat3(globalData.vMatrix) * norm) + 0.5;
    
#ifndef DISTANCE
    // Normal mapping only for near tiles
    float normalScale = Interpolate(
        Materials.materials[indices.x].normalScale,
        Materials.materials[indices.y].normalScale,
        Materials.materials[indices.z].normalScale,
        Materials.materials[indices.w].normalScale,
        off
        );
    normalFS = SampleNormal(off, indices, tiling);
    vec3 tang = vec3(1.0, 0.0, 0.0);
    tang.y = -((norm.x*tang.x) / norm.y) - ((norm.z*tang.z) / norm.y);
    tang = normalize(tang);
    vec3 bitang = normalize(cross(tang, norm));
    mat3 tbn = mat3(tang, bitang, norm);
    normalFS = normalize(tbn * (2.0 * normalFS - 1.0));
    normalFS = mix(norm, normalFS, normalScale);
    ao *= SampleAo(off, indices, tiling);
    roughness *= SampleRoughness(off, indices, tiling);
#else
    normalFS = norm;
#endif    
    
    normalFS = 0.5 * normalize(mat3(globalData.vMatrix) * normalFS) + 0.5;
    roughnessMetalnessAoFS = vec3(roughness, metalness, ao);
    
    // Calculate velocity
    vec2 ndcL = ndcLast.xy / ndcLast.z;
    vec2 ndcC = ndcCurrent.xy / ndcCurrent.z;

    ndcL -= globalData.jitterLast;
    ndcC -= globalData.jitterCurrent;

    velocityFS = (ndcL - ndcC) * 0.5;
    
}