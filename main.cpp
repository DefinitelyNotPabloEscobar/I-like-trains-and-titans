////////////////////////////////////////////////////////////////////////////////
//
//  Loading meshes from external files
//
//
//
// INTRODUCES:
// MODEL DATA, ASSIMP, mglMesh.hpp
//
////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <cmath>
#include <random>
#include <array>
#include <vector>

#include <iostream>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "mgl/mgl.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>	

#include <irrKlang.h>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <../../Image_H/stb-master/stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <../../Image_H/stb-master/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/cimport.h>


////////////////////////////////////////////////////////////////////////// SOUND

irrklang::ISoundEngine* SoundEngineEnv = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* SoundEngineTrain = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* SoundEngineWalk = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* SoundEngineRun = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* SoundEngineJump = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* SoundEngineBigBoy = irrklang::createIrrKlangDevice();
const float soundVolume = 0.3f;
bool jump_playing = false;

////////////////////////////////////////////////////////////////////////// WINDOW

int win_width = 1600;
int win_heigth = 1200;

////////////////////////////////////////////////////////////////////////// MYAPP

struct Mesh_obj
{
	mgl::Mesh* Mesh = nullptr;
	Mesh_obj* next_pointer = nullptr;
	glm::vec3 color;

} Mesh_obj;


class MyApp : public mgl::App {

public:
	void initCallback(GLFWwindow* win) override;
	void displayCallback(GLFWwindow* win, double elapsed) override;
	void windowSizeCallback(GLFWwindow* win, int width, int height) override;
	void windowCloseCallback(GLFWwindow* win) override;
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
private:
	const GLuint POSITION = 0, COLOR = 1, UBO_BP = 0;
	GLuint noiseTexture, grassTexture, grassBump, dirtTexture, snowTexture;

	glm::vec3 axis_x = { 1.0f, 0.0f, 0.0f };
	glm::vec3 axis_y = { 0.0f, 1.0f, 0.0f };
	glm::vec3 axis_z = { 0.0f, 0.0f, 1.0f };

	const float GRAVITY = 0.08f;
	const float FAR_POINT = 100000.f;
	const int ATTRITION = 200;

	// PERSON
	
	float person_alfa = 224.5f;
	float person_beta = 88.55f;
	float person_position_x = 10.f;
	float person_position_z = 10.f;
	float person_velocity = 0.f;
	float person_side_velocity = 0.f;
	const float add_velocity = 0.1f;
	int person_momentum = 0;
	int person_side_momentum = 0;
	const float person_min_y = 0.25f;
	float person_jump_val = 0.f;
	const float person_jump_decay = 0.08f;
	float person_position_y = person_min_y;
	bool inPersonMode = true;

	// TITAN

	float titan_position_x = 0.f;\
	float titan_position_z= -1500.f;
	float titan_axis_angle = 0.f;
	int change_mesh = 0;
	int current_mesh = 0;
	int next_mesh = 1;
	bool titan_is_follow = false;
	glm::mat4 titanMatrixMovement = glm::translate(glm::vec3(titan_position_x, 0.0f, titan_position_z)) *
		glm::rotate(glm::radians(titan_axis_angle), axis_y);


	// CAMERA1
	glm::quat initial_postion_c1 = { 0.0f, 15.f, 15.f, 15.f };
	float alfa = 0.0f;
	float beta = 0.0f;
	int accelaration_x = 0;
	int accelaration_y = 0;
	float zoom = 2.f;
	bool zooming = true;
	bool projection_camera1 = true;

	// CAMERA2
	float zoom2 = 2.f;
	bool projection_camera2 = true;

	//CAMERA3
	glm::vec3 initial_position_c3 = { 0.f, 1.3f, 0.f };
	float zoom3 = 0.7f;
	float angle3 = 0.0f;
	int accelaration_x3 = 0;
	int accelaration_y3 = 0;

	//CAMERA4
	float zoom4 = 1.f;

	bool OLD_P_CLICKED = false;
	bool OLD_C_CLICKED = false;
	bool OLD_CTR_CLICKED = false;
	bool OLD_R_CLICKED = false;
	bool OLD_T_CLICKED = false;
	bool OLD_V_CLICKED = false;

	int current_camera = 0;
	int CAMERA_NUMBER = 4;

	glm::mat4 c1_ChangingViewMatrix;
	glm::mat4 c2_ChangingViewMatrix;

	double xpos, ypos = 0;
	double old_xpos, old_ypos = 0;
	int button, action, mods;

	mgl::ShaderProgram* Shaders_Train = nullptr;
	mgl::ShaderProgram* Shaders_Ground = nullptr;
	mgl::ShaderProgram* Shaders_Rod_Reiss = nullptr;

	//Camera

	mgl::Camera* Camera = nullptr;
	mgl::Camera* Camera2 = nullptr;
	mgl::Camera* Camera3 = nullptr;
	mgl::Camera* Camera4 = nullptr;
	mgl::Camera* CameraP = nullptr;
	GLint ModelMatrixId;
	GLint ModelMatrixIdGrassField;
	GLint NormalMatrixId;
	GLint ModelMatrixRodTitan;
	GLint NormalMatrixRodTitan;


