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
#include <dynamics/DynamicSystemRenderable.hpp>
#include <dynamics/DampingForceField.hpp>
#include <dynamics/ConstantForceField.hpp>
#include <dynamics/SpringForceField.hpp>
#include <dynamics/EulerExplicitSolver.hpp>

#include <dynamics/ParticleRenderable.hpp>
#include <dynamics/ParticleListRenderable.hpp>
#include <dynamics/ConstantForceFieldRenderable.hpp>
#include <dynamics/SpringForceFieldRenderable.hpp>
#include <dynamics/SpringListRenderable.hpp>
#include <dynamics/ControlledForceFieldRenderable.hpp>
#include <texturing/FlagRenderable.hpp>

const std::string MESHES_PATH = "../../sfmlGraphicsPipeline/meshes/";
const std::string TEXTURE_PATH = "../../sfmlGraphicsPipeline/textures/";

void createFlag(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable);

ShaderProgramPtr addShader(Viewer& viewer, std::string vertex, std::string fragment) {
	std::string shaderPath = "../../sfmlGraphicsPipeline/shaders/";
	ShaderProgramPtr shader = std::make_shared<ShaderProgram>(shaderPath+vertex+".glsl", shaderPath+fragment+".glsl");
	viewer.addShaderProgram(shader);
	return shader;
}

ShaderProgramPtr addShader(Viewer& viewer, std::string shad) {
	return addShader(viewer, shad+"Vertex", shad+"Fragment");
}

void setCameraPosition(Viewer& viewer, glm::vec3 initPos,  glm::vec3 lookAt) {
	viewer.getCamera().setViewMatrix(glm::lookAt(initPos, lookAt, glm::vec3(0, 1, 0)));
}

void setCameraPosition(Viewer& viewer, glm::mat4 viewMatrix) {
	viewer.getCamera().setViewMatrix(viewMatrix);
}

void addCubeMap(Viewer& viewer, std::string texture) {
	ShaderProgramPtr cubeMapShader = addShader(viewer, "cubeMap");
	std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/"+texture;
	auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
	viewer.addRenderable(cubemap);
}

std::shared_ptr<TexturedLightedMeshRenderable> createTexturedLightedObj(ShaderProgramPtr shader, std::string obj, std::string texture, MaterialPtr material) {
	return std::make_shared<TexturedLightedMeshRenderable>(shader, MESHES_PATH + obj, material, TEXTURE_PATH + texture);
}

