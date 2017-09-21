#include <iostream>
#include <list>
#include <functional>
#include <ucontext.h>
#include "bowl.h"

using namespace Bowl;

Noodle::Noodle(Entry f, void *data, int ssize) : stack_size(ssize), user(data), start(f) {
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

NoodleManager::NoodleManager(int ssize) : stack_size(ssize) {
	stack = new unsigned char[stack_size];
	getcontext(&uc);
	uc.uc_link = &uc_end;
	uc.uc_stack.ss_sp = stack;
	uc.uc_stack.ss_size = stack_size;
	makecontext(&uc, (void (*)(void))Bowl::noodle_manager_manage, 1, this);
}

NoodleManager::~NoodleManager() {
	delete[] stack;
}

void NoodleManager::manage() {
	while (true) {
		if (active_noodles.size() == 0)
			break;
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
		if (active_noodles.size()) {
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

int NoodleManager::new_noodle(Noodle::Entry f, void *data, int ssize) {
	Noodle *noodle = new Noodle(f, data, ssize);
	getcontext(&noodle->uc);
	noodle->uc.uc_link = &uc;
	noodle->uc.uc_stack.ss_sp = noodle->stack;	
	noodle->uc.uc_stack.ss_size = noodle->stack_size;
	makecontext(&noodle->uc, (void(*)(void))f, 1, noodle);
	noodle->manager = this;
	noodle->state = Noodle::State::INIT;
	active_noodles.push_back(noodle);
}

void Bowl::noodle_manager_manage(NoodleManager *nm) {
	nm->manage();
}

