#ifndef MASTERRENDERER_H
#define MASTERRENDERER_H

#include "../System.h"
#include "../VertexArray.h"

#include "GeometryRenderer.h"
#include "TerrainRenderer.h"
#include "ShadowRenderer.h"
#include "DirectionalVolumetricRenderer.h"
#include "DirectionalLightRenderer.h"
#include "PointLightRenderer.h"
#include "SkyboxRenderer.h"
#include "AtmosphereRenderer.h"
#include "PostProcessRenderer.h"
#include "TextRenderer.h"

class MasterRenderer {

public:
	MasterRenderer();

	void RenderScene(Window* window, RenderTarget* target, Camera* camera, Scene* scene);

	void RenderTexture(Window* window, Texture* texture, float x, float y, float width, float height,
		bool alphaBlending = false, Framebuffer* framebuffer = nullptr);

	void RenderTexture(Window* window, Texture* texture, float x, float y, float width, float height,
		vec4 clipArea, vec4 blendArea, bool alphaBlending = false, Framebuffer* framebuffer = nullptr);

	void RenderRectangle(Window* window, vec4 color, float x, float y, float width, float height,
		bool alphaBlending = false, Framebuffer* framebuffer = nullptr);

	void RenderRectangle(Window* window, vec4 color, float x, float y, float width, float height,
		vec4 clipArea, vec4 blendArea, bool alphaBlending = false, Framebuffer* framebuffer = nullptr);

	~MasterRenderer();

	TextRenderer* textRenderer;

	static string rectangleVertexPath;
	static string rectangleFragmentPath;

private:
	void GetUniforms();

	VertexArray * vertexArray;

	Shader* rectangleShader;
	Shader* texturedRectangleShader;

	Uniform* rectangleProjectionMatrix;
	Uniform* rectangleOffset;
	Uniform* rectangleScale;
	Uniform* rectangleColor;
	Uniform* rectangleClipArea;
	Uniform* rectangleBlendArea;

	Uniform* texturedRectangleProjectionMatrix;
	Uniform* texturedRectangleOffset;
	Uniform* texturedRectangleScale;
	Uniform* texturedRectangleTexture;
	Uniform* texturedRectangleClipArea;
	Uniform* texturedRectangleBlendArea;

	GeometryRenderer* geometryRenderer;
	TerrainRenderer* terrainRenderer;
	ShadowRenderer* shadowRenderer;
	DirectionalVolumetricRenderer* directionalVolumetricRenderer;
	DirectionalLightRenderer* directionalLightRenderer;
	PointLightRenderer* pointLightRenderer;
	SkyboxRenderer* skyboxRenderer;
	AtmosphereRenderer* atmosphereRenderer;
	PostProcessRenderer* postProcessRenderer;

};

#endif