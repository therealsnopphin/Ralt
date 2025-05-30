#pragma once

class playlayer : public Walnut::Layer
{
private:
	renderer m_renderer;
	player m_player;
	camera m_camera;
	world m_world;
	std::vector<enemy> m_enemys;
	SimpleUDPSocket m_client = { "34.141.136.27", 12345 };
	static inline std::vector<GamePacket> m_serverPlayers;
	float m_scale = 4;
	static inline float m_timestep = 0;

	FMOD::Sound* m_sound = nullptr;
	FMOD::System* m_system = nullptr;
	FMOD::Channel* m_channel = nullptr;
public:
	virtual void OnAttach() override;

	virtual void OnUpdate(float timestep) override;
	virtual void OnUIRender() override;

	static inline bool scene_playlayer = false;
};