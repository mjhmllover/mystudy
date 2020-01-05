#include <iostream>
#include <map>
using namespace std;

int main()
{
	multimap<int, int> mymap;
	mymap.insert(make_pair(1, 1000));
	mymap.insert(make_pair(1, 2000));

	typedef multimap<int, int>::iterator itmap;
	pair<itmap, itmap> pairFind = mymap.equal_range(1);
	for (itmap itmapCur = pairFind.first; pairFind.second != itmapCur; ++itmapCur)
	{
		cout << itmapCur->first << ":" << itmapCur->second << endl;
	}

	return 0;
}
