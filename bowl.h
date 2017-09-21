#ifndef _BOWL_H__
#define _BOWL_H__

#include <iostream>
#include <list>
#include <ucontext.h>

namespace Bowl {

class NoodleManager;
class Noodle {
	friend class NoodleManager;
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

class ChanBase {
protected:
	int cap;
	int len;
	int head;
public:
	ChanBase(int _cap) : cap(_cap), len(0), head(0) {}
	virtual ~ChanBase() {}
};

template <typename T, int C = 1>
struct Chan : public ChanBase {
	T item[C];
	Chan<T,C>() : ChanBase(C) {}
	const T& get(Noodle* n) {
		while (len == 0) {
			n->yield();
		}
		len --;
		const T& v = item[head++];
		if (head >= cap)
			head = 0;
		return v;
	}
	void put(Noodle* n, const T& t) {
		while (len == cap) {
			n->yield();
		} 
		len ++;
		int tail = head + len;
		tail = (tail >= cap) ? tail - cap : tail;
		item[tail] = t;
	}
	virtual ~Chan<T,C>() {}
};

class NoodleManager {
	friend class Noodle;
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
	template<typename T, int Size = 1> 
	Chan<T,Size>* new_chan() {
		Chan<T,Size>* chan = new Chan<T,Size>();
		chans.push_back(chan);
		return chan;
	};
	void *get_share() { return share; }
protected:
	static void noodle_manager_manage(NoodleManager *b);
	void manage();
	std::list<Noodle*> active_noodles;
	std::list<Noodle*> pause_noodles;
	std::list<ChanBase*> chans;
	ucontext_t uc;
	ucontext_t uc_end;
	int stack_size;
	unsigned char *stack;
	bool exit_flag;
	void *share;
};

} // endof namespace Bowl

#endif

