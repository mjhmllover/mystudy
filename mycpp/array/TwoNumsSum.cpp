#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;

vector<int> twoSum(vector<int>& nums, int target) {
	vector<int> vecRes;
	multimap<int, int> mapLoad;
	for (int i = 0; i < nums.size(); ++i)
	{
		/* 不支持直接下标引用插入 */
		// mapLoad[nums[i]] = i;
		mapLoad.insert(make_pair(nums[i], i));
	}
	
	typedef multimap<int, int>::iterator itmap;
	pair<itmap, itmap> pairGet;
	itmap itmapFirst;
	itmap itmapSecond;
	for (int i = 0; i < nums.size(); ++i)
	{
		pairGet = mapLoad.equal_range(nums[i]);
		/* 找得到只有两种情况：
		 * 1. pairGet只有一个iterator，且找到的key与当前元素值不等
		 * 2. pairGet有两个iterator
		 */
		itmapFirst = pairGet.first;
		itmapSecond = pairGet.second;
		if ((1 == itmapSecond - itmapFirst) && (nums[i] != itmapFirst->first))
		{
			vecRes.push_back(i);
			vecRes.push_back((pairGet.first)->second);
			return vecRes;
		}
		else if (2 == pairGet.second - pairGet.first)
		{
			int iOther = (i == (pairGet.first)->second) ? ((pairGet.first + 1)->second) : ((pairGet.first)->second);  
			vecRes.push_back(i);
			vecRes.push_back(iOther);
			return vecRes;
		}
	}
	
	return vecRes;
}

vector<int> stringToIntegerVector(string input) {
    vector<int> output;
    input = input.substr(1, input.length() - 2);
    stringstream ss;
    ss.str(input);
    string item;
    char delim = ',';
    stringstream ssTmp;
    int unit = 0;
    while (getline(ss, item, delim)) {
		ssTmp << item;
		ssTmp >> unit;
        output.push_back(unit);
		ssTmp.clear();
    }
    return output;
}

int main() {
    string line;
	vector<int> vecInput;
    if (getline(cin, line)) {
        vecInput = stringToIntegerVector(line);
	}
	
	string num;
	int target;
	if (getline(cin, num)) {
		stringstream ssTmp;
		ssTmp << num;
		ssTmp >> target;
	}
        
	vector<int> ret = twoSum(vecInput, target);

	cout << "[ ";
	for (int i = 0; i < ret.size(); ++i)
	{
		cout << ret[i] << " ";
    }
	cout << "]" << endl;
	
    return 0;
}