	//Camera Types

	// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,100)
	glm::mat4 ProjectionMatrix1 =
		glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10000.0f);

	// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(x)
	glm::mat4 ProjectionMatrix2 =
		glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 10000.0f);

	static const int MESH_SIZE = 1;


	//Train Movement
	float train_z_position = 0.0f;
	float train_x_position = 0.0f;
	float train_axis_angle = 0.0f;
	const float train_max_speed = 100.0f;
	int train_velocity = 0;
	glm::mat4 trainMatrixMovement = ChangingModelMatrix;

	struct Mesh_obj* train;
	struct Mesh_obj* grass_field;
	static const int RodReissMeshNumber = 17;
	struct Mesh_obj* rod_reiss_titan[RodReissMeshNumber];
	const aiScene* scene;
	//All secondary scene obj
	struct Mesh_obj* Head;
	struct Mesh_obj* Tail;

	//Movement var
	float parametric_movement = 0.0f;
	const float param_sensitivity = 0.011f;
	const float max_param = 1.0f;
	const float min_param = 0.0f;

	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	const glm::mat4 ChangingModelMatrix = ModelMatrix;

	//Shader behavior
	float Levers[4];

	void createMeshes();
	void createShaderPrograms();
	void createCamera();
	void drawScene();
	void processMouseMovement(GLFWwindow* win);
	void processKeyInput(GLFWwindow* win);
	void draw_meshs();
	void updateMatrices(float ratio);
	void saveScreenshot(const char* filename);
	void processTrainMovement();
	void processTitanMovement();
	void processSound();
	void createTextures();
	double noise(double x, double y, double z);
	double perlin(double x, double y, double z);
	double gradient(int xi, int yi, int zi, double xf, double yf, double zf, int h);
	double cubicInterpolate(double a, double b, double c, double d, double t);
	void processPersonMovement();
};

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {

	std::string mesh_dir = "../assets/";

	std::string train_file_obj = "chocho.obj";
	train = new struct Mesh_obj;
	train->Mesh = new mgl::Mesh();
	//train->Mesh->generateTexcoords();
	train->Mesh->joinIdenticalVertices();
	//train->Mesh->flipUVs();
	train->Mesh->create(mesh_dir + train_file_obj);
	train->color = { 0.9f, 0.9f, 0.9f };

	std::string grass_field_file_obj = "terrain.obj";
	grass_field = new struct Mesh_obj;
	grass_field->Mesh = new mgl::Mesh();
	grass_field->Mesh->joinIdenticalVertices();
	grass_field->Mesh->create(mesh_dir + grass_field_file_obj);
	grass_field->color = { 0.05f, 0.2f, 0.1f };

	std::string rod_dir = "/RodReissMovement/";

	for (int i = 0; i < RodReissMeshNumber; i++) {
		std::string rod_titan_file_obj = "RodReiss_VER" + std::to_string(i + 1) + ".obj";
		rod_reiss_titan[i] = new struct Mesh_obj;
		rod_reiss_titan[i]->Mesh = new mgl::Mesh();
		rod_reiss_titan[i]->Mesh->joinIdenticalVertices();
		rod_reiss_titan[i]->Mesh->create(mesh_dir + rod_dir + rod_titan_file_obj);
	}

}


///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {

	// Train
	Shaders_Train = new mgl::ShaderProgram();
	Shaders_Train->addShader(GL_VERTEX_SHADER, "train-vs.glsl");
	Shaders_Train->addShader(GL_FRAGMENT_SHADER, "train-fs.glsl");

	Shaders_Train->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	if (train->Mesh->hasNormals()) {
		Shaders_Train->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
	}
	if (train->Mesh->hasTexcoords()) {
		Shaders_Train->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
	}
	if (train->Mesh->hasTangentsAndBitangents()) {
		Shaders_Train->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
	}

	Shaders_Train->addUniform(mgl::MODEL_MATRIX);
	Shaders_Train->addUniform(mgl::NORMAL_MATRIX);
	Shaders_Train->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
	Shaders_Train->addUniform("LightPosition");
	Shaders_Train->addUniform("Levers");
	Shaders_Train->addUniform("noiseSampler");
	Shaders_Train->create();

	ModelMatrixId = Shaders_Train->Uniforms[mgl::MODEL_MATRIX].index;
	NormalMatrixId = Shaders_Train->Uniforms[mgl::NORMAL_MATRIX].index;

	//Ground

	Shaders_Ground = new mgl::ShaderProgram();
	Shaders_Ground->addShader(GL_VERTEX_SHADER, "ground-vs.glsl");
	Shaders_Ground->addShader(GL_FRAGMENT_SHADER, "ground-fs.glsl");

	Shaders_Ground->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	if (grass_field->Mesh->hasNormals()) {
		Shaders_Ground->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
	}
	if (grass_field->Mesh->hasTexcoords()) {
		Shaders_Ground->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
	}
	if (grass_field->Mesh->hasTangentsAndBitangents()) {
		Shaders_Ground->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
	}

	Shaders_Ground->addUniform(mgl::MODEL_MATRIX);
	Shaders_Ground->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
	Shaders_Ground->addUniform("Text_Grass");
	Shaders_Ground->addUniform("Displacement");
	Shaders_Ground->addUniform("Text_Dirt");
	Shaders_Ground->addUniform("Text_Snow");
	Shaders_Ground->create();

	ModelMatrixIdGrassField = Shaders_Ground->Uniforms[mgl::MODEL_MATRIX].index;

	//Rod Reiss
	Shaders_Rod_Reiss = new mgl::ShaderProgram();
	Shaders_Rod_Reiss->addShader(GL_VERTEX_SHADER, "rodReiss-vs.glsl");
	Shaders_Rod_Reiss->addShader(GL_FRAGMENT_SHADER, "rodReiss-fs.glsl");

	Shaders_Rod_Reiss->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	if (rod_reiss_titan[0]->Mesh->hasNormals()) {
		Shaders_Rod_Reiss->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
	}
	if (rod_reiss_titan[0]->Mesh->hasTexcoords()) {
		Shaders_Rod_Reiss->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
	}
	if (rod_reiss_titan[0]->Mesh->hasTangentsAndBitangents()) {
		Shaders_Rod_Reiss->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
	}

	Shaders_Rod_Reiss->addUniform(mgl::MODEL_MATRIX);
	Shaders_Rod_Reiss->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
	Shaders_Rod_Reiss->create();

	ModelMatrixRodTitan = Shaders_Rod_Reiss->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// CAMERA


