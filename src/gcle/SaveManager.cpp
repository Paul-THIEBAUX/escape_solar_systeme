#include "SaveManager.h"

void SaveManager::OpenFile(const std::string& path)
{
	std::ifstream jsonFile(path);

	if (!jsonFile.is_open())
	{
		std::cout << "Can't open file " << std::endl;
		return;
	}

	jsonFile >> jsonData;
	jsonFile.close();
	fileOpen = true;
}

void SaveManager::SaveInFile(const std::string& path)
{
	std::ofstream jsonFile(path);
	if (!jsonFile.is_open())
	{
		std::cout << "Can't open file " << std::endl;
		return;
	}

	jsonFile << jsonData.dump(4);
	jsonFile.close();
}
