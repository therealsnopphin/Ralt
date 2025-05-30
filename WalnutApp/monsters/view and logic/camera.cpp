#include "pch.h"

camera::camera(int worldwidth, int worldheight, int rendererwidth, int rendererheight, float playerxpos, float playerypos)
{
    setviewport(worldwidth, worldheight, rendererwidth, rendererheight, playerxpos, playerypos);
}

void camera::setviewport(int worldwidth, int worldheight, int rendererwidth, int rendererheight, float playerxpos, float playerypos)
{
    float target_x = playerxpos - rendererwidth / 2.0f;
    float target_y = playerypos - rendererheight / 2.0f;

    m_xposition += (target_x - m_xposition) * m_lerp;
    m_yposition += (target_y - m_yposition) * m_lerp;
    if (m_xposition < 0) {
        m_xposition = 0;
    }
    if (m_xposition + rendererwidth > worldwidth) {
        m_xposition = worldwidth - rendererwidth;
    }
    if (m_yposition < 0) {
        m_yposition = 0;
    }
    if (m_yposition + rendererheight > worldheight) {
        m_yposition = worldheight - rendererheight;
    }
}
