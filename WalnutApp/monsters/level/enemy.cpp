#include "pch.h"

void enemy::attack(float timestep, player& Player, const camera& Camera)
{
    // Track time since the last attack
    static float timeSinceLastAttack = 0.0;
    float dx = Player.m_xpos - m_xpos;
    float dy = Player.m_ypos - m_ypos;

    // Calculate the distance between enemy and player
    float distance = sqrt(dx * dx + dy * dy);

    // If enemy is within attack range, deal damage
    if (distance <= m_attackRange)
    {
        timeSinceLastAttack += timestep;

        // Only attack if enough time has passed since the last attack
        if (timeSinceLastAttack >= m_attackCooldown)
        {
            Player.hp -= m_attackDamage; // Reduce player's HP
            timeSinceLastAttack = 0.0; // Reset the attack timer
        }
    }
    else 
    {
        if (distance > 0.0) // Avoid division by zero
        {
            dx /= distance;
            dy /= distance;
        }

        // Move the enemy toward the player
        m_xpos += dx * m_enemySpeed * timestep;
        m_ypos += dy * m_enemySpeed * timestep;
    }
}
