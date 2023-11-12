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
	
	/*MATERIALS*/

	MaterialPtr simpleMaterial = std::make_shared<Material>(glm::vec3(0.8), glm::vec3(0.6), glm::vec3(0.3), 10);

	/*OBJECTS*/

	auto penguin = createTexturedLightedObj(texShader, "penguin_main.obj", "penguin.png", simpleMaterial);
	penguin -> setGlobalTransform(getTranslationMatrix(0,0,0) * getScaleMatrix(0.1f, 0.1f, 0.1f));

	auto right_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	right_arm_penguin -> setGlobalTransform(getTranslationMatrix(-0.1,0.2,0));
	// right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(M_PI/4, glm::vec3(1,0,0)), 0);
	// right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(-M_PI/4, glm::vec3(1,0,0)), 1);
	// right_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(M_PI/4, glm::vec3(1,0,0)), 2);

	auto left_arm_penguin = createTexturedLightedObj(texShader, "bras.obj", "penguin.png", simpleMaterial);
	left_arm_penguin -> setGlobalTransform(getTranslationMatrix(0.1,0.2,0) * getRotationMatrix(M_PI, glm::vec3(0,1,0)));
	
	left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(M_PI, glm::vec3(1,0,0)), 0);
	left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(0, glm::vec3(1,0,0)), 1);
	left_arm_penguin->addLocalTransformKeyframe(getRotationMatrix(-M_PI, glm::vec3(1,0,0)), 2);
	// left_arm_penguin->addLocalTransformKeyframe(lookAtModel(glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0,0,0)), 0);
	// left_arm_penguin->addLocalTransformKeyframe(lookAt(glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,0,0)), 1);
	// left_arm_penguin->addLocalTransformKeyframe(lookAtModel(glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0,0,0)), 2);

	HierarchicalRenderable::addChild(penguin, right_arm_penguin);
	HierarchicalRenderable::addChild(penguin, left_arm_penguin);

	/*ADD RENDERABLES*/
	viewer.addRenderable(penguin);

	/*START ANIMATION*/
	viewer.startAnimation();

	/*CAMERA*/
	Camera& camera = viewer.getCamera();
	glm::vec3 forward = glm::vec3(0, 0, -1); // In OpenGL, the camera's forward axis is -z

	setCameraPosition(viewer, glm::mat4({0.728592, 0.257469, -0.634715, -0, 1.49012e-08, 0.926662, 0.375896, -0, 0.684948, -0.273875, 0.675159, -0, -0.129517, -0.156329, -0.634549, 1}));
	addCubeMap(viewer, "skybox");
	viewer.setKeyboardSpeed(2);
	while( viewer.isRunning()) {
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}	
	return EXIT_SUCCESS;
}