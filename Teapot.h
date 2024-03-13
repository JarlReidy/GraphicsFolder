
#pragma once
#include "GeometryHelper.h"
#include "BezierPatch.h"


class Teapot : public GeometryHelper
{
    std::vector<glm::vec3> positions;//pos of vertices
    std::vector<glm::vec3> normals;//normals
    std::vector<glm::vec3> tangents;//tangents
    std::vector<glm::vec2> uvs;//uv val

public:
    Teapot(int nSubDivisions)
    {
#include "teapotverts.inc"
        glm::vec3 vertexPositions[nTeapotVerts];
        for (int iVerts = 0; iVerts < nTeapotVerts; iVerts++)
        {
            vertexPositions[iVerts] = glm::vec3(tpverts[3 * iVerts + 0], tpverts[3 * iVerts + 1], tpverts[3 * iVerts + 2]);
        }
        for (int iPatch = 0; iPatch < nTeapotPatches; iPatch++)
        {
            std::vector<glm::vec3> controlPoints;
            for (int k = 0; k < 16; k++)
            {
                controlPoints.push_back(vertexPositions[patchIndices[16 * iPatch + k] - 1]); // -1 because the indices in the file are 1-based
            }
            // create a patch here... and add all its vertex arrays to the vertex arrays for this object..
            BezierPatch patch(controlPoints, nSubDivisions);
            for (int i = 0; i < patch.GetNumVertices(); i++)
            {
                positions.push_back(patch.GetPositionVector()[i]);
                normals.push_back(patch.GetNormalVector()[i]);
                tangents.push_back(patch.GetTangentVector()[i]);
                uvs.push_back(patch.GetUVVector()[i]);
            }

        }
    }


    const std::vector<glm::vec3>& GetPositionVector() const { return positions; }
    const std::vector<glm::vec3>& GetNormalVector() const { return normals; }
    const std::vector<glm::vec2>& GetUVVector() const { return uvs; }
    const std::vector<glm::vec3>& GetTangentVector() const { return tangents; }
    int GetNumVertices() const { return positions.size(); }
};
