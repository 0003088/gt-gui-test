#include <ckdb.h>
#include <iostream>
#include <string>

using namespace std;

int main()
{
	ckdb config("sw/test");

	config.set ("t1", "�hhm, S��e!");
	cout << config.get ("t1") << endl;
	//config.set ("t2", "=()=<NO>");
	//config.set ("t3", "�$ -- $�");
	config.set ("t1", "successfully overwritten");
	cout << config.get ("t1") << endl;
	config.setFlag("t1");
	return 0;
}
