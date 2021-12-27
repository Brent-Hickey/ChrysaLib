#ifndef VIEW_H
#define VIEW_H

#include "property.h"
#include "region.h"
#include "../mail/msg.h"
#include <list>
#include <mutex>
#include <map>
#include <functional>
#include <memory>
#include <vector>

class Ctx;

//view event msg
enum
{
	ev_type_mouse,
	ev_type_key,
	ev_type_action,
	ev_type_gui,
	ev_type_wheel,
	ev_type_enter,
	ev_type_exit
};

//view flags
enum
{
	view_flag_solid = 1 << 0,
	view_flag_opaque = 1 << 1,
	view_flag_dirty_all = 1 << 2,
	view_flag_hidden = 1 << 3,
	view_flag_at_back = 1 << 4,
	view_flag_at_front = 1 << 5,
};

struct view_pos
{
	int m_x = 0;
	int m_y = 0;
};

struct view_size
{
	//can be compared !
	bool operator==(const view_size &p) const
	{
		return std::tie(p.m_w, p.m_h) == std::tie(m_w, m_h);
	}
	int m_w = 0;
	int m_h = 0;
};

struct view_bounds
{
	int m_x = 0;
	int m_y = 0;
	int m_w = 0;
	int m_h = 0;
};

//view class
//base class for all widgets
class View
{
public:
	struct Event
	{
		unsigned long m_target_id;
		unsigned long m_type;
	};
	struct Event_mouse : public Event
	{
		unsigned int m_buttons;
		unsigned int m_count;
		int m_x;
		int m_y;
		int m_rx;
		int m_ry;
	};
	struct Event_wheel : public Event
	{
		unsigned int m_direction;
		int m_x;
		int m_y;
	};
	struct Event_key : public Event
	{
		unsigned int m_keycode;
		unsigned int m_key;
		unsigned int m_mod;
	};
	struct Event_action : public Event
	{
		unsigned long m_source_id;
	};
	struct Event_gui : public Event
	{};
	struct Event_enter : public Event
	{};
	struct Event_exit : public Event
	{};
	View();
	//properties
	View *def_prop(const std::string &prop, std::shared_ptr<Property>);
	View *set_prop(const std::string &prop, std::shared_ptr<Property>);
	std::shared_ptr<Property> got_prop(const std::string &prop);
	std::shared_ptr<Property> get_prop(const std::string &prop);
	int64_t got_long_prop(const std::string &prop);
	const std::string got_string_prop(const std::string &prop);
	int64_t get_long_prop(const std::string &prop);
	const std::string get_string_prop(const std::string &prop);
	//children
	std::vector<std::shared_ptr<View>> children();
	View *add_front(std::shared_ptr<View> child);
	View *add_back(std::shared_ptr<View> child);
	View *sub();
	View *to_back();
	View *to_front();
	//tree iteration
	View *forward_tree(std::function<bool(View&)>down, std::function<bool(View&)>up);
	View *backward_tree(std::function<bool(View&)>down, std::function<bool(View&)>up);
	//visability
	View *add_opaque(const Rect &rect);
	View *sub_opaque(const Rect &rect);
	View *clr_opaque();
	//dirty areas
	View *add_dirty(const Rect &rect);
	View *trans_dirty(int rx, int ry);
	View *dirty();
	View *dirty_all();
	View *change_dirty(int x, int y, int w, int h);
	//flags
	View *set_flags(unsigned int flags, unsigned int mask);
	//info
	int64_t get_id() { return m_id; }
    view_pos get_pos();
    view_size get_size();
    view_bounds get_bounds();
	//action
	View *connect(uint64_t id) { m_actions.push_back(id); return this; }
	View *emit() { return this; }
	//subclass overides
    virtual view_size pref_size();
	virtual View *layout();
	virtual View *change(int x, int y, int w, int h);
	virtual View *add_child(std::shared_ptr<View> child) { return add_back(child); }
	virtual View *draw(const Ctx &ctx);
	virtual View *action(const std::shared_ptr<Msg> &event) { return this; }
	virtual View *mouse_down(const std::shared_ptr<Msg> &event) { return this; }
	virtual View *mouse_up(const std::shared_ptr<Msg> &event) { return this; }
	virtual View *mouse_move(const std::shared_ptr<Msg> &event) { return this; }
	virtual View *mouse_hover(const std::shared_ptr<Msg> &event) { return this; }
	virtual View *mouse_wheel(const std::shared_ptr<Msg> &event) { return this; }

	static std::recursive_mutex m_mutex;
	static int64_t m_next_id;
	View *m_parent = nullptr;
	std::list<std::shared_ptr<View>> m_children;
	std::map<std::string, std::shared_ptr<Property>> m_properties;
	Region m_dirty;
	Region m_opaque;
	int m_x = 0;
	int m_y = 0;
	int m_w = 0;
	int m_h = 0;
	int m_ctx_x = 0;
	int m_ctx_y = 0;
	unsigned int m_flags = view_flag_solid;
	int64_t m_id = 0;
	std::vector<int64_t> m_actions;
};

#endif
