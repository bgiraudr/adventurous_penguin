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

int main() {
	Viewer viewer(1280,720, glm::vec4(0.8, 0.8, 0.8, 1.0));
	
	/*SHADERS*/
	ShaderProgramPtr flatShader = addShader(viewer, "flat");
	ShaderProgramPtr texShader = addShader(viewer, "texture");
	ShaderProgramPtr wavesShader = addShader(viewer, "waves");
	ShaderProgramPtr defaultShader = addShader(viewer, "default");

	//Add a 3D frame to the viewer
	FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
	viewer.addRenderable(frame);

	/*LIGHTS*/

	glm::mat4 globalTransformation, localTransformation;

	//Define a directional light for the whole scene
	glm::vec3 d_direction = glm::normalize(glm::vec3(1.0,-1.0,0.0));
	glm::vec3 d_ambient(0.4,0.4,0.4), d_diffuse(0.4,0.4,0.4), d_specular(0.5,0.5,0.5);
	
	glm::vec3 lightPosition(-12,15,-10);
	DirectionalLightPtr directionalLight = std::make_shared<DirectionalLight>(d_direction, d_ambient, d_diffuse, d_specular);
	directionalLight->setGlobalTransform(getTranslationMatrix(lightPosition) * directionalLight->getGlobalTransform());
	
	//Add a renderable to display the light and control it via mouse/key event
	DirectionalLightRenderablePtr directionalLightRenderable = std::make_shared<DirectionalLightRenderable>(flatShader, directionalLight);
	localTransformation = getScaleMatrix(0.5);
	directionalLightRenderable->setLocalTransform(localTransformation);
	viewer.addDirectionalLight(directionalLight);

	//Define a spot light
	glm::vec3 s_position(9,2.0,-2.0), s_spotDirection = glm::vec3(5,0,-1);
	//glm::vec3 s_ambient(0.0,0.0,0.0), s_diffuse(0.0,0.0,0.0), s_specular(0.0,0.0,0.0);
	glm::vec3 s_ambient(0.0,0.0,0.05), s_diffuse(0.7,0.7,0.7), s_specular(1.0,1.0,1.0);
	float s_constant=1.0, s_linear=0.0, s_quadratic=0.0;
	float s_innerCutOff=std::cos(glm::radians(10.0f)), s_outerCutOff=std::cos(glm::radians(20.0f));
	SpotLightPtr spotLight = std::make_shared<SpotLight>(s_position, s_spotDirection,
														 s_ambient, s_diffuse, s_specular,
														 s_constant, s_linear, s_quadratic,
														 s_innerCutOff, s_outerCutOff);
	SpotLightRenderablePtr spotLightRenderable = std::make_shared<SpotLightRenderable>(flatShader, spotLight);
	localTransformation = glm::scale(glm::mat4(1.0), glm::vec3(0.5,0.5,0.5));

	spotLightRenderable->setLocalTransform(localTransformation);
	viewer.addSpotLight(spotLight);

	SpotLightPtr spotLight2 = std::make_shared<SpotLight>(glm::vec3(1.5,1.9,-9), glm::vec3(4,0.6,-8),
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

	auto boat = createTexturedLightedObj(texShader, "boat.obj", "boat.png", simpleMaterial);
	boat -> addGlobalTransformKeyframe(getTranslationMatrix(5,-1,10) * getScaleMatrix(0.15f, 0.15f, 0.15f) * getRotationMatrix(M_PI, glm::vec3(0,1,0)),0);
	boat -> addGlobalTransformKeyframe(getTranslationMatrix(5,-1,10) * getScaleMatrix(0.15f, 0.15f, 0.15f) * getRotationMatrix(M_PI, glm::vec3(0,1,0)),1);

	// global key frame has an impact of all children of the object
	for (int i = 0; i < 5; i++) {
		float nb = 3;
		boat -> addGlobalTransformKeyframe(getTranslationMatrix(5,-1,-(20*i)) * getScaleMatrix(0.15f, 0.15f, 0.15f) * getRotationMatrix(M_PI, glm::vec3(0,1,0)) * getRotationMatrix(M_PI/16, glm::vec3(0,0,1)),2*nb*i+nb);
		boat -> addGlobalTransformKeyframe(getTranslationMatrix(5,-1,-(20*i)-10) * getScaleMatrix(0.15f, 0.15f, 0.15f) * getRotationMatrix(M_PI, glm::vec3(0,1,0)) * getRotationMatrix(-M_PI/16, glm::vec3(0,0,1)),2*nb*i+2*nb);
	}

	auto penguin = createTexturedLightedObj(texShader, "penguin_main.obj", "penguin.png", simpleMaterial);
	penguin -> setGlobalTransform(getTranslationMatrix(0,10,-6));

	// arms are hierarchical of the pengouin itself
	auto right_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	right_arm_penguin -> setGlobalTransform(getTranslationMatrix(0.8,2,0));

	auto left_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	left_arm_penguin -> setGlobalTransform(getTranslationMatrix(-0.8,2,0) * getRotationMatrix(M_PI, glm::vec3(0,1,0)));

	// use a simple loop to simulate a movement of the arms
	for (int i = 0; i < 50; i++) {
		float nb = 0.2;
		right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(M_PI/4, glm::vec3(1,0,0)), 2*nb*i+nb);
		right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(-M_PI/4, glm::vec3(1,0,0)), 2*nb*i+nb*2);

		left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(M_PI/4, glm::vec3(1,0,0)), 2*nb*i+nb);
		left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(-M_PI/4, glm::vec3(1,0,0)), 2*nb*i+nb*2);
	}
	
	// icebergs are just a mountain underneath the sea plane. With an ice texture
	auto icebergs = createTexturedLightedObj(texShader, "hills.obj", "iceberg.png", iceMaterial);
	icebergs->setGlobalTransform(getTranslationMatrix(0,-3.5,0) * getScaleMatrix(20));
	icebergs->setWrapOption(2);

	auto hills = createTexturedLightedObj(texShader, "hills.obj", "hills.png", myMaterial);
	hills->setGlobalTransform(getTranslationMatrix(-4,-8,-40) * getScaleMatrix(40));

	// custom wave shader
	auto waterPlane = std::make_shared<TexturedPlaneRenderable>(wavesShader, TEXTURE_PATH + "ocean/0.png");
	waterPlane->setGlobalTransform(getRotationMatrix(M_PI/2, glm::vec3(1,0,0)) * getScaleMatrix(300));
	waterPlane->setWrapOption(2);

	auto iceberg = createTexturedLightedObj(texShader, "ice_pic.obj", "iceberg.png", iceMaterial);
	iceberg->setGlobalTransform(getTranslationMatrix(5,-20,-55) * getScaleMatrix(15) * getRotationMatrix(M_PI, glm::vec3(1,0,0)));
	iceberg -> addGlobalTransformKeyframe(getTranslationMatrix(5,0,-55) * getScaleMatrix(15) * getRotationMatrix(M_PI, glm::vec3(1,0,0)),16.5);
	iceberg -> addGlobalTransformKeyframe(getTranslationMatrix(5,0,-55) * getScaleMatrix(15) * getRotationMatrix(M_PI, glm::vec3(1,0,0)),25);

	auto iceberg1 = createTexturedLightedObj(texShader, "ice_pic.obj", "iceberg.png", iceMaterial);
	iceberg1->setGlobalTransform(getTranslationMatrix(14,0,-22) * getScaleMatrix(6) * getRotationMatrix(M_PI, glm::vec3(1,0,0)));

	auto snow = createTexturedLightedObj(wavesShader, "hills.obj", "snow.jpg", snowMaterial);
	snow->setGlobalTransform(getTranslationMatrix(51,0,-31) * getScaleMatrix(30,6,30) * getRotationMatrix(M_PI/18, glm::vec3(0,0,1)));
	snow->setWrapOption(2);

	// use of hierarchicalRenderable to move every objects at once
	HierarchicalRenderable::addChild(boat, penguin);

	HierarchicalRenderable::addChild(penguin, right_arm_penguin);
	HierarchicalRenderable::addChild(penguin, left_arm_penguin);

	/*ADD RENDERABLES*/
	viewer.addRenderable(boat);
	viewer.addRenderable(penguin);
	viewer.addRenderable(waterPlane);
	viewer.addRenderable(icebergs);
	viewer.addRenderable(iceberg);
	viewer.addRenderable(hills);
	viewer.addRenderable(iceberg1);
	viewer.addRenderable(snow);

	/*START ANIMATION*/
	viewer.startAnimation();

	/*CAMERA*/
	Camera& camera = viewer.getCamera();
	glm::vec3 forward = glm::vec3(0, 0, -1); // In OpenGL, the camera's forward axis is -z
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(6.9,1,1.7), glm::vec3(5,1,0), forward), 0);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(6.9,1,1.7), glm::vec3(5,1,0), forward), 2.5);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(8,1,-3), glm::vec3(5,1,-5), forward), 5);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.9,1,-8), glm::vec3(4.8,1,-6), forward), 5.01);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(3.9,1.2,-10), glm::vec3(4.8,1.2,-9), forward), 6.5);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(5,1.2,-11), glm::vec3(5,1.2,-30), forward) * getRotationMatrix(-M_PI/16, glm::vec3(0,0,1)), 6.5001);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(5,1.2,-19.2), glm::vec3(5,1.2,-50), forward) * getRotationMatrix(M_PI/16, glm::vec3(0,0,1)), 9);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(5,1.2,-27.3), glm::vec3(5,1.2,-50), forward) * getRotationMatrix(-M_PI/16, glm::vec3(0,0,1)) * getRotationMatrix(-M_PI/2.2, glm::vec3(0,1,0)), 11.3);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(5,1.2,-35.5), glm::vec3(5,1.2,-50), forward) * getRotationMatrix(M_PI/16, glm::vec3(0,0,1)) * getRotationMatrix(M_PI/2.2, glm::vec3(0,1,0)), 13.8);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(5,1.2,-39.6), glm::vec3(5,1.2,-50), forward) * getRotationMatrix(M_PI/16, glm::vec3(0,0,1)) * getRotationMatrix(0, glm::vec3(0,1,0)), 15);
	camera.addGlobalTransformKeyframe(lookAtModel(glm::vec3(5,1.2,-52), glm::vec3(5,1.2,-80), forward) * getRotationMatrix(M_PI/16, glm::vec3(0,0,1)) * getRotationMatrix(0, glm::vec3(0,1,0)), 19);


	setCameraPosition(viewer, glm::mat4({0.786206, -0.0631481, 0.61473, -0, -3.72529e-09, 0.994765, 0.102187, 0, -0.617965, -0.0803402, 0.78209, -0, -4.3985, -0.190594, -5.6621, 1}));
	addCubeMap(viewer, "skybox");
	viewer.setKeyboardSpeed(15);
	viewer.setSimulationTime(0);

	while( viewer.isRunning()) {
		viewer.handleEvent();
		viewer.animate();

		// render the gif located in the ocean folder. Displaying the 20 images in one second
		int imageNumber = static_cast<int>(viewer.getTime()/2 * 20) % 20;
		waterPlane->setImage(TEXTURE_PATH + "ocean/"+std::to_string(imageNumber)+".png");

		viewer.draw();
		viewer.display();
	}	
	return EXIT_SUCCESS;
}