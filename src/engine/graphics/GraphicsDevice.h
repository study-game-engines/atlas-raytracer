#ifndef AE_GRAPHICSDEVICE_H
#define AE_GRAPHICSDEVICE_H

#include "Common.h"
#include "Surface.h"
#include "Queue.h"
#include "SwapChain.h"
#include "CommandList.h"
#include "Shader.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "Image.h"
#include "Sampler.h"
#include "Descriptor.h"
#include "QueryPool.h"
#include "BLAS.h"
#include "TLAS.h"
#include "Framebuffer.h"
#include "MemoryManager.h"

#include "../common/Ref.h"

#include <optional>
#include <vector>
#include <mutex>

namespace Atlas {

    namespace Graphics {

        class Instance;
        class ImguiWrapper;

        struct DeviceSupport {
            bool hardwareRayTracing = false;
            bool shaderPrintf = false;
        };

        struct CommandListSubmission {
            CommandList* cmd;

            VkPipelineStageFlags waitStage;
        };

        class FrameData {
        public:
            VkSemaphore semaphore = VK_NULL_HANDLE;
            VkFence fence = VK_NULL_HANDLE;

            std::mutex commandListsMutex;
            std::vector<CommandList*> commandLists;

            std::mutex submissionMutex;
            std::vector<CommandList*> submittedCommandLists;

            std::vector<CommandListSubmission> submissions;

            void WaitAndReset(VkDevice device) {
                if (submittedCommandLists.size() > 0) {
                    std::vector<VkFence> fences;
                    for (auto commandList : submittedCommandLists) {
                        fences.push_back(commandList->fence);
                    }
                    VK_CHECK(vkWaitForFences(device, uint32_t(fences.size()), fences.data(), true, 1000000000))
                    VK_CHECK(vkResetFences(device, uint32_t(fences.size()), fences.data()))
                }

                for (auto commandList : submittedCommandLists) {
                    commandList->ResetDescriptors();
                    commandList->wasSwapChainAccessed = false;
                    commandList->isLocked = false;
                }
                submittedCommandLists.clear();
                submissions.clear();
            }

