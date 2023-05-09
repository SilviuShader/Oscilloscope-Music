#include "audiomesh.h"

#include <raymath.h>

Vector2 AudioToNDC(const float leftSample, const float rightSample)
{
    Vector2 res;

    res.x = leftSample * 2.0f - 1.0f;
    res.y = rightSample * 2.0f - 1.0f;

    return res;
}

Mesh GenFrameMesh(const AudioData audioData, const float uSize, const int firstSampleIndex, const int lastSampleIndex)
{
    Mesh mesh = { 0 };
    mesh.triangleCount = (lastSampleIndex - firstSampleIndex) * 2;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));    // 3 vertices, 3 coordinates each (x, y, z)
    mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));   // 3 vertices, 2 coordinates each (x, y)
    mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));     // 3 vertices, 3 coordinates each (x, y, z)

    for (int i = firstSampleIndex; i < lastSampleIndex - 1; i++)
    {
        float leftSample = audioData.leftSamples[i];
        float nextLeftSample = audioData.leftSamples[i + 1];

        float rightSample = audioData.rightSamples[i];
        float nextRightSample = audioData.rightSamples[i + 1];

        Vector2 prevPosition = AudioToNDC(leftSample, rightSample);
        Vector2 nextPosition = AudioToNDC(nextLeftSample, nextRightSample);

        Vector2 dir = Vector2Subtract(nextPosition, prevPosition);
        float z = Vector2Length(dir);
        if (z > EPSILON)
        {
            dir.x /= z;
            dir.y /= z;
        }
        else
        {
            dir.x = 1.0f;
            dir.y = 0.0f;
        }

        dir.x *= uSize;
        dir.y *= uSize;

        Vector2 norm;
        norm.x = -dir.y;
        norm.y = dir.x;

        const int iVertex = (i - firstSampleIndex) * 18;
        const int iNormal = (i - firstSampleIndex) * 18;
        const int iTexCoords = (i - firstSampleIndex) * 12;

        // ===================== first triangle =====================

        mesh.vertices[iVertex] = prevPosition.x - dir.x - norm.x;
        mesh.vertices[iVertex + 1] = prevPosition.y - dir.y - norm.y;
        mesh.vertices[iVertex + 2] = 0.0f;

        mesh.normals[iNormal] = -uSize;
        mesh.normals[iNormal + 1] = -uSize;
        mesh.normals[iNormal + 2] = z;

        mesh.texcoords[iTexCoords] = 0;
        mesh.texcoords[iTexCoords + 1] = 0;


        mesh.vertices[iVertex + 3] = prevPosition.x - dir.x + norm.x;
        mesh.vertices[iVertex + 4] = prevPosition.y - dir.y + norm.y;
        mesh.vertices[iVertex + 5] = 0.0f;

        mesh.normals[iNormal + 3] = -uSize;
        mesh.normals[iNormal + 4] = uSize;
        mesh.normals[iNormal + 5] = z;

        mesh.texcoords[iTexCoords + 2] = 1;
        mesh.texcoords[iTexCoords + 3] = 0;


        mesh.vertices[iVertex + 6] = nextPosition.x + dir.x - norm.x;
        mesh.vertices[iVertex + 7] = nextPosition.y + dir.y - norm.y;
        mesh.vertices[iVertex + 8] = 0.0f;

        mesh.normals[iNormal + 6] = z + uSize;
        mesh.normals[iNormal + 7] = -uSize;
        mesh.normals[iNormal + 8] = z;

        mesh.texcoords[iTexCoords + 4] = 0;
        mesh.texcoords[iTexCoords + 5] = 1;

        // ===================== second triangle =====================

        mesh.vertices[iVertex + 9] = prevPosition.x - dir.x + norm.x;
        mesh.vertices[iVertex + 10] = prevPosition.y - dir.y + norm.y;
        mesh.vertices[iVertex + 11] = 0.0f;

        mesh.normals[iNormal + 9] = -uSize;
        mesh.normals[iNormal + 10] = uSize;
        mesh.normals[iNormal + 11] = z;

        mesh.texcoords[iTexCoords + 6] = 1;
        mesh.texcoords[iTexCoords + 7] = 0;


        mesh.vertices[iVertex + 12] = nextPosition.x + dir.x - norm.x;
        mesh.vertices[iVertex + 13] = nextPosition.y + dir.y - norm.y;
        mesh.vertices[iVertex + 14] = 0.0f;

        mesh.normals[iNormal + 12] = z + uSize;
        mesh.normals[iNormal + 13] = -uSize;
        mesh.normals[iNormal + 14] = z;

        mesh.texcoords[iTexCoords + 8] = 0;
        mesh.texcoords[iTexCoords + 9] = 1;


        mesh.vertices[iVertex + 15] = nextPosition.x + dir.x + norm.x;
        mesh.vertices[iVertex + 16] = nextPosition.y + dir.y + norm.y;
        mesh.vertices[iVertex + 17] = 0.0f;

        mesh.normals[iNormal + 15] = z + uSize;
        mesh.normals[iNormal + 16] = uSize;
        mesh.normals[iNormal + 17] = z;

        mesh.texcoords[iTexCoords + 10] = 1;
        mesh.texcoords[iTexCoords + 11] = 1;
    }

    UploadMesh(&mesh, false);

    return mesh;
}