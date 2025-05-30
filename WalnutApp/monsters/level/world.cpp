#include "pch.h"

world::world()
{
    createworld();
}

void world::createworld()
{
    m_data = new int[m_width * m_height];

    static int randomBlocks[] = {
        -9395396, -15063505, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396, -9395396,
        -9395396, -9395396
    };

    int tilesX = (m_width + m_sizeTile - 1) / m_sizeTile;
    int tilesY = (m_height + m_sizeTile - 1) / m_sizeTile;

    std::vector<int> tileBlock(tilesX * tilesY, -1);
    
    for (int tileY = 0; tileY < tilesY; tileY++)
    {
        for (int tileX = 0; tileX < tilesX; tileX++)
        {
            int seed = tileY * (m_width / m_sizeTile + 1) + tileX;
            Walnut::Random::SetSeed(seed);

            if (tileBlock[tileY * tilesX + tileX] != -1)
                continue; // Already filled by tree

            int randomvalue = Walnut::Random::UInt(0, 38);

            // Try to generate tree
            if (randomBlocks[randomvalue] == -15063505)
            {
                int treeHeight = Walnut::Random::UInt(1, 2); // 2-3 blocks tall
                int treeWidth = Walnut::Random::UInt(2, 3);
                int leavesHeight = Walnut::Random::UInt(2, 4);
                int leavesWidth = Walnut::Random::UInt(1, 2);

                int totalHeight = treeHeight + leavesHeight;

                // Bounds check
                if (tileX + leavesWidth < tilesX && tileY + totalHeight < tilesY)
                {
                    // Place trunk
                    for (int y = 0; y < treeHeight; y++)
                    {
                        for (int x = 0; x < treeWidth; x++)
                        {
                            tileBlock[(tileY + y) * tilesX + (tileX + x)] = -10625393;
                        }
                    }

                    // Place leaves
                    for (int y = 0; y < leavesHeight; y++)
                    {
                        for (int x = 0; x < leavesWidth; x++)
                        {
                            tileBlock[((tileY + treeHeight) + y) * tilesX + (tileX + x)] = -15063505;
                        }
                    }

                    continue; // Skip random fill for this tree
                }
            }

            // Default block
            tileBlock[tileY * tilesX + tileX] = randomBlocks[randomvalue];
        }
    }

    int caveXblock = Walnut::Random::UInt(5, tilesX - 5);
    int caveYblock = Walnut::Random::UInt(5, tilesY -5);
    //5x5
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            tileBlock[(caveYblock + y) * tilesX + (caveXblock + x)] = -16711423;
        }
    }

    // Fill m_data from tileBlock
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            int tileX = x / m_sizeTile;
            int tileY = y / m_sizeTile;
            int tileIndex = tileY * tilesX + tileX;

            m_data[y * m_width + x] = tileBlock[tileIndex];
        }
    }
}
