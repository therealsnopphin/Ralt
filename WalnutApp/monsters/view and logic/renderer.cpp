#include "pch.h"

struct Color {
    int ToPackedInt() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    unsigned char a, r, g, b;
};


Color IntToColor(int packedColor) {
    return {
        static_cast<unsigned char>((packedColor >> 24) & 0xFF), // Alpha
        static_cast<unsigned char>((packedColor >> 16) & 0xFF), // Red
        static_cast<unsigned char>((packedColor >> 8) & 0xFF),  // Green
        static_cast<unsigned char>(packedColor & 0xFF)          // Blue
    };
}

std::string ToInventoryName(const std::string& path) {
    std::string result = path;

    // Remove "Resources/" prefix
    const std::string prefix = "Resource/";
    if (result.rfind(prefix, 0) == 0) { // starts with
        result = result.substr(prefix.length());
    }

    // Remove ".png" suffix
    const std::string suffix = ".png";
    if (result.size() >= suffix.size() &&
        result.compare(result.size() - suffix.size(), suffix.size(), suffix) == 0) {
        result = result.substr(0, result.size() - suffix.size());
    }

    return result;
}

void renderGlyphToIntBuffer(
    FT_Library& ftLibrary,
    FT_Face& ftFace,
    char character,
    int xcoord,
    int ycoord,
    int* renderbuffer,
    int renderwidth,
    int renderheight,
    int* rendererData)
{
    if (FT_Load_Char(ftFace, character, FT_LOAD_RENDER)) {
        std::cerr << "Could not load character '" << character << "'" << std::endl;
        return;
    }

    FT_Bitmap& bitmap = ftFace->glyph->bitmap;
    int font_width = bitmap.width;
    int font_height = bitmap.rows;

    for (int y = 0; y < font_height; ++y) {
        int drawY = ycoord + y;
        if (drawY < 0 || drawY >= renderheight)
            continue; // Skip out-of-bounds Y rows

        for (int x = 0; x < font_width; ++x) {
            int drawX = xcoord + x;

            if (drawX < 0 || drawX >= renderwidth) {
                continue; // Skip out-of-bounds X columns
            }

            unsigned char pixel = bitmap.buffer[y * bitmap.pitch + x];
            int bufferIndex = drawY * renderwidth + drawX;

            if (pixel > 0) {
                renderbuffer[bufferIndex] = (pixel << 24) | (pixel << 16) | (pixel << 8) | 255;
            }
            else {
                renderbuffer[bufferIndex] = rendererData[bufferIndex];
            }
        }
    }
}


Color ApplyLighting(Color baseColor, double darknessFactor, double distance, double maxDistance) {
    double intensity = 1.0 - min(distance / maxDistance, 1.0);

    // Blend base color with light color based on intensity
    Color result;
    result.r = static_cast<int>(baseColor.r * darknessFactor * (1 - intensity) + baseColor.r * intensity);
    result.g = static_cast<int>(baseColor.g * darknessFactor * (1 - intensity) + baseColor.g * intensity);
    result.b = static_cast<int>(baseColor.b * darknessFactor * (1 - intensity) + baseColor.b * intensity);

    return result;
}

void renderer::drawplayer(const player& Player, const camera& camera)
{
    int cameraOffsetX = Player.m_xpos - camera.m_xposition;
    int cameraOffsetY = Player.m_ypos - camera.m_yposition;

    // Clip the sprite to the screen's boundaries (to avoid checking every pixel)
    int startX = max(0, -cameraOffsetX); // Start clipping at the left edge
    int startY = max(0, -cameraOffsetY); // Start clipping at the top edge
    int endX = min(Player.m_sprite.m_width, m_width - cameraOffsetX); // Clip at the right edge
    int endY = min(Player.m_sprite.m_height, m_height - cameraOffsetY); // Clip at the bottom edge

    // Loop through the clipped sprite region
    for (int spritey = startY; spritey < endY; spritey++) // Iterate over sprite's height
    {
        for (int spritex = startX; spritex < endX; spritex++) // Iterate over sprite's width
        {
            int destx = cameraOffsetX + spritex;
            int desty = cameraOffsetY + spritey;

            // Get the color of the pixel from the sprite at (spritex, spritey)
            int pixelData = Player.m_sprite.m_data[spritey * Player.m_sprite.m_width + spritex];

            // Set the pixel at the destination coordinates on the screen
            if (pixelData != 0) // Only set non-transparent pixels
            {
                m_data[desty * m_width + destx] = pixelData;
            }
        }
    }
}

