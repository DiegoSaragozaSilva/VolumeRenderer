#include "Voxelizer.hpp"

std::vector<glm::vec3> Voxelizer::voxelizeMesh(std::vector<std::vector<glm::vec3>> mesh, int lateralResolution) {
    // Voxelizes a given mesh using "OpenGL Insights 22" and "A Hybrid GPU Rendering Pipeline for Alias-Free Hard Shadows" methods 

    // Voxelized mesh data
    std::vector<glm::vec3> voxelizedMesh;

    // Fragment length and diagonal
    float fragLength = 1.0f / (float)lateralResolution;
    float fragDiagonal = 1.4142135637309f / (float)lateralResolution;

    // Fill the voxelized mesh with data
    for (const auto &triangle : mesh) {
        // Project triangle into the axis that maximizes the area
        int projectedAxis;
        std::vector<glm::vec3> triProjected = orthoProjectTriangle(triangle, &projectedAxis);

        // Calculate the bounding polygon of the projected triangle
        glm::vec4 AABB;
        AABB.x = triProjected[0].x;
        AABB.y = triProjected[0].y;
        AABB.z = triProjected[0].x;
        AABB.w = triProjected[0].y;

        AABB.x = std::min(triProjected[1].x, AABB.x);
        AABB.y = std::min(triProjected[1].y, AABB.y);
        AABB.z = std::max(triProjected[1].x, AABB.x);
        AABB.w = std::max(triProjected[1].y, AABB.y);

        AABB.x = std::min(triProjected[2].x, AABB.x);
        AABB.y = std::min(triProjected[2].y, AABB.y);
        AABB.z = std::max(triProjected[2].x, AABB.x);
        AABB.w = std::max(triProjected[2].y, AABB.y);

        // Enlarge the AABB by half a fragment length
        AABB.x -= fragLength;
        AABB.y -= fragLength;
        AABB.z += fragLength;
        AABB.w += fragLength;

        // Projected triangle edges
        std::vector<glm::vec3> triEdges(3);
        triEdges[0] = glm::vec3(triProjected[1].xy() - triProjected[0].xy(), 0);
        triEdges[1] = glm::vec3(triProjected[2].xy() - triProjected[1].xy(), 0);
        triEdges[2] = glm::vec3(triProjected[0].xy() - triProjected[2].xy(), 0);

        // Edges normals
        std::vector<glm::vec3> edgeNormals(3);
        edgeNormals[0] = glm::cross(triEdges[0], glm::vec3(0, 0, 1));
        edgeNormals[1] = glm::cross(triEdges[1], glm::vec3(0, 0, 1));
        edgeNormals[2] = glm::cross(triEdges[2], glm::vec3(0, 0, 1));

        // Dilate the triangle in the direction of its edges by fragment diagonal length
        glm::vec2 dilatedVertex = triProjected[0].xy() + fragDiagonal * ((triEdges[2].xy() / glm::dot(triEdges[2].xy(), edgeNormals[0].xy())) + (triEdges[0].xy() / glm::dot(triEdges[0].xy(), edgeNormals[2].xy())));
        triProjected[0].x = dilatedVertex.x;
        triProjected[0].y = dilatedVertex.y;
        std::cout << glm::dot(triEdges[0].xy(), edgeNormals[2].xy()) << std::endl;
        
        dilatedVertex = triProjected[1].xy() + fragDiagonal * ((triEdges[0].xy() / glm::dot(triEdges[0].xy(), edgeNormals[1].xy())) + (triEdges[1].xy() / glm::dot(triEdges[1].xy(), edgeNormals[0].xy())));
        triProjected[1].x = dilatedVertex.x;
        triProjected[1].y = dilatedVertex.y;

        dilatedVertex = triProjected[2].xy() + fragDiagonal * ((triEdges[1].xy() / glm::dot(triEdges[1].xy(), edgeNormals[2].xy())) + (triEdges[2].xy() / glm::dot(triEdges[2].xy(), edgeNormals[1].xy())));
        triProjected[2].x = dilatedVertex.x;
        triProjected[2].y = dilatedVertex.y;

        for (const auto &point : triProjected)
            std::cout << point.x << " " << point.y << " " << point.z << '\n';
        std::cout << std::endl;

        // Iter over the modified triangle vertices and fill the voxelized mesh data
        for (const auto &vertex : triProjected) {
            for (int fragY = 0; fragY < lateralResolution; fragY++)
                for (int fragX = 0; fragX < lateralResolution; fragX++) {
                    // If vertex is out of the AABB bounds, discard vertex
                    if (vertex.x < AABB.x || vertex.y < AABB.y || vertex.x > AABB.z || vertex.y > AABB.w)
                        goto discardVertex;
                   
                    // Calculate the z depth of fragment coordinate
                    float fragZ = getPointTriangleZ(triProjected, glm::vec2(fragX, fragY));
                    
                    // Find the voxel coordinate based on the projected axis
                    glm::vec3 temp = { fragX, fragY, lateralResolution * fragZ};
                    glm::vec3 texCoord;
                    if (projectedAxis == 0) {
                        texCoord.x = lateralResolution - temp.z;
                        texCoord.y = temp.x;
                        texCoord.z = temp.y;
                    }
                    else if (projectedAxis == 1) {
                        texCoord.x = temp.y;
                        texCoord.y = lateralResolution - temp.z;
                        texCoord.z = temp.x;
                    }
                    else
                        texCoord = temp;
                    
                    // Store the voxel
                    voxelizedMesh.push_back(texCoord);
                }
            discardVertex:
                continue;
        }
    }
    return voxelizedMesh;
}

