#pragma once
class camera
{
public:
	camera() = default;
	camera(int worldwidth, int worldheight, int rendererwidth, int rendererheight, float playerxpos, float playerypos);

	void setviewport(int worldwidth, int worldheight, int rendererwidth, int rendererheight, float playerxpos, float playerypos);

	float m_lerp = 0.2f;
	float m_xposition = 0, m_yposition = 0;
};

