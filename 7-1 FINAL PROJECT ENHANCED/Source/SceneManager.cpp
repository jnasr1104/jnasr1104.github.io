// ======================================================================
// ENHANCED SCENEMANAGER.CPP - STRUCTURED, COMMENTED, AND OPTIMIZED
// ======================================================================

// [INFO] /////////////////////////////////////////////////////////////////////////////
// [INFO] scenemanager.cpp
// [INFO] ============
// [INFO] manage the preparing and rendering of 3D scenes - textures, materials, lighting
// [INFO] 
// [INFO] AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
// [INFO] Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
// [INFO] /////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// [INFO] declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/

 // -------------------------------------------
 // SCENEMANAGER FUNCTION
 // -------------------------------------------
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();

	// [INFO] initialize the texture collection
	for (int i = 0; i < 16; i++)
	{
		m_textureIDs[i].tag = "/0";
		m_textureIDs[i].ID = -1;
	}
	m_loadedTextures = 0;


}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/

 // -------------------------------------------
 // ~SCENEMANAGER FUNCTION
 // -------------------------------------------
SceneManager::~SceneManager()
{
	// [INFO] free the allocated objectsFp
	m_pShaderManager = NULL;
	if (NULL != m_basicMeshes)
	{
		delete m_basicMeshes;
		m_basicMeshes = NULL;
	}

	// [INFO] free the allocated OpenGL textures
	DestroyGLTextures();
}
/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/

 // -------------------------------------------
 // STRING TAG) FUNCTION
 // -------------------------------------------
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// [INFO] indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// [INFO] try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// [INFO] if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// [INFO] set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// [INFO] set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// [INFO] if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// [INFO] if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// [INFO] generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// [INFO] free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// [INFO] register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// [INFO] Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/

 // -------------------------------------------
 // BINDGLTEXTURES FUNCTION
 // -------------------------------------------
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// [INFO] bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/

 // -------------------------------------------
 // DESTROYGLTEXTURES FUNCTION
 // -------------------------------------------
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/

 // -------------------------------------------
 // STRING TAG) FUNCTION
 // -------------------------------------------
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/

 // -------------------------------------------
 // STRING TAG) FUNCTION
 // -------------------------------------------
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/

 // -------------------------------------------
 // STRING TAG, OBJECT_MATERIAL& MATERIAL) FUNCTION
 // -------------------------------------------
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/

 // -------------------------------------------
 // SETTRANSFORMATIONS FUNCTION
 // -------------------------------------------
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// [INFO] variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// [INFO] set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// [INFO] set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// [INFO] set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/

 // -------------------------------------------
 // SETSHADERCOLOR FUNCTION
 // -------------------------------------------
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// [INFO] variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/

 // -------------------------------------------
 // SETSHADERTEXTURE FUNCTION
 // -------------------------------------------
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/

 // -------------------------------------------
 // SETTEXTUREUVSCALE FUNCTION
 // -------------------------------------------
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/

 // -------------------------------------------
 // SETSHADERMATERIAL FUNCTION
 // -------------------------------------------
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
 // [INFO] TEXTURES

 // -------------------------------------------
 // LOADSCENETEXTURES FUNCTION
 // -------------------------------------------
void SceneManager::LoadSceneTextures() //loads textures in from texture file
{
	bool bReturn = false;


	// Load improved wood textures	
	bReturn = CreateGLTexture("textures/wood/wood1.jpg", "wood");


	// Load improved metal textures
	bReturn = CreateGLTexture("textures/metal/metal.jpg", "metal");


	// Load wall texture
	bReturn = CreateGLTexture("textures/wall/wall3.jpg", "wall");
	

	// Load cloth (mousepad/fabric) texture
	bReturn = CreateGLTexture("textures/fabric/fabric1.jpg", "fabric");



	// Load white plastic or marble texture (for keyboard or accessories)
	bReturn = CreateGLTexture("textures/plastic/plastic.jpg", "plastic");

	// Load keyboard material
	bReturn = CreateGLTexture("textures/keyboard/keyboard.jpg", "keyboard");

	// Load mouse material
	bReturn = CreateGLTexture("textures/mouse/mouse2.jpg", "mouse");

	// Load bottle material
	bReturn = CreateGLTexture("textures/bottle/bottle.jpg", "bottle");

	// Load silver material
	bReturn = CreateGLTexture("textures/silver/silver.jpg", "silver");



	BindGLTextures();
}
/***********************************************************
 *  DefineObjectMaterials()
 *
 *  This method is used for configuring the various material
 *  settings for all of the objects within the 3D scene.
 ***********************************************************/


 // -------------------------------------------
 // DEFINEOBJECTMATERIALS FUNCTION (Enhanced)
 // -------------------------------------------

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/

 // -------------------------------------------
 // SETUPSCENELIGHTS FUNCTION
 // -------------------------------------------
