#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Read the shader to a string for the main program to use as shader source
std::string readShader(const char *filepath) {
	std::string content;
	std::ifstream fileStream(filepath, std::ios::in);
	if (!fileStream.is_open()) {
		std::cerr << "couldn't read the file" << filepath << " Not found" << std::endl;
		return "";
	}
	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}
