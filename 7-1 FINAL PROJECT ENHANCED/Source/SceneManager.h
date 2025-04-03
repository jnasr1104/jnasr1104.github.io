// ======================================================================
// ENHANCED SCENEMANAGER.H - STRUCTURED, COMMENTED, AND OPTIMIZED HEADER
// ======================================================================

// [INFO] /////////////////////////////////////////////////////////////////////////////
// [INFO] scenemanager.h
// [INFO] ============
// [INFO] manage the preparing and rendering of 3D scenes - textures, materials, lighting
// [INFO] 
// [INFO] AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
// [INFO] Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
// [INFO] /////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"

#include <string>
#include <vector>

/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/
class SceneManager
{
public:
	// [INFO] constructor

	// --------------------------------------------------
	// SCENEMANAGER FUNCTION PROTOTYPE
	// --------------------------------------------------
	SceneManager(ShaderManager* pShaderManager);
	// [INFO] destructor

	// --------------------------------------------------
	// ~SCENEMANAGER FUNCTION PROTOTYPE
	// --------------------------------------------------
	~SceneManager();

	struct TEXTURE_INFO
	{
		std::string tag;
		uint32_t ID;
	};

	struct OBJECT_MATERIAL
	{
		float ambientStrength;
		glm::vec3 ambientColor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		float shininess;
		std::string tag;
	};

private:
	// [INFO] pointer to shader manager object
	ShaderManager* m_pShaderManager;
	// [INFO] pointer to basic shapes object
	ShapeMeshes* m_basicMeshes;
	// [INFO] total number of loaded textures
	int m_loadedTextures;
	// [INFO] loaded textures info
	TEXTURE_INFO m_textureIDs[16];
	// [INFO] defined object materials
	std::vector<OBJECT_MATERIAL> m_objectMaterials;

	// [INFO] load texture images and convert to OpenGL texture data
	bool CreateGLTexture(const char* filename, std::string tag);
	// [INFO] bind loaded OpenGL textures to slots in memory
	void BindGLTextures();
	// [INFO] free the loaded OpenGL textures
	void DestroyGLTextures();
	// [INFO] find a loaded texture by tag
	int FindTextureID(std::string tag);
	int FindTextureSlot(std::string tag);
	// [INFO] find a defined material by tag
	bool FindMaterial(std::string tag, OBJECT_MATERIAL& material);

	// [INFO] set the transformation values
	// [INFO] into the transform buffer
	void SetTransformations(
		glm::vec3 scaleXYZ,
		float XrotationDegrees,
		float YrotationDegrees,
		float ZrotationDegrees,
		glm::vec3 positionXYZ);

	// [INFO] set the color values into the shader
	void SetShaderColor(
		float redColorValue,
		float greenColorValue,
		float blueColorValue,
		float alphaValue);

	// [INFO] set the texture data into the shader
	void SetShaderTexture(
		std::string textureTag);

	// [INFO] set the UV scale for the texture mapping
	void SetTextureUVScale(
		float u, float v);

	// [INFO] set the object material into the shader
	void SetShaderMaterial(
		std::string materialTag);

	void BindMaterialTextures(const std::string& materialPrefix);

public:

	// [INFO] prepare the 3D scene for rendering
	void PrepareScene();
	// [INFO] render the objects in the 3D scene
	void RenderScene();

	// [INFO] load all of the needed textures before rendering
	void LoadSceneTextures();
	// [INFO] define all the object materials before rendering
	void DefineObjectMaterials();
	// [INFO] add and define the light sources before rendering
	void SetupSceneLights();

};