void renderer::drawinventory(FT_Library& ftLibrary, FT_Face& ftFace, const camera& Camera, const inventory& inventory)
{
    const int slotspacing = 60;

    for (int i = 0; i < inventory.m_maxiumitems; ++i)
    {
        const auto& item = inventory.m_items[i];
        const auto& sprite = item.m_sprite;

        int sprite_start_x = m_width / 2 + (i * slotspacing) - 48;
        int sprite_start_y = m_height - 48;

        drawtext(ftLibrary, sprite_start_x, sprite_start_y - 12, ftFace, std::to_string(item.amount));
        drawtext(ftLibrary, sprite_start_x, sprite_start_y + 36, ftFace, ToInventoryName(sprite.m_filepath));

        for (int y = 0; y < sprite.m_height; ++y)
        {
            int screen_y = sprite_start_y + y;
            if (screen_y >= m_height)
                continue;

            for (int x = 0; x < sprite.m_width; ++x)
            {
                int screen_x = sprite_start_x + x;
                if (screen_x >= m_width)
                    continue;

                int itemdest = y * sprite.m_width + x;
                int rendererdest = screen_y * m_width + screen_x;
                if (sprite.m_data[itemdest] != 0)
                {
                    m_data[rendererdest] = sprite.m_data[itemdest];
                }
            }
        }
    }
}

void renderer::drawplayercurrentitem(const item& item, const player& player, const camera& camera)
{
    int cameraOffsetX = player.m_xpos - camera.m_xposition;
    int cameraOffsetY = player.m_ypos - camera.m_yposition;

    // Clip the sprite to the screen's boundaries (to avoid checking every pixel)
    int PlayerstartX = max(0, -cameraOffsetX); // Start clipping at the left edge
    int PlayerstartY = max(0, -cameraOffsetY); // Start clipping at the top edge
    int PlayerendX = min(player.m_sprite.m_width, m_width - cameraOffsetX); // Clip at the right edge
    int PlayerendY = min(player.m_sprite.m_height, m_height - cameraOffsetY); // Clip at the bottom edge

    // Clip the sprite to the screen's boundaries (to avoid checking every pixel)
    int itemstartX = max(0, -cameraOffsetX); // Start clipping at the left edge
    int itemstartY = max(0, -cameraOffsetY); // Start clipping at the top edge
    int itemendX = min(item.m_sprite.m_width, m_width - cameraOffsetX); // Clip at the right edge
    int itemendY = min(item.m_sprite.m_height, m_height - cameraOffsetY); // Clip at the bottom edge

    for (int y = itemstartY; y < itemendY; y++)
    {
        for (int x = itemstartX; x < itemendX; x++)
        {
            int destx = cameraOffsetX + x;
            int desty = cameraOffsetY + y;

            // Get the color of the pixel from the sprite at (spritex, spritey)
            int pixelData = item.m_sprite.m_data[y * item.m_sprite.m_width + x];

            // Set the pixel at the destination coordinates on the screen
            if (pixelData != 0) // Only set non-transparent pixels
            {
                m_data[desty * m_width + destx] = pixelData;
            }
        }
    }
}

void renderer::drawtext(FT_Library& ftLibrary, int xcoords, int ycoords, FT_Face & ftFace, std::string text)
{
    if (m_width == 0 || m_height == 0)
        return; 
    int cursorX = 0;  // X coordinate to start drawing the text
    // Loop through each character in the string
    for (char character : text)
    {
        // Render the current character to the buffer
        renderGlyphToIntBuffer(ftLibrary, ftFace, character, cursorX + xcoords, ycoords, m_data, m_width, m_height, m_data);

        // Adjust cursorX to move to the next position for the next character
        // You can customize this depending on the font's width or the spacing you need
        cursorX += (ftFace->glyph->advance.x >> 6); // Using the horizontal advance (in 1/64th pixel units)
    }
}

