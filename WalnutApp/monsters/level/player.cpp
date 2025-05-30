#include "pch.h"


void player::updatenetwork(SimpleUDPSocket& client)
{
    client.sendPlayerPosition({ menulayer::s_playername, m_xpos, m_ypos, m_inventory.m_items[m_inventory.m_currentitem].m_sprite.m_filepath, m_chat_message, m_changedBlocks });
}

bool player::iscollisionoritem(renderer& Renderer, ImGuiKey key, camera Camera, world& World, float x, float y, std::vector<enemy>& enemys)
{
    Camera.setviewport(World.m_width, World.m_height, Renderer.m_width, Renderer.m_height, x, y);
    Renderer.drawworld(World, Camera, this);

    // Calculate the offset once for the entire sprite
    int cameraOffsetX = x - Camera.m_xposition;
    int cameraOffsetY = y - Camera.m_yposition;

    // Define the four corners of the sprite in screen space
    int corners[4][2] = {
        {cameraOffsetX, cameraOffsetY},                               // Top-left
        {cameraOffsetX + m_sprite.m_width - 1, cameraOffsetY},        // Top-right
        {cameraOffsetX, cameraOffsetY + m_sprite.m_height - 1},       // Bottom-left
        {cameraOffsetX + m_sprite.m_width - 1, cameraOffsetY + m_sprite.m_height - 1} // Bottom-right
    };

    // Define the four corners of the sprite in screen space
    int Worldcorners[4][2] = {
        {x, y},                               // Top-left
        {x + m_sprite.m_width - 1, y},        // Top-right
        {x, y + m_sprite.m_height - 1},       // Bottom-left
        {x + m_sprite.m_width - 1, y + m_sprite.m_height - 1} // Bottom-right
    };

    // Choose specific corners based on the key
    int indicesToCheck[2];
    int indicesCount = 0;

    switch (key)
    {
    case ImGuiKey_W: // Check the top corners
        indicesToCheck[0] = 0; // Top-left
        indicesToCheck[1] = 1; // Top-right
        indicesCount = 2;
        break;
    case ImGuiKey_S: // Check the bottom corners
        indicesToCheck[0] = 2; // Bottom-left
        indicesToCheck[1] = 3; // Bottom-right
        indicesCount = 2;
        break;
    case ImGuiKey_A: // Check the left corners
        indicesToCheck[0] = 0; // Top-left
        indicesToCheck[1] = 2; // Bottom-left
        indicesCount = 2;
        break;
    case ImGuiKey_D: // Check the right corners
        indicesToCheck[0] = 1; // Top-right
        indicesToCheck[1] = 3; // Bottom-right
        indicesCount = 2;
        break;
    }

    // Check only the relevant corners
    for (int i = 0; i < indicesCount; i++) {
        int destx = corners[indicesToCheck[i]][0];
        int desty = corners[indicesToCheck[i]][1];
        int worlddestx = (Worldcorners[indicesToCheck[i]][0] / World.m_sizeTile) * World.m_sizeTile;
        int worlddesty = (Worldcorners[indicesToCheck[i]][1] / World.m_sizeTile) * World.m_sizeTile;

        // Ensure the corner is within screen bounds
        if (destx >= 0 && destx < Renderer.m_width && desty >= 0 && desty < Renderer.m_height) {
#define CheckIfPlayerhaspickaxeanditcanbreakablock World.m_data[worlddesty * World.m_width + worlddestx] == -15063505 || World.m_data[worlddesty * World.m_width + worlddestx] == -10625393 && m_inventory.m_items[m_inventory.m_currentitem].m_sprite.m_filepath.find("pickaxe.png") != std::string::npos
#define CheckIfplayerisatcave \
World.m_data[worlddesty * World.m_width + worlddestx] == -16711423
#define CheckIfplayerisnotatcave World.m_data[worlddesty * World.m_width + worlddestx] == -13949659

#define CheckIfplayercollides \
    World.m_data[worlddesty * World.m_width + worlddestx] == -15063505 || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -10625393 || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -22963     || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -5547189   || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -16727297  || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -13548733  || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -14468804  || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -13228249  || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -16777216 || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -11582653 || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -1835162 || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -15063505 || \
     World.m_data[worlddesty * World.m_width + worlddestx] == -10625393 

            if (m_inventory.m_items[m_inventory.m_currentitem].m_sprite.m_filepath.find("pickaxe.png") != std::string::npos)
            {
                switch (World.m_data[worlddesty * World.m_width + worlddestx])
                {
                case -15063505:
                {
                    m_inventory.additem(items::s_tree);
                    placecurrentblock(worlddestx, worlddesty, items::s_grass, World);
                    break;
                }
                case -10625393:
                {
                    m_inventory.additem(items::s_leave);
                    placecurrentblock(worlddestx, worlddesty, items::s_grass, World);
                    break;
                }
                case -1835162:
                    m_inventory.additem(items::s_diamond);
                    placecurrentblock(worlddestx, worlddesty, items::s_grass, World);
                }
            }

            if (CheckIfplayerisatcave)
            {
            }
            else if (CheckIfplayerisnotatcave)
            {
            }
            // Check for collision at the corner
            if (CheckIfplayercollides)
            {
                return true;
            }
        }
    }

    return false;
}

