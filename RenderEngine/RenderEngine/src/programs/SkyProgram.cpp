#include "programs/SkyProgram.h"

Engine::SkyProgram::SkyProgram(std::string name)
	:Program(name)
{
}

Engine::SkyProgram::SkyProgram(const Engine::SkyProgram & other)
	: Program(other)
{
	uProjMatrix = other.uProjMatrix;
	uCubeMap = other.uCubeMap;

	inPos = other.inPos;
}

void Engine::SkyProgram::configureProgram()
{
	uProjMatrix = glGetUniformLocation(glProgram, "proj");
	uCubeMap = glGetUniformLocation(glProgram, "skybox");

	inPos = glGetAttribLocation(glProgram, "inPos");
}

void Engine::SkyProgram::configureMeshBuffers(Engine::MeshInstance * mesh)
{
	Engine::Mesh * m = mesh->getMesh();
	glBindVertexArray(m->vao);

	if (inPos != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->vboVertices);
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);
	}
}

void Engine::SkyProgram::onRenderObject(const Engine::Object * obj, const glm::mat4 & view, const glm::mat4 &proj)
{
	glm::mat4 modelViewProj = proj * view * obj->getModelMatrix();
	glUniformMatrix4fv(uProjMatrix, 1, GL_FALSE, &(modelViewProj[0][0]));
}

void Engine::SkyProgram::setCubemapUniform(Engine::TextureInstance * t)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, t->getTexture()->getTextureId());
	glUniform1i(uCubeMap, 0);

}