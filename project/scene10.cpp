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

std::shared_ptr<FlagRenderable> createFlag(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable);

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
	glm::vec3 d_ambient(0.2,0.2,0.4), d_diffuse(0.4,0.4,0.4), d_specular(0.5,0.5,0.5);
	
	glm::vec3 lightPosition(-12,15,-10);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
	
	//Add a renderable to display the light and control it via mouse/key event
	DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
	localTransformation = getScaleMatrix(0.5);
	directionalLightRenderable->setLocalTransform(localTransformation);
	viewer.addDirectionalLight(directionalLight);

	//Define a spot light
	glm::vec3 s_position(11, 8.7, -3), s_spotDirection = glm::vec3(10,5,-2);
	//glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
	glm::vec3 s_ambient(0.0,0.0,0.03), s_diffuse(0.7,0.7,0.8), s_specular(1.0,1.0,1.0);
	float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
	float s_innerCutOff=std::cos(glm::radians(10.0f)), s_outerCutOff=std::cos(glm::radians(50.0f));
	SpotLightPtr spotLight = std::make_shared<SpotLight>(s_position, s_spotDirection,
														 s_ambient, s_diffuse, s_specular,
														 s_constant, s_linear, s_quadratic,
														 s_innerCutOff, s_outerCutOff);
	SpotLightRenderablePtr spotLightRenderable = std::make_shared<SpotLightRenderable>(flatShader, spotLight);
	localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));
	// spotLightRenderable->setLocalTransform(localTransformation);

	viewer.addSpotLight(spotLight);
	// viewer.addRenderable(spotLightRenderable);

	SpotLightPtr spotLight2 = std::make_shared<SpotLight>(glm::vec3(8.4,10,-17), glm::vec3(8,4.6,-15),
														 s_ambient, s_diffuse, s_specular,
														 s_constant, s_linear, s_quadratic,
														 s_innerCutOff, s_outerCutOff);
	SpotLightRenderablePtr spotLightRenderable2 = std::make_shared<SpotLightRenderable>(flatShader, spotLight2);
	spotLightRenderable->setLocalTransform(localTransformation);
	viewer.addSpotLight(spotLight2);

	/*MATERIALS*/

	glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
	float mShininess=1;
	MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);

	MaterialPtr iceMaterial = std::make_shared<Material>(glm::vec3(0.7,0.8,1.0), glm::vec3(0.7,0.8,1.0), glm::vec3(10), 50);
	MaterialPtr simpleMaterial = std::make_shared<Material>(glm::vec3(0.8), glm::vec3(0.6), glm::vec3(0.3), 10);
	MaterialPtr snowMaterial = std::make_shared<Material>(glm::vec3(0.9), glm::vec3(0.6), glm::vec3(0.9), 50);

	/*OBJECTS*/

	auto penguin = createTexturedLightedObj(texShader, "penguin_main.obj", "penguin.png", simpleMaterial);
	penguin -> setGlobalTransform(getTranslationMatrix(0,0.2,0.2)* getRotationMatrix(degToRad(180), glm::vec3(0,1,0)) * getRotationMatrix(degToRad(10), glm::vec3(1,0,0)) * getScaleMatrix(0.2f, 0.20f, 0.20f));

	auto right_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	right_arm_penguin -> setGlobalTransform(getTranslationMatrix(0.8,2,0));

	auto left_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	left_arm_penguin -> setGlobalTransform(getTranslationMatrix(-0.8,2,0) * getRotationMatrix(M_PI, glm::vec3(0,1,0)));
	

	auto raft = createTexturedLightedObj(texShader, "raft.obj", "raft.png", simpleMaterial);
	raft -> setGlobalTransform(getTranslationMatrix(8.1,4.2,-26.8) * getRotationMatrix(degToRad(0), glm::vec3(0,1,0)));
	raft->setLocalTransform(getScaleMatrix(0.6));
	raft->addGlobalTransformKeyframe(getTranslationMatrix(8.1,4.0,-20.8),0);
	raft->addGlobalTransformKeyframe(getTranslationMatrix(8.1,4.0,-20.8),2);

	raft->addGlobalTransformKeyframe(getTranslationMatrix(7.9,4.0,-32) * getRotationMatrix(degToRad(10), glm::vec3(0,1,0)),15);
	// raft->addGlobalTransformKeyframe(getTranslationMatrix(7.9,4.5,-32),2);

	auto snowHills = createTexturedLightedObj(texShader, "hills.obj", "snow.jpg", snowMaterial);
	snowHills -> setGlobalTransform(getTranslationMatrix(0,0.2,0) * getScaleMatrix(30));
	snowHills->setWrapOption(2);

	auto iceHills = createTexturedLightedObj(texShader, "hills.obj", "iceberg.png", iceMaterial);
	iceHills -> setGlobalTransform(getTranslationMatrix(0,0.2,-60) * getScaleMatrix(30));
	iceHills->setWrapOption(2);

	auto snowPlatform = createTexturedLightedObj(texShader, "ice_plateform.obj", "snow.jpg", snowMaterial);
	snowPlatform -> setGlobalTransform(getTranslationMatrix(0,-2,-6) * getScaleMatrix(2));
	snowPlatform->setWrapOption(2);

	auto mapPlane = std::make_shared<TexturedPlaneRenderable>(texShader, TEXTURE_PATH + "map.jpg");
	mapPlane->setGlobalTransform(getTranslationMatrix(0,1,-0.4) * getScaleMatrix(1, 0.7, 1));
	
	auto waterPlane = std::make_shared<TexturedPlaneRenderable>(texShader, TEXTURE_PATH + "ocean/0.png");
	waterPlane->setGlobalTransform(getTranslationMatrix(0,4,0)*getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getScaleMatrix(300));
	waterPlane->setWrapOption(2);

	std::vector<glm::vec3> icePos = {glm::vec3(-2.9,2,-45), glm::vec3(5.8,1, -60), glm::vec3(11.2, 4, -69), glm::vec3(22, 2.5, -49)};
	for (int i = 0; i < icePos.size(); i++) {
		auto iceberg = createTexturedLightedObj(texShader, "ice_pic.obj", "iceberg.png", iceMaterial);
		iceberg->setGlobalTransform(getTranslationMatrix(icePos[i]) * getScaleMatrix(7) * getRotationMatrix(M_PI, glm::vec3(1,0,0)));
		viewer.addRenderable(iceberg);
	}

	auto flag = createFlag(viewer, system, systemRenderable);
	flag->setGlobalTransform(getTranslationMatrix(-1,0.6,-0.4) * getScaleMatrix(3,1.2,3));

	HierarchicalRenderable::addChild(penguin, right_arm_penguin);
	HierarchicalRenderable::addChild(penguin, left_arm_penguin);

	HierarchicalRenderable::addChild(raft, mapPlane);
	HierarchicalRenderable::addChild(raft, penguin);
	HierarchicalRenderable::addChild(raft, flag);

	/*ADD RENDERABLES*/
	viewer.addRenderable(penguin);
	viewer.addRenderable(waterPlane);
	viewer.addRenderable(snowPlatform);
	viewer.addRenderable(snowHills);
	viewer.addRenderable(mapPlane);
	viewer.addRenderable(raft);
	viewer.addRenderable(iceHills);
	viewer.addRenderable(flag);

    system->setDt(8e-4);

	/*START ANIMATION*/
	viewer.startAnimation();

	/*CAMERA*/
	Camera& camera = viewer.getCamera();
	glm::vec3 forward = glm::vec3(0, 0, -1); // In OpenGL, the camera's forward axis is -z

	setCameraPosition(viewer, glm::mat4({0.794635, -0.277143, 0.540136, -0, 2.98023e-08, 0.889717, 0.456513, -0, -0.607088, -0.362761, 0.707, -0, -16.983, -7.4767, 1.52467, 1}));
	
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(8.2,5,-17), glm::vec3(7.6,5,-26), forward), 0);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(8.2,5,-17), glm::vec3(7.6,5,-26), forward), 4);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(8.2,5,-17), glm::vec3(8.3,15,-17), forward), 15);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(8.2,5,-17), glm::vec3(8.3,15,-17), forward), 30);

	addCubeMap(viewer, "night");
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

