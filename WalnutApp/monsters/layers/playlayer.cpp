#include "pch.h"

void playlayer::OnAttach()
{
	FMOD::System_Create(&m_system);
	m_system->init(512, FMOD_INIT_NORMAL, nullptr);
	m_system->createSound("Resource/as.mp3", FMOD_DEFAULT, nullptr, &m_sound);
	m_system->playSound(m_sound, nullptr, false, &m_channel);

	if (!m_client.start()) {
		std::cerr << "Failed to start client\n";
	}

	m_client.setReceiveCallback([](std::vector<GamePacket>&& players) {
		m_serverPlayers = std::move(players);
		});

	m_enemys.resize(0);

	m_player.m_inventory.additem(items::s_pickaxe);
}

void playlayer::OnUpdate(float timestep)
{
	m_timestep = timestep;

	if (!menulayer::scene_menulayer && playlayer::scene_playlayer)
	{
		for (auto& enemy : m_enemys)
		{
			enemy.attack(timestep, m_player, m_camera);
		}

		if (!m_player.m_isplayersleeping) //black s
		{
			m_camera.setviewport(m_world.m_width, m_world.m_height, m_renderer.m_width, m_renderer.m_height, m_player.m_xpos, m_player.m_ypos);
			m_renderer.drawworld(m_world, m_camera, &m_player);

			m_player.updatenetwork(m_client);

			int playerCount = 0;

			for (auto& network_playerinfo : m_serverPlayers) {
				player network_player;
				network_player.m_xpos = network_playerinfo.x;
				network_player.m_ypos = network_playerinfo.y;
				
				for (auto& block : network_playerinfo.m_changedblocks)
				{
					for (int y = 0; y < m_world.m_sizeTile; y++)
					{
						for (int x = 0; x < m_world.m_sizeTile; x++)
						{
							m_world.m_data[(block.m_ypos + y) * m_world.m_width + (block.m_xpos + x)] = block.m_color;
						}
					}
				}

				m_renderer.drawplayer(network_player, m_camera);
				m_renderer.drawplayername(menulayer::s_currentft, menulayer::s_currentface, m_camera, network_player, network_playerinfo.name);
				item network_item;

				//All items check if render current player item
				items::Networksyncitemornot(items::s_diamond, network_playerinfo.currentblock_name, network_item);
				items::Networksyncitemornot(items::s_grass, network_playerinfo.currentblock_name, network_item);
				items::Networksyncitemornot(items::s_leave, network_playerinfo.currentblock_name, network_item);
				items::Networksyncitemornot(items::s_pickaxe, network_playerinfo.currentblock_name, network_item);
				items::Networksyncitemornot(items::s_tree, network_playerinfo.currentblock_name, network_item);


				m_renderer.drawplayercurrentitem(network_item, network_player, m_camera);

				if (network_playerinfo.chat_message != "")
				{
					m_renderer.drawtext(menulayer::s_currentft, 0, playerCount * 16, menulayer::s_currentface, "Player " + network_playerinfo.name + ": " + network_playerinfo.chat_message);
					playerCount++;
				}
			}
			
			m_renderer.drawinventory(menulayer::s_currentft, menulayer::s_currentface, m_camera, m_player.m_inventory);

			m_renderer.drawenemys(m_enemys, m_camera);
			
			//m_renderer.drawtext(menulayer::s_currentft, m_renderer.m_width / 2 / 16, m_renderer.m_height / 2 / 16, menulayer::s_currentface, "hp: " + std::to_string(m_player.hp));
			
			/*
			if (m_granny.Hasspawned)
			{
				m_renderer.drawtext(menulayer::s_currentft, m_renderer.m_width / 2 / 16, m_renderer.m_height / 2 / 16, menulayer::s_currentface, "Granny: let´s eat??? Im going to kitchen");
				m_granny.gokitchen(m_camera, m_world, timestep, m_renderer);
			}
			*/
		}

		m_renderer.update();
		m_system->update();

		//death hp < 1
		if (m_player.hp < 1)
		{ //then exit the APlication
			exit(0);
		}
	}
}

void playlayer::OnUIRender()
{
	if (!menulayer::scene_menulayer && playlayer::scene_playlayer)
	{
		// Get the size of the viewport (entire window area)
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 windowPos = viewport->Pos;
		ImVec2 windowSize = viewport->Size;

		// Set the next window position and size
		ImGui::SetNextWindowPos(windowPos);
		ImGui::SetNextWindowSize(windowSize);

		ImGui::Begin("playlayer:", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |         // Removes scrollbar
			ImGuiWindowFlags_NoScrollWithMouse |   // Disables scrolling with mouse
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking);
		//Imgui keyboard
		m_player.updatekeyboard(m_timestep, m_renderer, m_enemys, m_world, m_camera);

		if (m_renderer.m_width != int(windowSize.x / m_scale) || m_renderer.m_height != int(windowSize.y / m_scale))
		{
			m_renderer.resize((windowSize.x / m_scale), (windowSize.y / m_scale));
			
		}

		ImGui::Image(m_renderer.m_vulkanrenderer.m_DescriptorSet, { (float)m_renderer.m_width * m_scale, (float)m_renderer.m_height * m_scale });
		ImGui::End();;;
	}
}
