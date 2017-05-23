#include "ShaderManager.h"
#include "Shader.h"



ShaderManager* ShaderManager::m_instance = nullptr;

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
	for (auto shader : m_shaders) {
		delete shader;
	}
}

void ShaderManager::Init()
{
	m_instance = new ShaderManager();

	// Setup Shaders
	Shader* shader = new Shader("Shaders/SimpleVertexShader.vs", "Shaders/SimpleFragmentShader.frag");
	Shader* shader2 = new Shader("Shaders/SimpleVertexShader.vs", "Shaders/SimpleFragmentShader2.frag");
	Shader* colorShader = new Shader("Shaders/SimpleVertexColorShader.vs", "Shaders/SimpleFragmentShader.frag");
	Shader* textureShader = new Shader("Shaders/SimpleTextureShader.vs", "Shaders/SimpleTextureShader.frag");
	Shader* _3dShader = new Shader("Shaders/Simple3DShader.vs", "Shaders/SimpleTextureShader.frag");
	Shader* _3dLightColorShader = new Shader("Shaders/Simple3DShaderLightTut.vs", "Shaders/SimpleShaderLightColor.frag");
	Shader* lightRepShader = new Shader("Shaders/Simple3DShaderLightTut.vs", "Shaders/SimpleFragmentLightRep.frag");
	Shader* lightGuroudShader = new Shader("Shaders/Simple3DShaderLightColor.vs", "Shaders/SimpleFragmentShader2.frag");

	m_instance->m_shaders.push_back(shader);
	m_instance->m_shaders.push_back(shader2);
	m_instance->m_shaders.push_back(colorShader);
	m_instance->m_shaders.push_back(textureShader);
	m_instance->m_shaders.push_back(_3dShader);
	m_instance->m_shaders.push_back(_3dLightColorShader);
	m_instance->m_shaders.push_back(lightRepShader);
	m_instance->m_shaders.push_back(lightGuroudShader);
}

void ShaderManager::Destroy()
{
	if (m_instance) {
		delete m_instance;
	}
}

Shader* ShaderManager::getShaderByType(int _type)
{
	if (_type < m_shaders.size()) {
		return m_shaders[_type];
	}
	return nullptr;
}