void renderer::drawplayername(FT_Library& ftLibrary, FT_Face& ftFace, const camera& Camera, const player& Player, std::string name)
{
    int cameraOffsetX = Player.m_xpos - Camera.m_xposition;
    int cameraOffsetY = Player.m_ypos - Camera.m_yposition;

    // Draw the text above the player
    drawtext(ftLibrary, cameraOffsetX, cameraOffsetY - Player.m_sprite.m_height, ftFace, name);
}



 void renderer::drawworld(const world& World, const camera &camera, const player* Player)
 {
     int startX = max(0, (int)camera.m_xposition);
     int startY = max(0, (int)camera.m_yposition);
     int endX = min(World.m_width, (int)(camera.m_xposition + m_width));
     int endY = min(World.m_height, (int)(camera.m_yposition + m_height));

     // Loop only through the visible portion of the world
     for (int y = startY; y < endY; y++)
     {
         for (int x = startX; x < endX; x++)
         {
             // Calculate the screen coordinates
             int destx = x - camera.m_xposition;
             int desty = y - camera.m_yposition;


             if (destx >= 0 && destx < m_width && desty >= 0 && desty < m_height)
             {
                 int pixelData = World.m_data[y * World.m_width + x];

                 if (pixelData != 0)
                 {           
                     m_data[desty * m_width + destx] = pixelData;
                 }
             }
         }
     }
 }

 void renderer::drawenemys(std::vector<enemy>& enemys, const camera& camera)
 {
     for (auto& enemy : enemys)
     {
         // Calculate the offset once for the entire sprite
         int cameraOffsetX = enemy.m_xpos - camera.m_xposition;
         int cameraOffsetY = enemy.m_ypos - camera.m_yposition;

         // Clip the sprite to the screen's boundaries (to avoid checking every pixel)
         int startX = max(0, -cameraOffsetX); // Start clipping at the left edge
         int startY = max(0, -cameraOffsetY); // Start clipping at the top edge
         int endX = min(enemy.m_sprite.m_width, m_width - cameraOffsetX); // Clip at the right edge
         int endY = min(enemy.m_sprite.m_height, m_height - cameraOffsetY); // Clip at the bottom edge

         // Loop through the clipped sprite region
         for (int spritey = startY; spritey < endY; spritey++) // Iterate over sprite's height
         {
             for (int spritex = startX; spritex < endX; spritex++) // Iterate over sprite's width
             {
                 int destx = cameraOffsetX + spritex;
                 int desty = cameraOffsetY + spritey;

                 // Get the color of the pixel from the sprite at (spritex, spritey)
                 int pixelData = enemy.m_sprite.m_data[spritey * enemy.m_sprite.m_width + spritex];

                 // Set the pixel at the destination coordinates on the screen
                 if (pixelData != 0) // Only set non-transparent pixels
                     m_data[desty * m_width + destx] = pixelData;

             }
         }
     }
 }
 void renderer::drawbackground(int color)
 {
     for (int y = 0; y < m_height; y++)
     {
         for (int x = 0; x < m_width; x++)
         {
             m_data[y * m_width + x] = color;
         }
     }
 }


 void renderer::drawsprite(sprite& sprite, int x, int y)
{
    for (int spritey = 0; spritey < sprite.m_height; spritey++)
    {
        for (int spritex = 0; spritex < sprite.m_width; spritex++) // Iterate over sprite's width
        {
            // Calculate the destination position on the screen
            int destx = x + spritex;
            int desty = y + spritey;

            // Ensure the destination coordinates are within bounds of the screen
            if (destx >= 0 && destx < m_width && desty >= 0 && desty < m_height)
            {
                // Get the color of the pixel from the sprite at (spritex, spritey)
                int pixelData = sprite.m_data[spritey * sprite.m_width + spritex];

                // Set the pixel at the destination coordinates on the screen
                if (pixelData != 0)
                {
                    m_data[desty * m_width + destx] = pixelData;
                }
            }
        }
    }
}

void renderer::drawcolor(int color, int x, int y)
{
}

void renderer::update()
{
    m_vulkanrenderer.Resize(m_width, m_height);
    if (m_data != nullptr)
            m_vulkanrenderer.SetData(m_data);

}


void renderer::resize(int width, int height)
{
    delete[] m_data;

	m_width = width;
	m_height = height;
	m_data = new int[width * height];
}