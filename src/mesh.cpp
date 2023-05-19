#include <geodesuka/core/graphics/mesh.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace geodesuka::core::graphics {

	mesh::vertex::vertex() {
		this->Position					= float3(0.0f, 0.0f, 0.0f);
		this->Normal					= float3(0.0f, 0.0f, 0.0f);
		this->Tangent					= float3(0.0f, 0.0f, 0.0f);
		this->Bitangent					= float3(0.0f, 0.0f, 0.0f);
		this->BoneID					= uint4(0u, 0u, 0u, 0u);
		this->BoneWeight				= float4(0.0f, 0.0f, 0.0f, 0.0f);
		this->TextureCoordinate			= float2(0.0f, 0.0f);
		this->Color						= float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	mesh::mesh(primitive aPrimitive) {
		this->zero_out();
		this->generate(aPrimitive);
	}

	void mesh::generate(primitive aPrimitive) {
		switch (aPrimitive) {
		case TRIANGLE:
		{
			// vertex VertexData[3];
			// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
			// VertexData[0].Position = float3(0.433f, -0.25f, 0.0f);
			// VertexData[1].Position = float3(-0.433f, -0.25f, 0.0f);
			// VertexData[2].Position = float3(0.0f, 0.5f, 0.0f);
			// for (size_t i = 0; i < this->VertexCount; i++) {
			// 	VertexData[i].Normal = float3(0.0f, 0.0f, 1.0f);
			// }

			// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
			// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
			// assert(this->Vertex);
			// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));
		}
			break;
		case QUAD:
		{
			// vertex VertexData[4];
			// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
			// VertexData[0].Position = float3(-0.5f, -0.5f, 0.0f); // Bottom Left
			// VertexData[1].Position = float3(0.5f, -0.5f, 0.0f); // Bottom Right
			// VertexData[2].Position = float3(0.5f, 0.5f, 0.0f); // Top Right
			// VertexData[3].Position = float3(-0.5f, 0.5f, 0.0f); // Top Left
			// for (size_t i = 0; i < this->VertexCount; i++) {
			// 	VertexData[i].Normal = float3(0.0f, 0.0f, 1.0f);
			// }

			// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
			// assert(this->Vertex);
			// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));

			// Setup indices for two triangle rendering

		}
			break;
		case GIZMO:
		{
			// vertex VertexData[4];
			// this->VertexCount = sizeof(VertexData) / sizeof(vertex);

			// // Gizmo Positions.
			// VertexData[0].Position = float3(0.0f, 0.0f, 0.0f); // Origin
			// VertexData[1].Position = float3(1.0f, 0.0f, 0.0f); // +X Axis
			// VertexData[2].Position = float3(0.0f, 1.0f, 0.0f); // +Y Axis
			// VertexData[3].Position = float3(0.0f, 0.0f, 1.0f); // +Z Axiz

			// // Gizmo Coloring.
			// VertexData[0].Color = float4(1.0f, 1.0f, 1.0f, 1.0f); // White
			// VertexData[1].Color = float4(1.0f, 0.0f, 0.0f, 1.0f); // Red
			// VertexData[2].Color = float4(0.0f, 1.0f, 0.0f, 1.0f); // Green
			// VertexData[3].Color = float4(0.0f, 0.0f, 1.0f, 1.0f); // Blue

			// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
			// assert(this->Vertex);
			// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));

			// this->FaceCount = 1;
			// this->Face = (face*)malloc(this->FaceCount * sizeof(face));
			// assert(this->Face);

			// // {0,1}, {0,2}, {0,3}
			// short IndexData[6] = {
			// 	0, 1,
			// 	0, 2,
			// 	0, 3
			// };

			// Face[0].Topology	= VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			// Face[0].IndexCount	= sizeof(IndexData) / sizeof(short);
			// Face[0].IndexType	= gcl::type::id::SHORT;
			// Face[0].IndexData	= malloc(Face[0].IndexCount * sizeof(short));
			// assert(Face[0].IndexData);
			// memcpy(Face[0].IndexData, IndexData, Face[0].IndexCount * sizeof(short));

		}
			break;
		case CUBE:
		{
			vertex VertexData[8];




		}
			break;
		case TETRAHEDRON:
			break;
		case PYRAMID:
			break;
		default:
			break;
		}
	}

	void mesh::zero_out() {
		this->Context = nullptr;
	}

}
