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
	Noodle(const std::string& name, Entry f, void *data = nullptr, int ssize = default_stack_size);
	~Noodle();
	void yield();
	void end();
	void* get_user() { return user; }
	const std::string& get_name() { return name; }
	NoodleManager *manager;
protected:
	State state;
	Entry start;
	ucontext_t uc;
	int stack_size;
	unsigned char *stack;
	std::string name;
	void *user;
};

struct NoodleManager {
	friend Noodle;
public:
	constexpr static const int default_stack_size = 8 * 1024;
	NoodleManager(void *share_data = NULL, int ssize = default_stack_size);
	~NoodleManager();
	void start();
	void exit() { exit_flag = true; }
	void remove(const std::string& name);
	void remove_all(const std::string& name);
	void set_next(const std::string& name);
	int new_noodle(const std::string& name, Noodle::Entry f, void *data = nullptr, int ssize = Noodle::default_stack_size);
	void *get_share() { return share; }
protected:
	static void noodle_manager_manage(NoodleManager *b);
	void manage();
	std::list<Noodle *> active_noodles;
	std::list<Noodle *> pause_noodles;
	ucontext_t uc;
	ucontext_t uc_end;
	int stack_size;
	unsigned char *stack;
	bool exit_flag;
	void *share;
};


} // endof namespace Bowl

#endif

