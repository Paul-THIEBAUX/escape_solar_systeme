#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class SaveManager
{
private:

	json jsonData;
	bool fileOpen = false;

public:

	static SaveManager& GetInstance()
	{
		static SaveManager instance;
		return instance;
	}

	bool IsFileOpen() { return fileOpen; };

	void OpenFile(const std::string& path);
	void SaveInFile(const std::string& path);

	json& GetJson() { return jsonData; };
};