#pragma once

#include "Program.h"

namespace Engine
{
	class ProceduralTerrainProgram : public Program
	{
	private:

		unsigned int tcsShader, tevalShader, gShader;

		unsigned int uInPos;
		unsigned int uInUV;

		unsigned int uModelView;
		unsigned int uModelViewProj;
		unsigned int uNormal;
		unsigned int uGridPos;

	public:
		ProceduralTerrainProgram(std::string name);
		ProceduralTerrainProgram(const ProceduralTerrainProgram & other);
		void initialize(std::string vShader, std::string fShader);
		void configureProgram();
		void setUniformGridPosition(unsigned int i, unsigned int j);
		void configureMeshBuffers(MeshInstance * mesh);
		void onRenderObject(const Object * obj, const glm::mat4 & view, const glm::mat4 &proj);
		void destroy();
	};
}