#include "Util.h"

#include <algorithm>

namespace Util
{
	// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
	void Barycentric(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& u, float& v, float& w)
	{
		glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
		float den = v0.x * v1.y - v1.x * v0.y;
		v = (v2.x * v1.y - v1.x * v2.y) / den;
		w = (v0.x * v2.y - v2.x * v0.y) / den;
		u = 1.0f - v - w;
	}

	void OrderVerticesByYThenX(Vertex& _high, Vertex& _low)
	{
		if (_high.Position.y >= _low.Position.y)
		{
			if (_high.Position.y == _low.Position.y)
			{
				if (_high.Position.x > _low.Position.x)
				{
					std::swap(_high, _low);
				}
			}
			else
			{
				std::swap(_high, _low);
			}
		}
	}

	void OrderVerticesByYThenX(Vertex& _high1, Vertex& _low1, Vertex& _high2, Vertex& _low2)
	{
		if (_high1.Position.y >= _low1.Position.y)
		{
			if (_high1.Position.y == _low1.Position.y)
			{
				if (_high1.Position.x > _low1.Position.x)
				{
					std::swap(_high1, _low1);
					std::swap(_high2, _low2);
				}
			}
			else
			{
				std::swap(_high1, _low1);
				std::swap(_high2, _low2);
			}
		}
	}
}