int main() {
	Viewer viewer(1280,720, glm::vec4(0.8, 0.8, 0.8, 1.0));
	
	/*SHADERS*/
	ShaderProgramPtr flatShader = addShader(viewer, "flat");
	ShaderProgramPtr texShader = addShader(viewer, "texture");
	ShaderProgramPtr wavesShader = addShader(viewer, "waves");
	ShaderProgramPtr nonRigidShader = addShader(viewer, "nonRigid");

	//Add a 3D frame to the viewer
	//Add a 3D frame to the viewer
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	DynamicSystemPtr system = std::make_shared<DynamicSystem>();
    EulerExplicitSolverPtr solver = std::make_shared<EulerExplicitSolver>();
    system->setSolver(solver);
    system->setDt(0.01);

	DynamicSystemRenderablePtr systemRenderable = std::make_shared<DynamicSystemRenderable>(system);
    viewer.addRenderable(systemRenderable);

	/*LIGHTS*/

	glm::mat4 globalTransformation, localTransformation;

	//Define a directional light for the whole scene
	glm::vec3 d_direction = glm::normalize(glm::vec3(1.0,-1.0,0.0));
	glm::vec3 d_ambient(0.5,0.5,0.6), d_diffuse(0.4,0.4,0.4), d_specular(0.5,0.5,0.9);
	
	glm::vec3 lightPosition(-12,15,-10);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
	
	//Add a renderable to display the light and control it via mouse/key event
	DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
	localTransformation = getScaleMatrix(0.5);
	directionalLightRenderable->setLocalTransform(localTransformation);
	viewer.addDirectionalLight(directionalLight);

	//Define a spot light
	glm::vec3 s_position(9, 11.4, -15.5), s_spotDirection = glm::vec3(10,5,-7);
	//glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
	glm::vec3 s_ambient(0.0,0.0,0.03), s_diffuse(0.7,0.7,0.8), s_specular(1.0,1.0,1.0);
	float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
	float s_innerCutOff=std::cos(glm::radians(10.0f)), s_outerCutOff=std::cos(glm::radians(20.0f));
	SpotLightPtr spotLight = std::make_shared<SpotLight>(s_position, s_spotDirection,
														 s_ambient, s_diffuse, s_specular,
														 s_constant, s_linear, s_quadratic,
														 s_innerCutOff, s_outerCutOff);
	SpotLightRenderablePtr spotLightRenderable = std::make_shared<SpotLightRenderable>(flatShader, spotLight);
	localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));
	// spotLightRenderable->setLocalTransform(localTransformation);

	viewer.addSpotLight(spotLight);
	// viewer.addRenderable(spotLightRenderable);

	SpotLightPtr spotLight2 = std::make_shared<SpotLight>(glm::vec3(27,9.6,-7.5), glm::vec3(19.4,5,-2.8),
														 s_ambient, s_diffuse, s_specular,
														 s_constant, s_linear, s_quadratic,
														 s_innerCutOff, s_outerCutOff);
	SpotLightRenderablePtr spotLightRenderable2 = std::make_shared<SpotLightRenderable>(flatShader, spotLight2);
	spotLightRenderable->setLocalTransform(localTransformation);
	viewer.addSpotLight(spotLight2);
	// viewer.addRenderable(spotLightRenderable2);

	/*MATERIALS*/

	glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
	float mShininess=1;
	MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);

	MaterialPtr iceMaterial = std::make_shared<Material>(glm::vec3(0.7,0.8,1.0), glm::vec3(0.7,0.8,1.0), glm::vec3(10), 50);
	MaterialPtr simpleMaterial = std::make_shared<Material>(glm::vec3(0.8), glm::vec3(0.6), glm::vec3(0.3), 10);
	MaterialPtr snowMaterial = std::make_shared<Material>(glm::vec3(0.9), glm::vec3(0.6), glm::vec3(0.9), 50);

	/*OBJECTS*/

	auto penguin = createTexturedLightedObj(nonRigidShader, "penguin_main.obj", "penguin.png", simpleMaterial);
	penguin -> setGlobalTransform(getTranslationMatrix(30,1.5,-51) * getScaleMatrix(0.15f, 0.15f, 0.15f));
	penguin -> addGlobalTransformKeyframe(getTranslationMatrix(29,0,-47) * getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getRotationMatrix(-M_PI/2, glm::vec3(0,0,1)) * getScaleMatrix(0.15f, 0.15f, 0.15f), 0);
	penguin -> addGlobalTransformKeyframe(getTranslationMatrix(31,0,-47) * getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getRotationMatrix(-M_PI/2, glm::vec3(0,0,1)) * getScaleMatrix(0.15f, 0.15f, 0.15f), 3);
	penguin -> addGlobalTransformKeyframe(getTranslationMatrix(31,0,-47) * getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getRotationMatrix(-M_PI/2, glm::vec3(0,0,1)) * getScaleMatrix(0.15f, 0.15f, 0.15f), 10);

	auto right_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	right_arm_penguin -> setGlobalTransform(getTranslationMatrix(0.8,2,0));

	auto left_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	left_arm_penguin -> setGlobalTransform(getTranslationMatrix(-0.8,2,0) * getRotationMatrix(M_PI, glm::vec3(0,1,0)));

	auto snow = createTexturedLightedObj(texShader, "hills.obj", "snow.jpg", snowMaterial);
	snow -> setGlobalTransform(getTranslationMatrix(0,-0.9,0) * getScaleMatrix(60,9,60));
	snow->setWrapOption(2);

	auto snowHills = createTexturedLightedObj(texShader, "hills.obj", "snow.jpg", snowMaterial);
	snowHills -> setGlobalTransform(getTranslationMatrix(50,-3,5) * getScaleMatrix(30) * getRotationMatrix(degToRad(-10), glm::vec3(1,0,0)));
	snowHills->setWrapOption(2);

	auto snowPlatform = createTexturedLightedObj(texShader, "ice_plateform.obj", "snow.jpg", snowMaterial);
	snowPlatform -> setGlobalTransform(getTranslationMatrix(47,1,-45) * getScaleMatrix(0.5) * getRotationMatrix(degToRad(-90), glm::vec3(1,0,0)));
	snowPlatform->setWrapOption(2);

	auto waterPlane = std::make_shared<TexturedPlaneRenderable>(texShader, TEXTURE_PATH + "ocean/0.png");
	waterPlane->setGlobalTransform(getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getScaleMatrix(300));
	waterPlane->setWrapOption(2);

	auto iceberg = createTexturedLightedObj(texShader, "ice_pic.obj", "iceberg.png", iceMaterial);
	iceberg->setGlobalTransform(getTranslationMatrix(14,2,-22) * getScaleMatrix(7) * getRotationMatrix(M_PI, glm::vec3(1,0,0)));

	// HierarchicalRenderable::addChild(boat, penguin);

	HierarchicalRenderable::addChild(penguin, right_arm_penguin);
	HierarchicalRenderable::addChild(penguin, left_arm_penguin);

	/*ADD RENDERABLES*/
	viewer.addRenderable(penguin);
	viewer.addRenderable(waterPlane);
	viewer.addRenderable(snowPlatform);
	viewer.addRenderable(snow);
	viewer.addRenderable(snowHills);
	viewer.addRenderable(iceberg);

	/*START ANIMATION*/
	viewer.startAnimation();

	/*CAMERA*/
	Camera& camera = viewer.getCamera();
	glm::vec3 forward = glm::vec3(0, 0, -1); // In OpenGL, the camera's forward axis is -z
	
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(30,1,-49), glm::vec3(32,1,-43), forward), 0);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(30,1,-48), glm::vec3(32,1,-43), forward), 5);

	// setCameraPosition(viewer, glm::mat4({0.254616, 0.0870552, 0.963116, -0, -1.86265e-09, 0.99594, -0.0900221, 0, -0.967042, 0.0229211, 0.253583, -0, -5.38636, -8.46124, -28.2959, 1}));
	addCubeMap(viewer, "skybox");
	viewer.setKeyboardSpeed(8);
	viewer.setSimulationTime(0);

	while( viewer.isRunning()) {
		viewer.handleEvent();
		viewer.animate();

		int imageNumber = static_cast<int>(viewer.getTime()/2 * 20) % 20;
		waterPlane->setImage(TEXTURE_PATH + "ocean/"+std::to_string(imageNumber)+".png");

		viewer.draw();
		viewer.display();
	}	
	return EXIT_SUCCESS;
}