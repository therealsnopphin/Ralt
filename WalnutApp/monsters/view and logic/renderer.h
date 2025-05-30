#pragma once

class renderer
{
public:
	void drawplayer(const player& Player, const camera& camera);
	void drawinventory(FT_Library& ftLibrary, FT_Face& ftFace, const camera& Camera, const inventory& inventory);
	void drawtext(FT_Library& ftLibrary, int xcoords, int ycoords,FT_Face& ftFace, std::string text);
	void drawplayername(FT_Library& ftLibrary, FT_Face& ftFace, const camera& Camera, const player& Player, std::string name);
	void drawplayercurrentitem(const item& item, const player& player, const camera& camera);
	void drawworld(const world& World, const camera& camera, const player* Player);
	void drawenemys(std::vector<enemy>& enemys, const camera& camera);
	void drawbackground(int color);

	void update();

	void resize(int width, int height);
private:
	void drawsprite(sprite& Sprite, int x, int y);
	void drawcolor(int color, int x, int y);

public:

	int* m_data = nullptr;
	int m_width = 0, m_height = 0;

	Walnut::Image m_vulkanrenderer = { 0, 0, Walnut::ImageFormat::RGBA, nullptr};
};