#include <Viewer.hpp>
#include <ShaderProgram.hpp>

#include <texturing/TexturedPlaneRenderable.hpp>
#include <texturing/TexturedCubeRenderable.hpp>
#include <texturing/TexturedMeshRenderable.hpp>
#include <texturing/TexturedLightedMeshRenderable.hpp>
#include <texturing/MultiTexturedCubeRenderable.hpp>
#include <texturing/MipMapCubeRenderable.hpp>
#include <texturing/BillBoardPlaneRenderable.hpp>
#include <lighting/DirectionalLightRenderable.hpp>
#include <lighting/PointLightRenderable.hpp>
#include <lighting/SpotLightRenderable.hpp>

#include <texturing/TexturedTriangleRenderable.hpp>
#include <texturing/CubeMapRenderable.hpp>
#include <FrameRenderable.hpp>

#include <iostream>

const std::string MESHES_PATH = "../../sfmlGraphicsPipeline/meshes/";
const std::string TEXTURE_PATH = "../../sfmlGraphicsPipeline/textures/";

// easy way to add a shader to the viewer
ShaderProgramPtr addShader(Viewer &viewer, std::string vertex, std::string fragment)
{
	std::string shaderPath = "../../sfmlGraphicsPipeline/shaders/";
	ShaderProgramPtr shader = std::make_shared<ShaderProgram>(shaderPath + vertex + ".glsl", shaderPath + fragment + ".glsl");
	viewer.addShaderProgram(shader);
	return shader;
}

// if the shader has the same name for vertex and fragment
ShaderProgramPtr addShader(Viewer &viewer, std::string shad)
{
	return addShader(viewer, shad + "Vertex", shad + "Fragment");
}

// set the initial position of the camera
void setCameraPosition(Viewer &viewer, glm::vec3 initPos, glm::vec3 lookAt)
{
	viewer.getCamera().setViewMatrix(glm::lookAt(initPos, lookAt, glm::vec3(0, 1, 0)));
}

// using the view matrix that can be exported from the camera itself
void setCameraPosition(Viewer &viewer, glm::mat4 viewMatrix)
{
	viewer.getCamera().setViewMatrix(viewMatrix);
}

// easy way to add a cubemap
void addCubeMap(Viewer &viewer, std::string texture)
{
	ShaderProgramPtr cubeMapShader = addShader(viewer, "cubeMap");
	std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/" + texture;
	auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
	viewer.addRenderable(cubemap);
}

// easy way to create a textured object
std::shared_ptr<TexturedLightedMeshRenderable> createTexturedLightedObj(ShaderProgramPtr shader, std::string obj, std::string texture, MaterialPtr material)
{
	return std::make_shared<TexturedLightedMeshRenderable>(shader, MESHES_PATH + obj, material, TEXTURE_PATH + texture);
}

