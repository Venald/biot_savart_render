#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
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
