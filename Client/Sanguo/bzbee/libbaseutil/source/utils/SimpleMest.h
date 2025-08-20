/*!
 * \file SimpleMest.h
 *
 * \author laizhisheng
 * \date 2017/09/11 10:32
 *
 */

#pragma once

#include "BaseType.h"
#include <tuple>

NS_BEGIN

class SimpleMesh
{
public:
	virtual ~SimpleMesh(){}

	virtual void* getVerticeBuffer() = 0;
	virtual void* getIndiceBuffer() = 0;

	virtual size_t getVerticeSize() = 0;
	virtual size_t getIndiceSize() = 0;
};


template<typename V, typename I>
class SimpleMeshT : public SimpleMesh
{
public:
	using vertex_type = V;
	using index_type = I;

public:
	void generateMeshByRect(const glm::vec2& leftBottom, const glm::vec2& rightTop, float z)
	{
		m_indices = { 0, 1, 2, 2, 1, 3 };

		m_vertices.resize(4);
		m_vertices[0].x = leftBottom.x;
		m_vertices[0].y = leftBottom.y;
		m_vertices[0].z = z;
		m_vertices[0].u = 0.0f;
		m_vertices[0].v = 0.0f;

		m_vertices[1].x = rightTop.x;
		m_vertices[1].y = leftBottom.y;
		m_vertices[1].z = z;
		m_vertices[1].u = 1.0f;
		m_vertices[1].v = 0.0f;

		m_vertices[2].x = leftBottom.x;
		m_vertices[2].y = rightTop.y;
		m_vertices[2].z = z;
		m_vertices[2].u = 0.0f;
		m_vertices[2].v = 1.0f;

		m_vertices[3].x = rightTop.x;
		m_vertices[3].y = rightTop.y;
		m_vertices[3].z = z;
		m_vertices[3].u = 1.0f;
		m_vertices[3].v = 1.0f;
	}

	void generateMeshByTriangleStrip(const std::vector<std::tuple<glm::vec3, glm::vec3>>& verticesList)
	{
		if (verticesList.size() < 2) return;

		int verticesNum = verticesList.size() * 2;
		int triangleNum = verticesNum - 2;
		int rectangleNum = triangleNum / 2;

		m_vertices.resize(verticesNum);
		m_indices.resize(triangleNum * 3);

		for (int index = 0; index < (int)verticesList.size(); ++index)
		{
			float rate = (float)index / (float)verticesList.size();
			int n = index * 2;

			// bottom
			const glm::vec3& pos1 = std::get<0>(verticesList[index]);
			m_vertices[n].x = pos1.x;
			m_vertices[n].y = pos1.y;
			m_vertices[n].z = pos1.z;
			m_vertices[index].u = rate;
			m_vertices[0].v = 0.0f;

			// top
			const glm::vec3& pos2 = std::get<1>(verticesList[index]);
			m_vertices[n + 1].x = pos2.x;
			m_vertices[n + 1].y = pos2.y;
			m_vertices[n + 1].z = pos2.z;
			m_vertices[index].u = rate;
			m_vertices[1].v = 1.0f;
		}

		static std::vector<index_type> s_indices = { 0, 1, 2, 2, 1, 3 };
		for (int rect = 0; rect < rectangleNum; ++rect)
		{
			for (int i = 0; i < (int)s_indices.size(); ++i)
			{
				int index = s_indices.size() * rect + i;
				m_indices[index] = s_indices[i] + rect * 2;
			}
		}
	}

