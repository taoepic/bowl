#ifndef _BOWL_H__
#define _BOWL_H__

#include <iostream>
#include <list>
#include <functional>
#include <ucontext.h>

namespace Bowl {

struct NoodleManager;
struct Noodle {
	friend NoodleManager;
public:
	constexpr static const int default_stack_size = 8 * 1024;
	typedef void (*Entry)(Noodle*);
	enum class State {
		INIT,
		SUSPEND,
		RUNNING,
		READY_EXIT
	};
	Noodle(Entry f, void *data = nullptr, int ssize = default_stack_size);
	~Noodle();
	void yield();
	void end();
	void* get_user() { return user; }
	NoodleManager *manager;
protected:
	State state;
	Entry start;
	ucontext_t uc;
	int stack_size;
	unsigned char *stack;
	void *user;
};

struct NoodleManager {
	friend Noodle;
public:
	constexpr static const int default_stack_size = 2 * 1024;
	NoodleManager(int ssize = default_stack_size);
	~NoodleManager();
	void manage();
	void start();
	int new_noodle(Noodle::Entry f, void *data = nullptr, int ssize = Noodle::default_stack_size);
protected:
	std::list<Noodle *> active_noodles;
	std::list<Noodle *> pause_noodles;
	ucontext_t uc;
	ucontext_t uc_end;
	int stack_size;
	unsigned char *stack;
};

void noodle_manager_manage(NoodleManager *b);

} // endof namespace Bowl

#endif