void MyApp::updateMatrices(float ratio) {
	ProjectionMatrix1 = glm::ortho(-2.f * ratio, 2.f * ratio, -2.0f * ratio, 2.0f* ratio, 1.0f, 10000.0f);
	ProjectionMatrix2 = glm::perspective(glm::radians(30.0f), ratio, 1.0f, 10000.0f);
}

void MyApp::createCamera() {

	CameraP = new mgl::Camera(UBO_BP);
	Camera4 = new mgl::Camera(UBO_BP);
	Camera3 = new mgl::Camera(UBO_BP);
	Camera2 = new mgl::Camera(UBO_BP);
	Camera = new mgl::Camera(UBO_BP);
	Camera->setViewMatrix(glm::lookAt({ initial_postion_c1.x,initial_postion_c1.y,initial_postion_c1.z },
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)));
	Camera->setProjectionMatrix(ProjectionMatrix2);

}

/////////////////////////////////////////////////////////////////////////// DRAW

void MyApp::drawScene() {

	if (inPersonMode) {

		const glm::mat4 ChangingViewMatrix =
			glm::lookAt(glm::vec3(person_position_x, person_position_y, person_position_z),
				glm::vec3(FAR_POINT*glm::sin(glm::radians(person_alfa)*glm::sin(glm::radians(person_beta))),
					FAR_POINT*glm::cos(glm::radians(person_beta)), 
					FAR_POINT*glm::cos(glm::radians(person_alfa))*glm::sin(glm::radians(person_beta))),
				glm::vec3(0.0f, 1.0f, 0.0f));
		CameraP->setViewMatrix(ChangingViewMatrix);
		CameraP->setProjectionMatrix(ProjectionMatrix2);
		draw_meshs();
		return;
	}

	switch (current_camera){
		glm::mat4 ChangingViewMatrix;
		case 0:
			glm::quat qy = glm::angleAxis(glm::radians(-alfa), axis_y);
			glm::quat qx = glm::angleAxis(glm::radians(beta), axis_x);
			glm::quat q1 = qx * initial_postion_c1 * glm::inverse(qx);
			glm::quat q2 = qy * q1 * glm::inverse(qy);
			glm::vec3 vf = { q2.x, q2.y, q2.z };

			ChangingViewMatrix =
				glm::lookAt(glm::vec3((vf.x / zoom) + train_x_position, vf.y / zoom, (vf.z / zoom) + train_z_position),
					glm::vec3(train_x_position, 0.0f, train_z_position),
					glm::vec3(0.0f, 1.0f, 0.0f));

			Camera->setViewMatrix(ChangingViewMatrix);
			if (projection_camera1)
				Camera->setProjectionMatrix(ProjectionMatrix2);
			else
				Camera->setProjectionMatrix(ProjectionMatrix1);
			break;
		case 1:
			ChangingViewMatrix =
				glm::lookAt(glm::vec3((train_x_position + initial_position_c3.x),
					initial_position_c3.y,
					train_z_position + initial_position_c3.z),
					glm::vec3(train_x_position - 4.f * glm::sin(glm::radians(train_axis_angle)),
						0.6f,
						train_z_position - 4.f * glm::cos(glm::radians(train_axis_angle))),
					glm::vec3(0.0f, 1.0f, 0.0f));

			Camera2->setViewMatrix(ChangingViewMatrix);
			Camera2->setProjectionMatrix(ProjectionMatrix2);
			if (projection_camera2)
				Camera2->setProjectionMatrix(ProjectionMatrix2);
			else
				Camera2->setProjectionMatrix(ProjectionMatrix1);
			break;
		case 2:
			ChangingViewMatrix =
				glm::lookAt(glm::vec3((train_x_position + initial_position_c3.x),
					initial_position_c3.y,
					train_z_position + initial_position_c3.z),
					glm::vec3(train_x_position + 4.f * glm::sin(glm::radians(train_axis_angle)),
						0.6f,
						train_z_position + 4.f * glm::cos(glm::radians(train_axis_angle))),
					glm::vec3(0.0f, 1.0f, 0.0f));

			Camera3->setViewMatrix(ChangingViewMatrix);
			Camera3->setProjectionMatrix(ProjectionMatrix2);
			break;
		case 3:
			ChangingViewMatrix =
				glm::lookAt(glm::vec3(0, 500 / zoom4, 0),
					glm::vec3(person_position_x, 0, person_position_z),
					glm::vec3(0.0f, 1.0f, 0.0f));
			Camera4->setViewMatrix(ChangingViewMatrix);
			Camera4->setProjectionMatrix(ProjectionMatrix2);
			break;
	default:
		break;
	}

	draw_meshs();
}