	void generateMeshByBox(const glm::vec3& leftBottomFront, const glm::vec3& rightTopBack)
	{
		m_indices = {/*front*/ 0, 1, 2, 2, 1, 3, /*right*/1, 6, 3, 3, 6, 4, /*left*/7, 0, 5, 5, 0, 2, /*top*/2, 3, 5, 5, 3, 4, /*bottom*/7, 6, 0, 0, 6, 1, /*back*/ 6, 7, 4, 4, 7, 5 };

		m_vertices.resize(8);
		m_vertices[0].x = leftBottomFront.x;
		m_vertices[0].y = leftBottomFront.y;
		m_vertices[0].z = leftBottomFront.z;
		m_vertices[0].u = 0.0f;
		m_vertices[0].v = 0.0f;

		m_vertices[1].x = rightTopBack.x;
		m_vertices[1].y = leftBottomFront.y;
		m_vertices[1].z = leftBottomFront.z;
		m_vertices[1].u = 1.0f;
		m_vertices[1].v = 0.0f;

		m_vertices[2].x = leftBottomFront.x;
		m_vertices[2].y = rightTopBack.y;
		m_vertices[2].z = leftBottomFront.z;
		m_vertices[2].u = 0.0f;
		m_vertices[2].v = 1.0f;

		m_vertices[3].x = rightTopBack.x;
		m_vertices[3].y = rightTopBack.y;
		m_vertices[3].z = leftBottomFront.z;
		m_vertices[3].u = 1.0f;
		m_vertices[3].v = 1.0f;

		m_vertices[4].x = rightTopBack.x;
		m_vertices[4].y = rightTopBack.y;
		m_vertices[4].z = rightTopBack.z;
		m_vertices[4].u = 0.0f;
		m_vertices[4].v = 0.0f;

		m_vertices[5].x = leftBottomFront.x;
		m_vertices[5].y = rightTopBack.y;
		m_vertices[5].z = rightTopBack.z;
		m_vertices[5].u = 1.0f;
		m_vertices[5].v = 0.0f;

		m_vertices[6].x = rightTopBack.x;
		m_vertices[6].y = leftBottomFront.y;
		m_vertices[6].z = rightTopBack.z;
		m_vertices[6].u = 0.0f;
		m_vertices[6].v = 1.0f;

		m_vertices[7].x = leftBottomFront.x;
		m_vertices[7].y = leftBottomFront.y;
		m_vertices[7].z = rightTopBack.z;
		m_vertices[7].u = 1.0f;
		m_vertices[7].v = 1.0f;
	}

	void appendMesh(const std::vector<vertex_type>& sourceVerts, const std::vector<index_type>& sourceIndis)
	{
		index_type indexOffset = (index_type)m_vertices.size();

		m_vertices.insert(m_vertices.end(), sourceVerts.begin(), sourceVerts.end());
		m_indices.insert(m_indices.end(), sourceIndis.begin(), sourceIndis.end());

		if (indexOffset > 0)
		{
			int rbegin = m_indices.size() - 1;
			int rend = m_indices.size() - sourceIndis.size();
			for (int i = rbegin; i >= rend; --i)
			{
				m_indices[i] += indexOffset;
			}
		}
	}

	void clear()
	{
		m_vertices.clear();
		m_indices.clear();
	}

	void transform(const glm::mat4& mat)
	{
		for (auto& vetex : m_vertices)
		{
			glm::vec4 transPos = mat * glm::vec4(vetex.x, vetex.y, vetex.z, 1.0f);
			vetex.x = transPos.x;
			vetex.y = transPos.y;
			vetex.z = transPos.z;
		}
	}

	void fillInColor(const glm::vec4& color)
	{
		for (auto& vetex : m_vertices)
		{
			vetex.color = color;
		}
	}

	void fillInColor(const Color& color)
	{
		for (auto& vetex : m_vertices)
		{
			vetex.color = color;
		}
	}

	void fillInUV(const glm::vec2* uvs, int size)
	{
		for (int i = 0; i < (int)m_vertices.size(); ++i)
		{
			int index = i % size;
			m_vertices[i].u = uvs[index].x;
			m_vertices[i].v = uvs[index].y;
		}
	}

	void fillInNormal(float normal)
	{
		for (auto& vetex : m_vertices)
		{
			vetex.n = normal;
		}
	}

	void fillInNormal(const glm::vec3& nromal)
	{
		for (auto& vetex : m_vertices)
		{
			vetex.nx = nromal.x;
			vetex.ny = nromal.y;
			vetex.nz = nromal.z;
		}
	}

	virtual void* getVerticeBuffer() override
	{
		return m_vertices.data();
	}

	virtual void* getIndiceBuffer() override
	{
		return m_indices.data();
	}

	virtual size_t getVerticeSize() override
	{
		return m_vertices.size();
	}

	virtual size_t getIndiceSize() override
	{
		return m_indices.size();
	}

	const std::vector<vertex_type>& getVertexs(){ return m_vertices; }
	const std::vector<index_type>& getIndices(){ return m_indices; }

private:
	std::vector<vertex_type> m_vertices;
	std::vector<index_type> m_indices;
};

NS_END