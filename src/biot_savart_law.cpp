// biot_svart_shader.cpp : Defines the entry point for the console application.
//

#include "biot_savart_law.h"
#include "Shaderloader.h"

// Buffer Struct for the data for GPU
struct BufferData {
	glm::vec4 vector4;
};

float scale_to(float x, float min, float max, float a, float b) {
	return (b - a)*(x - min) / (max - min) + a;
}

GLuint compile_compute_shader(void) {
	std::ofstream debug_file;
	debug_file.open("debug_file.txt");
	GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	GLuint program;

	// Load sources
	std::string compute_shader_str = readShader("line_integral.comp");

	//converts sources to const char* strings
	const char *compute_shader_src = compute_shader_str.c_str();

	// Attach sources
	glShaderSource(compute_shader, 1, &compute_shader_src, NULL);

	// Compile
	glCompileShader(compute_shader);
	GLint isCompiled = 0;
	glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(compute_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(compute_shader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		debug_file << "Error in compute shader" << std::endl;
		for (size_t i = 0; i < errorLog.size(); i++)
		{
			debug_file << errorLog[i];
		}
		debug_file << std::endl;
		// Exit with failure.
		glDeleteShader(compute_shader); // Don't leak the shader.
		debug_file.close();
		return NULL;
	}
	program = glCreateProgram();

	//Attach shaders
	glAttachShader(program, compute_shader);

	// Link the program
	glLinkProgram(program);

	// Delete the shader so it doesn't leak
	glDeleteShader(compute_shader);
	debug_file.close();
	return program;
}

// Compile the shaders for drawing the result
GLuint compile_drawing_shaders(void) {
	std::ofstream debug_file;
	debug_file.open("debug_file.txt");
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint program;

	// Load sources
	std::string frag_shader_str = readShader("frag_shader.frag");
	std::string vert_shader_str = readShader("vert_shader.vert");

	//converts sources to const char* strings
	const char *frag_shader_src = frag_shader_str.c_str();
	const char *vert_shader_srt = vert_shader_str.c_str();

	// Attach sources
	glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
	glShaderSource(vert_shader, 1, &vert_shader_srt, NULL);

	// Compile fragment and vertex shaders
	glCompileShader(frag_shader);
	GLint is_compiled = 0;
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_FALSE) {
		GLint max_length = 0;
		glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &max_length);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(max_length);
		glGetShaderInfoLog(frag_shader, max_length, &max_length, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		debug_file << "Error in fragment shader" << std::endl;
		for (size_t i = 0; i < errorLog.size(); i++)
		{
			debug_file << errorLog[i];
		}
		debug_file << std::endl;
		// Exit with failure.
		glDeleteShader(frag_shader); // Don't leak the shader.
		debug_file.close();
		return NULL;
	}

	glCompileShader(vert_shader);
	is_compiled = 0;
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(vert_shader, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		debug_file << "Error in vertex shader" << std::endl;
		for (size_t i = 0; i < errorLog.size(); i++)
		{
			debug_file << errorLog[i];
		}
		debug_file << std::endl;
		// Exit with failure.
		glDeleteShader(vert_shader); // Don't leak the shader.
		debug_file.close();
		return NULL;
	}
	program = glCreateProgram();

	//Attach shaders
	glAttachShader(program, frag_shader);
	glAttachShader(program, vert_shader);

	// Link the program
	glLinkProgram(program);

	// Delete the shaders so they don't leak
	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);

	debug_file.close();
	return program;
}

