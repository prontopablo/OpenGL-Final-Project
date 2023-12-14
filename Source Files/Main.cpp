#include<math.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include<SFML\Audio.hpp>

#include"Model.h"
#include"CelestialBody.h"
#include"Bird.h"
#include"Cactus.h"

// Size of Screen
const unsigned int screenWidth = 1900;
const unsigned int screenHeight = 1000;

// MSAA Samples
unsigned int samples = 8;

// Skybox
float skyboxVertices[] =
{
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,     
	 1.0f, -1.0f, -1.0f,    
	-1.0f, -1.0f, -1.0f,      
	-1.0f,  1.0f,  1.0f,     
	 1.0f,  1.0f,  1.0f,     
	 1.0f,  1.0f, -1.0f,     
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

// Utility function generate random floats [-1.0, 1.0]
float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

int main()
{
	// Initialize GLFW
	glfwInit();

	// using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, samples);
	
	// CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Darragh Nolan Final Project", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	
	// Viewport goes from x = 0, y = 0, to x = width, y = height
	glViewport(0, 0, screenWidth, screenHeight);

	// Generate Shader objects
	Shader shaderProgram("default.vert", "default.frag");
	Shader specularProgram("specular.vert", "specular.frag");
	Shader sunProgram("sun.vert", "sun.frag");
	Shader moonProgram("moon.vert", "moon.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	// Lights
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(40.0, 50.0, 0.0);
	glm::vec4 lightColor2 = glm::vec4(0.7, 0.7, 1.0, 1.0f);
	glm::vec3 lightPos2 = glm::vec3(40.0, 50.0, 0.0);
	glm::vec4 lightColor3 = glm::vec4(1.0, 0.0, 0.0, 1.0f);
	glm::vec3 lightPos3 = glm::vec3(-6.0, 10.0, -2.0);

	// Activate shaders
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor2"), lightColor2.x, lightColor2.y, lightColor2.z, lightColor2.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor3"), lightColor3.x, lightColor3.y, lightColor3.z, lightColor3.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);

	specularProgram.Activate();
	glUniform4f(glGetUniformLocation(specularProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(specularProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform4f(glGetUniformLocation(specularProgram.ID, "lightColor2"), lightColor2.x, lightColor2.y, lightColor2.z, lightColor2.w);
	glUniform3f(glGetUniformLocation(specularProgram.ID, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform4f(glGetUniformLocation(specularProgram.ID, "lightColor3"), lightColor3.x, lightColor3.y, lightColor3.z, lightColor3.w);
	glUniform3f(glGetUniformLocation(specularProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);

	sunProgram.Activate();
	glUniform4f(glGetUniformLocation(sunProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(sunProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform4f(glGetUniformLocation(sunProgram.ID, "lightColor2"), lightColor2.x, lightColor2.y, lightColor2.z, lightColor2.w);
	glUniform3f(glGetUniformLocation(sunProgram.ID, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform4f(glGetUniformLocation(sunProgram.ID, "lightColor3"), lightColor3.x, lightColor3.y, lightColor3.z, lightColor3.w);
	glUniform3f(glGetUniformLocation(sunProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);

	moonProgram.Activate();
	glUniform4f(glGetUniformLocation(moonProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(moonProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform4f(glGetUniformLocation(moonProgram.ID, "lightColor2"), lightColor2.x, lightColor2.y, lightColor2.z, lightColor2.w);
	glUniform3f(glGetUniformLocation(moonProgram.ID, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
	glUniform4f(glGetUniformLocation(moonProgram.ID, "lightColor3"), lightColor3.x, lightColor3.y, lightColor3.z, lightColor3.w);
	glUniform3f(glGetUniformLocation(moonProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);

	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Multisampling
	glEnable(GL_MULTISAMPLE);

	// Enables Cull Facing
	// glEnable(GL_CULL_FACE);
	// Keeps front faces
	// glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	// glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(screenWidth, screenHeight, glm::vec3(-5.0f, 10.0f, 15.0f));

	// Load in models
	Model sun("models/sun/scene.gltf");
	Model moon("models/moon/scene.gltf");
	Model riverWall("models/riverWall/scene.gltf");
	Model riverWall2("models/riverWall2/scene.gltf");
	Model riverbed("models/riverbed/scene.gltf");
	
	// Bridge
	Model bridge("models/bridge/scene.gltf");
	Model harpBase("models/harpBase/scene.gltf");

	// Bridge strings
	Model string1("models/string1/scene.gltf");
	Model string2("models/string2/scene.gltf");
	Model string3("models/string3/scene.gltf");
	Model string4("models/string4/scene.gltf");
	Model string5("models/string5/scene.gltf");
	Model string6("models/string6/scene.gltf");
	Model string7("models/string7/scene.gltf");
	Model string8("models/string8/scene.gltf");

	// Animals
	Model birdModel("models/bird/scene.gltf");
	Model snake("models/snake/scene.gltf");

	// Dunes
	Model cityBase1("models/cityBase1/scene.gltf");
	Model cityBase2("models/cityBase2/scene.gltf");

	// Convention Centre
	Model conventionCentreBase("models/conventionCentreBase/scene.gltf");
	Model canOfBeans("models/canOfBeans/scene.gltf");
	Model canOfBeansBase("models/canOfBeansBase/scene.gltf");

	// Fences
	Model metal1("models/metal1/scene.gltf");
	Model metal2("models/metal2/scene.gltf");
	Model metal3("models/metal3/scene.gltf");
	Model metal4("models/metal4/scene.gltf");
	Model metal5("models/metal5/scene.gltf");
	Model metal6("models/metal6/scene.gltf");
	Model metal7("models/metal7/scene.gltf");
	Model metal8("models/metal8/scene.gltf");
	Model metal9("models/metal9/scene.gltf");
	Model metal10("models/metal10/scene.gltf");
	Model metal11("models/metal11/scene.gltf");
	Model metal12("models/metal12/scene.gltf");

	// Electricity poles/wires
	Model pole1("models/pole1/scene.gltf");
	Model pole2("models/pole2/scene.gltf");
	Model pole3("models/pole3/scene.gltf");
	Model wire1("models/wire1/scene.gltf");
	Model wire2("models/wire2/scene.gltf");
	Model wire3("models/wire3/scene.gltf");
	Model wire4("models/wire4/scene.gltf");
	Model wire5("models/wire5/scene.gltf");
	Model wire6("models/wire6/scene.gltf");
	Model wire7("models/wire7/scene.gltf");
	Model wire8("models/wire8/scene.gltf");
	Model wire9("models/wire9/scene.gltf");
	Model wire10("models/wire10/scene.gltf");

	// Single model is drawn many times
	Model cactus1("models/cactus1/scene.gltf");

	// Solar panels
	Model spStand("models/spStand/scene.gltf");
	Model spStand2("models/spStand2/scene.gltf");
	Model spStand3("models/spStand3/scene.gltf");
	Model spStand4("models/spStand4/scene.gltf");

	Model spJoint("models/spJoint/scene.gltf");
	Model spJoint2("models/spJoint2/scene.gltf");
	Model spJoint3("models/spJoint3/scene.gltf");
	Model spJoint4("models/spJoint4/scene.gltf");

	Model spPanel("models/spPanel/scene.gltf");
	Model spPanel2("models/spPanel2/scene.gltf");
	Model spPanel3("models/spPanel3/scene.gltf");
	Model spPanel4("models/spPanel4/scene.gltf");

	Model light1("models/lights/scene.gltf");
	Model light2("models/lights2/scene.gltf");
	Model light3("models/lights3/scene.gltf");
	Model light4("models/lights4/scene.gltf");

	// Other buildings
	Model mcCannBase("models/building2Base/scene.gltf");
	Model building3("models/building3/scene.gltf");

	// Model Positions
	glm::vec3 origin = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 sunPosition = glm::vec3(40.0, 50.0, 0.0);
	glm::vec3 snakeHeadPosition = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 snakeBodyPosition = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 snakeTailPosition = glm::vec3(0.0, 0.0, 0.0);

	// Model Rotations
	glm::quat birdRotation = glm::quat(0.0f, 1.0f, 0.0f, 0.0f);
	glm::quat spStandRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::quat spJointRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::quat spPanelRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);

	glm::quat snakeHeadRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::quat snakeBodyRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::quat snakeTailRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);

	// Snake variables
	float snakeTargetX = 50.0f;
	float snakeTargetZ = 50.0f;
	glm::vec3 target(snakeTargetX, 0.0f, snakeTargetZ);
	float numSnakeParts = 5.0f;
	float snakeOffset = 2.05f;

	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double frameTimeDiff;
	// Keeps track of the amount of frames in frameTimeDiff
	unsigned int frameCounter = 0;


	// Use this to disable VSync
	//glfwSwapInterval(0);

	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// All the faces of the cubemap (exact order)
	std::string facesCubemap[6] =
	{
		"models/skybox/right.png",
		"models/skybox/left.png",
		"models/skybox/top.png",
		"models/skybox/bottom.png",
		"models/skybox/front.png",
		"models/skybox/back.png",
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	// Prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	// This might help with seams on some systems
	// glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// Load the Second Skybox Textures
	std::string facesSecondCubemap[6] =
	{
		"models/skybox/nightSky/right.png",
		"models/skybox/nightSky/left.png",
		"models/skybox/nightSky/top.png",
		"models/skybox/nightSky/bottom.png",
		"models/skybox/nightSky/front.png",
		"models/skybox/nightSky/back.png",
	};

	// Creates the second cubemap texture object
	unsigned int secondCubemapTexture;
	glGenTextures(1, &secondCubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, secondCubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Load textures into the second cubemap
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesSecondCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesSecondCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// Initialize 10 birds to circle around a target point
	glm::vec3 birdsCenterPoint(-63.0f, 30.0f, 0.0f);
	float birdsRadius = 25.0f;

	std::vector<Bird> birds;

	for (int i = 0; i < 10; ++i) {
		float startAngle = static_cast<float>(i) * glm::two_pi<float>() / 10.0f;
		birds.push_back(Bird(birdsCenterPoint, startAngle, birdsRadius));
	}

	// Grid based generation of cacti with slight random variations in position
	std::vector<Cactus> cacti;
	glm::vec3 cactusPosition;
	const int numCactiPerRow = 10;
	const int numCactiPerColumn = 12;
	const float spacingX = 100.0f;
	const float spacingZ = 100.0f;

	for (int row = 0; row < numCactiPerRow; ++row) {
		for (int col = 0; col < numCactiPerColumn; ++col) {
			float xPos = row * spacingX;
			float zPos = col * spacingZ;
			cactusPosition = glm::vec3(xPos + rand() % (100 - 50 + 1), 0.0, zPos + rand() % (100 - 50 + 1));
			cacti.push_back(Cactus(cactusPosition));
		}
	}

	// Create a sun at 0,0,0, with an angle of 0, radius of 400
	CelestialBody sunObject(glm::vec3(0.0, 0.0, 0.0), 0.0f, 400.0f);
	CelestialBody moonObject(glm::vec3(0.0, 0.0, 0.0), 0.0f, 400.0f, glm::pi<float>() * 4); // Offset moon to be across from the sun

	// GUI Library setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Music setup
	sf::Music music;
	music.openFromFile("C:/Users/darra/Desktop/Music/sandTunes.ogg");
	music.setVolume(15.0f);
	music.setPitch(1.0f);
	music.play();
	

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// FPS Counter
		crntTime = glfwGetTime();
		frameTimeDiff = crntTime - prevTime;
		frameCounter++;

		if (frameTimeDiff >= 1.0 / 30.0)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / frameTimeDiff) * frameCounter);
			std::string ms = std::to_string((frameTimeDiff / frameCounter) * 1000);
			std::string newTitle = "Darragh Nolan Final Project - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			frameCounter = 0;

			// Use if VSync disabled:
			// camera.Inputs(window);
		}

		// Color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clear back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		// Create ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(375, 550));

		// Handles camera inputs (delete if VSync disabled)
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);

		// Update sun position
		sunObject.Update(1.0f / 60.0f);

		moonObject.Update(1.0f / 60.0f);

		// Update sun direction
		glm::vec3 sunDirection = sunObject.GetCelestialBodyDirection();

		glm::vec3 moonDirection = moonObject.GetCelestialBodyDirection();

		// Sun shader (emmissive)
		sunProgram.Activate();
		glUniform3f(glGetUniformLocation(sunProgram.ID, "sunDirection"), sunDirection.x, sunDirection.y, sunDirection.z);
		glUniform3f(glGetUniformLocation(sunProgram.ID, "moonDirection"), moonDirection.x, moonDirection.y, moonDirection.z);
		glUniform3f(glGetUniformLocation(sunProgram.ID, "lightPos"), sunObject.position.x, sunObject.position.y, sunObject.position.z);
		glUniform3f(glGetUniformLocation(sunProgram.ID, "lightPos2"), moonObject.position.x, moonObject.position.y, moonObject.position.z);
		glUniform3f(glGetUniformLocation(sunProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);
		sun.Draw(sunProgram, camera, sunObject.position, sunDirection);

		// Moon shader (emmissive)
		moonProgram.Activate();
		glUniform4f(glGetUniformLocation(moonProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(moonProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform4f(glGetUniformLocation(moonProgram.ID, "lightColor2"), lightColor2.x, lightColor2.y, lightColor2.z, lightColor2.w);
		glUniform3f(glGetUniformLocation(moonProgram.ID, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform4f(glGetUniformLocation(moonProgram.ID, "lightColor3"), lightColor3.x, lightColor3.y, lightColor3.z, lightColor3.w);
		glUniform3f(glGetUniformLocation(moonProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);
		moon.Draw(moonProgram, camera, moonObject.position, moonDirection);

		// Specular Shader
		specularProgram.Activate();
		glUniform3f(glGetUniformLocation(specularProgram.ID, "sunDirection"), sunDirection.x, sunDirection.y, sunDirection.z);
		glUniform3f(glGetUniformLocation(specularProgram.ID, "moonDirection"), moonDirection.x, moonDirection.y, moonDirection.z);
		glUniform3f(glGetUniformLocation(specularProgram.ID, "lightPos"), sunObject.position.x, sunObject.position.y, sunObject.position.z);
		glUniform3f(glGetUniformLocation(specularProgram.ID, "lightPos2"), moonObject.position.x, moonObject.position.y, moonObject.position.z);
		glUniform3f(glGetUniformLocation(specularProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);

		canOfBeans.Draw(specularProgram, camera, origin);

		// Main lighting shader
		shaderProgram.Activate();
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "sunDirection"), sunDirection.x, sunDirection.y, sunDirection.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "moonDirection"), moonDirection.x, moonDirection.y, moonDirection.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), sunObject.position.x, sunObject.position.y, sunObject.position.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos2"), moonObject.position.x, moonObject.position.y, moonObject.position.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos3"), lightPos3.x, lightPos3.y, lightPos3.z);

		// Change pitch based on sun position
		float targetPitch = (sunObject.position.y < 0) ? 0.5f : 1.0f;
		float currentPitch = music.getPitch();

		float interpolationFactor = 0.005f; // how quickly to shift pitch

		// Smoothly transitions between pitches
		float newPitch = currentPitch + interpolationFactor * (targetPitch - currentPitch);
		music.setPitch(newPitch);

		// Draw models
		riverWall.Draw(shaderProgram, camera, origin);
		riverWall2.Draw(shaderProgram, camera, origin);
		riverbed.Draw(shaderProgram, camera, origin);
		bridge.Draw(shaderProgram, camera, origin);

		harpBase.Draw(shaderProgram, camera, origin);

		string1.Draw(shaderProgram, camera, origin);
		string2.Draw(shaderProgram, camera, origin);
		string3.Draw(shaderProgram, camera, origin);
		string4.Draw(shaderProgram, camera, origin);
		string5.Draw(shaderProgram, camera, origin);
		string6.Draw(shaderProgram, camera, origin);
		string7.Draw(shaderProgram, camera, origin);
		string8.Draw(shaderProgram, camera, origin);

		cityBase1.Draw(shaderProgram, camera, origin);
		cityBase2.Draw(shaderProgram, camera, origin);

		conventionCentreBase.Draw(shaderProgram, camera, origin);
		// canOfBeans.Draw(shaderProgram, camera, origin);
		canOfBeansBase.Draw(shaderProgram, camera, origin);

		metal1.Draw(shaderProgram, camera, origin);
		metal2.Draw(shaderProgram, camera, origin);
		metal3.Draw(shaderProgram, camera, origin);
		metal4.Draw(shaderProgram, camera, origin);
		metal5.Draw(shaderProgram, camera, origin);
		metal6.Draw(shaderProgram, camera, origin);
		metal7.Draw(shaderProgram, camera, origin);
		metal8.Draw(shaderProgram, camera, origin);
		metal9.Draw(shaderProgram, camera, origin);
		metal10.Draw(shaderProgram, camera, origin);
		metal11.Draw(shaderProgram, camera, origin);
		metal12.Draw(shaderProgram, camera, origin);

		pole1.Draw(shaderProgram, camera, origin);
		pole2.Draw(shaderProgram, camera, origin);
		pole3.Draw(shaderProgram, camera, origin);
		wire1.Draw(shaderProgram, camera, origin);
		wire2.Draw(shaderProgram, camera, origin);
		wire3.Draw(shaderProgram, camera, origin);
		wire4.Draw(shaderProgram, camera, origin);
		wire5.Draw(shaderProgram, camera, origin);
		wire6.Draw(shaderProgram, camera, origin);
		wire7.Draw(shaderProgram, camera, origin);
		wire8.Draw(shaderProgram, camera, origin);
		wire9.Draw(shaderProgram, camera, origin);
		wire10.Draw(shaderProgram, camera, origin);
		

		// sun.Draw(shaderProgram, camera, sunObject.position, sunDirection);
		// moon.Draw(shaderProgram, camera, moonObject.position, moonDirection);

		// Draw Cacti
		for (auto& cactus : cacti) {
			cactus1.Draw(shaderProgram, camera, cactus.position);
		}

		// Update bird positions
		for (int i = 0; i < birds.size(); ++i) {
			birds[i].Update(1.0f / 60.0f, birdsCenterPoint, 0.5f);
		}

		// Draw birds
		for (auto& bird : birds) {
			birdModel.Draw(shaderProgram, camera, bird.position, bird.birdRotation);
		}

		// Draw Solar Panel
		float clampedX = glm::clamp(sunObject.position.x, -300.0f, 240.0f);
		spPanelRotation = glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, -clampedX * 0.20)));

		spPanel.Draw(shaderProgram, camera, origin, spPanelRotation);
		spStand.Draw(shaderProgram, camera, origin);
		spJoint.Draw(shaderProgram, camera, origin);

		spPanel2.Draw(shaderProgram, camera, origin, spPanelRotation);
		spStand2.Draw(shaderProgram, camera, origin);
		spJoint2.Draw(shaderProgram, camera, origin);
		
		spPanel3.Draw(shaderProgram, camera, origin, spPanelRotation);
		spStand3.Draw(shaderProgram, camera, origin);
		spJoint3.Draw(shaderProgram, camera, origin);
		
		spPanel4.Draw(shaderProgram, camera, origin, spPanelRotation);
		spStand4.Draw(shaderProgram, camera, origin);
		spJoint4.Draw(shaderProgram, camera, origin);

		// Lights
		light1.Draw(shaderProgram, camera, origin);
		light2.Draw(shaderProgram, camera, origin);
		light3.Draw(shaderProgram, camera, origin);
		light4.Draw(shaderProgram, camera, origin);

		// Buildings
		mcCannBase.Draw(shaderProgram, camera, origin);

		building3.Draw(shaderProgram, camera, origin);

		// Snake
		glm::vec3 snakeHeadScale = glm::vec3(1.3f, 1.3f, 1.3f);
		// Update tail rotation
		glm::vec3 tailToBodyDirection = glm::normalize(snakeBodyPosition - snakeTailPosition);
		glm::quat tailRotationQuaternion = glm::quatLookAt(tailToBodyDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		snakeTailRotation = tailRotationQuaternion;

		// Update body rotation
		glm::vec3 bodyToHeadDirection = glm::normalize(snakeHeadPosition - snakeBodyPosition);
		glm::quat bodyRotationQuaternion = glm::quatLookAt(bodyToHeadDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		snakeBodyRotation = bodyRotationQuaternion;

		// Update head position and rotation
		target = glm::vec3(snakeTargetX, 0.0f, snakeTargetZ);
		float distanceToTarget = glm::length(snakeHeadPosition - target);

		if (distanceToTarget < 0.5f) {
			snakeTargetX = glm::clamp(randf() * 90, -10.0f, 90.0f);
			snakeTargetZ = glm::clamp(randf() * 90, -10.0f, 90.0f);
		}

		float oscillationAmplitude = 0.01f;
		float oscillationFrequency = 1.5f;
		float phaseShift = 5.0f;

		// Head
		glm::vec3 directionToTarget = glm::normalize(target - snakeHeadPosition);

		// Add oscillation
		snakeHeadPosition.x += oscillationAmplitude * sin(glfwGetTime() * oscillationFrequency + phaseShift);
		snakeHeadRotation.z = glm::radians(oscillationAmplitude * sin(glfwGetTime() * oscillationFrequency * 2.0f + phaseShift));

		snakeHeadPosition += directionToTarget * 1.0f / 60.0f; // Move snake towards target
		
		glm::quat targetRotation = glm::quatLookAt(directionToTarget, glm::vec3(0.0f, 1.0f, 0.0f)); // Look towards target
		snakeHeadRotation = glm::slerp(snakeHeadRotation, targetRotation, 1.0f / 60.0f * 0.75f);
		snake.Draw(shaderProgram, camera, snakeHeadPosition, snakeHeadRotation, snakeHeadScale);

		// Body
		snakeBodyPosition = snakeHeadPosition - (directionToTarget * snakeOffset);
		bodyRotationQuaternion.x += 2.0f;
		snake.Draw(shaderProgram, camera, snakeBodyPosition, snakeBodyRotation); // Draw first body
		
		float currentPhaseShift = phaseShift;
		// Draw other body parts + update their positions
		for (int i = 0; i < numSnakeParts; i++) {
			bodyToHeadDirection = glm::normalize(snakeBodyPosition - (snakeBodyPosition - (directionToTarget * snakeOffset)));
			bodyRotationQuaternion = glm::quatLookAt(bodyToHeadDirection, glm::vec3(0.0f, 1.0f, 0.0f));
			snakeBodyRotation = glm::slerp(snakeBodyRotation, bodyRotationQuaternion, 1.0f / 60.0f * 0.50f);
			
			// Add oscillation to snakeBodyPosition.y and snakeBodyRotation.z
			snakeBodyPosition.x += oscillationAmplitude * sin(glfwGetTime() * oscillationFrequency + currentPhaseShift);
			snakeBodyRotation.z = glm::radians(oscillationAmplitude * sin(glfwGetTime() * oscillationFrequency * 2.0f + currentPhaseShift));

			snakeBodyPosition -= directionToTarget * snakeOffset;
			snake.Draw(shaderProgram, camera, snakeBodyPosition, snakeBodyRotation);
			currentPhaseShift += 5.0f;
		}

		// Tail
		snakeTailPosition = snakeBodyPosition - (directionToTarget * snakeOffset);
		snakeTailPosition.x += oscillationAmplitude * sin(glfwGetTime() * oscillationFrequency + currentPhaseShift);
		snakeTailRotation.z = glm::radians(oscillationAmplitude * sin(glfwGetTime() * oscillationFrequency * 2.0f + currentPhaseShift));

		snake.Draw(shaderProgram, camera, snakeTailPosition, snakeTailRotation);

		// Cubemap will always have a depth of 1.0
		glDepthFunc(GL_LEQUAL);

		// Skybox positioning
		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / screenHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draws the cubemap as the last object for performance
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		if (sunObject.position.y < 0) {
			glBindTexture(GL_TEXTURE_CUBE_MAP, secondCubemapTexture);
		}
		else {
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		}
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);

		// Add ImGui window
		ImGui::Begin("Light");
		//ImGui::Text("light3pos");
		//
		//ImGui::SliderFloat("X", &birdsCenterPoint.x, -100.0f, 100.0f);
		//ImGui::SliderFloat("Y", &birdsCenterPoint.y, -100.0f, 100.0f);
		//ImGui::SliderFloat("Z", &birdsCenterPoint.z, -100.0f, 100.0f);

		////ImGui::SliderFloat("X", &sunObject.position.x, -1000.0f, 1000.0f);
		////ImGui::SliderFloat("Y", &sunObject.position.y, -1000.0f, 1000.0f);
		////ImGui::SliderFloat("Z", &sunObject.position.z, -1000.0f, 1000.0f);

		//ImGui::Text("Camera");
		//ImGui::SliderFloat("X", &camera.Position.x, -1000.0f, 1000.0f);
		//ImGui::SliderFloat("Y", &camera.Position.y, -1000.0f, 1000.0f);
		//ImGui::SliderFloat("Z", &camera.Position.z, -1000.0f, 1000.0f);

		//ImGui::Text("Pitch");
		//ImGui::SliderFloat("Pitch", &currentPitch, 0.0f, 1.0f);

		//ImGui::Text("target position");
		//ImGui::SliderFloat("x", &target.x, -100.0f, 100.0f);
		//ImGui::SliderFloat("y", &target.y, -100.0f, 100.0f);
		//ImGui::SliderFloat("z", &target.z, -100.0f, 100.0f)

		ImGui::Text("Light3 Position");
		ImGui::SliderFloat("x", &lightPos3.x, -20.0f, 20.0f);
		ImGui::SliderFloat("y", &lightPos3.y, -20.0f, 20.0f);
		ImGui::SliderFloat("z", &lightPos3.z, -20.0f, 20.0f);

		ImGui::Text("Body Length");
		ImGui::SliderFloat("num", &numSnakeParts, 0.0f, 25.0f);

		ImGui::End();
		

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete all the objects created
	shaderProgram.Delete();
	specularProgram.Delete();
	skyboxShader.Delete();
	sunProgram.Delete();
	moonProgram.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}