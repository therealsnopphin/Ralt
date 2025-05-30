#pragma once

struct inventory {
	int m_currentitem = 0;
	static constexpr int m_maxiumitems = 3;
	item m_items[m_maxiumitems];
	//Original additem.. call it
	int m_slotsleft = 0;
	void additem(const item& Item)
	{
		for (int i = 0; i < m_maxiumitems; i++)
		{
			if (m_items[i].m_sprite.m_filepath == Item.m_sprite.m_filepath)
			{
				m_items[i].amount++;
				return;
			}
		}

		if (m_slotsleft >= m_maxiumitems)
		{
			return;
		}

		m_items[m_slotsleft] = Item;
		m_slotsleft++;
	}

	void destoryitem(const item& Item)
	{
		for (int i = 0; i < m_maxiumitems; i++)
		{
			if (m_items[i].m_sprite.m_filepath == Item.m_sprite.m_filepath)
			{
				m_items[i].amount--;
				return;
			}
		}

		if (m_slotsleft >= m_maxiumitems)
		{
			return;
		}

		m_items[m_slotsleft] = Item;
		m_slotsleft--;
	}
};