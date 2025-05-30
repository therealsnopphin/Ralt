#include "pch.h"

sprite::sprite(std::string imagefilepath)
{
	loadimagergba(imagefilepath);
}

void sprite::loadimagergba(std::string imagefilepath)
{
	m_data = (int*)stbi_load(imagefilepath.c_str(), &m_width, &m_height, &m_channels, 0);
	m_filepath = imagefilepath;
}

void sprite::deleteimage()
{
	delete[] m_data;
}
