#include "title.h"

Title::Title()
	: Label()
{}

Title *Title::mouse_down(const std::shared_ptr<Msg> &event)
{
	std::lock_guard<std::recursive_mutex> l(m_mutex);
	auto event_body = (View::Event_mouse*)&*(event->begin());
	auto window = m_parent;
	while (!window->got_prop("is_window")) { window = window->m_parent; }
	m_drag_x = window->m_x - event_body->m_x;
	m_drag_y = window->m_y - event_body->m_y;
	if (event_body->m_buttons == 1) window->to_front();
	else window->to_back();
	return this;
}

Title *Title::mouse_move(const std::shared_ptr<Msg> &event)
{
	std::lock_guard<std::recursive_mutex> l(m_mutex);
	auto event_body = (View::Event_mouse*)&*(event->begin());
	auto window = m_parent;
	while (!window->got_prop("is_window")) { window = window->m_parent; }
	window->change_dirty(event_body->m_x + m_drag_x, event_body->m_y + m_drag_y, window->m_w, window->m_h);
	return this;
}
