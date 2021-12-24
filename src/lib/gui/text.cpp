#include "text.h"
#include "ctx.h"

view_size Text::get_pref_size()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	auto font = get_string_prop("font");
	auto text = get_string_prop("text");
	return view_size{(int)(8 * text->size()), 16};
}

Text *Text::draw(Ctx *ctx)
{
	//allready locked by GUI thread
	auto font = get_string_prop("font");
	auto text = get_string_prop("text");
	auto col = get_long_prop("color");
	auto ink_col = get_long_prop("ink_color");
	ctx->set_color(col);
	ctx->filled_box(0, 0, m_w, m_h);
	ctx->set_color(ink_col);
	ctx->box(0, 0, m_w, m_h);
	return this;
}
