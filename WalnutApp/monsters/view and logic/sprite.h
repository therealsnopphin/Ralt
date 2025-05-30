#pragma once

class sprite
{
public:
	sprite() = default;
	sprite(std::string imagefilepath);

	void loadimagergba(std::string imagefilepath);

	void deleteimage();
	std::string m_filepath;

	int* m_data = nullptr;
	int m_width = 0, m_height = 0;
	int m_channels = 0;
};