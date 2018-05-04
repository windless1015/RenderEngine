#include "programs/ProceduralTerrainProgram.h"

#include <iostream>

#include "WorldConfig.h"

const std::string Engine::ProceduralTerrainProgram::PROGRAM_NAME = "ProceduralTerrainProgram";

const unsigned long long Engine::ProceduralTerrainProgram::WIRE_DRAW_MODE = 0x01;
const unsigned long long Engine::ProceduralTerrainProgram::SHADOW_MAP = 0x02;

// ==================================================================================

Engine::ProceduralTerrainProgram::ProceduralTerrainProgram(std::string name, unsigned long long params) 
	:Program(name, params)
{
	vShaderFile =		"shaders/terrain/terrain.vert";
	tcsShaderFile =		"shaders/terrain/terrain.tesctrl";
	tevalShaderFile =	"shaders/terrain/terrain.teseval";
	gShaderFile =		"shaders/terrain/terrain.geom";
	fShaderFile =		"shaders/terrain/terrain.frag";
}

Engine::ProceduralTerrainProgram::ProceduralTerrainProgram(const ProceduralTerrainProgram & other)
	: Program(other)
{
	tcsShaderFile = other.tcsShaderFile;
	tevalShaderFile = other.tevalShaderFile;
	gShaderFile = other.gShaderFile;

	uModelView = other.uModelView;
	uModelViewProj = other.uModelViewProj;
	uNormal = other.uNormal;

	uLightDepthMatrix = other.uLightDepthMatrix;
	uDepthTexture = other.uDepthTexture;
	uLightDirection = other.uLightDirection;

	uAmplitude = other.uAmplitude;
	uFrecuency = other.uFrecuency;
	uScale = other.uScale;
	uOctaves = other.uOctaves;

	uInPos = other.uInPos;
	uInUV = other.uInUV;

	uGridPos = other.uGridPos;
}

void Engine::ProceduralTerrainProgram::initialize()
{
	std::string configStr = "";

	if (parameters & Engine::ProceduralTerrainProgram::WIRE_DRAW_MODE)
	{
		configStr += "#define WIRE_MODE";
	}

	if (parameters & Engine::ProceduralTerrainProgram::SHADOW_MAP)
	{
		configStr += "#define SHADOW_MAP";
	}

	vShader = loadShader(vShaderFile, GL_VERTEX_SHADER, configStr);
	tcsShader = loadShader(tcsShaderFile, GL_TESS_CONTROL_SHADER, configStr);
	tevalShader = loadShader(tevalShaderFile, GL_TESS_EVALUATION_SHADER, configStr);

	if (!(parameters & Engine::ProceduralTerrainProgram::SHADOW_MAP))
	{
		gShader = loadShader(gShaderFile, GL_GEOMETRY_SHADER, configStr);
	}

	fShader = loadShader(fShaderFile, GL_FRAGMENT_SHADER, configStr);

	glProgram = glCreateProgram();

	glAttachShader(glProgram, vShader);
	glAttachShader(glProgram, tcsShader);
	glAttachShader(glProgram, tevalShader);

	if (!(parameters & Engine::ProceduralTerrainProgram::SHADOW_MAP))
	{
		glAttachShader(glProgram, gShader);
	}

	glAttachShader(glProgram, fShader);

	glLinkProgram(glProgram);

	int linked;
	glGetProgramiv(glProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint logLen;
		glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &logLen);
		char *logString = new char[logLen];
		glGetProgramInfoLog(glProgram, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;
		exit(-1);
	}

	configureProgram();
}

