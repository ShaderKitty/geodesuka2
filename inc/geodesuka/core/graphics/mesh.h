#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_MESH_H
#define GEODESUKA_CORE_GRAPHICS_MESH_H

#include <vector>

#include "../util/string.h"

#include "../gcl/variable.h"

#include "../gcl/context.h"
#include "../gcl/buffer.h"

namespace geodesuka::core::graphics {

	class mesh {
	public:

		enum primitive {
			TRIANGLE	,
			QUAD		,
			GIZMO		,
			TETRAHEDRON	,
			PYRAMID		, 
			CUBE		,
		};

		struct vertex {
			glm::vec3					Position;
			glm::vec2					TextureCoordinate;
			glm::vec4					Color;
			glm::vec3					Normal;
			glm::vec3					Tangent;
			glm::vec3					Bitangent;
			glm::uvec4					BoneID;
			glm::vec4					BoneWeight;
			vertex();
		};

		struct face {
			// How the vertices are to be read.
			VkPrimitiveTopology	Topology;
			size_t					IndexCount;
			VkIndexType 			IndexType;
			void*					IndexData;
		};

		struct bone {
			struct vertex_weight {
				unsigned int			ID;
				float					Weight;
			};
			util::string			Name;
			unsigned int			VertexCount;
			vertex_weight*			Vertex;
		};

		// ----- Host Data ----- //

		std::vector<vertex> 		Vertex;
		std::vector<face> 			Face;
		std::vector<bone> 			Bone;

		// ----- Device Data ----- //

		gcl::context* 				Context;
		gcl::buffer 				VertexBuffer;
		std::vector<gcl::buffer> 	IndexBuffer;
		gcl::buffer 				BoneBuffer;

		// Units are in meters.
		static mesh triangle(float aBase, float aHeight);
		static mesh quad(float aWidth, float aHeight);

		// Primitive geometry generation.
		mesh(primitive aPrimitive);

		// 16 bit vertex indexing. Count < 65,536 vertices.
		mesh(size_t aVertexCount, vertex* aVertex, size_t aIndexCount, unsigned short* aIndex);

		// 32 bit vertex indexing. Count > 65,536 vertices.
		mesh(size_t aVertexCount, vertex* aVertex, size_t aIndexCount, unsigned int* aIndex);


		mesh(gcl::context* aContext, size_t aVertexCount, vertex* aVertex, size_t aIndexCount, unsigned short* aIndex);
		mesh(gcl::context* aContext, size_t aVertexCount, vertex* aVertex, size_t aIndexCount, unsigned int* aIndex);

		mesh(gcl::context* aContext, primitive aPrimitive);

		~mesh();

	private:

		void generate(primitive aPrimitive);

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MESH_H
