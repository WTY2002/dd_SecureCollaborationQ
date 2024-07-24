/**
* @author: WTY
* @date: 2024/7/11
* @description: Definition of constants, operations, and header files
*/

#include "SecureCollaborationQ.h"
#include<iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <queue>
#include <openssl/bn.h>
#include <thread>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <chrono>
using namespace std::chrono;

// 原始数据集
vector<vector<vector<BIGNUM*>>> rawData;

BN_CTX* CTX = BN_CTX_new();

// 定义协同的用户数
int numUsers = 3;

// 定义堆中元素的结构体
struct HeapNode {
    BIGNUM* distence;
    int index1; // 哪个用户
    int index2; // 哪个数据
    HeapNode(BIGNUM* distence, int index1, int index2) {
        this->distence = BN_dup(distence);
        this->index1 = index1;
        this->index2 = index2;
    }
    HeapNode() {
        distence = BN_CTX_get(CTX);
        index1 = -1;
        index2 = -1;
    }

    bool operator<(const HeapNode& other) const {
        return BN_cmp(distence, other.distence) > 0;
    }

};

// 打印花费的时间
void printTime(clock_t start_time,char * desc){
    clock_t end_time = clock();
    double execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    printf("%s的时间是：%f 毫秒\n",desc, execution_time);
    fflush(stdout);
}


/**
 * @Method: 从文件中读取BIGNUMs
 * @param filename 文件名
 * @return vector<vector<BIGNUM*>> BIGNUMs列表
 */
vector<vector<BIGNUM*>> readBIGNUMsFromFile(char* filename) {
    vector<vector<BIGNUM*>> data_list;
    ifstream infile(filename);

    if (!infile.is_open()) {
        cerr << "Error opening file" << endl;
        return data_list;
    }

    string line;
    while (getline(infile, line)) {
        vector<BIGNUM*> row;
        stringstream ss(line);
        string number;

        while (ss >> number) {
            BIGNUM *bn = BN_new();
            BN_dec2bn(&bn, number.c_str());
            row.push_back(bn);
        }

        data_list.push_back(row);
    }

    infile.close();
    return data_list;
}

/**
 * @Method: 从文件中读取BIGNUMs，产生两个列表
 * @param string filename 文件名
 * @param int lineNumber 行号
 * @return vector<BIGNUM*> BIGNUMs列表
 */
vector<BIGNUM*> readBIGNUMsFromFile(char* filename, int lineNumber) {
    ifstream infile(filename);
    string line;
    vector<BIGNUM*> result;
    int currentLine = 0;

    while (getline(infile, line)) {
        if (++currentLine == lineNumber) {
            istringstream iss(line);
            string number;
            while (iss >> number) {
                BIGNUM* bn = BN_new();
                BN_dec2bn(&bn, number.c_str());
                result.push_back(bn);
            }
            break;
        }
    }

    return result;
}


/**
 * @Method: 读取数据集
 * @param char* fileString 读取数据集的地址
 * @return 状态码，1：成功；0：失败
 */
int dealData(char* fileString) {
    vector<vector<BIGNUM*>> data = readBIGNUMsFromFile(fileString);

    if (data.empty()) {
        return 0;
    }

    rawData.resize(numUsers);


    int n = data.size();
    int baseSize = n / numUsers;
    int remainder = n % numUsers;

    int count = 0;
    int num = 0;

    for (int i = 0; i < numUsers; ++i) {
        num = baseSize + (i < remainder ? 1 : 0);
        vector<vector<BIGNUM*>> t(num);
        for (int j = 0; j < num; ++j) {
            t[j] = data[count++];
        }
        rawData[i] = t;
    }
    return 1;
}

/**
 * @Method: 发起查询请求
 * @param fileString 读取数据的地址
 * @param resultFilePath 输出数据的地址
 * @return 状态码，1：成功；0：失败
 */