void player::placecurrentblock(int tileX, int tileY, const item& item, const world& world)
{
    for (int y = 0; y < world.m_sizeTile; y++)
    {
        for (int x = 0; x < world.m_sizeTile; x++)
        {
            world.m_data[(y + tileY) * world.m_width + (x + tileX)] = item.m_sprite.m_data[y * item.m_sprite.m_width + x];
            //The sprite will be one color, so no difference
        }
    }

    int centerIndex = 1 * item.m_sprite.m_width + 1;
    m_changedBlocks.push_back({ tileX, tileY, item.m_sprite.m_data[centerIndex] });

    m_inventory.destoryitem(item);
}



void player::updatekeyboard(float timestep, renderer& Renderer, std::vector<enemy>& enemys, world& World, camera& Camera) {
    if (m_writing_chat_time)
    {
        static float time;
        time += timestep;

        if (time > 5.0f)
        {
            m_chat_message = "";
            m_writing_chat_time = false;
            time = 0;
        }
    }

    if (ImGui::IsKeyDown(ImGuiKey_T))
    {
        m_writing_chat_enabled = true;
        return;
    }

    if (ImGui::IsKeyDown(ImGuiKey_Enter) && m_writing_chat_enabled)
    {
        m_writing_chat_enabled = false;
        m_writing_chat_time = true;
        return;
    }

    if (m_writing_chat_enabled)
    {
        ImGuiIO& io = ImGui::GetIO();

        for (int i = 0; i < io.InputQueueCharacters.Size; i++)
        {
            ImWchar c = io.InputQueueCharacters[i];

            //if (c >= 32 && c < 127) // Printable ASCII characters
            //{
                m_chat_message += static_cast<char>(c);
            //}
            if (c == 8 && !m_chat_message.empty()) // Backspace
            {
                m_chat_message.pop_back();
            }
        }
        io.InputQueueCharacters.resize(0); // Clear queue after processing
        return;
    }

    float proposedY = m_ypos;
    float proposedX = m_xpos;
    float movementSpeed = 350;

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        movementSpeed += 2000;

    auto attempt_move = [&](ImGuiKey key, float& pos, float delta, bool vertical) {
        float testX = vertical ? proposedX : proposedX + delta;
        float testY = vertical ? proposedY + delta : proposedY;

        if (!iscollisionoritem(Renderer, key, Camera, World, testX, testY, enemys)) {
            pos += delta;
        }
        };

    if (ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_UpArrow))
        attempt_move(ImGuiKey_W, m_ypos, -movementSpeed * timestep, true);
    if (ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_DownArrow))
        attempt_move(ImGuiKey_S, m_ypos, movementSpeed * timestep, true);
    if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_LeftArrow))
        attempt_move(ImGuiKey_A, m_xpos, -movementSpeed * timestep, false);
    if (ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_RightArrow))
        attempt_move(ImGuiKey_D, m_xpos, movementSpeed * timestep, false);

    updatebounds();

    if (ImGui::IsKeyDown(ImGuiKey_1))
    {
        m_inventory.m_currentitem = 0;
    }
    if (ImGui::IsKeyDown(ImGuiKey_2))
    {
        m_inventory.m_currentitem = 1;
    }
    if (ImGui::IsKeyDown(ImGuiKey_3))
    {
        m_inventory.m_currentitem = 2;
    }

    if (ImGui::IsKeyDown(ImGuiKey_Space) && m_inventory.m_items[m_inventory.m_currentitem].m_sprite.m_filepath.find("pickaxe.png") == std::string::npos)
    {
        placecurrentblock(m_xpos, m_ypos, m_inventory.m_items[m_inventory.m_currentitem], World);
    }
}

void player::updatebounds()
{
    if (m_xpos < 0)
    {
        m_xpos = 0;
    }
    if (m_xpos > 5120)
    {
        m_xpos = 5120;
    }
    if (m_ypos > 5120)
    {
        m_ypos = 5120;
    }
    if (m_ypos < 0)
    {
        m_ypos = 0;
    }
}