#include <iostream>
#include <list>
#include <functional>
#include <algorithm>
#include <ucontext.h>
#include "bowl.h"

using namespace Bowl;

Noodle::Noodle(const std::string& n, Entry f, void *data, int ssize) 
		: name(n), stack_size(ssize), user(data), start(f) {
	stack = new unsigned char[stack_size];
}

Noodle::~Noodle() {
	delete []stack;
}

void Noodle::yield() {
	state = Noodle::State::SUSPEND;
	swapcontext(&uc, &manager->uc);
}

void Noodle::end() {
	state = Noodle::State::READY_EXIT;
	swapcontext(&uc, &manager->uc);
}

NoodleManager::NoodleManager(void *s, int ssize) : share(s), stack_size(ssize), exit_flag(false) {
	stack = new unsigned char[stack_size];
	getcontext(&uc);
	uc.uc_link = &uc_end;
	uc.uc_stack.ss_sp = stack;
	uc.uc_stack.ss_size = stack_size;
	makecontext(&uc, (void (*)(void))noodle_manager_manage, 1, this);
}

NoodleManager::~NoodleManager() {
	std::for_each(active_noodles.begin(), active_noodles.end(), [](Noodle *n) {
		delete n;
	});
	std::for_each(chans.begin(), chans.end(), [](ChanBase* cb) {
		delete cb;
	});
	delete[] stack;
}

void NoodleManager::set_next(const std::string& name) {
	Noodle *n = nullptr;
	for (std::list<Noodle*>::iterator it = active_noodles.begin(); it != active_noodles.end(); it++) {
		if ((*it)->name == name) {
			n = *it;
			break;
		}
	}
	if (n != nullptr) {
		std::list<Noodle*>::iterator pos = ++active_noodles.begin();
		active_noodles.insert(pos, n);
	}
}

void NoodleManager::wakeup(const std::string& name) {
	Noodle *n = nullptr;
	for (std::list<Noodle*>::iterator it = active_noodles.begin(); it != active_noodles.end(); it++) {
		if ((*it)->name == name) {
			n = *it;
			break;
		}
	}
	if (n != nullptr) {
		std::list<Noodle*>::iterator pos = ++active_noodles.begin();
		active_noodles.insert(pos, n);
		active_noodles.front()->yield();
	}
}

void NoodleManager::remove(const std::string& name) {
	for (std::list<Noodle*>::iterator it = active_noodles.begin(); it != active_noodles.end(); it++) {
		if ((*it)->name == name) {
			delete (*it);
			active_noodles.erase(it);
			return;
		}
	}
}

void NoodleManager::remove_all(const std::string& name) {
	std::list<Noodle*>::iterator it = active_noodles.begin();
	while (it != active_noodles.end()) {
		if ((*it)->name == name) {
			delete (*it);
			it = active_noodles.erase(it);
		} else
			it++;
	}
}

void NoodleManager::manage() {
	while (active_noodles.size() > 0 && !exit_flag) {
		Noodle* n = active_noodles.front();
		if (n->state == Noodle::State::INIT) {
			n->state = Noodle::State::RUNNING;
			swapcontext(&uc, &n->uc);
			continue;
		} 
		active_noodles.pop_front();
		if (n->state == Noodle::State::SUSPEND) {
			active_noodles.push_back(n);
		} else if (n->state == Noodle::State::READY_EXIT) {
			delete n;
		}
		if (active_noodles.size() > 0 && !exit_flag) {
			Noodle* m = active_noodles.front();
			m->state = Noodle::State::RUNNING;
			swapcontext(&uc, &m->uc);
		}
	}
	swapcontext(&uc, &uc_end);
}

void NoodleManager::start() {
	swapcontext(&uc_end, &uc);
}

int NoodleManager::new_noodle(const std::string &name, Noodle::Entry f, void *data, int ssize) {
	Noodle *noodle = new Noodle(name, f, data, ssize);
	getcontext(&noodle->uc);
	noodle->uc.uc_link = &uc;
	noodle->uc.uc_stack.ss_sp = noodle->stack;	
	noodle->uc.uc_stack.ss_size = noodle->stack_size;
	makecontext(&noodle->uc, (void(*)(void))f, 1, noodle);
	noodle->manager = this;
	noodle->state = Noodle::State::INIT;
	active_noodles.push_back(noodle);
}

void NoodleManager::noodle_manager_manage(NoodleManager *nm) {
	nm->manage();
}

