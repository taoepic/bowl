#include "bowl.h"
#include <iostream>

using namespace std;
using namespace Bowl;

void f1(Noodle* noodle) {
	for (int i = 0; ; i++) {
		cout << "coroutine " << noodle->get_name() << ": " << i << endl;
		if (noodle->get_name() == to_string(2) && i == 3) {
			cout << "remove routine 3 from routine " << noodle->get_name() << endl;
			noodle->manager->remove(to_string(3));
		}
		if (noodle->get_name() == to_string(4) && i == 2) {
			cout << "remove routine 5 from routine " << noodle->get_name() << endl;
			noodle->manager->remove(to_string(5));
		}
		if (i == 10) {
			/* prepare make all routine exit */
			noodle->manager->exit();
		}
		noodle->yield();
	}
}

int main() {
	NoodleManager nm;

	for (int i = 0; i < 5; i++) {
		cout << "create routine " << i << endl;
		nm.new_noodle(to_string(i), f1);
	}
	nm.start();
	cout << "all exit." << endl;
}