int secureCollaborationQ(char* fileString, char* resultFilePath) {
    // 读取两行数据，第一行含有一个数据k；第二行为数据集y中的一个数据，空格分隔
    vector<vector<BIGNUM*>> data_list(2);
    data_list[0] = readBIGNUMsFromFile(fileString, 1);
    data_list[1] = readBIGNUMsFromFile(fileString, 2);

    // 将data_list[0][0]转化为int类型
    int k = static_cast<int>(BN_get_word(data_list[0][0]));

    //------------------------------------------------------------------------------------------------

    // BIGNUM* distence = BN_CTX_get(CTX);
    // BIGNUM* temp = BN_CTX_get(CTX);
    //
    // vector<priority_queue<HeapNode>> heapNodes(numUsers);
    //
    // //将以下代码按照numUsers的值开启多进程进行计算
    // for (int i = 0; i < numUsers; ++i) {
    //
    //     clock_t start = clock();
    //
    //     priority_queue<HeapNode> q;
    //     // 维护每个用户的前k个最小距离
    //     for (int j = 0; j < rawData[i].size(); ++j) {
    //         BN_zero(distence);
    //         for (int l = 0; l < rawData[i][j].size(); ++l) {
    //             BN_sub(temp, data_list[1][l], rawData[i][j][l]);
    //             BN_mul(temp, temp, temp, CTX);
    //             BN_add(distence, distence, temp);
    //         }
    //         HeapNode node = {distence, i, j};
    //         if (q.size() < k) {
    //             q.push(node);
    //         } else if (node < q.top()) {
    //             q.pop();
    //             q.push(node);
    //         }
    //     }
    //
    //     cout << "第" << i << "个用户计算k临近";
    //     printTime(start,"");
    //
    //     heapNodes[i] = q;
    // }



    vector<int> pipes(numUsers * 2);
    for (int i = 0; i < numUsers; ++i) {
        if (pipe(pipes.data() + i * 2) == -1) {
            cerr << "Pipe creation failed" << endl;
            return 0;
        }
    }

    for (int i = 0; i < numUsers; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            cerr << "Fork failed" << endl;
            return 0;
        } else if (pid == 0) { // Child process
            close(pipes[i * 2]); // Close reading end in child process
            priority_queue<HeapNode> q;
            BIGNUM* distence = BN_CTX_get(CTX);
            BIGNUM* temp = BN_CTX_get(CTX);
            for (int j = 0; j < rawData[i].size(); ++j) {
                BN_zero(distence);
                for (int l = 0; l < rawData[i][j].size(); ++l) {
                    BN_sub(temp, data_list[1][l], rawData[i][j][l]);
                    BN_mul(temp, temp, temp, CTX);
                    BN_add(distence, distence, temp);
                }
                HeapNode node = {distence, i, j};
                if (q.size() < k) {
                    q.push(node);
                } else if (node < q.top()) {
                    q.pop();
                    q.push(node);
                }
            }
            int heapSize = q.size();
            write(pipes[i * 2 + 1], &heapSize, sizeof(heapSize));
            while (!q.empty()) {
                HeapNode node = q.top();
                q.pop();
                int index1 = node.index1;
                int index2 = node.index2;
                char* distStr = BN_bn2dec(node.distence);
                write(pipes[i * 2 + 1], &index1, sizeof(index1));
                write(pipes[i * 2 + 1], &index2, sizeof(index2));
                int distLen = strlen(distStr) + 1;
                write(pipes[i * 2 + 1], &distLen, sizeof(distLen));
                write(pipes[i * 2 + 1], distStr, distLen);
                OPENSSL_free(distStr);
            }
            close(pipes[i * 2 + 1]); // Close writing end in child process
            exit(0);
        } else { // Parent process
            close(pipes[i * 2 + 1]); // Close writing end in parent process
        }
    }

    vector<priority_queue<HeapNode>> heapNodes(numUsers);
    for (int i = 0; i < numUsers; ++i) {
        int heapSize;
        read(pipes[i * 2], &heapSize, sizeof(heapSize));
        while (heapSize-- > 0) {
            HeapNode node;
            int index1, index2, distLen;
            read(pipes[i * 2], &index1, sizeof(index1));
            read(pipes[i * 2], &index2, sizeof(index2));
            read(pipes[i * 2], &distLen, sizeof(distLen));
            char* distStr = new char[distLen];
            read(pipes[i * 2], distStr, distLen);
            node.index1 = index1;
            node.index2 = index2;
            node.distence = BN_new();
            BN_dec2bn(&node.distence, distStr);
            delete[] distStr;
            heapNodes[i].push(node);
        }
        close(pipes[i * 2]); // Close reading end in parent process
    }

    for (int i = 0; i < numUsers; ++i) {
        wait(NULL); // Wait for all child processes to finish
    }



    //------------------------------------------------------------------------------------------------

    // 合并每个最大堆
    priority_queue<HeapNode> mergedHeap;
    for (int i = 0; i < numUsers; ++i) {

        while (!heapNodes[i].empty()) {
            if (mergedHeap.size() < k) {
                mergedHeap.push(heapNodes[i].top());
            } else if (heapNodes[i].top() < mergedHeap.top()) {
                mergedHeap.pop();
                mergedHeap.push(heapNodes[i].top());
            }
            heapNodes[i].pop();
        }
    }

    // 将mergedHeap内的数据写入文件
    ofstream outfile(resultFilePath);

    if (outfile.is_open()) {

        while (!mergedHeap.empty()) {
            for (int i = 0; i < rawData[mergedHeap.top().index1][mergedHeap.top().index2].size(); ++i) {
                char* bn_str = BN_bn2dec(rawData[mergedHeap.top().index1][mergedHeap.top().index2][i]);
                outfile << bn_str << " ";
                OPENSSL_free(bn_str);
            }
            outfile << endl;
            mergedHeap.pop();
        }
    }
    else {
        cerr << "Unable to open file " << resultFilePath << endl;
        return 0;
    }


    // 释放 data_list 中的 BIGNUM 对象
    for (auto& bn : data_list[0]) {
        BN_free(bn);
    }
    for (auto& bn : data_list[1]) {
        BN_free(bn);
    }
    return 1;
}

/**
 * @Method: 清理函数，用于释放 rawData 中的 BIGNUM 对象的内存
 * @return 状态码，1：成功；0：失败
 */
int cleanupRawData() {
    if (rawData.empty()) {
        return 0;
    }
    for (auto& user : rawData) {
        for (auto& vec : user) {
            for (auto& bn : vec) {
                BN_free(bn);
            }
        }
    }
    rawData.clear();
    return 1;
}