std::vector<glm::vec3> Voxelizer::orthoProjectTriangle(std::vector<glm::vec3> triangle, int* projectedAxis) {
    // Project the given triangle into the axis that maximizes the area. Also saves the projected axis
    
    // Main axes
    glm::vec3 xAxis = {1, 0, 0}, yAxis = {0, 1, 0}, zAxis = {0, 0, 1};

    // Triangle normal
    glm::vec3 U = triangle[1] - triangle[0];
    glm::vec3 V = triangle[2] - triangle[0];

    glm::vec3 triNormal = {
        U.y * V.z - U.z * V.y,
        U.z * V.x - U.x * V.z,
        U.x * V.y - U.y * V.x
    };

    triNormal = glm::normalize(triNormal);

    // Find the axis that maximizes the area
    float lX, lY, lZ, lMax;

    lX = std::fabs(triNormal.x);
    lY = std::fabs(triNormal.y);
    lZ = std::fabs(triNormal.z);
    lMax = std::max(std::max(lX, lY), lZ);

    // Project the triangle into the dominant axis
    glm::mat3 orthoMatrix = glm::mat3(1.0f);
    if (lMax == lX)
        orthoMatrix[0][0] = 0.0f, *projectedAxis = 0;
    else if (lMax == lY)
        orthoMatrix[1][1] = 0.0f, *projectedAxis = 1;
    else
        orthoMatrix[2][2] = 0.0f, *projectedAxis = 2;

    std::vector<glm::vec3> triProjected(3);
    for (int i = 0; i < 3; i++)
        triProjected[i] = glm::vec3(orthoMatrix * triangle[i]);

    return triProjected;
}

float Voxelizer::getPointTriangleZ(std::vector<glm::vec3> triangle, glm::vec2 point) {
    // Calculates the z coordinate of a point inside a triangle using 4 points coplanarity
    
    float leftUp = point.x * triangle[2].y * triangle[1].z + point.y * triangle[2].z * triangle[0].x + triangle[2].x * triangle[1].y * triangle[0].z;
    float rightUp = triangle[0].x * triangle[1].y * triangle[2].z + triangle[0].y * triangle[1].z * point.x + triangle[0].z * triangle[2].x * point.y;   
    float bottom = triangle[2].y * triangle[1].x - triangle[1].x * triangle[0].y;

    return (leftUp - rightUp) / bottom;
}
