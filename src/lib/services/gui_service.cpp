#include "gui_service.h"
#include "../gui/region.h"
#include "../gui/ctx.h"
#include "../gui/backdrop.h"
#include <iostream>
#include <sstream>
#include <SDL.h>

//////
// gui
//////

void GUI_Service::run()
{
	//get my mailbox address, id was allocated in the constructor
	auto mbox = m_router.validate(m_net_id);
	auto entry = m_router.declare(m_net_id, "gui", "GUI_Service v0.1");

	m_screen = std::make_shared<Backdrop>(0, 0, 1280, 960);
	m_screen->set_flags(view_flag_dirty_all, view_flag_dirty_all);
	auto screen_w = m_screen->m_w;
	auto screen_h = m_screen->m_h;

	auto test = std::make_shared<Backdrop>(107, 107, 256, 256);
	auto col = std::make_shared<Property>();
	auto ink_col = std::make_shared<Property>();
	col->set_int(0xffffff00);
	ink_col->set_int(0xffff0000);
	test->def_prop("color", col);
	test->def_prop("ink_color", ink_col);
	m_screen->add_back(test);

	//init SDL
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	//create window
	SDL_Window *window = SDL_CreateWindow("GUI Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	//hide host cursor
	//SDL_ShowCursor(0);
	//renderer
	m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	SDL_Texture *texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, screen_w, screen_h);
	//set blend mode
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	//event loop
	while (m_running)
	{
		auto msg = mbox->poll();
		if (msg)
		{
			auto evt = (Event*)msg->begin();
			switch (evt->m_evt)
			{
			case evt_add_front_view:
			{
				//add view to screen
				auto body_struct = (Event_add_front_view*)msg->begin();
				break;
			}
			case evt_add_back_view:
			{
				//add view to screen
				auto body_struct = (Event_add_back_view*)msg->begin();
				break;
			}
			case evt_sub_view:
			{
				//sub view from screen
				auto body_struct = (Event_sub_view*)msg->begin();
				break;
			}
			default:
				break;
			}
		}

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				m_running = false;
			}
			if (e.type == SDL_KEYDOWN)
			{
				m_running = false;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				m_running = false;
			}
		}

		if (m_gui_flags)
		{
			//resize back buffer and then do full redraw
			SDL_DestroyTexture(texture);
			SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, screen_w, screen_h);
			m_screen->set_flags(view_flag_dirty_all, view_flag_dirty_all);
			m_dirty_flag = true;
			m_gui_flags = 0;
		}
		if (m_dirty_flag)
		{
			SDL_SetRenderTarget(m_renderer, texture);
			composit();
			SDL_SetRenderTarget(m_renderer, 0);
			SDL_RenderCopy(m_renderer, texture, 0, 0);
			SDL_RenderPresent(m_renderer);
			m_dirty_flag = false;
		}

		//frame polling loop
		std::this_thread::sleep_for(std::chrono::milliseconds(GUI_FRAME_RATE));
	}

	//quit SDL
	SDL_DestroyWindow(window);
	SDL_Quit();

	//forget myself
	m_router.forget(entry);
}

void GUI_Service::composit()
{
	auto screen_w = m_screen->m_w;
	auto screen_h = m_screen->m_h;

	//iterate through views back to front, setting abs cords of views
	struct abs_cords
	{
		int m_x = 0;
		int m_y = 0;
	};
	abs_cords abs;
	m_screen->backward_tree(&abs,
		[&](View *view, void *user)
		{
			auto abs = (abs_cords*)user;
			abs->m_x += view->m_x;
			abs->m_y += view->m_y;
			view->m_ctx_x = abs->m_x;
			view->m_ctx_y = abs->m_y;
			return true;
		},
		[&](View *view, void *user)
		{
			auto abs = (abs_cords*)user;
			abs->m_x -= view->m_x;
			abs->m_y -= view->m_y;
			return true;
		});

	m_screen->backward_tree(&abs,
		[&](View *view, void *user)
		{
			Region region;
			region.paste_rect(Rect(
				view->m_ctx_x, view->m_ctx_y,
				view->m_ctx_x + view->m_w, view->m_ctx_y + view->m_h));
			Ctx ctx;
			ctx.m_x = view->m_ctx_x;
			ctx.m_y = view->m_ctx_y;
			ctx.m_renderer = m_renderer;
			ctx.m_region = &region;
			view->draw(&ctx);
			return true;
		},
		[&](View *view, void *user)
		{
			return true;
		});
}

