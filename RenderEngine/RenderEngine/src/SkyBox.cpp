#include "SkyBox.h"

#include "datatables/ProgramTable.h"
#include "datatables/MeshInstanceTable.h"

Engine::SkyBox::SkyBox(Engine::TextureInstance * skyboxTextureCubeMap)
	:skyCubeMap(skyboxTextureCubeMap)
{
	skyCubeMap->setMinificationFilterType(GL_NEAREST);
	skyCubeMap->setMagnificationFilterType(GL_NEAREST);
	skyCubeMap->setAnisotropicFilterEnabled(false);
	skyCubeMap->configureTexture();
	initialize();
}

Engine::SkyBox::~SkyBox()
{
	if (skyCubeMap != NULL)
	{
		delete skyCubeMap;
	}

	if (cubeMesh != NULL)
	{
		delete cubeMesh;
	}
}

void Engine::SkyBox::render(Engine::Camera * camera)
{
	Engine::Mesh * data = cubeMesh->getMesh();

	glUseProgram(shader->getProgramId());
	glBindVertexArray(data->vao);

	const glm::vec3 pos = camera->getPosition();
	glm::vec3 cubePos(pos);
	cubeMesh->setTranslation(cubePos * -1.0f);
	shader->onRenderObject(cubeMesh, camera->getViewMatrix(), camera->getProjectionMatrix());
	shader->setCubemapUniform(skyCubeMap);


	glDrawElements(GL_TRIANGLES, data->getNumFaces() * data->getNumVerticesPerFace(), GL_UNSIGNED_INT, (void*)0);
}

void Engine::SkyBox::initialize()
{
	shader = dynamic_cast<SkyProgram*>(Engine::ProgramTable::getInstance().getProgramByName("SkyProgram"));
	Engine::MeshInstanceTable::getInstance().instantiateMesh("cube", shader->getName());

	cubeMesh = new Engine::Object(Engine::MeshInstanceTable::getInstance().getMeshInstance("cube", "SkyProgram"));
	cubeMesh->setScale(glm::vec3(10, 10, 10));
}