void SceneManager::SetupSceneLights()
{
	// [INFO] Enable lighting calculations
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	// ===== Directional Light =====
	m_pShaderManager->setVec3Value("directionalLight.direction", -0.3f, -0.4f, -0.5f);
	m_pShaderManager->setVec3Value("directionalLight.ambient", 0.3f, 0.3f, 0.3f);
	m_pShaderManager->setVec3Value("directionalLight.diffuse", 0.6f, 0.6f, 0.6f);
	m_pShaderManager->setVec3Value("directionalLight.specular", 0.9f, 0.9f, 0.9f);
	m_pShaderManager->setBoolValue("directionalLight.bActive", true);

	// ===== Spotlight (Camera Flash or Desk Spotlight) =====
	m_pShaderManager->setVec3Value("spotLight.ambient", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("spotLight.diffuse", 1.2f, 1.2f, 1.2f);
	m_pShaderManager->setVec3Value("spotLight.specular", 1.5f, 1.5f, 1.5f);
	m_pShaderManager->setFloatValue("spotLight.constant", 1.0f);
	m_pShaderManager->setFloatValue("spotLight.linear", 0.07f);
	m_pShaderManager->setFloatValue("spotLight.quadratic", 0.017f);
	m_pShaderManager->setFloatValue("spotLight.cutOff", glm::cos(glm::radians(18.0f)));
	m_pShaderManager->setFloatValue("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));
	m_pShaderManager->setBoolValue("spotLight.bActive", true);

	// ===== New Point Light (Above Desk / Ceiling Lamp Effect) =====
	m_pShaderManager->setVec3Value("pointLight.position", 0.0f, 10.0f, 0.0f);  // Above desk
	m_pShaderManager->setVec3Value("pointLight.ambient", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setVec3Value("pointLight.diffuse", 1.0f, 0.9f, 0.8f);   // Slight warm tone
	m_pShaderManager->setVec3Value("pointLight.specular", 1.3f, 1.3f, 1.3f);
	m_pShaderManager->setFloatValue("pointLight.constant", 1.0f);
	m_pShaderManager->setFloatValue("pointLight.linear", 0.045f);
	m_pShaderManager->setFloatValue("pointLight.quadratic", 0.0075f);
	m_pShaderManager->setBoolValue("pointLight.bActive", true);
}


// -------------------------------------------
// PREPARESCENE FUNCTION
// -------------------------------------------
void SceneManager::PrepareScene()
{
	// [INFO] only one instance of a particular mesh needs to be
	// [INFO] loaded in memory no matter how many times it is drawn
	// [INFO] in the rendered 3D scene

	LoadSceneTextures();


	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadTorusMesh();

}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/

 // -------------------------------------------
 // RENDERSCENE FUNCTION
 // -------------------------------------------
void SceneManager::RenderScene()
{
	// [INFO] declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/

	// [INFO] set the XYZ scale for the mesh //THIS IS THE PLANE
	scaleXYZ = glm::vec3(20.0f, 1.5f, 10.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] tShaderColor(1, 1, 1, 1);
	// [INFO] draw the mesh with transformation values


	SetShaderTexture("wood");//creates the texture onto the plane
	SetTextureUVScale(1.0, 1.0);
	SetShaderMaterial("plastic");

	m_basicMeshes->DrawBoxMesh();

//////////////////////////////////////////////////////////////////////

// [INFO] CYLINDER ONE //MONITOR ARM
// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.5f, 3.0f, 0.25f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-1.0f, 1.0f, -5.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(2.0, 2.0);
	SetShaderMaterial("plastic");

	m_basicMeshes->DrawCylinderMesh();

//////////////////////////////////////////////////////////////////////

	// [INFO] CYLINDER TWO //MONITOR ARM
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.5f, 7.0f, 0.25f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 15.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 60.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-1.0f, 4.0f, -5.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(2.0, 2.0);
	SetShaderMaterial("plastic");


	m_basicMeshes->DrawCylinderMesh();

//////////////////////////////////////////////////////////////////////

// [INFO] BOX ONE //MONITOR
// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(7.0f, 4.0f, 1.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-5.0f, 6.0f, -3.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("metal");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);
	SetShaderMaterial("plastic");

	m_basicMeshes->DrawBoxMesh();


//////////////////////////////////////////////////////////////////////

// [INFO] BOX TWO //MONITOR Stand
// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.0f, 1.0f, 1.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 1.0f, -5.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);
	SetShaderMaterial("plastic");

	// [INFO] draw the mesh with transformation values
	m_basicMeshes->DrawBoxMesh();




///////////////////////////////////////////////////////////////////
// [INFO] draw the plane wall mesh

	scaleXYZ = glm::vec3(15.0f, 3.0f, 15.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 4.0f, -5.5f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);
	// [INFO] draw the mesh with transformation values
	SetShaderTexture("wall");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);
	m_basicMeshes->DrawPlaneMesh();

//////////////////////////////////////////////////////////////////////

// [INFO] CYLINDER THREE //MONITOR ARM
// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.5f, 3.0f, 0.25f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(1.0f, 1.0f, -5.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(2.0, 2.0);
	SetShaderMaterial("plastic");

	m_basicMeshes->DrawCylinderMesh();


//////////////////////////////////////////////////////////////////////

	// [INFO] CYLINDER FOUR //MONITOR ARM
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.5f, 7.0f, 0.25f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 15.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -60.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(1.0f, 4.0f, -5.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(2.0, 2.0);
	SetShaderMaterial("plastic");


	m_basicMeshes->DrawCylinderMesh();

//////////////////////////////////////////////////////////////////////

	// [INFO] BOX THREE //MONITOR
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(7.0f, 4.0f, 1.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, 6.0f, -3.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("metal");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);
	SetShaderMaterial("plastic");

	m_basicMeshes->DrawBoxMesh();

//////////////////////////////////////////////////////////////////////

	// [INFO] BOX FOUR //MOUSE PAD
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(18.0f, 0.15f, 4.5f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.5f, 1.0f, -0.10f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("fabric");//creates the texture onto the shape
	SetTextureUVScale(0.5, 0.5);

	m_basicMeshes->DrawBoxMesh();
//////////////////////////////////////////////////////////////////////
	// [INFO] BOX FIVE //KEYBOARD
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(6.0f, 1.0f, 2.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.0f, 1.0f, -0.25f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("keyboard");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawBoxMesh();
/////////////////////////////////////////////////////////////////////////////////

		// [INFO] BOX SIX //KEYBOARD
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(6.0f, 1.0f, 0.25f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.0f, 1.1f, 0.65f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawBoxMesh();
///////////////////////////////////////////////////////////////////////////////
	// [INFO] BOX SEVEN //KEYBOARD
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(6.0f, 1.0f, 0.25f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.0f, 1.1f, -1.25f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawBoxMesh();

///////////////////////////////////////////////////////////////////////////////
	// [INFO] BOX EIGHT //KEYBOARD
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.25f, 0.75f, 2.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-5.0f, 1.23f, -0.25f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawBoxMesh();

///////////////////////////////////////////////////////////////////////////////
	// [INFO] BOX NINE //KEYBOARD
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.25f, 0.75f, 2.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(1.0f, 1.23f, -0.25f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("plastic");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawBoxMesh();

///////////////////////////////////////////////////////////////////////////////
	// [INFO] SPHERE //MOUSE
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.50f, 0.60f, 1.0f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(4.0f, 1.0f, -0.25f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("mouse");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawSphereMesh();
/////////////////////////////////////////////////////////////////////////////////

	// [INFO] CYLINDER //BOTTLE
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.75f, 0.30f, 0.75f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-8.0f, 0.85f, 4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("silver");//creates the texture onto the shape
	SetTextureUVScale(0.5, 0.5);

	m_basicMeshes->DrawCylinderMesh();

/////////////////////////////////////////////////////////////////////////////////

	// [INFO] CYLINDER 2 //BOTTLE
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.75f, 4.0f, 0.75f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-8.0f, 1.20f, 4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("bottle");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawCylinderMesh();
/////////////////////////////////////////////////////////////////////////////////

	// [INFO] TORUS //BOTTLE
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.55f, 0.55f, 0.55f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 90.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-8.0f, 5.25f, 4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("silver");//creates the texture onto the shape
	SetTextureUVScale(0.5, 0.5);

	m_basicMeshes->DrawTorusMesh();
/////////////////////////////////////////////////////////////////////////////////

	// [INFO] CYLINDER 3 //BOTTLE
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.50f, 0.50f, 0.50f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-8.0f, 5.20f, 4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("silver");//creates the texture onto the shape
	SetTextureUVScale(0.5, 0.5);

	m_basicMeshes->DrawCylinderMesh();
/////////////////////////////////////////////////////////////////////////////////

	// [INFO] TORUS 2 //BOTTLE
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.40f, 0.40f, 0.50f);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-8.0f, 5.75f, 4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("silver");//creates the texture onto the shape
	SetTextureUVScale(0.5, 0.5);

	m_basicMeshes->DrawTorusMesh();
/////////////////////////////////////////////////////////////////////////////////
	//DESK LEGS
	// [INFO] BOX TEN
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 10.0f, 1.5);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-8.0f, -5.5f, -4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("wood");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawBoxMesh();
/////////////////////////////////////////////////////////////////////////////////

	// [INFO] BOX ELEVEN
		// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 10.0f, 1.5);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(8.0f, -5.5f, 4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("wood");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);


	m_basicMeshes->DrawBoxMesh();

/////////////////////////////////////////////////////////////////////////////////

	// [INFO] BOX TWELVE
		// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 10.0f, 1.5);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(8.0f, -5.5f, -4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("wood");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);


	m_basicMeshes->DrawBoxMesh();
/////////////////////////////////////////////////////////////////////////////////

	// [INFO] BOX THIRTEEN
	// [INFO] set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 10.0f, 1.5);

	// [INFO] set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// [INFO] set the XYZ position for the mesh
	positionXYZ = glm::vec3(-8.0f, -5.5f, 4.0f);

	// [INFO] set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// [INFO] SetShaderColor(1, 1, 1, 1);

	// [INFO] draw the mesh with transformation values
	SetShaderTexture("wood");//creates the texture onto the shape
	SetTextureUVScale(1.0, 1.0);

	m_basicMeshes->DrawBoxMesh();

}