// 	;iterate through views back to front
// 	;setting abs cords
// 	(vp-xor-rr :r7 :r7)
// 	(vp-xor-rr :r8 :r8)
// 	(call 'view :backward_tree '(:r0 :r0 ($ abs_down_callback) ($ abs_up_callback)))

// 	;iterate through views back to front
// 	;create visible region at root
// 	(call 'view :backward_tree '(:r0 :r0 ($ visible_down_callback) ($ visible_up_callback)))

// 	(when (/= num_old_regions 0)
// 		;copy new damaged region
// 		(vp-xor-rr :r1 :r1)
// 		(assign '(:r1) '((:rsp local_damaged_region)))
// 		(fn-bind 'sys/statics/statics :r4)
// 		(call 'region :copy_rect '((& :r4 statics_gui_rect_heap) (& :r0 view_dirty_region)
// 			(& :rsp local_damaged_region) 0 0 (:r0 view_w) (:r0 view_h)) '(:r0))

// 		;paste old damaged region into root
// 		(fn-bind 'sys/statics/statics :r4)
// 		(assign '((& :r4 statics_gui_old_regions) (:r4 statics_gui_old_region_index) (:rsp local_root)) '(:r1 :r2 :r3))
// 		(call 'region :paste_region '(:r0 (& :r1 :r2) (& :r3 view_dirty_region) 0 0) '(:r0))

// 		;free old damaged region
// 		(fn-bind 'sys/statics/statics :r4)
// 		(assign '((& :r4 statics_gui_old_regions) (:r4 statics_gui_old_region_index)) '(:r1 :r2))
// 		(call 'region :free '(:r0 (& :r1 :r2)) '(:r0))

// 		;paste new damaged region into all old damaged regions
// 		(assign '((:r4 statics_gui_old_region_index)) '(:r2))
// 		(loop-start)
// 			(vp-add-cr +ptr_size :r2)
// 			(vpif (list :r2 '= (* num_old_regions +ptr_size)))
// 				(vp-xor-rr :r2 :r2)
// 			(endif)
// 			(assign '((& :r4 statics_gui_old_regions) (:r4 statics_gui_old_region_index)) '(:r1 :r3))
// 			(breakif '(:r2 = :r3))
// 			(assign '(:r2) '((:rsp local_index)))
// 			(call 'region :paste_region '(:r0 (& :rsp local_damaged_region) (& :r1 :r2) 0 0) '(:r0))
// 			(assign '((:rsp local_index)) '(:r2))
// 			(fn-bind 'sys/statics/statics :r4)
// 		(loop-end)
// 		(assign '((:rsp local_damaged_region)) '(:r0))
// 		(assign '(:r0) '((:r1 :r2)))

// 		;move on to next old region
// 		(vp-add-cr +ptr_size :r2)
// 		(vpif (list :r2 '= (* num_old_regions +ptr_size)))
// 			(vp-xor-rr :r2 :r2)
// 		(endif)
// 		(assign '((:rsp local_root) :r2) '(:r0 (:r4 statics_gui_old_region_index))))

// 	;iterate through views front to back
// 	;distribute visible region
// 	(vp-xor-rr :r1 :r1)
// 	(assign '(:r1) '((:rsp local_ctx_flist)))
// 	(call 'view :forward_tree '(:r0 :rsp ($ distribute_down_callback) ($ distribute_up_callback)))

// 	;draw all views on draw list, and free dirty regions
// 	(loop-flist :rsp local_ctx_flist :r0 :r0)
// 		(assign '(:r0) '((:rsp local_ctx_next)))
// 		(vp-sub-cr view_ctx_node :r0)
// 		(call 'view :draw '(:r0))
// 		(assign '((:rsp local_ctx_next)) '(:r1))
// 		(vp-sub-cr (- view_ctx_node view_dirty_region) :r1)
// 		(fn-bind 'sys/statics/statics :r0)
// 		(vp-add-cr statics_gui_rect_heap :r0)
// 		(call 'region :free '(:r0 :r1))
// 		(assign '((:rsp local_ctx_next)) '(:r0))
// 	(loop-end)

// 	(vp-free local_size)
// 	(vp-ret)

// (vp-label 'visible_down_callback)
// 	(def-struct vis 0
// 		(ptr this root next region))

// 	;save inputs
// 	(vp-alloc vis_size)
// 	(entry 'view :forward_tree_callback '((:rsp vis_this) (:rsp vis_root)))

