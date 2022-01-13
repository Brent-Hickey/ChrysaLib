#include "task.h"
#include "kernel_service.h"
#include <algorithm>

///////
// task
///////

void Task::run_then_join()
{
	run();
	Kernel_Service::join_task(shared_from_this());
}

void Task::start_thread()
{
	if (m_running) return;
	m_running = true;
	m_thread = std::thread(&Task::run_then_join, this);
}

void Task::stop_thread()
{
	if (!m_running) return;
	m_running = false;
	//wake the thread with an exit message
	auto msg = std::make_shared<Msg>(sizeof(Event));
	msg->set_dest(m_net_id);
	auto event_body = (Event*)msg->begin();
	event_body->m_evt = evt_exit;
	global_router->send(msg);
}

void Task::join_thread()
{
	if (m_thread.joinable()) m_thread.join();
}

std::vector<Net_ID> Task::alloc_select(uint32_t size)
{
	auto select = std::vector<Net_ID>{m_net_id};
	for (auto i = 1; i < size; ++i) select.emplace_back(global_router->alloc());
	return select;
}

void Task::free_select(std::vector<Net_ID> &select)
{
	std::for_each(begin(select) + 1, end(select), [&] (const auto &id) { global_router->free(id); });
	select.clear();
}
