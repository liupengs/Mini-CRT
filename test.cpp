#include "string"
#include "iostream"

using namespace std;

int main(int argc, char * argv[])
{
	string* msg = new string("hello world");
	cout << *msg << endl;
	cout <<"the second string"<<endl;
	cout << 1 << " " << 2 << " " <<endl;
	cout << endl;
	delete msg;
	return 0;
}
