#include "bowl.h"
#include <iostream>

using namespace std;
using namespace Bowl;

void sender(Noodle* noodle) {
	string name = noodle->get_name();
	int index = std::stoi(name);
	cout << "sender " << name << " -> " << index << endl;
	Chan<int> *chan = static_cast<Chan<int>*>(noodle->get_user());
	chan->put(noodle, std::stoi(noodle->get_name()));
}

void receiver(Noodle *noodle) {
	Chan<int> *chan = static_cast<Chan<int>*>(noodle->get_user());
	for (int i = 0; i < 5; i++) {
		cout << "recv " << noodle->get_name() << " -> " << chan->get(noodle) << endl;
	}
}

int main() {
	NoodleManager nm;

	Chan<int> *chan = nm.new_chan<int>();
	for (int i = 0; i < 10; i++) {
		nm.new_noodle(to_string(i), sender, (void*)chan);
	}
	nm.new_noodle("receiver1", receiver, (void*)chan);
	nm.new_noodle("receiver2", receiver, (void*)chan);

	nm.start();
	cout << "all exit." << endl;
}