// 	;region heap
// 	(fn-bind 'sys/statics/statics :r0)
// 	(vp-add-cr statics_gui_rect_heap :r0)

// 	;remove opaque region from ancestors if not root
// 	(assign '((:rsp vis_this) (:rsp vis_root)) '(:r1 :r2))
// 	(vpif '(:r1 /= :r2))
// 		;remove my opaque region from ancestors
// 		(vp-xor-rr :r2 :r2)
// 		(assign '(:r2 (:r1 view_flags)) '((:rsp vis_region) :r3))
// 		(vp-and-cr view_flag_opaque :r3)
// 		(vpif '(:r3 /= 0))
// 			;remove entire view from ancestors
// 			(assign '(0 0 (:r1 view_w) (:r1 view_h)) '(:r7 :r8 :r9 :r10))
// 			(loop-start)
// 				(assign '((:r1 hmap_parent)) '(:r2))
// 				(assign '(:r2) '((:rsp vis_next)))

// 				;translate region
// 				(assign '((:r1 view_x) (:r1 view_y) (:r2 view_w) (:r2 view_h)) '(:r11 :r12 :r13 :r14))
// 				(vp-add-rr :r11 :r7)
// 				(vp-add-rr :r12 :r8)
// 				(vp-add-rr :r11 :r9)
// 				(vp-add-rr :r12 :r10)

// 				;clip to parent, exit if clipped away
// 				(breakif '(:r7 >= :r13) '(:r8 >= :r14) '(:r9 <= 0) '(:r10 <= 0))
// 				(vpif '(:r7 < 0))
// 					(assign '(0) '(:r7))
// 				(endif)
// 				(vpif '(:r8 < 0))
// 					(assign '(0) '(:r8))
// 				(endif)
// 				(vpif '(:r9 > :r13))
// 					(assign '(:r13) '(:r9))
// 				(endif)
// 				(vpif '(:r10 > :r14))
// 					(assign '(:r14) '(:r10))
// 				(endif)

// 				;remove opaque region
// 				(call 'region :remove_rect '(:r0 (& :r2 view_dirty_region) :r7 :r8 :r9 :r10) '(:r0))

// 				(assign '((:rsp vis_next) (:rsp vis_root)) '(:r1 :r2))
// 			(loop-until '(:r1 = :r2))
// 		(else)
// 			;use opaque region, so my opaque area is the visble region
// 			(assign '((:r1 hmap_parent) (:r1 view_x) (:r1 view_y)) '(:r2 :r7 :r8))
// 			(assign '((:r2 view_w) (:r2 view_h)) '(:r9 :r10))
// 			(vp-mul-cr -1 :r7)
// 			(vp-mul-cr -1 :r8)
// 			(vp-add-rr :r7 :r9)
// 			(vp-add-rr :r8 :r10)
// 			(vp-add-cr view_opaque_region :r1)
// 			(vp-lea-i :rsp vis_region :r2)
// 			(call 'region :copy_rect '(:r0 :r1 :r2 :r7 :r8 :r9 :r10) '(:r0))

// 			;remove from ancestors
// 			(assign '((:rsp vis_this)) '(:r1))
// 			(loop-start)
// 				(assign '((:r1 hmap_parent)) '(:r2))
// 				(assign '(:r2) '((:rsp vis_next)))

// 				;exit if clipped away
// 				(assign '((:rsp vis_region)) '(:r3))
// 				(breakif '(:r3 = 0))

// 				;translate temp opaque region
// 				(assign '((:r1 view_x) (:r1 view_y)) '(:r7 :r8))
// 				(vp-lea-i :rsp vis_region :r1)
// 				(call 'region :translate '(:r1 :r7 :r8))

// 				;clip temp opaque region
// 				(assign '((:rsp vis_next)) '(:r2))
// 				(vp-lea-i :rsp vis_region :r1)
// 				(call 'region :clip_rect '(:r0 :r1 0 0 (:r2 view_w) (:r2 view_h)) '(:r0))

// 				;remove temp opaque region
// 				(vp-lea-i :rsp vis_region :r1)
// 				(assign '((:rsp vis_next)) '(:r2))
// 				(vp-add-cr view_dirty_region :r2)
// 				(call 'region :remove_region '(:r0 :r1 :r2 0 0) '(:r0))

// 				(assign '((:rsp vis_next) (:rsp vis_root)) '(:r1 :r2))
// 			(loop-until '(:r1 = :r2))

