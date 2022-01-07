#include "label.h"
#include "ctx.h"

Label::Label()
	: View()
	, m_flow(std::make_shared<Flow>())
	, m_text(std::make_shared<Text>())
{
	add_back(m_flow)->def_props({
		{"flow_flags", flow_flag_right | flow_flag_align_vcenter},
		{"border", 0},
		});
	m_flow->set_flags(0, -1)->add_child(m_text);
}

Label *Label::add_child(std::shared_ptr<View> child)
{
	m_flow->add_child(child);
	return this;
}

view_size Label::pref_size()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	auto border = std::abs((int32_t)get_long_prop("border"));
	auto mw = (int32_t)got_long_prop("min_width");
	auto mh = (int32_t)got_long_prop("min_height");
	auto s = m_flow->pref_size();
	return view_size{std::max(s.m_w + 2 * border, mw), std::max(s.m_h + 2 * border, mh)};
}

Label *Label::layout()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	auto col = (uint32_t)get_long_prop("color");
	auto border = get_long_prop("border");
	m_flow->change(border, border, m_w - border * 2, m_h - border * 2)->layout();
	if ((col >> 24) == 0xff) set_flags(view_flag_opaque, view_flag_opaque);
	return this;
}

Label *Label::draw(const Ctx &ctx)
{
	//allready locked by GUI thread
	auto col = (uint32_t)get_long_prop("color");
	auto border = get_long_prop("border");
	ctx.panel(col, true, border, 0, 0, m_w, m_h);
	return this;
}
