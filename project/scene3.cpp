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
	ShaderProgramPtr defaultShader = addShader(viewer, "default");

	// Add a 3D frame to the viewer
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	/*LIGHTS*/

	glm::mat4 globalTransformation, localTransformation;

	// Define a directional light for the whole scene
	glm::vec3 d_direction = glm::normalize(glm::vec3(1.0, -1.0, 0.0));
	glm::vec3 d_ambient(0.1, 0.1, 0.1), d_diffuse(0.3, 0.3, 0.3), d_specular(0.5, 0.5, 0.5);

	glm::vec3 lightPosition(-12, 15, -10);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());

	// Add a renderable to display the light and control it via mouse/key event
	DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
	localTransformation = getScaleMatrix(0.5);
	directionalLightRenderable->setLocalTransform(localTransformation);
	viewer.addDirectionalLight(directionalLight);

	// Define a spot light
	glm::vec3 s_position(9, 2.0, -2.0), s_spotDirection = glm::vec3(5, 0, -1);
	// glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
	glm::vec3 s_ambient(0.0, 0.0, 0.0), s_diffuse(0.2, 0.2, 0.4), s_specular(1.0, 1.0, 1.0);
	float s_constant = 1.0, s_linear = 0.0, s_quadratic = 0.0;
	float s_innerCutOff = std::cos(glm::radians(10.0f)), s_outerCutOff = std::cos(glm::radians(20.0f));

	SpotLightPtr spotLight2 = std::make_shared<SpotLight>(glm::vec3(0, 5, 0), glm::vec3(0, 0, 0),
														  s_ambient, s_diffuse, s_specular,
														  s_constant, s_linear, s_quadratic,
														  s_innerCutOff, s_outerCutOff);
	SpotLightRenderablePtr spotLightRenderable2 = std::make_shared<SpotLightRenderable>(flatShader, spotLight2);
	viewer.addSpotLight(spotLight2);

	/*MATERIALS*/

	glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
	float mShininess = 1;
	MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);
	MaterialPtr iceMaterial = std::make_shared<Material>(glm::vec3(0.7, 0.8, 1.0), glm::vec3(0.7, 0.8, 1.0), glm::vec3(10), 50);
	MaterialPtr simpleMaterial = std::make_shared<Material>(glm::vec3(0.8), glm::vec3(0.6), glm::vec3(0.3), 10);

	/*OBJECTS*/

	auto boat = createTexturedLightedObj(texShader, "boat.obj", "boat.png", simpleMaterial);
	boat->setGlobalTransform(getTranslationMatrix(0, -1, 0) * getScaleMatrix(0.15f, 0.15f, 0.15f));

	auto penguin = createTexturedLightedObj(texShader, "penguin_main.obj", "penguin.png", simpleMaterial);
	penguin->setGlobalTransform(getTranslationMatrix(0, 0.5, -0.9) * getScaleMatrix(0.15f, 0.15f, 0.15f));

	auto right_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	right_arm_penguin->setGlobalTransform(getTranslationMatrix(0.8, 2, 0));
	right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(M_PI, glm::vec3(1, 0, 0)), 0);
	right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(0, glm::vec3(1, 0, 0)), 0.15);
	right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(-M_PI, glm::vec3(1, 0, 0)), 0.3);

	auto left_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	left_arm_penguin->setGlobalTransform(getTranslationMatrix(-0.8, 2, 0) * getRotationMatrix(M_PI, glm::vec3(0, 1, 0)));
	left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(M_PI, glm::vec3(1, 0, 0)), 0);
	left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(0, glm::vec3(1, 0, 0)), 0.15);
	left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(-M_PI, glm::vec3(1, 0, 0)), 0.3);

	HierarchicalRenderable::addChild(penguin, right_arm_penguin);
	HierarchicalRenderable::addChild(penguin, left_arm_penguin);

	/*ADD RENDERABLES*/
	viewer.addRenderable(boat);
	viewer.addRenderable(penguin);

	/*START ANIMATION*/
	viewer.startAnimation();

	/*CAMERA*/
	Camera &camera = viewer.getCamera();

	setCameraPosition(viewer, glm::mat4({0.996051, -0.0265845, 0.0847113, -0, 1.86265e-09, 0.95412, 0.299426, -0, -0.0887847, -0.298244, 0.950352, -0, -0.117166, -1.09141, 0.445884, 1}));
	viewer.setKeyboardSpeed(6);
	viewer.setSimulationTime(5.5);
	viewer.setBackgroundColor(glm::vec4(0));

	double startFov = 0.5;
	double targetFov = 2.5;
	double transitionDuration = 0.5; // seconds
	double currentFov = startFov;
	auto startTime = std::chrono::high_resolution_clock::now();
	bool increasingFov = true;

	// this scene is not used in the project. It's just a poc using the fov of the camera
	while (viewer.isRunning()) {
		viewer.handleEvent();
		viewer.animate();

		auto currentTime = std::chrono::high_resolution_clock::now();
		double elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime).count();

		if (elapsedTime < transitionDuration) {
			double progress = elapsedTime / transitionDuration;
			currentFov = startFov + progress * (targetFov - startFov);
		}
		else {
			if (increasingFov) {
				currentFov = targetFov;
				increasingFov = false;
				startTime = std::chrono::high_resolution_clock::now();
			} else {
				currentFov = startFov;
				increasingFov = true;
				startTime = std::chrono::high_resolution_clock::now();
			}
		}

		camera.setFov(currentFov);

		viewer.draw();
		viewer.display();
	}
	return EXIT_SUCCESS;
}