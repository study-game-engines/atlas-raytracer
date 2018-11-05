#include "postprocessrenderer.h"
#include "masterrenderer.h"

PostProcessRenderer::PostProcessRenderer(const char* vertexSource, const char* fragmentSoure) {

	rectangleVAO = MasterRenderer::GenerateRectangleVAO();

	shader = new Shader();

	shader->AddComponent(VERTEX_SHADER, vertexSource);
	shader->AddComponent(FRAGMENT_SHADER, fragmentSoure);

	shader->Compile();

	GetUniforms();

}

void PostProcessRenderer::Render(Window* window, RenderTarget* target, Camera* camera, Scene* scene, bool masterRenderer) {

	glViewport(window->viewport->x, window->viewport->y, window->viewport->width, window->viewport->height);

	PostProcessing* postProcessing = scene->postProcessing;

	bool shaderChanged = false;

	bool hasFilmicTonemappingMacro = shader->HasMacro("FILMIC_TONEMAPPING");
	bool hasVignetteMacro = shader->HasMacro("VIGNETTE");
	bool hasChromaticAberrationMacro = shader->HasMacro("CHROMATIC_ABERRATION");

	if (postProcessing->filmicTonemapping && !hasFilmicTonemappingMacro) {
		shader->AddMacro("FILMIC_TONEMAPPING");
		shaderChanged = true;
	}
	else if (!postProcessing->filmicTonemapping && hasFilmicTonemappingMacro) {
		shader->RemoveMacro("FILMIC_TONEMAPPING");
		shaderChanged = true;
	}

	if (postProcessing->vignette->use && !hasVignetteMacro) {
		shader->AddMacro("VIGNETTE");
		shaderChanged = true;
	}
	else if (!postProcessing->vignette->use && hasVignetteMacro) {
		shader->RemoveMacro("VIGNETTE");
		shaderChanged = true;
	}

	if (postProcessing->chromaticAberration->use && !hasChromaticAberrationMacro) {
		shader->AddMacro("CHROMATIC_ABERRATION");
		shaderChanged = true;
	}
	else if (!postProcessing->chromaticAberration->use && hasChromaticAberrationMacro) {
		shader->RemoveMacro("CHROMATIC_ABERRATION");
		shaderChanged = true;
	}

	if (shaderChanged) {
		shader->Compile();
		GetUniforms();
	}

	shader->Bind();

	exposure->SetValue(postProcessing->exposure);
	saturation->SetValue(postProcessing->saturation);

	if (postProcessing->chromaticAberration->use) {
		float reversedValue = postProcessing->chromaticAberration->colorsReversed ? 1.0f : 0.0f;
		aberrationStrength->SetValue(postProcessing->chromaticAberration->strength);
		aberrationReversed->SetValue(reversedValue);
	}

	if (postProcessing->vignette->use) {
		vignetteOffset->SetValue(postProcessing->vignette->offset);
		vignettePower->SetValue(postProcessing->vignette->power);
		vignetteStrength->SetValue(postProcessing->vignette->strength);
		vignetteColor->SetValue(postProcessing->vignette->color);
	}

	target->postProcessingFramebuffer->components[0]->Bind(GL_TEXTURE0);

	glBindVertexArray(rectangleVAO);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

}

void PostProcessRenderer::GetUniforms() {

	delete exposure;
	delete saturation;
	delete bloomPassses;
	delete aberrationStrength;
	delete aberrationReversed;
	delete vignetteOffset;
	delete vignettePower;
	delete vignetteStrength;
	delete vignetteColor;

	exposure = shader->GetUniform("exposure");
	saturation = shader->GetUniform("saturation");
	bloomPassses = shader->GetUniform("bloomPassses");
	aberrationStrength = shader->GetUniform("aberrationStrength");
	aberrationReversed = shader->GetUniform("aberrationReversed");
	vignetteOffset = shader->GetUniform("vignetteOffset");
	vignettePower = shader->GetUniform("vignettePower");
	vignetteStrength = shader->GetUniform("vignetteStrength");
	vignetteColor = shader->GetUniform("vignetteColor");

}