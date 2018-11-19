#ifndef SHADERSOURCE_H
#define SHADERSOURCE_H

#include "../System.h"
#include "ShaderConstant.h"

#include <list>
#include <string>

#define VERTEX_SHADER GL_VERTEX_SHADER
#define FRAGMENT_SHADER GL_FRAGMENT_SHADER
#define GEOMETRY_SHADER GL_GEOMETRY_SHADER
#define TESSELATION_CONTROL_SHADER GL_TESS_CONTROL_SHADER
#define TESSELATION_EVALUATION_SHADER GL_TESS_EVALUATION_SHADER

class ShaderSource {

public:
	ShaderSource(int32_t type, const char* filename);

	ShaderSource(ShaderSource* source);

	bool Reload();

	void AddMacro(const char* macro);

	void RemoveMacro(const char* macro);

	ShaderConstant* GetConstant(const char* constant);

	bool Compile();

	static void SetSourceDirectory(const char* directory);

	~ShaderSource();

	int32_t ID;
	int32_t type;

	const char* filename;

private:
	string ReadShaderFile(const char* filename, bool mainFile);

	time_t GetLastModified();

	string code;
	list<string> macros;
	list<ShaderConstant*> constants;

	time_t lastModified;

	static string sourceDirectory;

};


#endif