int main()
{
	Viewer viewer(1280, 720, glm::vec4(0.8, 0.8, 0.8, 1.0));

	/*SHADERS*/
	ShaderProgramPtr flatShader = addShader(viewer, "flat");
	ShaderProgramPtr texShader = addShader(viewer, "texture");
	ShaderProgramPtr wavesShader = addShader(viewer, "waves");
	ShaderProgramPtr defaultShader = addShader(viewer, "default");

	// Add a 3D frame to the viewer
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	/*LIGHTS*/

	glm::mat4 globalTransformation, localTransformation;

	// Define a directional light for the whole scene
	glm::vec3 d_direction = glm::normalize(glm::vec3(1.0, -1.0, 0.0));
	glm::vec3 d_ambient(0.5, 0.5, 0.6), d_diffuse(0.3, 0.3, 0.3), d_specular(0.5, 0.5, 0.5);

	glm::vec3 lightPosition(-12, 15, -10);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());

	// Add a renderable to display the light and control it via mouse/key event
	DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
	localTransformation = getScaleMatrix(0.5);
	directionalLightRenderable->setLocalTransform(localTransformation);
	viewer.addDirectionalLight(directionalLight);

	/*MATERIALS*/

	glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
	float mShininess = 1;
	MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);

	// custom ice material with more blue reflection and a big shininess
	MaterialPtr iceMaterial = std::make_shared<Material>(glm::vec3(0.7, 0.8, 1.0), glm::vec3(0.7, 0.8, 1.0), glm::vec3(10), 50);
	MaterialPtr simpleMaterial = std::make_shared<Material>(glm::vec3(0.8), glm::vec3(0.6), glm::vec3(0.3), 10);

	/*OBJECTS*/

	// create a simple seal who can dive into the sea
	auto seal = createTexturedLightedObj(texShader, "seal.obj", "seal.png", simpleMaterial);
	seal->setGlobalTransform(getTranslationMatrix(0.66, 1.6, 1.36) * getScaleMatrix(0.05f));
	seal->addLocalTransformKeyframe(getTranslationMatrix(0.66, 1.6, 1.36), 0);
	seal->addLocalTransformKeyframe(getTranslationMatrix(0.66, 1.6, 1.36), 1);

	seal->addLocalTransformKeyframe(getTranslationMatrix(0, 10, 10) * getRotationMatrix(-M_PI / 8, glm::vec3(1, 0, 0)), 1.3);
	seal->addLocalTransformKeyframe(getTranslationMatrix(0, -50, 55) * getRotationMatrix(M_PI / 2, glm::vec3(1, 0, 0)), 2.2);
	seal->addLocalTransformKeyframe(getTranslationMatrix(0, -50, 300) * getRotationMatrix(-M_PI / 8, glm::vec3(1, 0, 0)), 4);
	seal->addLocalTransformKeyframe(getTranslationMatrix(0, -35, 300) * getRotationMatrix(-M_PI / 8, glm::vec3(1, 0, 0)), 4.8);
	seal->addLocalTransformKeyframe(getTranslationMatrix(0, -35, 300) * getRotationMatrix(-M_PI / 8, glm::vec3(1, 0, 0)), 6);

	auto icebergs = createTexturedLightedObj(texShader, "hills.obj", "iceberg.png", iceMaterial);
	auto icebergs2 = createTexturedLightedObj(wavesShader, "hills.obj", "iceberg.png", myMaterial);
	icebergs->setGlobalTransform(getTranslationMatrix(0, -3.5, 0) * getScaleMatrix(20));
	icebergs->setWrapOption(2);

	auto hills = createTexturedLightedObj(texShader, "hills.obj", "hills.png", myMaterial);
	hills->setGlobalTransform(getTranslationMatrix(0, -8, -40) * getScaleMatrix(40));

	// use a custom wave shader to simulate some waves using sin
	auto waterPlane = std::make_shared<TexturedPlaneRenderable>(wavesShader, TEXTURE_PATH + "ocean/0.png");
	waterPlane->setGlobalTransform(getRotationMatrix(M_PI / 2, glm::vec3(1, 0, 0)) * getScaleMatrix(200));
	// custom function to set the wrap of a renderable
	waterPlane->setWrapOption(2);

	/*ADD RENDERABLES*/
	viewer.addRenderable(seal);
	viewer.addRenderable(waterPlane);
	viewer.addRenderable(icebergs2);
	viewer.addRenderable(icebergs);
	viewer.addRenderable(hills);

	/*START ANIMATION*/
	viewer.startAnimation();

	/*CAMERA*/
	Camera &camera = viewer.getCamera();
	glm::vec3 forward = glm::vec3(0, 0, -1); // In OpenGL, the camera's forward axis is -z
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-2.1, 6, 12.9), glm::vec3(-0.2, 7, 10), forward), 0);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-2.1, 4, 12.9), glm::vec3(-0.2, 1, 10), forward), 3.5);
	// trick to simulate a teleportation of the camera
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-0, 0.6, 17.3), glm::vec3(0.5, 0.29, 16.3), forward), 3.51);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-0, 0.6, 17.3), glm::vec3(0.5, 0.29, 16.3), forward), 6);

	addCubeMap(viewer, "skybox");
	while (viewer.isRunning())
	{
		viewer.handleEvent();
		viewer.animate();

		// render the gif located in the ocean folder. Displaying the 20 images in one second
		int imageNumber = static_cast<int>(viewer.getTime() / 2 * 20) % 20;
		waterPlane->setImage(TEXTURE_PATH + "ocean/" + std::to_string(imageNumber) + ".png");

		viewer.draw();
		viewer.display();
	}
	return EXIT_SUCCESS;
}