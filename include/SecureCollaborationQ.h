/**
* @author: WTY
* @date: 2024/7/11
* @description: Definition of constants, operations, and header files
*/

#ifndef SECURECOLLABORATIONQ_H
#define SECURECOLLABORATIONQ_H


#include<cstring>
#include <iostream>
using namespace std;

// 打印花费的时间
void printTime(clock_t start_time,char * desc);


/**
 * @Method: 读取数据集
 * @param char* fileString 读取数据集的地址
 */
void dealData(char* fileString);


/**
 * @Method: 发起查询请求
 * @param fileString 读取数据的地址
 * @param resultFilePath 输出数据的地址
 * @return 状态码，1：成功；0：失败
 */
int secureCollaborationQ(char* fileString, char* resultFilePath);



#endif //SECURECOLLABORATIONQ_H
