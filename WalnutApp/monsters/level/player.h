#pragma once

namespace items
{
	static item s_grass = { "Resource/grass.png",1 };
	static item s_diamond = { "Resource/diamond.png", 1 };
	static item s_leave = { "Resource/leave.png", 1 };
	static item s_tree = { "Resource/tree.png", 1 };

	static item s_pickaxe = { "Resource/pickaxe.png", 1 };

	inline void Networksyncitemornot(item& src, const std::string & filePath, item& dest)
	{
		if (src.m_sprite.m_filepath == filePath)
		{
			dest = src;
		}
	}
}

class renderer;
class enemy;

class player
{
public:
	void updatenetwork(SimpleUDPSocket& client);
	bool iscollisionoritem(renderer& Renderer, ImGuiKey key, camera Camera, world& World, float x, float y, std::vector<enemy>& enemys);
	void placecurrentblock(int tileX, int tileY, const item& item, const world& world);
	void updatekeyboard(float timestep, renderer& Renderer, std::vector<enemy>& enemys, world& World, camera& Camera);
private:
	void updatebounds();
public:
	bool m_isplayersleeping = false;
	float m_bedsleeping = 0;

	sprite m_sprite = { "Resource/play.png" };

	float m_xpos = 0, m_ypos = 0;

	int hp = 100;
	inventory m_inventory;
	std::vector<BlockPacket> m_changedBlocks;
	bool m_writing_chat_enabled = false;
	bool m_writing_chat_time = false;
	std::string m_chat_message = ""; //Gets erased after typing and then 5 seconds erase
};