void MyApp::draw_meshs() {

	Shaders_Train->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glm::vec3 lightDirection = glm::vec3(100.0f, 100.0f, 100.0f);
	glm::mat4 computedModelMatrix = ChangingModelMatrix * trainMatrixMovement;
	glUniform3fv(Shaders_Train->Uniforms["LightPosition"].index, 1, glm::value_ptr(lightDirection));
	glUniform1fv(Shaders_Train->Uniforms["Levers"].index, 4, Levers);
	glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(computedModelMatrix));
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(computedModelMatrix));
	glUniformMatrix3fv(NormalMatrixId, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	train->Mesh->draw();
	Shaders_Train->unbind();

	Shaders_Ground->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	glUniform1i(Shaders_Ground->Uniforms["Text_Grass"].index, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassBump);
	glUniform1i(Shaders_Ground->Uniforms["Displacement"].index, 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dirtTexture);
	glUniform1i(Shaders_Ground->Uniforms["Text_Dirt"].index, 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, snowTexture);
	glUniform1i(Shaders_Ground->Uniforms["Text_Snow"].index, 3);
	glUniformMatrix4fv(ModelMatrixIdGrassField, 1, GL_FALSE, glm::value_ptr(ChangingModelMatrix));
	glDisable(GL_CULL_FACE);
	grass_field->Mesh->draw();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	Shaders_Ground->unbind();

	Shaders_Rod_Reiss->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassBump);
	glm::mat4 computedModelMatrix2 = ChangingModelMatrix * titanMatrixMovement;
	glUniformMatrix4fv(ModelMatrixRodTitan, 1, GL_FALSE, glm::value_ptr(computedModelMatrix2));
	glDisable(GL_CULL_FACE);
	rod_reiss_titan[current_mesh]->Mesh->draw();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	Shaders_Rod_Reiss->unbind();
}


