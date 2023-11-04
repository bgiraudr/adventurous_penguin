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

void initialize_scene( Viewer& viewer )
{
    //Position the camera
    
    //Default shader
    ShaderProgramPtr flatShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/flatVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/flatFragment.glsl");
    viewer.addShaderProgram( flatShader );

    //Add a 3D frame to the viewer
    FrameRenderablePtr frame = std::make_shared<FrameRenderable>(flatShader);
    viewer.addRenderable(frame);

    //Textured shader
    //    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>("../shaders/textureVertex.glsl","../shaders/textureFragment.glsl");
    ShaderProgramPtr texShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/textureVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/textureFragment.glsl");
    viewer.addShaderProgram( texShader );

    ShaderProgramPtr defaultShader = std::make_shared<ShaderProgram>(   "../../sfmlGraphicsPipeline/shaders/defaultVertex.glsl",
                                                                    "../../sfmlGraphicsPipeline/shaders/defaultFragment.glsl");
    viewer.addShaderProgram( defaultShader );

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
	/*
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
    viewer.addRenderable(spotLightRenderable);*/

    { // Exercice 1 : Textured bunny
		glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
        float mShininess=1;
        MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);

        // viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(0, 0, 2 ), glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) ) );
        std::string bunny_mesh_path = "./../../sfmlGraphicsPipeline/meshes/iceberg.obj";
        std::string bunny_texture_path = "./../../sfmlGraphicsPipeline/textures/iceberg.png";
        auto bunny = std::make_shared<TexturedLightedMeshRenderable>(texShader, bunny_mesh_path, myMaterial, bunny_texture_path);
		bunny -> setGlobalTransform(getScaleMatrix(0.1f, 0.1f, 0.1f));
        viewer.addRenderable(bunny);
    }

		glm::vec3 mAmbient(1.0), mDiffuse(1.0), mSpecular(1.0);
        float mShininess=1;
        MaterialPtr myMaterial = std::make_shared<Material>(mAmbient, mDiffuse, mSpecular, mShininess);

        std::string bunny_mesh_path = "./../../sfmlGraphicsPipeline/meshes/boat6.obj";
        std::string boat_texture_path = "./../../sfmlGraphicsPipeline/textures/boat2.png";
        auto boat = std::make_shared<TexturedLightedMeshRenderable>(texShader, bunny_mesh_path, myMaterial, boat_texture_path);
		boat -> setGlobalTransform(getTranslationMatrix(0,0,5) * getScaleMatrix(0.15f, 0.15f, 0.15f));

        viewer.addRenderable(boat);

        std::string pengouin_mesh_path = "./../../sfmlGraphicsPipeline/meshes/pengouin.obj";
        std::string pengouin_texture_path = "./../../sfmlGraphicsPipeline/textures/pengouin.png";
        auto pengouin = std::make_shared<TexturedLightedMeshRenderable>(texShader, pengouin_mesh_path, myMaterial, pengouin_texture_path);
		pengouin -> setGlobalTransform(getTranslationMatrix(0,2,4) * getScaleMatrix(0.1f, 0.1f, 0.1f));

        viewer.addRenderable(pengouin);


	viewer.getCamera().setViewMatrix( glm::lookAt( glm::vec3(1, 1, 1 ), glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) ) );
	ShaderProgramPtr cubeMapShader = std::make_shared<ShaderProgram>(  "../../sfmlGraphicsPipeline/shaders/cubeMapVertex.glsl",
																"../../sfmlGraphicsPipeline/shaders/cubeMapFragment.glsl");
	viewer.addShaderProgram(cubeMapShader);
	
	std::string cubemap_dir = "../../sfmlGraphicsPipeline/textures/skybox2";
	auto cubemap = std::make_shared<CubeMapRenderable>(cubeMapShader, cubemap_dir);

	viewer.addRenderable(cubemap);
}

int main() 
{
	Viewer viewer(1280,720, glm::vec4(0.8, 0.8, 0.8, 1.0));
	initialize_scene(viewer);

	while( viewer.isRunning() )
	{
		viewer.handleEvent();
		viewer.animate();
		viewer.draw();
		viewer.display();
	}	

	return EXIT_SUCCESS;
}