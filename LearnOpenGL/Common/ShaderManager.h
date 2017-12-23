#ifndef __SHADER_MANGER_H__

#define __SHADER_MANAGER_H_

#include <vector>

#define SHADER_TYPE_VERTICE_ONLY 0
#define SHADER_TYPE_VERTICE_SET_COLOR 1
#define SHADER_TYPE_VERTICE_ONLY2 2
#define SHADER_TYPE_VERTICE_TEXCOORD 3
#define SHADER_TYPE_VERTICE_TEXCOORD_TRANSFORM 4
#define SHADER_TYPE_VERTICE_LIGHT_PHONG 5
#define SHADER_TYPE_VERTICE_LIGHT_REP 6
#define SHADER_TYPE_VERTICE_LIGHT_GUROUD 7
#define SHADER_TYPE_VERTICE_DEPTH_ONLY 8
#define SHADER_TYPE_VERTICE_SINGLE_COLOR 9
#define SHADER_TYPE_VERTICE_TRANSPARENT_COLOR 10


class Shader;

class ShaderManager
{
private:
	
	static ShaderManager *m_instance;

	ShaderManager();
	
	~ShaderManager();

public:

	static void Init();
	static void Destroy();
	static ShaderManager* getInstance() { return m_instance; }

	Shader* getShaderByType(int _type);

private:
	std::vector<Shader*> m_shaders;

};

#endif // 

