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

void addCubeMap(Viewer& viewer, std::string texture) {
	ShaderProgramPtr cubeMapShader = addShader(viewer, "cubeMap");
	std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/"+texture;
	auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);
	viewer.addRenderable(cubemap);
}

std::shared_ptr<TexturedLightedMeshRenderable> createTexturedLightedObj(ShaderProgramPtr shader, std::string obj, std::string texture, MaterialPtr material) {
	return std::make_shared<TexturedLightedMeshRenderable>(shader, MESHES_PATH + obj, material, TEXTURE_PATH + texture);
}

void initialize_scene(Viewer& viewer) {
	/*SHADERS*/
	ShaderProgramPtr flatShader = addShader(viewer, "flat");
	ShaderProgramPtr texShader = addShader(viewer, "texture");
	ShaderProgramPtr defaultShader = addShader(viewer, "default");

	//Add a 3D frame to the viewer
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	Camera& camera = viewer.getCamera();
	float t = 3;
	glm::vec3 forward = glm::vec3(0, 0, -1); // In OpenGL, the camera's forward axis is -z
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-t,t,-t), glm::vec3(0,0,0), forward), 0 );
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-t,t,t), glm::vec3(0,5,0), forward), 2 );
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(t,t,t), glm::vec3(0,0,6), forward), 4);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(t,t,-t), glm::vec3(-3,0,0), forward), 6);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-t,t,-t), glm::vec3(0,0,0), forward), 8);

	/*LIGHTS*/

	glm::mat4 globalTransformation, localTransformation;

	//Define a directional light for the whole scene
	glm::vec3 d_direction = glm::normalize(glm::vec3(1.0,-1.0,0.0));
	glm::vec3 d_ambient(0.3,0.3,0.3), d_diffuse(0.3,0.3,0.3), d_specular(0.3,0.3,0.3);
	//glm::vec3 d_ambient(0.0,0.0,0.0), d_diffuse(0.0,0.0,0.0), d_specular(0.0,0.0,0.0);
	glm::vec3 lightPosition(-5.0,5.0,0.0);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
	
	//Add a renderable to display the light and control it via mouse/key event
	DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
	localTransformation = getScaleMatrix(0.5);
	directionalLightRenderable->setLocalTransform(localTransformation);
	viewer.addDirectionalLight(directionalLight);
	viewer.addRenderable(directionalLightRenderable);

	//Define a point light
	glm::vec3 p_position(0.0,0.0,0.0), p_ambient(0.0,0.0,0.0), p_diffuse(0.0,0.0,0.0), p_specular(0.0,0.0,0.0);
	float p_constant=0.0, p_linear=0.0, p_quadratic=0.0;

	p_position = glm::vec3(-8, 15.0, 5.0);
	p_ambient = glm::vec3(1.0,1.0,1.0);
	p_diffuse = glm::vec3(1.0,1.0,1.0);
	p_specular = glm::vec3(1.0,1.0,1.0);
	p_constant=1.0;
	p_linear=5e-1;
	p_quadratic=0;
	PointLightPtr pointLight1 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse, p_specular, p_constant, p_linear, p_quadratic);
	PointLightRenderablePtr pointLightRenderable1 = std::make_shared<PointLightRenderable>(flatShader, pointLight1);
	localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));
	pointLightRenderable1->setLocalTransform(localTransformation);
	viewer.addPointLight(pointLight1);
	viewer.addRenderable(pointLightRenderable1);

	p_position = glm::vec3(8, 5.0, 5.0);
	p_ambient = glm::vec3(0.0,0.0,0.0);
	p_diffuse = glm::vec3(0.0,0.0,1.0);
	p_specular = glm::vec3(0.0,0.0,1.0);
	p_constant=1.0;
	p_linear=5e-1;
	p_quadratic=0;
	PointLightPtr pointLight2 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse, p_specular, p_constant, p_linear, p_quadratic);
	PointLightRenderablePtr pointLightRenderable2 = std::make_shared<PointLightRenderable>(flatShader, pointLight2);
	localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));
	pointLightRenderable2->setLocalTransform(localTransformation);
	viewer.addPointLight(pointLight2);
	viewer.addRenderable(pointLightRenderable2);
	
	//Define a spot light
	glm::vec3 s_position(0.0,5.0,-8.0), s_spotDirection = glm::normalize(glm::vec3(0.0,-1.0,1.0));
	//glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
	glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.7,0.7,0.7), s_specular(1.0,1.0,1.0);
	float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
	float s_innerCutOff=std::cos(glm::radians(20.0f)), s_outerCutOff=std::cos(glm::radians(40.0f));
	SpotLightPtr spotLight = std::make_shared<SpotLight>(s_position, s_spotDirection,
														 s_ambient, s_diffuse, s_specular,
														 s_constant, s_linear, s_quadratic,
														 s_innerCutOff, s_outerCutOff);
	SpotLightRenderablePtr spotLightRenderable = std::make_shared<SpotLightRenderable>(flatShader, spotLight);
	localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));

	spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3,5,3), glm::vec3(0), Light::base_forward), 0);
	spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-3,5,3), glm::vec3(0), Light::base_forward), 3);
	spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(-3,5,-3), glm::vec3(0), Light::base_forward), 6);
	spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3,5,-3), glm::vec3(0), Light::base_forward), 9);
	spotLight->addGlobalTransformKeyframe(lookAtModel(glm::vec3(3,5,3), glm::vec3(0), Light::base_forward), 12);

	spotLightRenderable->setLocalTransform(localTransformation);
	viewer.addSpotLight(spotLight);
	viewer.addRenderable(spotLightRenderable);

	/*MATERIALS*/

	glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
	float mShininess=1;
	MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);

	MaterialPtr iceMaterial = std::make_shared<Material>(glm::vec3(0.7,0.8,1.0), glm::vec3(0.7,0.8,1.0), glm::vec3(10), 50);
	MaterialPtr simpleMaterial = std::make_shared<Material>(glm::vec3(0.8), glm::vec3(0.6), glm::vec3(0.3), 10);

	/*OBJECTS*/

	auto boat = createTexturedLightedObj(texShader, "boat6.obj", "boat2.png", simpleMaterial);
	boat -> setGlobalTransform(getTranslationMatrix(0,-1,5) * getScaleMatrix(0.15f, 0.15f, 0.15f) * getRotationMatrix(M_PI, glm::vec3(0,1,0)));
	boat->addLocalTransformKeyframe(getTranslationMatrix(0,-1,5), 0);
	boat->addLocalTransformKeyframe(getTranslationMatrix(0,-1,100) * getRotationMatrix(M_PI/8, glm::vec3(0,0,1)), 3);
	boat->addLocalTransformKeyframe(getTranslationMatrix(0,-1,5), 6);

	auto pengouin = createTexturedLightedObj(texShader, "pengouin.obj", "pengouin.png", simpleMaterial);
	pengouin -> setGlobalTransform(getTranslationMatrix(0,2,4) * getScaleMatrix(0.1f, 0.1f, 0.1f));

	auto seal = createTexturedLightedObj(texShader, "seal.obj", "seal.png", myMaterial);
	seal -> setGlobalTransform(getTranslationMatrix(0.5,0.6,-1) * getScaleMatrix(0.05f));

	auto waterPlane = std::make_shared<TexturedPlaneRenderable>(texShader, TEXTURE_PATH + "ocean.gif");
	waterPlane->setGlobalTransform(getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getScaleMatrix(200));
	waterPlane->setWrapOption(2);

	auto icebergs = createTexturedLightedObj(texShader, "hills.obj", "iceberg.png", iceMaterial);
	icebergs->setGlobalTransform(getTranslationMatrix(0,-3.5,0) * getScaleMatrix(20));
	icebergs->setWrapOption(2);

	auto hills = createTexturedLightedObj(texShader, "hills.obj", "hills.png", myMaterial);
	hills->setGlobalTransform(getTranslationMatrix(0,-6,-40) * getScaleMatrix(40));

	

	/*ADD RENDERABLES*/
	viewer.addRenderable(boat);
	viewer.addRenderable(pengouin);
	viewer.addRenderable(seal);
	viewer.addRenderable(waterPlane);
	viewer.addRenderable(icebergs);
	viewer.addRenderable(hills);

	/*START ANIMATION*/
	viewer.startAnimation();
}

int main() {
	Viewer viewer(1280,720, glm::vec4(0.8, 0.8, 0.8, 1.0));
	initialize_scene(viewer);
	setCameraPosition(viewer, glm::vec3(1,1,1), glm::vec3(0,0,0));
	addCubeMap(viewer, "skybox2");

	while( viewer.isRunning()) {
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}	
	return EXIT_SUCCESS;
}