#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include<math.h>
#include"Model.h"

const unsigned int width = 800;
const unsigned int height = 800;
// Number of samples per pixel for MSAA
unsigned int samples = 8;

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
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

float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, samples);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "Darragh Nolan Lab 2"
	GLFWwindow* window = glfwCreateWindow(width, height, "Darragh Nolan Lab 2", NULL, NULL);
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
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	// Generates Shader objects
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader asteroidShader("asteroid.vert", "default.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	asteroidShader.Activate();
	glUniform4f(glGetUniformLocation(asteroidShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(asteroidShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Multisampling
	glEnable(GL_MULTISAMPLE);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 70.0f));

	// Load in models
	Model jupiter("models/jupiter/scene.gltf");
	//Model map("models/map/scene.gltf");
	Model base("models/rotatingSign/base.gltf");
	Model mid("models/rotatingSign/mid.gltf");
	Model top("models/rotatingSign/top.gltf");

	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;

	// Use this to disable VSync (not advized)
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


	// All the faces of the cubemap (make sure they are in this exact order)
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
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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


	// The number of asteroids to be created
	const unsigned int number = 5000;
	// Radius of circle around which asteroids orbit
	float radius = 100.0f;
	// How much ateroids deviate from the radius
	float radiusDeviation = 25.0f;

	// Holds all transformations for the asteroids
	std::vector <glm::mat4> instanceMatrix;

	for (unsigned int i = 0; i < number; i++)
	{
		// Generates x and y for the function x^2 + y^2 = radius^2 which is a circle
		float x = randf();
		float finalRadius = radius + randf() * radiusDeviation;
		float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

		// Holds transformations before multiplying them
		glm::vec3 tempTranslation;
		glm::quat tempRotation;
		glm::vec3 tempScale;

		// Makes the random distribution more even
		if (randf() > 0.5f)
		{
			// Generates a translation near a circle of radius "radius"
			tempTranslation = glm::vec3(y * finalRadius, randf(), x * finalRadius);
		}
		else
		{
			// Generates a translation near a circle of radius "radius"
			tempTranslation = glm::vec3(x * finalRadius, randf(), y * finalRadius);
		}
		// Generates random rotations
		tempRotation = glm::quat(1.0f, randf(), randf(), randf());
		// Generates random scales
		tempScale = 0.1f * glm::vec3(randf(), randf(), randf());


		// Initialize matrices
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);

		// Transform the matrices to their correct form
		trans = glm::translate(trans, tempTranslation);
		rot = glm::mat4_cast(tempRotation);
		sca = glm::scale(sca, tempScale);

		// Push matrix transformation
		instanceMatrix.push_back(trans * rot * sca);
	}
	// Create the asteroid model with instancing enabled
	Model asteroid("models/asteroid/scene.gltf", number, instanceMatrix);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 30.0)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "Darragh Nolan Lab 2 - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;

			// Use this if you have disabled VSync
			//camera.Inputs(window);
		}

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(375, 550));

		// Handles camera inputs (delete this if you have disabled VSync)
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);


		glm::vec3 jupiterPosition = glm::vec3(80.0, 0.0, 0.0);

		// Draw jupiter
		jupiter.Draw(shaderProgram, camera, jupiterPosition);
		// Draw the asteroids
		// asteroid.Draw(asteroidShader, camera);

		// Time-based animation parameters
		float time = glfwGetTime();
		float baseRotationSpeed = 20.0f;  // Speed of rotation for the base model
		float midRotationSpeed = -15.0f;  // Speed of rotation for the mid model
		float midVerticalMovementRange = 2.0f; // Range of vertical movement for the "mid" model
		float midVerticalMovementSpeed = 1.0f; // Speed of vertical movement

		// Calculate new positions and rotations based on time
		glm::vec3 basePosition = glm::vec3(0.0, 0.0, 0.0);
		glm::quat baseRotation = glm::angleAxis(glm::radians(time * baseRotationSpeed), glm::vec3(0.0, 1.0, 0.0));

		glm::vec3 midPosition = glm::vec3(0.0, midVerticalMovementRange * glm::sin(time * midVerticalMovementSpeed), 0.0);
		glm::quat midRotation = glm::angleAxis(glm::radians(time * midRotationSpeed), glm::vec3(0.0, 0.0, 1.0));

		// Call the Draw function for each model with the updated parameters
		base.Draw(shaderProgram, camera, basePosition, baseRotation);
		mid.Draw(shaderProgram, camera, midPosition, midRotation);

		
		// float topRotationSpeed = 30.0f;  // Speed of rotation for the top model around the y-axis
		// glm::vec3 topPosition = glm::vec3(0.0, glm::sin(time) * 2.0, 0.0);
		// glm::quat topRotation = glm::angleAxis(glm::radians(time * topRotationSpeed), glm::vec3(0.0, 1.0, 0.0));
		//top.Draw(shaderProgram, camera, topPosition, topRotation);


		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
		// The last row and column affect the translation of the skybox (which we don't want to affect)
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);

		ImGui::Begin("Hierarchical Information");
		ImGui::Text("Rotation for Base:");
		ImGui::Text("X: %f", glm::degrees(glm::eulerAngles(baseRotation)).x);
		ImGui::Text("Y: %f", glm::degrees(glm::eulerAngles(baseRotation)).y);
		ImGui::Text("Z: %f", glm::degrees(glm::eulerAngles(baseRotation)).z);

		ImGui::Text("Rotation for Top:");
		ImGui::Text("X: %f", glm::degrees(glm::eulerAngles(midRotation)).x);
		ImGui::Text("Y: %f", glm::degrees(glm::eulerAngles(midRotation)).y);
		ImGui::Text("Z: %f", glm::degrees(glm::eulerAngles(midRotation)).z);

		ImGui::Text("Translation for Base:");
		ImGui::Text("X: %f", basePosition.x);
		ImGui::Text("Y: %f", basePosition.y);
		ImGui::Text("Z: %f", basePosition.z);

		ImGui::Text("Translation for Top:");
		ImGui::Text("X: %f", midPosition.x);
		ImGui::Text("Y: %f", midPosition.y);
		ImGui::Text("Z: %f", midPosition.z);

		// Uniform Scaling
		float uniformScale = 1.0; // Assuming uniform scale
		ImGui::Text("Uniform Scaling:");
		ImGui::Text("Scale: %f", uniformScale);

		// Non-uniform Scaling (e.g., x, y, and z components of the scale)
		ImGui::Text("Non-uniform Scaling:");
		ImGui::Text("Scale X: %f", 1.0);
		ImGui::Text("Scale Y: %f", 1.0);
		ImGui::Text("Scale Z: %f", 1.0);

		// Combined Transformations
		glm::mat4 combinedTransformBase = glm::translate(glm::mat4(1.0f), basePosition) * glm::toMat4(baseRotation);
		ImGui::Text("Combined Transformations:");
		ImGui::Text("Translation: X: %f, Y: %f, Z: %f", basePosition.x, basePosition.y, basePosition.z);
		glm::vec3 combinedRotation = glm::degrees(glm::eulerAngles(baseRotation));
		ImGui::Text("Rotation: X: %f, Y: %f, Z: %f", combinedRotation.x, combinedRotation.y, combinedRotation.z);
		ImGui::Text("Scaling: X: %f, Y: %f, Z: %f", 1.0, 1.0, 1.0);

		glm::mat4 combinedTransformMid = glm::translate(glm::mat4(1.0f), midPosition) * glm::toMat4(midRotation);
		ImGui::Text("Combined Transformations:");
		ImGui::Text("Translation: X: %f, Y: %f, Z: %f", midPosition.x, midPosition.y, midPosition.z);
		glm::vec3 combinedRotationMid = glm::degrees(glm::eulerAngles(midRotation));
		ImGui::Text("Rotation: X: %f, Y: %f, Z: %f", combinedRotationMid.x, combinedRotationMid.y, combinedRotationMid.z);
		ImGui::Text("Scaling: X: %f, Y: %f, Z: %f", 1.0, 1.0, 1.0);

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

	// Delete all the objects we've created
	shaderProgram.Delete();
	skyboxShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}