int main()
{

	const int NUM_DOTS_H = 800;
	const int NUM_DOTS_V = 800;
	const int NUM_DOTS = NUM_DOTS_H * NUM_DOTS_V;
	const float float_max = std::numeric_limits<float>::max();
	const float float_min = std::numeric_limits<float>::min();
	bool exit = false;
	bool sRGB = false;
	float m_pi = 3.14159265359f;

	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;
	contextSettings.sRgbCapable = sRGB;
	sf::RenderWindow window(sf::VideoMode(800, 800), "Biot-Savart's law",
		sf::Style::Default, contextSettings);
	glewInit();
	window.setVerticalSyncEnabled(true);


	GLuint compute_program;
	GLuint shader_program;
	std::vector<BufferData> bufferPosition(NUM_DOTS);
	std::vector<BufferData> bufferColor(NUM_DOTS);
	std::ofstream outfile;

	bool fileSave = false;
	if (fileSave == true) {
		outfile.open("outputdata.dat");
	}
	// Create the buffers and vao
	GLuint buffers[2];
	GLuint vao;
	sf::Clock clock;
	while (!exit) {
		sf::Vector2u wsize = window.getSize();
		
		// Set the initial data for the array
		for (size_t i = 0; i < NUM_DOTS_H; i++)
		{
			for (size_t j = 0; j < NUM_DOTS_V; j++)
			{
				int offset = i * NUM_DOTS_H + j;
				// B
				bufferColor.at(offset).vector4.x = 0.0f;
				bufferColor.at(offset).vector4.y = 0.0f;
				bufferColor.at(offset).vector4.z = 0.0f;
				bufferColor.at(offset).vector4.w = 0.0f;
				// X
				bufferPosition.at(offset).vector4.x = scale_to(i * 1.0f, 0, NUM_DOTS_H, -1, 1);
				// Y
				bufferPosition.at(offset).vector4.y = scale_to(j * 1.0f, 0, NUM_DOTS_V, -1, 1);
				// Z
				bufferPosition.at(offset).vector4.z = scale_to(i * 1.0f, 0, NUM_DOTS_H, -1, 1);
				// W
				bufferPosition.at(offset).vector4.w = 1.0f;

			}

		}

		// Create vao and two buffers one for position data and second one for the intesitiy of magnetic field
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(2, buffers);

		// Compile required shaders
		compute_program = compile_compute_shader();
		shader_program = compile_drawing_shaders();

		sf::Time running_time;
		sf::Clock frame_clock;
		sf::Time frame_time_start;
		while (window.isOpen()) {
			sf::Event event;
			frame_time_start = frame_clock.getElapsedTime();
			running_time = clock.getElapsedTime();
			while (window.pollEvent(event)) {
				// Close window on exit
				if (event.type == sf::Event::Closed) {
					exit = true;
					window.close();
				}
				// Escape key: exit
				if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				{
					exit = true;
					window.close();
				}

				// Adjust the viewport when the window is resized
				if (event.type == sf::Event::Resized)
				{
					glViewport(0, 0, event.size.width, event.size.height);
				}
			}


			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
			glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);

			// Bind the buffers to be used at compute shader and buffer the data in to them
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[0]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, bufferPosition.size() * sizeof(BufferData), bufferPosition.data(), GL_STATIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[0]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, bufferColor.size() * sizeof(BufferData), bufferColor.data(), GL_STATIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[1]);




			// Run the compute shaders
			glUseProgram(compute_program);

			GLint ilocI = glGetUniformLocation(compute_program, "I");
			GLint ilocRadius = glGetUniformLocation(compute_program, "Radius");
			GLint ilocmu0 = glGetUniformLocation(compute_program, "mu0");
			GLint ilocpi = glGetUniformLocation(compute_program, "pi");
			GLint ilocZpos = glGetUniformLocation(compute_program, "zloc");
			glUniform1f(ilocI, 1.0f);
			glUniform1f(ilocRadius, 0.2f);
			glUniform1f(ilocmu0, 1);
			glUniform1f(ilocpi, m_pi);
			float z_point = 0.0f;
			//float z_point = 1 + (2.0f / m_pi * 1.0f * asin(sin(2.0f * m_pi / (10.0f)*running_time.asSeconds())));
			std::cout << "Z-pos: " << z_point << std::endl;
			glUniform1f(ilocZpos, z_point);

			// Dispatch the computation
			glDispatchCompute(NUM_DOTS / 32, 1, 1);
			glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

			// Find the max and min values of the color for later normalization
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1]);
			GLfloat *ptr_to_data;
			ptr_to_data = (GLfloat *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			float b_x_max = float_min;
			float b_x_min = float_max;
			float b_y_max = float_min;
			float b_y_min = float_max;
			float b_z_max = float_min;
			float b_z_min = float_max;

			// Find the min,max for each component for color scaling in the rendering of final image
			for (size_t i = 0; i < NUM_DOTS * 4; i += 4) {
				if (fileSave == true) {
					outfile << ptr_to_data[i] << " " << ptr_to_data[i + 1] << " " << ptr_to_data[i + 2] << std::endl;
				}
				if (b_x_max < ptr_to_data[i]) {
					b_x_max = ptr_to_data[i];
				}
				if (b_x_min > ptr_to_data[i]) {
					b_x_min = ptr_to_data[i];
				}
				if (b_y_max < ptr_to_data[i + 1]) {
					b_y_max = ptr_to_data[i + 1];
				}
				if (b_y_min > ptr_to_data[i + 1]) {
					b_y_min = ptr_to_data[i + 1];
				}
				if (b_z_max < ptr_to_data[i + 2]) {
					b_z_max = ptr_to_data[i + 2];
				}
				if (b_z_min > ptr_to_data[i + 2]) {
					b_z_min = ptr_to_data[i + 2];
				}
			}
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);





			// Run the shader program and draw the points
			glUseProgram(shader_program);
			// Get the shader's paramters and assign values
			GLint iloc_x = glGetUniformLocation(shader_program, "b_x_min_max");
			GLint iloc_y = glGetUniformLocation(shader_program, "b_y_min_max");
			GLint iloc_z = glGetUniformLocation(shader_program, "b_z_min_max");
			glUniform2f(iloc_x, b_x_min, b_x_max);
			glUniform2f(iloc_y, b_y_min, b_y_max);
			glUniform2f(iloc_z, b_z_min, b_z_max);

			glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glPointSize(1);

			glClearColor(0.0f, 0.25f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glClear(GL_DEPTH_BUFFER_BIT);

			glDrawArrays(GL_POINTS, 0, NUM_DOTS);
			float frame_time = (-frame_time_start.asSeconds() + frame_clock.getElapsedTime().asSeconds());
			std::cout << "Frame time = " << frame_time << std::endl;
			window.display();
			//clock.restart();
			if (fileSave == true) {
				fileSave = false;
			}
		}
		outfile.close();
		return 0;
	}

}

