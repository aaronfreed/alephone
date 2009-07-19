/*

	Copyright (C) 1991-2001 and beyond by Bungie Studios, Inc.
	and the "Aleph One" developers.
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This license is contained in the file "COPYING",
	which is included with this source code; it is available online at
	http://www.gnu.org/licenses/gpl.html

*/

/*
 *  HUDRenderer_Lua.h - HUD rendering using Lua scripts
 *
 *  Written in 2009 by Jeremiah Morris
 */

#ifndef _HUD_RENDERER_LUA_H_
#define _HUD_RENDERER_LUA_H_

#include "HUDRenderer.h"

struct blip_info {
	short mtype;
	short intensity;
	world_distance distance;
	angle direction;
};
typedef struct blip_info blip_info;

class FontSpecifier;
class Image_Blitter;

class HUD_Lua_Class : public HUD_Class
{
public:
	HUD_Lua_Class() : m_drawing(false) {}
	~HUD_Lua_Class() {}

	void update_motion_sensor(short time_elapsed);
	void clear_entity_blips(void);
	void add_entity_blip(short mtype, short intensity, short x, short y);
	
	size_t entity_blip_count(void);
	blip_info entity_blip(size_t index);
	
	void start_draw(void);
	void end_draw(void);
	void apply_clip(void);
	
	void fill_rect(float x, float y, float w, float h,
								 float r, float g, float b, float a);
	void frame_rect(float x, float y, float w, float h,
								  float r, float g, float b, float a,
									float t);
	void draw_text(FontSpecifier *font, const char *text,
	               float x, float y,
								 float r, float g, float b, float a);
	void draw_image(Image_Blitter *image, float x, float y);
	
protected:
	std::vector<blip_info> m_blips;
	bool m_drawing;
	bool m_opengl;
	SDL_Surface *m_surface;
	
	void render_motion_sensor(short time_elapsed);
	void draw_all_entity_blips(void);
	void draw_or_erase_unclipped_shape(short x, short y, shape_descriptor shape, bool draw) {}
	void erase_entity_blip(point2d *location, shape_descriptor shape) {}
	void draw_entity_blip(point2d *location, shape_descriptor shape) {}

	virtual void draw_message_area(short) {}

	void DrawShape(shape_descriptor shape, screen_rectangle *dest, screen_rectangle *src) {}
	void DrawShapeAtXY(shape_descriptor shape, short x, short y, bool transparency = false) {}
	void DrawText(const char *text, screen_rectangle *dest, short flags, short font_id, short text_color) {}
	void FillRect(screen_rectangle *r, short color_index) {}
	void FrameRect(screen_rectangle *r, short color_index) {}

	void DrawTexture(shape_descriptor texture, short x, short y, int size) {}

	void SetClipPlane(int x, int y, int c_x, int c_y, int radius) {}
	void DisableClipPlane(void) {}
};

HUD_Lua_Class *Lua_HUDInstance();
void Lua_DrawHUD(short time_elapsed);

#endif
