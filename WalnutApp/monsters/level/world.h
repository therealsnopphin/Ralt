#pragma once



class world
{
public:
	world();

	void createworld();
public:
	float m_darkness_scale = 1;

	int* m_data = nullptr;
	//sprite m_sprite = { "Resource/3.png" };

	const int m_sizeTile =20;
	const int m_width = 256 * m_sizeTile, m_height = 256 * m_sizeTile;

	bool m_cave = false;
};