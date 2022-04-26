/*
 * =====================================================================================
 *
 *       Filename:  chosenum.cpp
 *
 *    Description:  已知 n 个整数，以及1个整数k(k<n)。从n个整数中任选k个整数相加，可分别得到一系列的和。计算出和为素数共有多少种。
 *
 *        Version:  1.0
 *        Created:  2021年04月02日 15时29分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  mooyoung (NO), mai.jianhua@zte.com.cn
 *   Organization:  Platform R & D Department Two
 *
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <sstream>
#include <numeric>

using namespace std;
int g_iPrimeNum = 0;


bool IsPrime(int iNum)
{
    for (int i = 2; i * i < iNum + 1; i++)
    {
        if (0 == iNum % i)
        { 
            return false;
        }
    }

    return true;
}

void get_all_sum(int iCurPos, int iSum, int iChoseNum, int iAllNum, int iPickNum, vector<int> &vecNums)
{
    /* 递归结束条件 */
    if (iCurPos == iAllNum || iChoseNum == iPickNum)
    {
        if (iChoseNum == iPickNum && IsPrime(iSum))
        {
            g_iPrimeNum++;
            return;
        }

        return;
    }

    /* 取定当前数，分两种情况考虑：
     * 1）iSum不包含当前数；
     * 2）iSum包含当前数；
     */
    get_all_sum(iCurPos + 1, iSum, iChoseNum, iAllNum, iPickNum, vecNums);
    get_all_sum(iCurPos + 1, iSum + vecNums.at(iCurPos), iChoseNum + 1, iAllNum, iPickNum, vecNums);

    return;
}

int main()
{
    /* 获取输入 */
    int iAllNum = 0;
    int iPickNum = 0;

    cin >> iAllNum >> iPickNum;

    vector<int> vecNums;
    int iTmp = 0;

    int iCount = iAllNum;
    while (iCount--)
    {
        cin >> iTmp;
        vecNums.push_back(iTmp);
    }

    /* 打印所有输入 */
    // stringstream ss;
    // vector<int>::iterator iter = vecNums.begin();
    // for (; vecNums.end() != iter; ++iter)
    // {
    //     ss << *iter << " ";
    // }

    // cout << "Input line1: " << iAllNum << " " << iPickNum << endl;
    // cout << "Input line2: " << ss.str() << endl; 
    // ss.clear();

    /* 递归获取所有组合 */
    get_all_sum(0, 0, 0, iAllNum, iPickNum, vecNums);

    cout << g_iPrimeNum << endl;
    
    return 0;
}
