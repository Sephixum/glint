#ifndef GLINT_BASE_UTIL_LINKED_LIST_H
#define GLINT_BASE_UTIL_LINKED_LIST_H

#define g_check_nil(nil, p) ((p) == 0 || (p) == nil)
#define g_set_nil(nil, p) ((p) = nil)

#define g_double_ll_insert_npz(nil, head, tail, position, new_node, next, prev)                                        \
	(g_check_nil(nil, head)                                                                                            \
		 ? ((head) = (tail) = (new_node), g_set_nil(nil, (new_node)->next), g_set_nil(nil, (new_node)->prev))          \
	 : g_check_nil(nil, position) ? ((new_node)->next = (head),                                                        \
									 (head)->prev	  = (new_node),                                                    \
									 (head)			  = (new_node),                                                    \
									 g_set_nil(nil, (new_node)->prev))                                                 \
	 : ((position) == (tail))	  ? ((tail)->next	  = (new_node),                                                    \
									 (new_node)->prev = (tail),                                                        \
									 (tail)			  = (new_node),                                                    \
									 g_set_nil(nil, (new_node)->next))                                                 \
								  : (((!g_check_nil(nil, position) && g_check_nil(nil, (position)->next))              \
										  ? (0)                                                                        \
										  : ((position)->next->prev = (new_node))),                                    \
									 ((new_node)->next = (position)->next),                                            \
									 ((position)->next = (new_node)),                                                  \
									 ((new_node)->prev = (position))))

#define g_double_ll_push_back_npz(nil, head, tail, new_node, next, prev)                                               \
	g_double_ll_insert_npz(nil, head, tail, tail, new_node, next, prev)

#define g_double_ll_push_front_npz(nil, head, tail, new_node, next, prev)                                              \
	g_double_ll_insert_npz(nil, tail, head, head, new_node, prev, next)

#define g_double_ll_remove_npz(nil, head, tail, node, next, prev)                                                      \
	(((node) == (head) ? (head) = (node)->next : (0)),                                                                 \
	 ((node) == (tail) ? (tail) = (tail)->prev : (0)),                                                                 \
	 (g_check_nil(nil, (node)->prev) ? (0) : ((node)->prev->next = (node)->next)),                                     \
	 (g_check_nil(nil, (node)->next) ? (0) : ((node)->next->prev = (node)->prev)))

// singly-linked, doubly-headed lists (queues)
#define g_single_ll_queue_push_nz(nil, head, tail, node, next)                                                         \
	(g_check_nil(nil, head) ? ((head) = (tail) = (node), g_set_nil(nil, (node)->next))                                 \
							: ((tail)->next = (node), (tail) = (node), g_set_nil(nil, (node)->next)))

#define g_single_ll_queue_push_front_nz(nil, head, tail, node, next)                                                   \
	(g_check_nil(nil, head) ? ((head) = (tail) = (node), g_set_nil(nil, (node)->next))                                 \
							: ((node)->next = (head), (head) = (node)))

#define g_single_ll_queue_pop_nz(nil, head, tail, next)                                                                \
	((head) == (tail) ? (g_set_nil(nil, head), g_set_nil(nil, tail)) : ((head) = (head)->next))

// singly-linked, singly-headed lists (stacks)
#define g_single_ll_stack_push_n(head, node, next) ((node)->next = (head), (head) = (node))
#define g_single_ll_stack_pop_n(head, next) ((head) ? ((head) = (head)->next) : (0))

// doubly-linked-list helpers
#define g_double_ll_insert_np(head, tail, position, node, next, prev)                                                  \
	g_double_ll_insert_npz(0, head, tail, position, node, next, prev)

#define g_double_ll_push_back_np(head, tail, node, next, prev)                                                         \
	g_double_ll_push_back_npz(0, head, tail, node, next, prev)

#define g_double_ll_push_front_np(head, tail, node, next, prev)                                                        \
	g_double_ll_push_front_npz(0, head, tail, node, next, prev)

#define g_double_ll_remove_np(head, tail, node, next, prev) g_double_ll_remove_npz(0, head, tail, node, next, prev)

#define g_double_ll_insert(head, tail, position, node) g_double_ll_insert_npz(0, head, tail, position, node, next, prev)

#define g_double_ll_push_back(head, tail, node) g_double_ll_push_back_npz(0, head, tail, node, next, prev)

#define g_double_ll_push_front(head, tail, node) g_double_ll_push_front_npz(0, head, tail, node, next, prev)

#define g_double_ll_remove(head, tail, node) g_double_ll_remove_npz(0, head, tail, node, next, prev)

// singly-linked, doubly-headed list helpers
#define g_single_ll_queue_push_n(head, tail, node, next) g_single_ll_queue_push_nz(0, head, tail, node, next)

#define g_single_ll_queue_push_front_n(head, tail, node, next)                                                         \
	g_single_ll_queue_push_front_nz(0, head, tail, node, next)

#define g_single_ll_queue_pop_n(head, tail, next) g_single_ll_queue_pop_nz(0, head, tail, next)

#define g_single_ll_queue_push(head, tail, node) g_single_ll_queue_push_nz(0, head, tail, node, next)

#define g_single_ll_queue_push_front(head, tail, node) g_single_ll_queue_push_front_nz(0, head, tail, node, next)

#define g_single_ll_queue_pop(head, tail) g_single_ll_queue_pop_nz(0, head, tail, next)

#define g_single_ll_stack_push_n(head, node, next) ((node)->next = (head), (head) = (node))

#define g_single_ll_stack_push(head, node) g_single_ll_stack_push_n(head, node, next)
#define g_single_ll_stack_pop(head) g_single_ll_stack_pop_n(head, next)

#endif // GLINT_BASE_UTIL_LINKED_LIST_H
