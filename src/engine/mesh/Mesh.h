#ifndef AE_MESH_H
#define AE_MESH_H

#include "../System.h"
#include "../Material.h"
#include "resource/Resource.h"
#include "../buffer/VertexArray.h"

#include "MeshData.h"
#include "Impostor.h"

#include "../graphics/Buffer.h"
#include "../graphics/BLAS.h"

namespace Atlas {

    namespace Mesh {

        enum class MeshMobility {
            Stationary = 0,
            Movable
        };

        class Mesh {

        public:
            Mesh() = default;

            explicit Mesh(MeshData& meshData,
                MeshMobility mobility = MeshMobility::Stationary);

            void SetTransform(mat4 transform);

            void UpdateData();

            bool CheckForLoad();

            std::string name = "";

            MeshData data;
            MeshMobility mobility = MeshMobility::Stationary;

            Buffer::VertexArray vertexArray;
            Buffer::IndexBuffer indexBuffer;
            Buffer::VertexBuffer vertexBuffer;

            Ref<Graphics::BLAS> blas = nullptr;

            Impostor* impostor = nullptr;

            bool cullBackFaces = true;
            bool depthTest = true;

            bool castShadow = true;
            bool vegetation = false;

            int32_t allowedShadowCascades = 6;

            float impostorDistance = 300.0f;
            float impostorShadowDistance = 100.0f;

            bool invertUVs = false;

        private:
            bool isLoaded = false;

        };


    }

}

#endif