            void RecreateSemaphore(VkDevice device) {
                vkDestroySemaphore(device, semaphore, nullptr);

                VkSemaphoreCreateInfo semaphoreInfo = Initializers::InitSemaphoreCreateInfo();
                VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore))
            }
        };

        class GraphicsDevice {

            friend ImguiWrapper;

        public:
            explicit GraphicsDevice(Surface* surface, bool enableValidationLayers = false);

            GraphicsDevice(const GraphicsDevice& that) = delete;

            ~GraphicsDevice();

            GraphicsDevice& operator=(const GraphicsDevice& that) = delete;

            SwapChain* CreateSwapChain(VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR,
                ColorSpace preferredColorSpace = SRGB_NONLINEAR);

            Ref<RenderPass> CreateRenderPass(RenderPassDesc desc);

            Ref<FrameBuffer> CreateFrameBuffer(FrameBufferDesc desc);

            Ref<Shader> CreateShader(ShaderDesc desc);

            Ref<Pipeline> CreatePipeline(GraphicsPipelineDesc desc);

            Ref<Pipeline> CreatePipeline(ComputePipelineDesc desc);

            Ref<Buffer> CreateBuffer(BufferDesc desc);

            Ref<MultiBuffer> CreateMultiBuffer(BufferDesc desc);

            Ref<Image> CreateImage(ImageDesc desc);

            Ref<Sampler> CreateSampler(SamplerDesc desc);

            Ref<DescriptorPool> CreateDescriptorPool();

            Ref<QueryPool> CreateQueryPool(QueryPoolDesc desc);

            Ref<BLAS> CreateBLAS(BLASDesc desc);

            Ref<TLAS> CreateTLAS(TLASDesc desc);

            CommandList* GetCommandList(QueueType queueType = QueueType::GraphicsQueue,
                bool frameIndependentList = false);

            void SubmitCommandList(CommandList* cmd, VkPipelineStageFlags waitStage =
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, ExecutionOrder order = ExecutionOrder::Sequential);

            void FlushCommandList(CommandList* cmd);

            void CompleteFrame();

            bool CheckFormatSupport(VkFormat format, VkFormatFeatureFlags featureFlags);

            QueueRef GetAndLockQueue(QueueType queueType);

            void WaitForIdle() const;

            void ForceMemoryCleanup();

            Instance* instance = nullptr;
            SwapChain* swapChain = nullptr;
            MemoryManager* memoryManager = nullptr;
            Surface* surface = nullptr;

            VkPhysicalDevice physicalDevice;
            VkDevice device;

            VkPhysicalDeviceProperties2 deviceProperties = {};
            VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties = {};
            VkPhysicalDeviceAccelerationStructurePropertiesKHR accelerationStructureProperties = {};

            VkPhysicalDeviceFeatures2 features = {};
            VkPhysicalDeviceVulkan11Features features11 = {};
            VkPhysicalDeviceVulkan12Features features12 = {};

            DeviceSupport support;

            bool isComplete = false;

            static GraphicsDevice* DefaultDevice;

        private:
            struct QueueFamily {
                uint32_t index;

                std::vector<Ref<Queue>> queues;
                std::vector<float> queuePriorities;

                bool supportsGraphics;
                bool supportsTransfer;
                bool supportsPresentation;
            };

            struct QueueFamilyIndices {
                std::optional<uint32_t> queueFamilies[3];
                std::vector<QueueFamily> families;

                bool IsComplete() {
                    return queueFamilies[QueueType::GraphicsQueue].has_value() &&
                        queueFamilies[QueueType::PresentationQueue].has_value() &&
                        queueFamilies[QueueType::TransferQueue].has_value();
                }
            };

            QueueRef SubmitAllCommandLists();

            void SubmitCommandList(CommandListSubmission* submission, VkSemaphore previousSemaphore,
                const QueueRef& queue, const QueueRef& nextQueue);

            bool SelectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface,
                const std::vector<const char*>& requiredExtensions, std::vector<const char*>& optionalExtensions);

            int32_t RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR surface,
                const std::vector<const char*>& requiredExtensions, std::vector<const char*>& optionalExtensions);

            bool FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

            std::vector<VkDeviceQueueCreateInfo> CreateQueueInfos();

            bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice,
                const std::vector<const char*>& extensionNames);

            std::vector<const char*> CheckDeviceOptionalExtensionSupport(VkPhysicalDevice physicalDevice,
                std::vector<const char*>& extensionNames);

            void BuildPhysicalDeviceFeatures(VkPhysicalDevice device);

            void GetPhysicalDeviceProperties(VkPhysicalDevice device);

            void CreateDevice(const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos,
                const std::vector<const char*>& extensions, bool enableValidationLayers);

            bool CheckForWindowResize();

            void CreateFrameData();

            void DestroyFrameData();

            FrameData* GetFrameData();

            void DestroyUnusedGraphicObjects();

            CommandList* GetOrCreateCommandList(QueueType queueType, std::mutex& mutex,
                std::vector<CommandList*>& commandLists, bool frameIndependent);

            QueueRef FindAndLockQueue(QueueType queueType);

            QueueRef FindAndLockQueue(uint32_t familyIndex);

            QueueFamilyIndices queueFamilyIndices;

            std::vector<Ref<RenderPass>> renderPasses;
            std::vector<Ref<FrameBuffer>> frameBuffers;
            std::vector<Ref<Shader>> shaders;
            std::vector<Ref<Pipeline>> pipelines;
            std::vector<Ref<Buffer>> buffers;
            std::vector<Ref<MultiBuffer>> multiBuffers;
            std::vector<Ref<Image>> images;
            std::vector<Ref<Sampler>> samplers;
            std::vector<Ref<DescriptorPool>> descriptorPools;
            std::vector<Ref<QueryPool>> queryPools;
            std::vector<Ref<BLAS>> blases;
            std::vector<Ref<TLAS>> tlases;

            std::mutex commandListsMutex;
            std::vector<CommandList*> commandLists;

            int32_t frameIndex = 0;
            FrameData frameData[FRAME_DATA_COUNT];

            int32_t windowWidth = 0;
            int32_t windowHeight = 0;
        };

    }

}

#endif
