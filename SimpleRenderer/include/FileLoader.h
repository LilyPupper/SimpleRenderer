#pragma once

class Mesh;

class FileLoader
{
public:
	// Caller owns Mesh*
	static Mesh* LoadMesh(const char* _meshPath);
};