void MyApp::processKeyInput(GLFWwindow* win) {

	int state_print_ctr = glfwGetKey(win, GLFW_KEY_LEFT_CONTROL);
	int state = glfwGetKey(win, GLFW_KEY_P);

	if (state && !OLD_P_CLICKED && !state_print_ctr)
		// ctr + p doesnt active p
	{
		if (current_camera == 0)
			projection_camera1 = !projection_camera1;
		else if (current_camera == 1)
			projection_camera2 = !projection_camera2;
	}
	if (state && state_print_ctr && (!OLD_P_CLICKED || !OLD_CTR_CLICKED))
	{
		saveScreenshot("../Screenshots/Screenshot.png");
	}

	if (state)
		OLD_P_CLICKED = true;
	else
		OLD_P_CLICKED = false;

	if (state_print_ctr)
		OLD_CTR_CLICKED = true;
	else
		OLD_CTR_CLICKED = false;


	if (glfwGetKey(win, GLFW_KEY_R) && !OLD_R_CLICKED) {
		if (!inPersonMode) {
			// If I was on the train
			glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			person_position_x = train_x_position;
			person_position_z = train_z_position;
			person_alfa = train_axis_angle;
			person_momentum = static_cast<int>((train_velocity * glm::cos(glm::radians(train_axis_angle))) / ATTRITION);
			person_side_momentum += 1;
			inPersonMode = !inPersonMode;
		}
		else {
			if (sqrt(pow(person_position_x - train_x_position, 2) + pow(person_position_z - train_z_position, 2)) <= 5.f) {
				glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				inPersonMode = !inPersonMode;
				current_camera = 2;
			}
		}
	}

	if (glfwGetKey(win, GLFW_KEY_R))
		OLD_R_CLICKED = true;
	else
		OLD_R_CLICKED = false;

	if (glfwGetKey(win, GLFW_KEY_T) && !OLD_T_CLICKED) {
		if (!titan_is_follow) {
			SoundEngineEnv->setSoundVolume(0.0f);
			SoundEngineBigBoy->setSoundVolume(0.5f);
		}
		else {
			SoundEngineEnv->setSoundVolume(soundVolume);
			SoundEngineBigBoy->setSoundVolume(0.0f);
		}

		titan_is_follow = !titan_is_follow;
	}

	if (glfwGetKey(win, GLFW_KEY_T))
		OLD_T_CLICKED = true;
	else
		OLD_T_CLICKED = false;


	if (inPersonMode) {

		CameraP->Update(UBO_BP);
		if (glfwGetKey(win, GLFW_KEY_W))
			person_velocity += add_velocity;

		if (glfwGetKey(win, GLFW_KEY_S))
			person_velocity -= add_velocity;

		if (glfwGetKey(win, GLFW_KEY_A)) {
			person_side_velocity += add_velocity;
		}
		if (glfwGetKey(win, GLFW_KEY_D)) {
			person_side_velocity -= add_velocity;
		}
		if (glfwGetKey(win, GLFW_KEY_SPACE) && person_position_y <= person_min_y) {
			person_jump_val += 1.5f;
		}
		if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT)) {
			person_velocity *= 2;
			person_side_velocity *= 2;
		}
	}
	else{
		if ((glfwGetKey(win, GLFW_KEY_C) && !OLD_C_CLICKED) || glfwGetKey(win, GLFW_KEY_R)){

			if((glfwGetKey(win, GLFW_KEY_C) && !OLD_C_CLICKED))
				current_camera += 1;

			if (current_camera > CAMERA_NUMBER - 1) {
				current_camera = 0;
			}
			switch (current_camera){
				case 0:
					Camera->Update(UBO_BP);
					break;	
				case 1:
					Camera2->Update(UBO_BP);
					break;
				case 2:
					Camera3->Update(UBO_BP);
					break;
				case 3:
					Camera4->Update(UBO_BP);
					break;
			default:
				break;
			}
		}
		if (glfwGetKey(win, GLFW_KEY_C))
			OLD_C_CLICKED = true;
		else
			OLD_C_CLICKED = false;

		if (glfwGetKey(win, GLFW_KEY_V) && !OLD_V_CLICKED) {
			if (current_camera == 1) {
				current_camera = 2;
				Camera3->Update(UBO_BP);
			}
			else {
				current_camera = 1;
				Camera2->Update(UBO_BP);
			}
		}
		if (glfwGetKey(win, GLFW_KEY_V))
			OLD_V_CLICKED = true;
		else
			OLD_V_CLICKED = false;

		//TRAIN MOVEMENT KEYS

		if (glfwGetKey(win, GLFW_KEY_W))
			train_velocity += 2;

		if (glfwGetKey(win, GLFW_KEY_S))
			train_velocity -= 2;

		if (glfwGetKey(win, GLFW_KEY_A)) {
			train_axis_angle += 0.01f * train_velocity;
		}
		if (glfwGetKey(win, GLFW_KEY_D)) {
			train_axis_angle -= 0.01f * train_velocity;
		}
	}

	//SHADERS CONTROL

	if (glfwGetKey(win, GLFW_KEY_1)) {
		Levers[0] += 0.01f;
	}
	if (glfwGetKey(win, GLFW_KEY_2)) {
		Levers[0] -= 0.01f;
	}
	if (glfwGetKey(win, GLFW_KEY_3)) {
		Levers[1] += 0.01f;
	}
	if (glfwGetKey(win, GLFW_KEY_4)) {
		Levers[1] -= 0.01f;
	}
	if (glfwGetKey(win, GLFW_KEY_5)) {
		Levers[2] += 0.01f;
	}
	if (glfwGetKey(win, GLFW_KEY_6)) {
		Levers[2] -= 0.01f;
	}
	if (glfwGetKey(win, GLFW_KEY_7)) {
		Levers[3] += 0.01f;
	}
	if (glfwGetKey(win, GLFW_KEY_8)) {
		Levers[3] -= 0.01f;
	}

}

void MyApp::processTrainMovement(){

	train_z_position += (float) (train_velocity * glm::cos(glm::radians(train_axis_angle))) / ATTRITION;
	train_x_position += (float) (train_velocity * glm::sin(glm::radians(train_axis_angle))) / ATTRITION;

	if (!inPersonMode) {
		person_position_x = train_x_position;
		person_position_z = train_z_position;
	}
	
	if (train_velocity > 0)
		train_velocity -= 1;
	else if (train_velocity < 0)
		train_velocity += 1;

	if (train_velocity > train_max_speed)
		train_velocity = train_max_speed;
	if(train_velocity < - (train_max_speed / 2))
		train_velocity = -(train_max_speed / 2);

	//Update the matrix
	trainMatrixMovement = glm::translate(glm::vec3(train_x_position, 0.0f, train_z_position)) *
		glm::rotate(glm::radians(train_axis_angle), axis_y);

	if (train_axis_angle >= 360.0f || train_axis_angle <= -360.0f)
		train_axis_angle = 0.0f;

	//Sound part
	processSound();
}

void MyApp::processPersonMovement() {

	person_position_z += (float)((person_velocity + person_momentum) * glm::cos(glm::radians(person_alfa)));
	person_position_x += (float)((person_velocity + person_momentum) * glm::sin(glm::radians(person_alfa)));

	const float angle_ajust = 90.f;
	person_position_z += (float)((person_side_velocity + person_side_momentum) * glm::cos(glm::radians(person_alfa + angle_ajust)));
	person_position_x += (float)((person_side_velocity + person_side_momentum) * glm::sin(glm::radians(person_alfa + angle_ajust)));

	if (person_jump_val > 0.f) {
		person_position_y += person_jump_decay;
		person_jump_val -= person_jump_decay;
	}
	else if(person_position_y > person_min_y)
		person_position_y -= GRAVITY;

	person_velocity = 0.f;
	person_side_velocity = 0.f;
	person_side_momentum = 0.f;

	const int atr = 2;
	if (person_momentum == 0)
		return;
	else if (person_momentum > atr)
		person_momentum -= atr;
	else if (person_momentum < -atr)
		person_momentum += atr;
	else if (person_momentum <= atr && person_momentum >= 0)
		person_momentum = 0;
	else if (person_momentum >= -atr && person_momentum <= 0)
		person_momentum = 0;
}

