#include "task.h"
#include "kernel_service.h"

///////
// task
///////

void Task::stop_thread()
{
	if (!m_running) return;
	//set stop_thread flag
	m_running = false;
	//wake the thread
	auto msg = std::make_shared<Msg>(sizeof(Event));
	msg->set_dest(m_net_id);
	auto msg_body = (Event*)msg->begin();
	msg_body->m_evt = evt_exit;
	m_router.send(msg);
}

//helpers

Net_ID Task::start_task(Task *task)
{
	//send task start request
	//kernel will call start_thread
	auto reply_id = m_router.alloc();
	auto reply_mbox = m_router.validate(reply_id);
	auto msg = std::make_shared<Msg>(sizeof(Kernel_Service::Event_start_task));
	auto msg_body = (Kernel_Service::Event_start_task*)msg->begin();
	msg->set_dest(Net_ID(m_router.get_dev_id(), Mailbox_ID{0}));
	msg_body->m_evt = Kernel_Service::evt_start_task;
	msg_body->m_reply = reply_id;
	msg_body->m_task = task;
	m_router.send(msg);
	//wait for reply
	auto reply = reply_mbox->read();
	m_router.free(reply_id);
	auto reply_body = (Kernel_Service::start_task_reply*)reply->begin();
	return reply_body->m_task;
}

void Task::stop_task()
{
	//send task stop request
	//kernel will call stop_thread and join_thread
	auto msg = std::make_shared<Msg>(sizeof(Kernel_Service::Event_stop_task));
	auto msg_body = (Kernel_Service::Event_stop_task*)msg->begin();
	msg->set_dest(Net_ID(m_router.get_dev_id(), Mailbox_ID{0}));
	msg_body->m_evt = Kernel_Service::evt_stop_task;
	m_router.send(msg);
}