std::shared_ptr<FlagRenderable> createFlag(Viewer& viewer, DynamicSystemPtr& system, DynamicSystemRenderablePtr &systemRenderable)
{
    //Initialize a shader for the following renderables
    ShaderProgramPtr textureShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/simpleTextureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/simpleTextureFragment.glsl");
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    ShaderProgramPtr instancedShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/instancedVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/instancedFragment.glsl");
    viewer.addShaderProgram( flatShader );
    viewer.addShaderProgram( instancedShader );
    viewer.addShaderProgram( textureShader );

    std::string texture_path = "../../sfmlGraphicsPipeline/textures/flag.jpg";
    auto flag = std::make_shared<FlagRenderable>(textureShader,
        0.65f, 1.1f, 15, 10, 10e3, 1, texture_path);

    for (const ParticlePtr & particle : flag->getParticles())
        system->addParticle(particle);
    for (const ForceFieldPtr & force_field : flag->getSprings())
        system->addForceField(force_field);

    //Add it to the system as a force field
    ConstantForceFieldPtr gravityForceField = std::make_shared<ConstantForceField>(system->getParticles(), DynamicSystem::gravity);
    system->addForceField(gravityForceField);
    ConstantForceFieldPtr windForceField = std::make_shared<ConstantForceField>(system->getParticles(), glm::vec3(3.0, 2.0, -20.0));
    system->addForceField(windForceField);

    //Initialize a force field that apply to all the particles of the system to simulate vicosity (air friction)
    float dampingCoefficient = 1.0;
    DampingForceFieldPtr dampingForceField = std::make_shared<DampingForceField>(system->getParticles(), dampingCoefficient);
    system->addForceField(dampingForceField);

	return flag;
}