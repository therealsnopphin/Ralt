#pragma once

class menulayer : public Walnut::Layer
{
public:
	static inline std::string s_playername = "";

	static inline FT_GlyphSlot s_currentglyph = nullptr;
	static inline FT_Library s_currentft = nullptr;
	static inline FT_Face s_currentface = nullptr;
public:
	virtual void OnAttach() override;
	virtual void OnUIRender() override;

	static inline bool scene_menulayer = true;
};