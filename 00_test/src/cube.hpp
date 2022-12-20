#pragma once

#include <cstdint>

/**
 * @brief This structure represents vertex that contains only position and
 * normal.
 */
struct CubeVertex {
	float position[3];  ///< position of vertex
	float normal[3];    ///< normal of vertex
};

/// This variable contains vertices of Standford cube.
extern struct CubeVertex const cubeVertices[8];

using VertexIndex = uint32_t;///< type of index

/// This variable contains Standford bunny indices.
extern const VertexIndex cubeIndices[12][3];