void Engine::ProceduralTerrainProgram::configureProgram()
{
	uModelView = glGetUniformLocation(glProgram, "modelView");
	uModelViewProj = glGetUniformLocation(glProgram, "modelViewProj");
	uNormal = glGetUniformLocation(glProgram, "normal");
	uGridPos = glGetUniformLocation(glProgram, "gridPos");

	uLightDepthMatrix = glGetUniformLocation(glProgram, "lightDepthMat");
	uLightDirection = glGetUniformLocation(glProgram, "lightDir");
	uDepthTexture = glGetUniformLocation(glProgram, "depthTexture");

	uAmplitude = glGetUniformLocation(glProgram, "amplitude");
	uFrecuency = glGetUniformLocation(glProgram, "frecuency");
	uScale = glGetUniformLocation(glProgram, "scale");
	uOctaves = glGetUniformLocation(glProgram, "octaves");

	uInPos = glGetAttribLocation(glProgram, "inPos");
	uInUV = glGetAttribLocation(glProgram, "inUV");
}

void Engine::ProceduralTerrainProgram::configureMeshBuffers(Engine::Mesh * data)
{
	glBindVertexArray(data->vao);

	if (uInPos != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, data->vboVertices);
		glVertexAttribPointer(uInPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(uInPos);
	}

	if (uInUV != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, data->vboUVs);
		glVertexAttribPointer(uInUV, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(uInUV);
	}
}

void Engine::ProceduralTerrainProgram::onRenderObject(const Engine::Object * obj, const glm::mat4 & view, const glm::mat4 &proj)
{
	glm::mat4 modelView = view * obj->getModelMatrix();
	glm::mat4 modelViewProj = proj * view * obj->getModelMatrix();
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

	glUniformMatrix4fv(uModelView, 1, GL_FALSE, &(modelView[0][0]));
	glUniformMatrix4fv(uModelViewProj, 1, GL_FALSE, &(modelViewProj[0][0]));
	glUniformMatrix4fv(uNormal, 1, GL_FALSE, &(normal[0][0]));

	unsigned int vertexPerFace = obj->getMesh()->getNumVerticesPerFace();
	glPatchParameteri(GL_PATCH_VERTICES, vertexPerFace);

	glUniform1f(uAmplitude, Engine::Settings::terrainAmplitude);
	glUniform1f(uFrecuency, Engine::Settings::terrainFrecuency);
	glUniform1f(uScale, Engine::Settings::terrainScale);
	glUniform1i(uOctaves, Engine::Settings::terrainOctaves);
}

void Engine::ProceduralTerrainProgram::setUniformGridPosition(unsigned int i, unsigned int j)
{
	glUniform2i(uGridPos, i, j);
}

void Engine::ProceduralTerrainProgram::setUniformLightDepthMatrix(const glm::mat4 & ldm)
{
	glUniformMatrix4fv(uLightDepthMatrix, 1, GL_FALSE, &(ldm[0][0]));
}

void Engine::ProceduralTerrainProgram::setUniformDepthTexture(Engine::TextureInstance * depthTexture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture->getTexture()->getTextureId());
	glUniform1i(uDepthTexture, 0);
}

void Engine::ProceduralTerrainProgram::setUniformLightDirection(const glm::vec3 & lightDir)
{
	glUniform3fv(uLightDirection, 1, &lightDir[0]);
}

void Engine::ProceduralTerrainProgram::destroy()
{
	glDetachShader(glProgram, vShader);
	glDeleteShader(vShader);

	glDetachShader(glProgram, tcsShader);
	glDeleteShader(tcsShader);

	glDetachShader(glProgram, tevalShader);
	glDeleteShader(tevalShader);

	glDetachShader(glProgram, gShader);
	glDeleteShader(gShader);

	glDetachShader(glProgram, fShader);
	glDeleteShader(fShader);

	glDeleteProgram(glProgram);
}

// ==============================================================================

Engine::Program * Engine::ProceduralTerrainProgramFactory::createProgram(unsigned long long parameters)
{
	Engine::ProceduralTerrainProgram * program = new Engine::ProceduralTerrainProgram(Engine::ProceduralTerrainProgram::PROGRAM_NAME, parameters);
	program->initialize();
	return program;
}