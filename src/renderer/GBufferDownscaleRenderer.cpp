#include "GBufferDownscaleRenderer.h"

namespace Atlas {

    namespace Renderer {

        GBufferDownscaleRenderer::GBufferDownscaleRenderer() {

            downscale.AddStage(AE_COMPUTE_STAGE, "downsampleGBuffer2x.csh");
            downscale.Compile();

            downscaleDepthOnly.AddStage(AE_COMPUTE_STAGE, "downsampleGBuffer2x.csh");
            downscaleDepthOnly.AddMacro("DEPTH_ONLY");
            downscaleDepthOnly.Compile();

        }

        void GBufferDownscaleRenderer::Render(Viewport *viewport, RenderTarget *target, 
            Camera *camera, Scene::Scene *scene) {


            
        }

        void GBufferDownscaleRenderer::Downscale(RenderTarget* target) {

            Profiler::BeginQuery("Downsample GBuffer");

            downscale.Bind();

            auto rt = target->GetDownsampledTextures(RenderResolution::FULL_RES);
            auto downsampledRt = target->GetDownsampledTextures(RenderResolution::HALF_RES);

            Downscale(rt, downsampledRt);

            Profiler::EndQuery();

        }

        void GBufferDownscaleRenderer::DownscaleDepthOnly(RenderTarget* target) {

            Profiler::BeginQuery("Downsample GBuffer depth only");

            downscaleDepthOnly.Bind();

            auto rt = target->GetDownsampledTextures(RenderResolution::FULL_RES);
            auto downsampledRt = target->GetDownsampledTextures(RenderResolution::HALF_RES);

            Downscale(rt, downsampledRt);

            Profiler::EndQuery();

        }

        void GBufferDownscaleRenderer::Downscale(DownsampledRenderTarget* rt, DownsampledRenderTarget* downsampledRt) {

            auto depthIn = rt->depthTexture;
            auto normalIn = rt->normalTexture;
            auto geometryNormalIn = rt->geometryNormalTexture;
            auto roughnessMetallicAoIn = rt->roughnessMetallicAoTexture;
            auto velocityIn = rt->velocityTexture;

            auto depthOut = downsampledRt->depthTexture;
            auto normalOut = downsampledRt->normalTexture;
            auto geometryNormalOut = downsampledRt->geometryNormalTexture;
            auto roughnessMetallicAoOut = downsampledRt->roughnessMetallicAoTexture;
            auto velocityOut = downsampledRt->velocityTexture;
            auto offsetOut = downsampledRt->offsetTexture;

            ivec2 res = ivec2(depthOut->width, depthOut->height);

            ivec2 groupCount = ivec2(res.x / 8, res.y / 8);
            groupCount.x += ((res.x % 8 == 0) ? 0 : 1);
            groupCount.y += ((res.y % 8 == 0) ? 0 : 1);

            depthIn->Bind(GL_TEXTURE0);
            normalIn->Bind(GL_TEXTURE1);
            geometryNormalIn->Bind(GL_TEXTURE2);
            roughnessMetallicAoIn->Bind(GL_TEXTURE3);
            velocityIn->Bind(GL_TEXTURE4);

            depthOut->Bind(GL_WRITE_ONLY, 0);
            normalOut->Bind(GL_WRITE_ONLY, 1);
            geometryNormalOut->Bind(GL_WRITE_ONLY, 2);
            roughnessMetallicAoOut->Bind(GL_WRITE_ONLY, 3);
            velocityOut->Bind(GL_WRITE_ONLY, 4);
            offsetOut->Bind(GL_WRITE_ONLY, 5);

            glDispatchCompute(groupCount.x, groupCount.y, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        }

    }

}