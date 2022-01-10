#include "app.h"
#include "../../gui/ui.h"

#include "../canvas/app.h"
#include "../services/app.h"
#include "../mandelbrot/app.h"

std::string to_utf8(uint32_t c);
uint32_t from_utf8(uint8_t **data);

void Launcher_App::run()
{
	enum
	{
		event_close,
		event_button,
	};
	
	ui_window(window, ({}))
		ui_flow(window_flow, ({
			{"flow_flags", flow_down_fill}}))
			ui_flow(title_flow, ({
				{"flow_flags", flow_left_fill}}))
				ui_grid(button_grid, ({
					{"grid_height", 1},
					{"font", Font::open("fonts/Entypo.ctf", 22)}}))
					ui_button(close_button, ({
						{"text", to_utf8(0xea19)}}))
					ui_end
				ui_end
				ui_title(title, ({
					{"text", "Launcher"}}))
				ui_end
			ui_end
			ui_grid(app_grid, ({
				{"color" , Property::get_default("env_toolbar2_col")->get_long()},
				{"grid_width", 2}}))
				ui_button(_1, ({
					{"text", "services"}}))
					ui_connect(event_button)
				ui_end
				ui_button(_2, ({
					{"text", "canvas"}}))
					ui_connect(event_button)
				ui_end
				ui_button(_3, ({
					{"text", "mandelbrot"}}))
					ui_connect(event_button)
				ui_end
			ui_end
		ui_end
	ui_end

	auto s = window->pref_size();
	window->change(0, 0, s.m_w * 110 / 100, s.m_h);

	//add to my GUI screen
	add_front(window);

	//event loop
	auto mbox = global_router->validate(get_id());
	for (;;)
	{
		auto msg = mbox->read();
		if (!m_running) break;
		auto body = (View::Event*)msg->begin();
		switch (body->m_target_id)
		{
		case event_button:
		{
			//launch app
			auto action_body = (View::Event_action*)body;
			auto source_id = action_body->m_source_id;
			auto source = window->find_id(source_id);
			auto text = source->get_string_prop("text");
			if (text == "services")
			{
				auto app = Services_App::create();
				Kernel_Service::start_task(app);
			}
			else if (text == "canvas")
			{
				auto app = Canvas_App::create();
				Kernel_Service::start_task(app);
			}
			else if (text == "mandelbrot")
			{
				auto app = Mandelbrot_App::create();
				Kernel_Service::start_task(app);
			}
			break;
		}
		default:
		{
			//dispatch to widgets
			window->event(msg);
			break;
		}
		}
	}

	//tidy up
	window->hide();
}
