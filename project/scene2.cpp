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
	ShaderProgramPtr defaultShader = addShader(viewer, "default");

	//Add a 3D frame to the viewer
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	/*LIGHTS*/

	glm::mat4 globalTransformation, localTransformation;

	//Define a directional light for the whole scene
	glm::vec3 d_direction = glm::normalize(glm::vec3(1.0,-1.0,0.0));
	glm::vec3 d_ambient(0.4,0.4,0.4), d_diffuse(0.3,0.3,0.3), d_specular(0.5,0.5,0.5);
	
	glm::vec3 lightPosition(-12,15,-10);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
	
	//Add a renderable to display the light and control it via mouse/key event
	DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
	localTransformation = getScaleMatrix(0.5);
	directionalLightRenderable->setLocalTransform(localTransformation);
	viewer.addDirectionalLight(directionalLight);
	// viewer.addRenderable(directionalLightRenderable);

	//Define a point light
	/*glm::vec3 p_position(0.0,0.0,0.0), p_ambient(0.0,0.0,0.0), p_diffuse(0.0,0.0,0.0), p_specular(0.0,0.0,0.0);
	float p_constant=0.0, p_linear=0.0, p_quadratic=0.0;

	p_position = glm::vec3(-8, 15.0, 5.0);
	p_ambient = glm::vec3(1.0,1.0,1.0);
	p_diffuse = glm::vec3(1.0,1.0,1.0);
	p_specular = glm::vec3(1.0,1.0,1.0);
	p_constant=1;
	p_linear=1.5;
	p_quadratic=0;
	PointLightPtr pointLight1 = std::make_shared<PointLight>(p_position, p_ambient, p_diffuse, p_specular, p_constant, p_linear, p_quadratic);
	PointLightRenderablePtr pointLightRenderable1 = std::make_shared<PointLightRenderable>(flatShader, pointLight1);
	localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));
	pointLightRenderable1->setLocalTransform(localTransformation);
	viewer.addPointLight(pointLight1);
	viewer.addRenderable(pointLightRenderable1);*/

	/*p_position = glm::vec3(8, 5.0, 5.0);
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
	viewer.addRenderable(pointLightRenderable2);*/
	
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
	// viewer.addSpotLight(spotLight);
	// viewer.addRenderable(spotLightRenderable);

	/*MATERIALS*/

	glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
	float mShininess=1;
	MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);

	MaterialPtr iceMaterial = std::make_shared<Material>(glm::vec3(0.7,0.8,1.0), glm::vec3(0.7,0.8,1.0), glm::vec3(10), 50);
	MaterialPtr simpleMaterial = std::make_shared<Material>(glm::vec3(0.8), glm::vec3(0.6), glm::vec3(0.3), 10);

	/*OBJECTS*/

	auto boat = createTexturedLightedObj(texShader, "boat.obj", "boat.png", simpleMaterial);
	boat -> setGlobalTransform(getTranslationMatrix(0,-1,5) * getScaleMatrix(0.15f, 0.15f, 0.15f) * getRotationMatrix(M_PI, glm::vec3(0,1,0)));
	boat->addLocalTransformKeyframe(getTranslationMatrix(0,-1,5), 0);
	boat->addLocalTransformKeyframe(getTranslationMatrix(0,-1,100) * getRotationMatrix(M_PI/8, glm::vec3(0,0,1)), 3);
	boat->addLocalTransformKeyframe(getTranslationMatrix(0,-1,5), 6);

	auto pengouin = createTexturedLightedObj(texShader, "pengouin.obj", "pengouin.png", simpleMaterial);
	pengouin -> setGlobalTransform(getTranslationMatrix(0,2,4) * getScaleMatrix(0.1f, 0.1f, 0.1f));

	auto seal = createTexturedLightedObj(texShader, "seal.obj", "seal.png", simpleMaterial);
	seal -> setGlobalTransform(getTranslationMatrix(0.5,0.6,-1) * getScaleMatrix(0.05f));

	auto icebergs = createTexturedLightedObj(texShader, "hills.obj", "iceberg.png", iceMaterial);
	icebergs->setGlobalTransform(getTranslationMatrix(0,-3.5,0) * getScaleMatrix(20));
	icebergs->setWrapOption(2);

	auto hills = createTexturedLightedObj(texShader, "hills.obj", "hills.png", myMaterial);
	hills->setGlobalTransform(getTranslationMatrix(0,-8,-40) * getScaleMatrix(40));

	auto waterPlane = std::make_shared<TexturedPlaneRenderable>(texShader, TEXTURE_PATH + "ocean/0.png");
	waterPlane->setGlobalTransform(getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getScaleMatrix(200));
	waterPlane->setWrapOption(2);

	/*ADD RENDERABLES*/
	// viewer.addRenderable(boat);
	// viewer.addRenderable(pengouin);
	viewer.addRenderable(seal);
	viewer.addRenderable(waterPlane);
	viewer.addRenderable(icebergs);
	viewer.addRenderable(hills);

	/*START ANIMATION*/
	viewer.startAnimation();

	/*CAMERA*/
	Camera& camera = viewer.getCamera();
	glm::vec3 forward = glm::vec3(0, 0, -1); // In OpenGL, the camera's forward axis is -z
	// camera.addGlobalTransformKeyframe(glm::mat4({-0.292907, -0.0145636, 0.95603, -0, -0, 0.999884, 0.0152316, 0, -0.956141, 0.00446145, -0.292873, -0, -2.70746, -3.32731, -16.0776, 1}), 0 );
	// camera.addGlobalTransformKeyframe(glm::mat4({-0.292907, -0.0145636, 0.95603, -0, -0, 0.999884, 0.0152316, 0, -0.956141, 0.00446145, -0.292873, -0, -2.70746, -3.32731, -16.0776, 1}), 2 );
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-2.1,6,12.9), glm::vec3(-0.2,7,10), forward), 0);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-2.1,4,12.9), glm::vec3(-0.2,1,10), forward), 4);
	// camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-2.1,4,12.9), glm::vec3(-0., 2.5, 16), forward), 0);
	// camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(-7.5,5.3,-19.6), glm::vec3(-5,4.1,-35.5), forward), 3);
	// 	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(14.5,3.5,-7.3), glm::vec3(-5,4.1,-35.5), forward), 6);


	//[   -2.707,   -3.327,  -16.078]
	// glm::mat4({-0.292907, -1.30385e-08, -0.956141,  0, -0.0145636, 0.999884, 0.00446144, 0,   0.95603,  0.0152316, -0.292873, -0,  14.5292,  3.57181, -7.28257, 1})
	// glm::mat4({-0.292907, -0.0145636  , 0.95603  , -0, -0        , 0.999884, 0.0152316 , 0, -0.956141, 0.00446145, -0.292873, -0, -2.70746, -3.32731, -16.0776, 1})
	// setCameraPosition(viewer, glm::vec3(-1.4,2.3,12.3), glm::vec3(11,2,-3));
	setCameraPosition(viewer, glm::mat4({-0.292907, -0.0145636, 0.95603, -0, -0, 0.999884, 0.0152316, 0, -0.956141, 0.00446145, -0.292873, -0, -2.70746, -3.32731, -16.0776, 1}));
	addCubeMap(viewer, "skybox");
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