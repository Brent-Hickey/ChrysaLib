#include "view.h"
#include "property.h"
#include <algorithm>
#include <vector>

std::recursive_mutex View::m_mutex;

std::vector<std::shared_ptr<View>> View::children()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	auto children = std::vector<std::shared_ptr<View>>{};
	for (auto &child : m_children) children.push_back(child);
	return children;
}

View *View::add_front(std::shared_ptr<View> child)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (child->m_parent) child->sub();
	child->m_parent = this;
	m_children.push_back(child);
	return this;
}

View *View::add_back(std::shared_ptr<View> child)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (child->m_parent) child->sub();
	child->m_parent = this;
	m_children.push_front(child);
	return this;
}

View *View::sub()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (m_parent)
	{
		auto itr = std::find_if(begin(m_parent->m_children), end(m_parent->m_children), [&] (auto &c)
		{
			return this == c.get();
		});
		if (itr != end(m_parent->m_children))
		{
			m_parent->m_children.erase(itr);
			m_parent = nullptr;
		}
	}
	return this;
}

std::shared_ptr<Property> View::get_prop(const std::string &prop)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	auto view = this;
	while (view != nullptr)
	{
		auto itr = view->m_properties.find(prop);
		if (itr != end(view->m_properties)) return itr->second;
		view = view->m_parent;
	}
	return nullptr;
}

View *View::def_prop(const std::string &prop, std::shared_ptr<Property> value)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_properties[prop] = value;
	return this;
}

View *View::add_opaque(const Rect &rect)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_opaque.paste_rect(rect);
	return this;
}

View *View::sub_opaque(const Rect &rect)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_opaque.remove_rect(rect);
	return this;
}

View *View::clr_opaque()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_opaque.free();
	return this;
}

View *View::add_dirty(const Rect &rect)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_dirty.paste_rect(rect);
	return this;
}

View *View::trans_dirty(int rx, int ry)
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_dirty.translate(rx, ry);
	return this;
}

View *View::dirty()
{
	add_dirty(Rect(0, 0, m_w, m_h));
	return this;
}

View *View::dirty_all()
{
	set_flags(view_flag_dirty_all, view_flag_dirty_all);
	return this;
}

View *View::forward_tree(void *user, std::function<bool(View *view, void *user)> down, std::function<bool(View *view, void *user)> up)
{
	//child function
	std::function<bool(View *view, void *user)> forward_tree = [&](View *view, void *user) -> View*
	{
		if (down(view, user))
		{
			std::for_each(begin(view->m_children), end(view->m_children), [&] (auto &child)
			{
				forward_tree(child.get(), user);
			});
		}
		up(view, user);
		return view;
	};
	//root locking function
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (down(this, user))
	{
		std::for_each(begin(m_children), end(m_children), [&] (auto &child)
		{
			forward_tree(child.get(), user);
		});
	}
	up(this, user);
	return this;
}

View *View::backward_tree(void *user, std::function<bool(View *view, void *user)> down, std::function<bool(View *view, void *user)> up)
{
	//child function
	std::function<bool(View *view, void *user)> backward_tree = [&](View *view, void *user) -> View*
	{
		if (down(view, user))
		{
			std::for_each(rbegin(view->m_children), rend(view->m_children), [&] (auto &child)
			{
				backward_tree(child.get(), user);
			});
		}
		up(view, user);
		return view;
	};
	//root locking function
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	if (down(this, user))
	{
		std::for_each(rbegin(m_children), rend(m_children), [&] (auto &child)
		{
			backward_tree(child.get(), user);
		});
	}
	up(this, user);
	return this;
}

View *View::set_flags(unsigned int flags, unsigned int mask)
{
	auto dirty_state = m_flags & view_flag_dirty_all;
	m_flags = (m_flags & ~mask) | flags | dirty_state;
	return this;
}
