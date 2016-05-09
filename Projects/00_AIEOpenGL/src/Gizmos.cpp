#include "Gizmos.h"
#include "gl_core_4_4.h"
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//#include <stdio.h>
#include <iostream>
#include "Shader.h"

using glm::mat4;
using glm::vec4;
using glm::vec3;
using glm::pi;
using glm::vec2;

Gizmos* Gizmos::sm_singleton = nullptr;

Gizmos::Gizmos(unsigned int maxLines, unsigned int maxTris, unsigned int max2DLines, unsigned int max2DTris) : 
	m_maxLines(maxLines),
	m_lineCount(0),
	m_lines(new GizmoLine[maxLines]),
	m_maxTris(maxTris),
	m_triCount(0),
	m_tris(new GizmoTri[maxTris]),
	m_transparentTriCount(0),
	m_transparentTris(new GizmoTri[maxTris]),
	m_max2DLines(max2DLines),
	m_2DlineCount(0),
	m_2Dlines(new GizmoLine[max2DLines]),
	m_max2DTris(max2DTris),
	m_2DtriCount(0),
	m_2Dtris(new GizmoTri[max2DTris])
{
	//create shaders
	m_shader = new Shader();
	m_shader->LoadShader(GL_VERTEX_SHADER, "../00_AIEOpenGL/src/Shaders/GizmosVertex.vs");
	m_shader->LoadShader(GL_FRAGMENT_SHADER, "../00_AIEOpenGL/src/Shaders/GizmosFragment.fs");

	m_shader->Link();
	m_shader->BindAttrib(0, "position");
	m_shader->BindAttrib(1, "colour");
    
    //create VBOs
	glGenBuffers(1, &m_lineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxLines * sizeof(GizmoLine), m_lines, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_triVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_tris, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_transparentTriVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_transparentTris, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_2DlineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DLines * sizeof(GizmoLine), m_2Dlines, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_2DtriVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DTris * sizeof(GizmoTri), m_2Dtris, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_lineVAO);
	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)(sizeof(float) * 4));

	glGenVertexArrays(1, &m_triVAO);
	glBindVertexArray(m_triVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)(sizeof(float) * 4));

	glGenVertexArrays(1, &m_transparentTriVAO);
	glBindVertexArray(m_transparentTriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)(sizeof(float) * 4));

	glGenVertexArrays(1, &m_2DlineVAO);
	glBindVertexArray(m_2DlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)(sizeof(float) * 4));

	glGenVertexArrays(1, &m_2DtriVAO);
	glBindVertexArray(m_2DtriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Gizmos::~Gizmos()
{
	delete[] m_lines;
	delete[] m_tris;
	delete[] m_transparentTris;
	glDeleteBuffers(1, &m_lineVBO);
	glDeleteBuffers(1, &m_triVBO);
	glDeleteBuffers(1, &m_transparentTriVBO);
	glDeleteVertexArrays(1, &m_lineVAO);
	glDeleteVertexArrays(1, &m_triVAO);
	glDeleteVertexArrays(1, &m_transparentTriVAO);
	delete[] m_2Dlines;
	delete[] m_2Dtris;
	glDeleteBuffers(1, &m_2DlineVBO);
	glDeleteBuffers(1, &m_2DtriVBO);
	glDeleteVertexArrays(1, &m_2DlineVAO);
	glDeleteVertexArrays(1, &m_2DtriVAO);
	delete m_shader;
}

void Gizmos::Create(unsigned int maxLines /* = 0xffff */, unsigned int maxTris /* = 0xffff */, unsigned int max2DLines /* = 0xff */, unsigned int max2DTris /* = 0xff */)
{
	if (sm_singleton == nullptr)
		sm_singleton = new Gizmos(maxLines, maxTris, max2DLines, max2DTris);
}

void Gizmos::Destroy()
{
	delete sm_singleton;
	sm_singleton = nullptr;
}

void Gizmos::Clear()
{
	sm_singleton->m_lineCount = 0;
	sm_singleton->m_triCount = 0;
	sm_singleton->m_transparentTriCount = 0;
	sm_singleton->m_2DlineCount = 0;
	sm_singleton->m_2DtriCount = 0;
}

//Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, at the transform's translation. Optional scale available.
void Gizmos::AddTransform(const mat4& transform, float scale /* = 1.f */)
{
	vec4 xAxis = transform[3] + transform[0] * scale;
	vec4 yAxis = transform[3] + transform[1] * scale;
	vec4 zAxis = transform[3] + transform[2] * scale;

	vec4 red(1, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 blue(0, 0, 1, 1);

	AddLine(transform[3].xyz(), xAxis.xyz(), red, red);
	AddLine(transform[3].xyz(), yAxis.xyz(), green, green);
	AddLine(transform[3].xyz(), zAxis.xyz(), blue, blue);
}

void Gizmos::AddAABB(const vec3& center, const vec3& rvExtents, const vec4& colour, const mat4* transform /* = nullptr */)
{
	vec3 verts[8];
	vec3 xVert(rvExtents.x, 0, 0);
	vec3 yVert(0, rvExtents.y, 0);
	vec3 zVert(0, 0, rvExtents.z);

	if (transform != nullptr)
	{
		xVert = (*transform * vec4(xVert, 0)).xyz();
		yVert = (*transform * vec4(yVert, 0)).xyz();
		zVert = (*transform * vec4(zVert, 0)).xyz();
	}

	//top verts
	verts[0] = center - xVert - zVert - yVert;
	verts[1] = center - xVert + zVert - yVert;
	verts[2] = center + xVert + zVert - yVert;
	verts[3] = center + xVert - zVert - yVert;

	//bottom verts
	verts[4] = center - xVert - zVert + yVert;
	verts[5] = center - xVert + zVert + yVert;
	verts[6] = center + xVert + zVert + yVert;
	verts[7] = center + xVert - zVert + yVert;

	AddLine(verts[0], verts[1], colour, colour);
	AddLine(verts[1], verts[2], colour, colour);
	AddLine(verts[2], verts[3], colour, colour);
	AddLine(verts[3], verts[0], colour, colour);

	AddLine(verts[4], verts[5], colour, colour);
	AddLine(verts[5], verts[6], colour, colour);
	AddLine(verts[6], verts[7], colour, colour);
	AddLine(verts[7], verts[4], colour, colour);

	AddLine(verts[0], verts[4], colour, colour);
	AddLine(verts[1], verts[5], colour, colour);
	AddLine(verts[2], verts[6], colour, colour);
	AddLine(verts[3], verts[7], colour, colour);
}

void Gizmos::AddAABBFilled(const vec3& center, const vec3& rvExtents, const vec4& fillColour, const mat4* transform /* = nullptr */)
{
	vec3 verts[8];
	vec3 xVert(rvExtents.x, 0, 0);
	vec3 yVert(0, rvExtents.y, 0);
	vec3 zVert(0, 0, rvExtents.z);

	if (transform != nullptr)
	{
		xVert = (*transform * vec4(xVert, 0)).xyz();
		yVert = (*transform * vec4(yVert, 0)).xyz();
		zVert = (*transform * vec4(zVert, 0)).xyz();
	}

	//top verts
	verts[0] = center - xVert - zVert - yVert;
	verts[1] = center - xVert + zVert - yVert;
	verts[2] = center + xVert + zVert - yVert;
	verts[3] = center + xVert - zVert - yVert;

	//bottom verts
	verts[4] = center - xVert - zVert + yVert;
	verts[5] = center - xVert + zVert + yVert;
	verts[6] = center + xVert + zVert + yVert;
	verts[7] = center + xVert - zVert + yVert;

	vec4 white(1, 1, 1, 1);

	AddLine(verts[0], verts[1], white, white);
	AddLine(verts[1], verts[2], white, white);
	AddLine(verts[2], verts[3], white, white);
	AddLine(verts[3], verts[0], white, white);

	AddLine(verts[4], verts[5], white, white);
	AddLine(verts[5], verts[6], white, white);
	AddLine(verts[6], verts[7], white, white);
	AddLine(verts[7], verts[4], white, white);

	AddLine(verts[0], verts[4], white, white);
	AddLine(verts[1], verts[5], white, white);
	AddLine(verts[2], verts[6], white, white);
	AddLine(verts[3], verts[7], white, white);

	//top
	AddTri(verts[2], verts[1], verts[0], fillColour);
	AddTri(verts[3], verts[2], verts[0], fillColour);

	//bottom
	AddTri(verts[5], verts[6], verts[4], fillColour);
	AddTri(verts[6], verts[7], verts[4], fillColour);

	//front
	AddTri(verts[4], verts[3], verts[0], fillColour);
	AddTri(verts[7], verts[3], verts[4], fillColour);

	//back
	AddTri(verts[1], verts[2], verts[5], fillColour);
	AddTri(verts[2], verts[6], verts[5], fillColour);

	//left
	AddTri(verts[0], verts[1], verts[4], fillColour);
	AddTri(verts[1], verts[5], verts[4], fillColour);

	//right
	AddTri(verts[2], verts[3], verts[7], fillColour);
	AddTri(verts[6], verts[2], verts[7], fillColour);
}

void Gizmos::AddCylinderFilled(const vec3& center, float radius, 
								float halfLength, 
								unsigned int segments, 
								const vec4& fillColour, 
								const mat4* transform /* = nullptr */)
{
	vec4 white(1, 1, 1, 1);

	float segmentSize = (2.f * pi<float>()) / segments;

	for (unsigned int i = 0 ; i < segments ; ++i)
	{
		vec3 top0(0, halfLength, 0);
		vec3 top1(sinf(i * segmentSize) * radius, halfLength, cosf(i * segmentSize) * radius);
		vec3 top2(sinf((i + 1) * segmentSize) * radius, halfLength, cosf((i + 1) * segmentSize) * radius);
		vec3 bottom0(0, -halfLength, 0);
		vec3 bottom1(sinf(i * segmentSize) * radius, -halfLength, cosf(i * segmentSize) * radius);
		vec3 bottom2(sinf((i + 1) * segmentSize) * radius, -halfLength, cosf((i + 1) * segmentSize) * radius);

		if (transform != nullptr)
		{
			top0 = (*transform * vec4(top0, 0)).xyz();
			top1 = (*transform * vec4(top1, 0)).xyz();
			top2 = (*transform * vec4(top2, 0)).xyz();
			bottom0 = (*transform * vec4(bottom0, 0)).xyz();
			bottom1 = (*transform * vec4(bottom1, 0)).xyz();
			bottom2 = (*transform * vec4(bottom2, 0)).xyz();
		}

		//triangles
		AddTri(center + top0, center + top1, center + top2, fillColour);
		AddTri(center + bottom0, center + bottom2, center + bottom1, fillColour);
		AddTri(center + top2, center + top1, center + bottom1, fillColour);
		AddTri(center + bottom1, center + bottom2, center + top2, fillColour);

		//lines
		AddLine(center + top1, center + top2, white, white);
		AddLine(center + top1, center + bottom1, white, white);
		AddLine(center + bottom1, center + bottom2, white, white);
	}
}

void Gizmos::AddRing(const vec3& center, 
						float innerRadius, float outerRadius, 
						unsigned int segments, 
						const vec4& fillColour, 
						const mat4* transform /* = nullptr */)
{
	vec4 solid = fillColour;
	solid.w = 1.f;

	float segmentSize = (2.f * pi<float>()) / segments;

	for (unsigned int i = 0 ; i < segments ; ++i)
	{
		vec3 outer1(sinf(i * segmentSize) * outerRadius, 0, cosf(i * segmentSize) * outerRadius);
		vec3 outer2(sinf((i + 1) * segmentSize) * outerRadius, 0, cosf((i + 1) * segmentSize) * outerRadius);
		vec3 inner1(sinf(i * segmentSize) * innerRadius, 0, cosf(i * segmentSize) * innerRadius);
		vec3 inner2(sinf((i + 1) * segmentSize) * innerRadius, 0, cosf((i + 1) * segmentSize) * innerRadius);

		if (transform != nullptr)
		{
			outer1 = (*transform * vec4(outer1, 0)).xyz();
			outer2 = (*transform * vec4(outer2, 0)).xyz();
			inner1 = (*transform * vec4(inner1, 0)).xyz();
			inner2 = (*transform * vec4(inner2, 0)).xyz();
		}

		if (fillColour.w != 0.f)
		{
			AddTri(center + outer2, center + outer1, center + inner1, fillColour);
			AddTri(center + inner1, center + inner2, center + outer2, fillColour);

			AddTri(center + inner1, center + outer1, center + outer2, fillColour);
			AddTri(center + outer2, center + inner2, center + inner1, fillColour);
		}
		else
		{
			//line
			AddLine(center + inner1 + center, center + inner2 + center, solid, solid);
			AddLine(center + outer1 + center, center + outer2 + center, solid, solid);
		}
	}
}

void Gizmos::AddDisk(const vec3& center, float radius, 
						unsigned int segments, 
						const vec4& fillColour, 
						const mat4* transform /* = nullptr */)
{
	vec4 solid = fillColour;
	solid.w = 1;

	float segmentSize = (2.f * pi<float>()) / segments;

	for (unsigned int i = 0 ; i < segments ; ++i)
	{
		vec3 outer1(sinf(i * segmentSize) * radius, 0, cosf(i * segmentSize) * radius);
		vec3 outer2(sinf((i + 1) * segmentSize) * radius, 0, cosf((i + 1) * segmentSize) * radius);

		if (transform != nullptr)
		{
			outer1 = (*transform * vec4(outer1, 0)).xyz();
			outer2 = (*transform * vec4(outer2, 0)).xyz();
		}

		if (fillColour.w != 0.f)
		{
			AddTri(center, center + outer1, center + outer2, fillColour);
			AddTri(center + outer2, center + outer1, center, fillColour);
		}
		else
			AddLine(center + outer1, center + outer2, solid, solid);	//line
	}
}

void Gizmos::AddArc(const vec3& center, 
					float rotation, 
					float radius, 
					float arcHalfAngle,
					unsigned int segments, 
					const vec4& fillColour, 
					const mat4* transform /* = nullptr */)
{
	vec4 solid = fillColour;
	solid.w = 1.f;

	float segmentSize = (2.f * arcHalfAngle) / segments;

	for (unsigned int i = 0 ; i < segments ; ++i)
	{
		vec3 outer1(sinf(i * segmentSize - arcHalfAngle + rotation) * radius, 0, cosf(i * segmentSize - arcHalfAngle + rotation) * radius);
		vec3 outer2(sinf((i + 1) * segmentSize - arcHalfAngle + rotation) * radius, 0, cosf((i + 1) * segmentSize - arcHalfAngle + rotation) * radius);

		if (transform != nullptr)
		{
			outer1 = (*transform * vec4(outer1, 0)).xyz();
			outer2 = (*transform * vec4(outer2, 0)).xyz();
		}

		if (fillColour.w != 0.f)
		{
			AddTri(center, center + outer1, center + outer2, fillColour);
			AddTri(center + outer2, center + outer1, center, fillColour);
		}
		else
			AddLine(center + outer1, center + outer2, solid, solid);	//line
	}

	//edge lines
	if (fillColour.w == 0.f)
	{
		vec3 outer1(sinf(-arcHalfAngle + rotation) * radius, 0, cosf(-arcHalfAngle + rotation) * radius);
		vec3 outer2(sinf(arcHalfAngle + rotation) * radius, 0, cosf(arcHalfAngle + rotation) * radius);

		if (transform != nullptr)
		{
			outer1 = (*transform * vec4(outer1, 0)).xyz();
			outer2 = (*transform * vec4(outer2, 0)).xyz();
		}

		AddLine(center, center + outer1, solid, solid);
		AddLine(center, center + outer2, solid, solid);
	}
}

void Gizmos::AddArcRing(const vec3& center, 
						float rotation, 
						float innerRadius, float outerRadius, 
						float arcHalfAngle,
						unsigned int segments, 
						const vec4& fillColour, 
						const mat4* transform /* = nullptr */)
{
	vec4 solid = fillColour;
	solid.w = 1.f;

	float segmentSize = (2.f * arcHalfAngle) / segments;

	for (unsigned int i = 0 ; i < segments ; ++i)
	{
		vec3 outer1(sinf(i * segmentSize - arcHalfAngle + rotation) * outerRadius, 0, cosf(i * segmentSize - arcHalfAngle + rotation) * outerRadius);
		vec3 outer2(sinf((i + 1) * segmentSize - arcHalfAngle + rotation) * outerRadius, 0, cosf((i + 1) * segmentSize - arcHalfAngle + rotation) * outerRadius);
		vec3 inner1(sinf(i * segmentSize - arcHalfAngle + rotation) * innerRadius, 0, cosf(i * segmentSize - arcHalfAngle + rotation) * innerRadius);
		vec3 inner2(sinf((i + 1) * segmentSize - arcHalfAngle + rotation) * innerRadius, 0, cosf((i + 1) * segmentSize - arcHalfAngle + rotation) * innerRadius);

		if (transform != nullptr)
		{
			outer1 = (*transform * vec4(outer1, 0)).xyz();
			outer2 = (*transform * vec4(outer2, 0)).xyz();
			inner1 = (*transform * vec4(inner1, 0)).xyz();
			inner2 = (*transform * vec4(inner2, 0)).xyz();
		}

		if (fillColour.w != 0)
		{
			AddTri(center + outer2, center + outer1, center + inner1, fillColour);
			AddTri(center + inner1, center + inner2, center + outer2, fillColour);

			AddTri(center + inner1, center + outer1, center + outer2, fillColour);
			AddTri(center + outer2, center + inner2, center + inner1, fillColour);
		}
		else
		{
			//line
			AddLine(center + inner1, center + inner2, solid, solid);
			AddLine(center + outer1, center + outer2, solid, solid);
		}
	}

	//edge lines
	if (fillColour.w == 0.f)
	{
		vec3 outer1(sinf(-arcHalfAngle + rotation) * outerRadius, 0, cosf(-arcHalfAngle + rotation) * outerRadius);
		vec3 outer2(sinf(arcHalfAngle + rotation) * outerRadius, 0, cosf(arcHalfAngle + rotation) * outerRadius);
		vec3 inner1(sinf(-arcHalfAngle + rotation) * innerRadius, 0, cosf(-arcHalfAngle + rotation) * innerRadius);
		vec3 inner2(sinf(arcHalfAngle + rotation) * innerRadius, 0, cosf(arcHalfAngle + rotation) * innerRadius);

		if (transform != nullptr)
		{
			outer1 = (*transform * vec4(outer1, 0)).xyz();
			outer2 = (*transform * vec4(outer2, 0)).xyz();
			inner1 = (*transform * vec4(inner1, 0)).xyz();
			inner2 = (*transform * vec4(inner2, 0)).xyz();
		}

		AddLine(center + inner1, center + outer1, solid, solid);
		AddLine(center + inner2, center + outer2, solid, solid);
	}
}

void Gizmos::AddSphere(const vec3& center, float radius, 
						int rows, int columns, 
						const vec4& fillColour, 
						const mat4* transform /*= nullptr*/, 
						float longMin /*= 0.f*/, float longMax /*= 360.f*/, 
						float latMin /*= -90.f*/, float latMax /*= 90.f*/)
{
	float inverseRadius = 1.f / radius;
	//Invert these first as the multiply is slightly quicker
	float invColumns = 1.f / float(columns);
	float invRows = 1.f / float(rows);

	float DEG2RAD = pi<float>() / 180.f;
	
	//Lets put everything in radians first
	float latitiudinalRange = (latMax - latMin) * DEG2RAD;
	float longitudinalRange = (longMax - longMin) * DEG2RAD;
	//for each row of the mesh
	vec3* vectorArray = new vec3[rows * columns + columns];

	for (int row = 0; row <= rows; ++row)
	{
		//y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis  = ratioAroundXAxis * latitiudinalRange + (latMin * DEG2RAD);
		float y = radius * sin(radiansAboutXAxis);
		float z = radius * cos(radiansAboutXAxis);
		
		for (int col = 0; col <= columns; ++col)
		{
			float ratioAroundYAxis = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (longMin * DEG2RAD);
			vec3 point(-z * sinf(theta), y, -z * cosf(theta));
			vec3 normal(inverseRadius * point.x, inverseRadius * point.y, inverseRadius * point.z);

			if (transform != nullptr)
			{
				point = (*transform * vec4(point, 0)).xyz();
				normal = (*transform * vec4(normal, 0)).xyz();
			}

			int index = row * columns + (col % columns);
			vectorArray[index] = point;
		}
	}
	
	for (int face = 0; face < (rows * columns); ++face)
	{
		int nextFace = face + 1;		
		
		if(nextFace % columns == 0)
			nextFace = nextFace - (columns);

		AddLine(center + vectorArray[face], center + vectorArray[face + columns], vec4(1.f, 1.f, 1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f));
		
		if(face % columns == 0 && longitudinalRange < (pi<float>() * 2.f))
			continue;
		AddLine(center + vectorArray[nextFace + columns], center + vectorArray[face + columns], vec4(1.f, 1.f, 1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f));

		AddTri(center + vectorArray[nextFace + columns], center + vectorArray[face], center + vectorArray[nextFace], fillColour);
		AddTri(center + vectorArray[nextFace + columns], center + vectorArray[face + columns], center + vectorArray[face], fillColour);
	}

	delete[] vectorArray;	
}

void Gizmos::AddHermiteSpline(const vec3& start, const vec3& end,
								const vec3& tangentStart, const vec3& tangentEnd, 
								unsigned int segments, 
								const vec4& colour)
{
	segments = segments > 1 ? segments : 1;

	vec3 prev = start;

	for (unsigned int i = 1; i <= segments; ++i)
	{
		float s = (float)i / (float)segments;

		float s2 = s * s;
		float s3 = s2 * s;
		float h1 = (2.f * s3) - (3.f * s2) + 1.f;
		float h2 = (-2.f * s3) + (3.f * s2);
		float h3 =  s3- (2.f * s2) + s;
		float h4 = s3 - s2;
		vec3 p = (start * h1) + (end * h2) + (tangentStart * h3) + (tangentEnd * h4);

		AddLine(prev, p, colour, colour);
		prev = p;
	}
}

void Gizmos::AddLine(const vec3& rv0,  const vec3& rv1, const vec4& colour) { AddLine(rv0, rv1, colour, colour); }

void Gizmos::AddLine(const vec3& rv0, const vec3& rv1, const vec4& colour0, const vec4& colour1)
{
	if (sm_singleton != nullptr && sm_singleton->m_lineCount < sm_singleton->m_maxLines)
	{
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.x = rv0.x;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.y = rv0.y;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.z = rv0.z;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.w = 1;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.r = colour0.r;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.g = colour0.g;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.b = colour0.b;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.a = colour0.a;

		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.x = rv1.x;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.y = rv1.y;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.z = rv1.z;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.w = 1;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.r = colour1.r;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.g = colour1.g;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.b = colour1.b;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.a = colour1.a;

		sm_singleton->m_lineCount++;
	}
}

void Gizmos::AddTri(const vec3& rv0, const vec3& rv1, const vec3& rv2, const vec4& colour)
{
	if (sm_singleton != nullptr)
	{
		if (colour.w == 1.f)
		{
			if (sm_singleton->m_triCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.x = rv0.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.y = rv0.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.z = rv0.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.w = 1;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.x = rv1.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.y = rv1.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.z = rv1.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.w = 1;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.x = rv2.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.y = rv2.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.z = rv2.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.w = 1;

				sm_singleton->m_tris[sm_singleton->m_triCount].v0.r = colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.g = colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.b = colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.a = colour.a;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.r = colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.g = colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.b = colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.a = colour.a;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.r = colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.g = colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.b = colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.a = colour.a;

				sm_singleton->m_triCount++;
			}
		}
		else
		{
			if (sm_singleton->m_transparentTriCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.x = rv0.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.y = rv0.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.z = rv0.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.w = 1;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.x = rv1.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.y = rv1.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.z = rv1.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.w = 1;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.x = rv2.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.y = rv2.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.z = rv2.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.w = 1;

				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.r = colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.g = colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.b = colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.a = colour.a;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.r = colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.g = colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.b = colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.a = colour.a;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.r = colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.g = colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.b = colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.a = colour.a;

				sm_singleton->m_transparentTriCount++;
			}
		}
	}
}

void Gizmos::Add2DAABB(const vec2& center, const vec2& extents, const vec4& colour, const mat4* transform /*= nullptr*/)
{	
	vec2 verts[4];
	vec2 xVert(extents.x, 0);
	vec2 yVert(0, extents.y);

	if (transform != nullptr)
	{
		xVert = (*transform * vec4(xVert, 0, 0)).xy();
		yVert = (*transform * vec4(yVert, 0, 0)).xy();
	}

	verts[0] = center - xVert - yVert;
	verts[1] = center + xVert - yVert;
	verts[2] = center - xVert + yVert;
	verts[3] = center + xVert + yVert;

	Add2DLine(verts[0], verts[1], colour, colour);
	Add2DLine(verts[1], verts[2], colour, colour);
	Add2DLine(verts[2], verts[3], colour, colour);
	Add2DLine(verts[3], verts[0], colour, colour);
}

void Gizmos::Add2DAABBFilled(const vec2& center, const vec2& extents, const vec4& colour, const mat4* transform /*= nullptr*/)
{	
	vec2 verts[4];
	vec2 xVert(extents.x, 0);
	vec2 yVert(0, extents.y);

	if (transform != nullptr)
	{
		xVert = (*transform * vec4(xVert, 0, 0)).xy();
		yVert = (*transform * vec4(yVert, 0, 0)).xy();
	}

	verts[0] = center - xVert - yVert;
	verts[1] = center + xVert - yVert;
	verts[2] = center + xVert + yVert;
	verts[3] = center - xVert + yVert;
	
	Add2DTri(verts[0], verts[1], verts[2], colour);
	Add2DTri(verts[0], verts[2], verts[3], colour);
}

void Gizmos::Add2DCircle(const vec2& center, float radius, unsigned int segments, const vec4& colour, const mat4* transform /*= nullptr*/)
{
	vec4 solidColour = colour;
	solidColour.w = 1.f;

	float segmentSize = (2.f * pi<float>()) / (float)segments;

	for (unsigned int i = 0 ; i < segments ; ++i)
	{
		vec2 outer1(sinf(i * segmentSize) * radius, cosf(i * segmentSize) * radius);
		vec2 outer2(sinf((i + 1) * segmentSize) * radius, cosf((i + 1) * segmentSize) * radius);

		if (transform != nullptr)
		{
			outer1 = (*transform * vec4(outer1, 0, 0)).xy();
			outer2 = (*transform * vec4(outer2, 0, 0)).xy();
		}

		if (colour.w != 0)
		{
			Add2DTri(center, center + outer1, center + outer2, colour);
			Add2DTri(center + outer2, center + outer1, center, colour);
		}
		else
			Add2DLine(center + outer1, center + outer2, solidColour, solidColour);	//line
	}
}

void Gizmos::Add2DLine(const vec2& rv0,  const vec2& rv1, const vec4& colour) { Add2DLine(rv0, rv1, colour, colour); }

void Gizmos::Add2DLine(const vec2& rv0, const vec2& rv1, const vec4& colour0, const vec4& colour1)
{
	if (sm_singleton != nullptr && sm_singleton->m_2DlineCount < sm_singleton->m_max2DLines)
	{
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.x = rv0.x;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.y = rv0.y;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.z = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.w = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.r = colour0.r;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.g = colour0.g;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.b = colour0.b;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.a = colour0.a;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.x = rv1.x;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.y = rv1.y;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.z = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.w = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.r = colour1.r;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.g = colour1.g;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.b = colour1.b;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.a = colour1.a;

		sm_singleton->m_2DlineCount++;
	}
}

void Gizmos::Add2DTri(const vec2& rv0, const vec2& rv1, const vec2& rv2, const vec4& colour)
{
	if (sm_singleton != nullptr)
	{
		if (sm_singleton->m_2DtriCount < sm_singleton->m_max2DTris)
		{
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.x = rv0.x;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.y = rv0.y;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.z = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.w = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.x = rv1.x;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.y = rv1.y;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.z = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.w = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.x = rv2.x;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.y = rv2.y;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.z = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.w = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.r = colour.r;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.g = colour.g;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.b = colour.b;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.a = colour.a;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.r = colour.r;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.g = colour.g;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.b = colour.b;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.a = colour.a;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.r = colour.r;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.g = colour.g;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.b = colour.b;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.a = colour.a;

			sm_singleton->m_2DtriCount++;
		}
	}
}

void Gizmos::Draw(const mat4& projection, const mat4& view) { Draw(projection * view); }

void Gizmos::Draw(const mat4& projectionView)
{
	if (sm_singleton != nullptr && (sm_singleton->m_lineCount > 0 || sm_singleton->m_triCount > 0 || sm_singleton->m_transparentTriCount > 0))
	{
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

		sm_singleton->m_shader->Bind();
		int loc = sm_singleton->m_shader->GetUniform("projectionView");
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(projectionView));

		if (sm_singleton->m_lineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_lineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_lineCount * sizeof(GizmoLine), sm_singleton->m_lines);

			glBindVertexArray(sm_singleton->m_lineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_lineCount * 2);
		}

		if (sm_singleton->m_triCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_triVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_triCount * sizeof(GizmoTri), sm_singleton->m_tris);

			glBindVertexArray(sm_singleton->m_triVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_triCount * 3);
		}

		if (sm_singleton->m_transparentTriCount > 0)
		{
			//not ideal to store these, but Gizmos must work stand-alone
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);
			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
			int src;
			int dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			//setup blend states
			if (blendEnabled == GL_FALSE)
				glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_transparentTriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_transparentTriCount * sizeof(GizmoTri), sm_singleton->m_transparentTris);

			glBindVertexArray(sm_singleton->m_transparentTriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_transparentTriCount * 3);

			//reset state
			glDepthMask(depthMask);
			glBlendFunc(src, dst);
			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(shader);
	}
}

void Gizmos::Draw2D(const mat4& projection)
{
	if (sm_singleton != nullptr && (sm_singleton->m_2DlineCount > 0 || sm_singleton->m_2DtriCount > 0))
	{
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

		sm_singleton->m_shader->Bind();
		int loc = sm_singleton->m_shader->GetUniform("projectionView");
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(projection));

		if (sm_singleton->m_2DlineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DlineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DlineCount * sizeof(GizmoLine), sm_singleton->m_2Dlines);

			glBindVertexArray(sm_singleton->m_2DlineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_2DlineCount * 2);
		}

		if (sm_singleton->m_2DtriCount > 0)
		{
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);

			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

			int src;
			int dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			if (blendEnabled == GL_FALSE)
				glEnable(GL_BLEND);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DtriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DtriCount * sizeof(GizmoTri), sm_singleton->m_2Dtris);

			glBindVertexArray(sm_singleton->m_2DtriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_2DtriCount * 3);

			glDepthMask(depthMask);

			glBlendFunc(src, dst);

			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(shader);
	}
}