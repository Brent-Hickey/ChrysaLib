#include "gui_task.h"
#include "kernel_service.h"
#include "gui_service.h"
#include "../gui/view.h"

///////////
// gui task
///////////

//helpers

Net_ID GUI_Task::my_gui()
{
	//return my GUI node
	auto filter = "gui," + global_router->get_dev_id().to_string();
	auto services = global_router->enquire(filter);
	auto fields = split_string(services[0], ",");
	return Net_ID::from_string(fields[1]);
}

void GUI_Task::add_front(std::shared_ptr<View> view)
{
	//message to my GUI
	view->m_owner = m_net_id;
	auto service_id = my_gui();
	auto reply_id = global_router->alloc();
	auto reply_mbox = global_router->validate(reply_id);
	auto msg = std::make_shared<Msg>(sizeof(GUI_Service::Event_add_front));
	auto event_body = (GUI_Service::Event_add_front*)msg->begin();
	msg->set_dest(service_id);
	event_body->m_evt = GUI_Service::evt_add_front;
	event_body->m_reply = reply_id;
	event_body->m_view = view;
	global_router->send(msg);
	//wait for reply
	reply_mbox->read();
	global_router->free(reply_id);
}

void GUI_Task::add_back(std::shared_ptr<View> view)
{
	//message to my GUI
	view->m_owner = m_net_id;
	auto service_id = my_gui();
	auto reply_id = global_router->alloc();
	auto reply_mbox = global_router->validate(reply_id);
	auto msg = std::make_shared<Msg>(sizeof(GUI_Service::Event_add_back));
	auto event_body = (GUI_Service::Event_add_back*)msg->begin();
	msg->set_dest(service_id);
	event_body->m_evt = GUI_Service::evt_add_back;
	event_body->m_reply = reply_id;
	event_body->m_view = view;
	global_router->send(msg);
	//wait for reply
	reply_mbox->read();
	global_router->free(reply_id);
}

void GUI_Task::sub(std::shared_ptr<View> view)
{
	//message to my GUI
	view->m_owner = m_net_id;
	auto service_id = my_gui();
	auto reply_id = global_router->alloc();
	auto reply_mbox = global_router->validate(reply_id);
	auto msg = std::make_shared<Msg>(sizeof(GUI_Service::Event_sub));
	auto event_body = (GUI_Service::Event_sub*)msg->begin();
	msg->set_dest(service_id);
	event_body->m_evt = GUI_Service::evt_sub;
	event_body->m_reply = reply_id;
	event_body->m_view = view;
	global_router->send(msg);
	//wait for reply
	reply_mbox->read();
	global_router->free(reply_id);
}