void MyApp::processSound() {

	float sound_mod = 200.0f;

	if (train_velocity == 0) {
		SoundEngineTrain->setSoundVolume(0.0f);
	}
	else if (train_velocity < 50 && train_velocity > 0) {
		SoundEngineTrain->setSoundVolume((train_velocity / sound_mod));
	}
	else if (train_velocity >= 50 && train_velocity > 0) {
		SoundEngineTrain->setSoundVolume((50 / sound_mod));
	}
	else if (train_velocity > -50 && train_velocity < 0) {
		SoundEngineTrain->setSoundVolume(abs(train_velocity / sound_mod));
	}
	else if (train_velocity <= -50 && train_velocity < 0) {
		SoundEngineTrain->setSoundVolume((50 / sound_mod));
	}

	if (person_jump_val > 0.0f) {
		SoundEngineWalk->setSoundVolume(0.0f);
		SoundEngineRun->setSoundVolume(0.0f);
		if (!jump_playing) {
			jump_playing = true;
			SoundEngineJump->play2D("../assets/jump.mp3", false);
			SoundEngineJump->setSoundVolume(0.3f);
		}
	}
	else if (abs(person_velocity) >= add_velocity * 2 || abs(person_side_velocity) >= add_velocity * 2) {
		SoundEngineWalk->setSoundVolume(0.0f);
		SoundEngineRun->setSoundVolume(0.3f);
		jump_playing = false;
	}
	else if ((abs(person_velocity) > 0.f || abs(person_side_velocity) > 0.f)){
		SoundEngineWalk->setSoundVolume(0.3f);
		SoundEngineRun->setSoundVolume(0.0f);
		jump_playing = false;
	}
	else {
		SoundEngineWalk->setSoundVolume(0.0f);
		SoundEngineRun->setSoundVolume(0.0f);
		jump_playing = false;
	}
	
}

void MyApp::processTitanMovement() {

	if(person_position_x >= titan_position_x)
		titan_position_x += 0.5f;
	else if (person_position_x < titan_position_x)
		titan_position_x -= 0.5f;

	if (person_position_z >= titan_position_z)
		titan_position_z += 0.5f;
	else if (person_position_z < titan_position_z)
		titan_position_z -= 0.5f;
	
	glm::vec2 object_position(titan_position_x, titan_position_z);
	glm::vec2 target_position(person_position_x, person_position_z);

	glm::vec2 delta = target_position - object_position;

	float delta_angle = glm::degrees(atan2(delta.x, delta.y));
	titan_axis_angle += delta_angle;

	if (change_mesh == 2) {
		if(current_mesh >= RodReissMeshNumber - 1){
			next_mesh = -1;
		}
		else if(current_mesh < 1) {
			next_mesh = 1;
		}
		current_mesh += next_mesh;
		change_mesh = 0;
	}
	change_mesh += 1;
	titanMatrixMovement = glm::translate(glm::vec3(titan_position_x, 0.0f, titan_position_z)) *
		glm::rotate(glm::radians(titan_axis_angle), axis_y);
}

void MyApp::saveScreenshot(const char* filename)
{
	std::cout << "Saving screenshot in: " << filename << std::endl;

	GLint dims[4] = { 0 };
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_FRONT);
	glGetIntegerv(GL_VIEWPORT, dims);
	GLint fbWidth = dims[2];
	GLint fbHeight = dims[3];

	// Allocate a buffer for the pixel data
	unsigned char* pixels = new unsigned char[fbWidth * fbHeight * 3];

	// Read the pixels from the framebuffer
	glReadPixels(0, -100, fbWidth, fbHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// Flip the pixels array since it will be upside down, happens to the best!
	unsigned char* row = new unsigned char[fbWidth * 3];
	for (int i = 0; i < fbHeight / 2; ++i) {
		memcpy(row, &pixels[i * fbWidth * 3], fbWidth * 3);
		memcpy(&pixels[i * fbWidth * 3], &pixels[(fbHeight - i - 1) * fbWidth * 3], fbWidth * 3);
		memcpy(&pixels[(fbHeight - i - 1) * fbWidth * 3], row, fbWidth * 3);
	}

	// Save the pixel data to a PNG file
	stbi_write_png(filename, fbWidth, fbHeight, 3, pixels, 0);
	// Clean up
	delete[] pixels;
	delete[] row;
}


void MyApp::scrollCallback(GLFWwindow * window, double xoffset, double yoffset) {

	if (current_camera == 0)
		zoom *= 1.f + (float)yoffset * 0.1f;
	else if(current_camera == 1)
		zoom2 *= 1.f + (float)yoffset * 0.1f;
	else if (current_camera == 3) {
		zoom4 *= 1.f + (float)yoffset * 0.1f;
	}
}


