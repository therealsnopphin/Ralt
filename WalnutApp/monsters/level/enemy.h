#pragma once

class enemy
{
public:
	void attack(float timestep, player& Player, const camera& Camera);

	const float m_enemySpeed = 100;
	const float m_attackRange = 1;
	const float m_attackDamage = 6.0;
	const float m_attackCooldown = 0.0;

	int m_hp = 100;
	sprite m_sprite = { "Resource/tall.png" };
	float m_xpos = 0, m_ypos = 0;
};