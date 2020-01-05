#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

int maxProfit(vector<int>& prices) {
	int iSum = 0;
    bool bAsecFlag = false;
        
	if (prices.empty() || (1 == prices.size()))
	{
		return 0;
    }
        
    /* 
  	 * 出正向连续组，求出该组首尾之差，累加差值
     * 若第i+1个数比第i个数要大，则更新尾数；反之，则更新首数
     */
	int iAsecFirst = prices[0];
    int iAsecLast = prices.back();
    vector<int>::iterator itvecPrices = prices.begin() + 1;
    for (; prices.end() != itvecPrices; ++itvecPrices)
    {
        if (iAsecFirst < (*itvecPrices))
        {
			bAsecFlag = true;
			iAsecLast = *itvecPrices;
        }
        else
        {
        if (bAsecFlag)
        {
            iSum += iAsecLast - iAsecFirst;
            bAsecFlag = false;
        }
        
        iAsecFirst = *itvecPrices;
        }
    }
        
    if (bAsecFlag)
    {
        iSum += iAsecLast - iAsecFirst;
    }
        
    return iSum;
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
    while (getline(cin, line)) {
        vector<int> prices = stringToIntegerVector(line);
        
        int ret = maxProfit(prices);

        cout << ret << endl;
    }
    return 0;
}