void MyApp::processMouseMovement(GLFWwindow * win) {

	glfwGetCursorPos(win, &xpos, &ypos);

	if (inPersonMode) {

		double diffx = xpos - win_width / 2;
		double diffy = ypos - win_heigth / 2;

		person_alfa -= (float)diffx * 0.05f;
		person_beta += (float)diffy * 0.05f;

		if (person_alfa >= 360.0f || person_alfa <= -360.0f)
			person_alfa = 0.0f;
		if (person_beta >= 180.0f)
			person_beta = 180.0f;
		if (person_beta <= 0)
			person_beta = 0.f;

		old_xpos = xpos;
		old_ypos = ypos;

		glfwSetCursorPos(win, win_width/2, win_heigth/2);
		
		return;
	}

	switch (current_camera){
		double diffx;
		double diffy;
		case 0:
			if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT)) {
				accelaration_x = 0;
				accelaration_y = 0;
				diffx = xpos - old_xpos;
				diffy = ypos - old_ypos;

				alfa += (float)diffx * 0.1f;
				accelaration_x += (int)diffx;

				beta -= (float)diffy * 0.1f;
				accelaration_y -= (int)diffy;
			}
			// So we dont overflow it
			if (alfa >= 360.0f || alfa <= -360.0f) {
				alfa = 0.0f;
			}
			if (beta >= 360.0f || beta <= -360.0f) {
				beta = 0.0f;
			}
			old_xpos = xpos;
			old_ypos = ypos;

			alfa += (float)accelaration_x / 10;
			beta += (float)accelaration_y / 10;

			if (accelaration_x > 0)
				accelaration_x -= 1;
			else if (accelaration_x < 0)
				accelaration_x += 1;

			if (accelaration_y > 0)
				accelaration_y -= 1;
			else if (accelaration_y < 0)
				accelaration_y += 1;
			break;
		case 1:
			break;
		case 2:
			if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT)) {
				accelaration_x3 = 0;
				accelaration_y3 = 0;
				diffx = xpos - old_xpos;
				diffy = ypos - old_ypos;

				angle3 += (float)diffx * 0.1f;
				accelaration_x3 += (int)diffx;
			}
			// So we dont overflow it
			if (angle3 >= 360.0f || angle3 <= -360.0f) {
				angle3 = 0.0f;
			}
			old_xpos = xpos;
			old_ypos = ypos;

			angle3 += (float)accelaration_x3 / 10;

			if (accelaration_x3 > 0)
				accelaration_x3 -= 1;
			else if (accelaration_x3 < 0)
				accelaration_x3 += 1;

			if (accelaration_y3 > 0)
				accelaration_y3 -= 1;
			else if (accelaration_y3 < 0)
				accelaration_y3 += 1;
			break;
		case 3:
			break;
		default:
			break;
	}

}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {

	Levers[0] = 0.16f;
	Levers[1] = 4.f;
	Levers[2] = 0.86f;
	Levers[3] = 3.53f;

	createMeshes();
	createShaderPrograms(); // after mesh;
	createCamera();
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	/* SOUND!*/
	SoundEngineEnv->setSoundVolume(soundVolume);
	SoundEngineEnv->play2D("../assets/Birds-chirping-sound-effect.mp3", true);
	SoundEngineBigBoy->setSoundVolume(0.0f);
	SoundEngineBigBoy->play2D("../assets/RodReissTheme.mp3", true);
	SoundEngineTrain->setSoundVolume(0.0f);
	SoundEngineTrain->play2D("../assets/train_loop2.wav", true);
	SoundEngineWalk->setSoundVolume(0.0f);
	SoundEngineWalk->play2D("../assets/walk.wav", true);
	SoundEngineRun->setSoundVolume(0.0f);
	SoundEngineRun->play2D("../assets/run.wav", true);

	createTextures();

}

double MyApp::noise(double x, double y, double z) {
	// Initialize variables to store the noise value and the current harmonic.
	double noise = 0;
	double harmonic = 1.1;

	// Set the number of harmonics to use and the persistence value.
	int numHarmonics = 8;
	double persistence = 0.5;

	// Generate the noise value by summing the contributions of each harmonic.
	for (int i = 0; i < numHarmonics; i++) {
		// Generate the next harmonic using 3D Perlin noise.
		double frequency = pow(2, i);
		double amplitude = pow(persistence, i);
		double decay = pow(harmonic, i);
		noise += (perlin(x * frequency, y * frequency, z * frequency) * amplitude) / decay;
		// Update the current harmonic.
		//harmonic *= 0.5;
	}

	return noise;
}

