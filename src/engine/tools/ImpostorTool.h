#ifndef AE_IMPOSTORTOOL_H
#define AE_IMPOSTORTOOL_H

#include "../System.h"
#include "../mesh/Impostor.h"
#include "../mesh/Mesh.h"
#include "../Camera.h"

namespace Atlas {

    namespace Tools {

        class ImpostorTool {

        public:
            static Mesh::Impostor* GenerateImpostor(Mesh::Mesh* mesh,
                int32_t views, int32_t resolution, bool octahedron = false);

        private:
            static vec3 HemiOctahedronToUnitVector(vec2 coord);

            static vec2 UnitVectorToHemiOctahedron(vec3 dir);

            static vec3 OctahedronToUnitVector(vec2 coord);

            static vec2 UnitVectorToOctahedron(vec3 dir);

        };

    }

}

#endif