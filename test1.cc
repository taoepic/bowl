#include "bowl.h"
#include <iostream>

using namespace std;
using namespace Bowl;

void f3(Noodle* noodle) {
	for (int i = 0; i < 10; i++) {
		cout << "*** " << i << endl;
		noodle->yield();
	}
	cout << "coroutine exit: " << noodle->get_name() << endl;
	noodle->end();
}

void f4(Noodle* noodle) {
	for (int i = 0; i < 10; i++) {
		cout << "**** " << i << endl;
		noodle->yield();
	}
	cout << "coroutine exit: " << noodle->get_name() << endl;
	noodle->end();
}

void f1(Noodle* noodle) {
	for (int i = 0; i < 10; i++) {
		cout << "* " << i << endl;
		if (i == 5) {
			cout << "createing new coroutine f3" << endl;
			noodle->manager->new_noodle("f3", f3);
		}
		noodle->yield();
	}
	cout << "coroutine exit: " << noodle->get_name() << endl;
	noodle->end();
}

void f2(Noodle* noodle) {
	for (int i = 0; i < 16; i++) {
		cout << "** " << i << endl;
		if (i == 8) {
			cout << "createing new coroutine f4" << endl;
			noodle->manager->new_noodle("f4", f4);
		}
		noodle->yield();
	}
	cout << "coroutine exit: " << noodle->get_name() << endl;
	noodle->end();
}

int main() {
	NoodleManager nm;

	cout << "createing new coroutine f1" << endl;
	nm.new_noodle("f1", f1);
	cout << "createing new coroutine f2" << endl;
	nm.new_noodle("f2", f2);

	nm.start();
	cout << "all exit." << endl;
}