// 			;free any temp region
// 			(call 'region :free '(:r0 (& :rsp vis_region)) '(:r0))
// 		(endif)
// 	(endif)

// 	(exit 'view :forward_tree_callback '((:rsp vis_this) :r0))
// 	(vp-free vis_size)
// 	(vp-ret)

// (vp-label 'visible_up_callback)
// 	;save inputs
// 	(vp-alloc vis_size)
// 	(entry 'view :forward_tree_callback '((:rsp vis_this) (:rsp vis_root)))

// 	;region heap
// 	(fn-bind 'sys/statics/statics :r0)
// 	(vp-add-cr statics_gui_rect_heap :r0)

// 	;clip local dirty region with parent bounds
// 	(assign '((:rsp vis_this) (:rsp vis_root)) '(:r1 :r3))
// 	(assign '((:r1 hmap_parent)) '(:r2))
// 	(vpif '(:r1 = :r3))
// 		(vp-cpy-rr :r1 :r2)
// 	(endif)
// 	(assign '((:r1 view_x) (:r1 view_y) (:r2 view_w) (:r2 view_h)) '(:r7 :r8 :r9 :r10))
// 	(vp-mul-cr -1 :r7)
// 	(vp-mul-cr -1 :r8)
// 	(vp-add-rr :r7 :r9)
// 	(vp-add-rr :r8 :r10)
// 	(vp-add-cr view_dirty_region :r1)
// 	(call 'region :clip_rect '(:r0 :r1 :r7 :r8 :r9 :r10) '(:r0))

// 	;paste local dirty region onto parent if not root
// 	(assign '((:rsp vis_this) (:rsp vis_root)) '(:r1 :r3))
// 	(vpif '(:r1 /= :r3))
// 		(assign '((:r1 view_x) (:r1 view_y) (:r1 hmap_parent)) '(:r7 :r8 :r2))
// 		(vp-add-cr view_dirty_region :r1)
// 		(vp-add-cr view_dirty_region :r2)
// 		(call 'region :paste_region '(:r0 :r1 :r2 :r7 :r8) '(:r0))

// 		;free local dirty region
// 		(assign '((:rsp vis_this)) '(:r1))
// 		(vp-add-cr view_dirty_region :r1)
// 		(call 'region :free '(:r0 :r1) '(:r0))
// 	(endif)

// 	;if dirty all flag then paste entire view onto parent
// 	(assign '((:rsp vis_this)) '(:r2))
// 	(assign '((:r2 view_flags)) '(:r3))
// 	(vp-and-cr view_flag_dirty_all :r3)
// 	(vpif '(:r3 /= 0))
// 		;clear dirty all flag
// 		(assign '((:r2 view_flags)) '(:r3))
// 		(vp-and-cr (lognot view_flag_dirty_all) :r3)
// 		(assign '(:r3 (:r2 view_x) (:r2 view_y) (:r2 view_w) (:r2 view_h)) '((:r2 view_flags) :r7 :r8 :r9 :r10))
// 		(vp-add-rr :r7 :r9)
// 		(vp-add-rr :r8 :r10)
// 		(assign '((:rsp vis_root) (:r2 hmap_parent)) '(:r3 :r1))
// 		(vpif '(:r2 = :r3))
// 			(vp-cpy-rr :r2 :r1)
// 		(endif)
// 		(vp-add-cr view_dirty_region :r1)
// 		(call 'region :paste_rect '(:r0 :r1 :r7 :r8 :r9 :r10))
// 	(endif)

// 	(exit 'view :forward_tree_callback '((:rsp vis_this) :r1))
// 	(vp-free vis_size)
// 	(vp-ret)

// (vp-label 'distribute_down_callback)
// 	(def-struct dist 0
// 		(ptr this data next))

// 	;save inputs
// 	(vp-alloc dist_size)
// 	(entry 'view :forward_tree_callback '((:rsp dist_this) :r1))
// 	(assign '(:r1) '((:rsp dist_data)))

// 	;region heap
// 	(fn-bind 'sys/statics/statics :r0)
// 	(vp-add-cr statics_gui_rect_heap :r0)

