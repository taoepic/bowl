#include "bowl.h"
#include <iostream>

using namespace std;
using namespace Bowl;

void f1(Noodle* noodle) {
	cout << "current coroutine: " << noodle->get_name() << endl;
	noodle->yield();
}

int main() {
	NoodleManager nm;

	for (int i = 0; i < 5000; i++) {
		cout << "create routine " << i << endl;
		nm.new_noodle(to_string(i), f1);
	}
	nm.start();
	cout << "all exit." << endl;
}