double MyApp::perlin(double x, double y, double z) {
	// Determine the grid cell that the position is in and the relative position
	// of the point within that cell.
	int xi = (int)std::floor(x);
	int yi = (int)std::floor(y);
	int zi = (int)std::floor(z);
	double xf = x - xi;
	double yf = y - yi;
	double zf = z - zi;

	// Generate a random permutation for the x, y, and z coordinates of the
	// grid cell.
	std::array<int, 8> permutation = {
		{0, 1, 2, 3, 4, 5, 6, 7} };
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(permutation.begin(), permutation.end(), g);

	// Calculate the noise contribution from each of the eight corners of the
	// grid cell.
	double n000 = gradient(xi, yi, zi, xf, yf, zf, permutation[0]);
	double n001 = gradient(xi, yi, zi + 1, xf, yf, zf - 1, permutation[1]);
	double n010 = gradient(xi, yi + 1, zi, xf, yf - 1, zf, permutation[2]);
	double n011 = gradient(xi, yi + 1, zi + 1, xf, yf - 1, zf - 1, permutation[3]);
	double n100 = gradient(xi + 1, yi, zi, xf - 1, yf, zf, permutation[4]);
	double n101 = gradient(xi + 1, yi, zi + 1, xf - 1, yf, zf - 1, permutation[5]);
	double n110 = gradient(xi + 1, yi + 1, zi, xf - 1, yf - 1, zf, permutation[6]);
	double n111 = gradient(xi + 1, yi + 1, zi + 1, xf - 1, yf - 1, zf - 1, permutation[7]);

	// Interpolate the noise value using a cubic spline.
	double x1 = cubicInterpolate(n000, n100, n010, n110, xf);
	double x2 = cubicInterpolate(n001, n101, n011, n111, xf);
	double y1 = cubicInterpolate(x1, x2, n011, n111, yf);
	double x3 = cubicInterpolate(n010, n110, n011, n111, xf);
	double x4 = cubicInterpolate(n000, n100, n001, n101, xf);
	double y2 = cubicInterpolate(x3, x4, n001, n101, yf);
	double z1 = cubicInterpolate(y1, y2, n001, n101, zf);
	return z1;
}

double MyApp::gradient(int xi, int yi, int zi, double xf, double yf, double zf, int h) {
	int hh = h & 0xF;
	double u = hh < 8 ? xf : yf;
	double v = hh < 4 ? yf : hh == 12 || hh == 14 ? xf : zf;
	return ((hh & 1) == 0 ? u : -u) + ((hh & 2) == 0 ? v : -v);
}

double MyApp::cubicInterpolate(double a, double b, double c, double d, double t) {
	double p = (d - c) - (a - b);
	return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
}

void MyApp::createTextures() {

	//Train
	int width = 16;
	int height = 16;
	int depth = 16;
	const unsigned int size = width * height * depth;
	float *image = new float[size];
	int idx = 0;
	double step = 1.0 / (width - 1.);
	for (double x = 0.0; x <= 1.0; x += step) {
		for (double y = 0.0; y <= 1.0; y += step) {
			for (double z = 0.0; z <= 1.0; z += step) {
				double noise_val = noise(x * 0.1, y * 0.1, z * 0.1);
				image[idx++] = static_cast<float>(noise_val);
			}
		}
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_RED, GL_FLOAT, image);
	glEnable(GL_TEXTURE_3D);

	delete[] image;

	//Grass
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &grassTexture);
	glBindTexture(GL_TEXTURE_2D, grassTexture);

	int Width, Height, Channels;
	unsigned char* image2 = stbi_load("../assets/grass.png", &Width, &Height, &Channels, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image2);

	//Bump

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &grassBump);
	glBindTexture(GL_TEXTURE_2D, grassBump);

	unsigned char* image3 = stbi_load("../assets/BumpMap.png", &Width, &Height, &Channels, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image3);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image3);

	//Dirt

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &dirtTexture);
	glBindTexture(GL_TEXTURE_2D, dirtTexture);

	unsigned char* image4 = stbi_load("../assets/dirt.jpg", &Width, &Height, &Channels, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image4);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image4);

	//Snow

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &snowTexture);
	glBindTexture(GL_TEXTURE_2D, snowTexture);

	unsigned char* image5 = stbi_load("../assets/snow.jpg", &Width, &Height, &Channels, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image5);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image5);

}

void MyApp::windowSizeCallback(GLFWwindow * win, int winx, int winy) {
	if (winy == 0) {
		winy = 1;
	}

	glViewport(0, 0, winx, winy);
	win_width = winx;
	win_heigth = winy;
	// change projection matrices to maintain aspect ratio
	float ratio = (float)winx / (float)winy;

	updateMatrices(ratio);

}

void MyApp::displayCallback(GLFWwindow * win, double elapsed) {
	processMouseMovement(win);
	processKeyInput(win);
	processTrainMovement();
	processPersonMovement();
	if(titan_is_follow)
		processTitanMovement();
	drawScene();
}

void MyApp::windowCloseCallback(GLFWwindow * win) {
	train->Mesh->destroyBufferObjects();
	grass_field->Mesh->destroyBufferObjects();
	for (int i = 0; i < RodReissMeshNumber; i++) {
		rod_reiss_titan[i]->Mesh->destroyBufferObjects();
	}

}

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[]) {
	mgl::Engine& engine = mgl::Engine::getInstance();
	engine.setApp(new MyApp());
	engine.setOpenGL(4, 6);
	engine.setWindow(win_width, win_heigth, "CHO CHO", 0, 1);
	engine.init();
	engine.run();
	exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