// 	;copy view from parent if not root
// 	(assign '((:rsp dist_this) (:r1 local_root)) '(:r2 :r3))
// 	(vpif '(:r2 /= :r3))
// 		;remove opaque region from ancestors
// 		(assign '((:r2 hmap_parent) (:r2 view_ctx_x) (:r2 view_ctx_y)
// 			(:r2 view_w) (:r2 view_h)) '(:r1 :r7 :r8 :r9 :r10))
// 		(vp-add-rr :r7 :r9)
// 		(vp-add-rr :r8 :r10)
// 		(vp-add-cr view_dirty_region :r1)
// 		(vp-add-cr view_dirty_region :r2)

// 		;copy my area from parent
// 		(call 'region :copy_rect '(:r0 :r1 :r2 :r7 :r8 :r9 :r10) '(:r0))

// 		;did we find any opaque region ?
// 		(assign '((:rsp dist_this)) '(:r2))
// 		(assign '((:r2 view_dirty_region)) '(:r1))
// 		(vpif '(:r1 /= 0))
// 			;remove my opaque region from ancestors
// 			(assign '((:r2 view_flags)) '(:r3))
// 			(vp-and-cr view_flag_opaque :r3)
// 			(vpif '(:r3 /= 0))
// 				;remove entire view from ancestors
// 				(loop-start)
// 					(assign '((:r2 hmap_parent)) '(:r1))
// 					(assign '(:r1) '((:rsp dist_next)))

// 					;clip to parent
// 					(assign '((:r1 view_ctx_x) (:r1 view_ctx_y) (:r1 view_w) (:r1 view_h)) '(:r11 :r12 :r13 :r14))
// 					(vp-add-rr :r11 :r13)
// 					(vp-add-rr :r12 :r14)
// 					(breakif '(:r7 >= :r13) '(:r8 >= :r14) '(:r9 <= :r11) '(:r10 <= :r12))
// 					(vpif '(:r7 < :r11))
// 						(assign '(:r11) '(:r7))
// 					(endif)
// 					(vpif '(:r8 < :r12))
// 						(assign '(:r12) '(:r8))
// 					(endif)
// 					(vpif '(:r9 > :r13))
// 						(assign '(:r13) '(:r9))
// 					(endif)
// 					(vpif '(:r10 > :r14))
// 						(assign '(:r14) '(:r10))
// 					(endif)

// 					(vp-add-cr view_dirty_region :r1)
// 					(call 'region :remove_rect '(:r0 :r1 :r7 :r8 :r9 :r10) '(:r0))

// 					(assign '((:rsp dist_data) (:rsp dist_next)) '(:r1 :r2))
// 					(assign '((:r1 local_root)) '(:r1))
// 				(loop-until '(:r1 = :r2))
// 			(else)
// 				;remove opaque region from ancestors
// 				(assign '(:r2) '(:r1))
// 				(loop-start)
// 					(assign '((:r1 hmap_parent)) '(:r2))
// 					(assign '(:r2) '((:rsp dist_next)))

// 					(assign '((:rsp dist_this)) '(:r1))
// 					(assign '((:r1 view_ctx_x) (:r1 view_ctx_y)) '(:r7 :r8))
// 					(vp-add-cr view_opaque_region :r1)
// 					(vp-add-cr view_dirty_region :r2)
// 					(call 'region :remove_region '(:r0 :r1 :r2 :r7 :r8) '(:r0))

// 					(assign '((:rsp dist_data) (:rsp dist_next)) '(:r2 :r1))
// 					(assign '((:r2 local_root)) '(:r2))
// 				(loop-until '(:r1 = :r2))
// 			(endif)

// 			;return flag for recursion or not
// 			(assign '((:rsp dist_this)) '(:r2))
// 			(assign '((:r2 view_dirty_region)) '(:r1))
// 		(endif)
// 	(endif)

// 	;:r1 will be 0 or not depending on haveing any dirty region
// 	(exit 'view :forward_tree_callback '((:rsp dist_this) :r1))
// 	(vp-free dist_size)
// 	(vp-ret)

// (vp-label 'distribute_up_callback)
// 	;add myself to draw list if not empty
// 	(entry 'view :forward_tree_callback '(:r0 :r1))

// 	(assign '((:r0 view_dirty_region)) '(:r2))
// 	(vpif '(:r2 /= 0))
// 		(vp-lea-i :r0 view_ctx_node :r2)
// 		(vp-add-cr local_ctx_flist :r1)
// 		(ln-add-fnode :r1 0 :r2 :r3)
// 	(endif)

// 	(exit 'view :forward_tree_callback '(:r0 :r1))
// 	(vp-ret)

// (def-func-end)
