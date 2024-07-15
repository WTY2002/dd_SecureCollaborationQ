/**
* @author: WTY
* @date: 2024/7/11
* @description: Definition of constants, operations, and header files
*/

#ifndef SECURECOLLABORATIONQ_H
#define SECURECOLLABORATIONQ_H


#include<cstring>
#include <iostream>
#ifdef _WIN32
#define EXPORT_SYMBOL __declspec(dllexport)
#else
#define EXPORT_SYMBOL __attribute__((visibility("default")))
#endif

using namespace std;

// 打印花费的时间
void printTime(clock_t start_time,char * desc);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	/**
	 * @Method: 读取数据集
	 * @param char* fileString 读取数据集的地址
	 */
	EXPORT_SYMBOL void dealData(char* fileString);

	/**
	 * @Method: 发起查询请求
	 * @param fileString 读取数据的地址
	 * @param resultFilePath 输出数据的地址
	 * @return 状态码，1：成功；0：失败
	 */
	EXPORT_SYMBOL int secureCollaborationQ(char* fileString, char* resultFilePath);

#ifdef __cplusplus
}
#endif

// 清理函数，用于释放 rawData 中的 BIGNUM 对象的内存
void cleanupRawData();



#endif //SECURECOLLABORATIONQ_H
