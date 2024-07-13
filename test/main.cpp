#include <iostream>
#include <vector>
#include <fstream>
#include <SecureCollaborationQ.h>
#include <random>
#include <ctime>
#include <openssl/bn.h>
#include<iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <openssl/crypto.h>
#include <mutex>
#include <sys/wait.h>
using namespace std;


vector<vector<BIGNUM*>> readBIGNUMsFromFile111(char* filename) {
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

// // 打印花费的时间
// void printTime(clock_t start_time,char * desc){
//     clock_t end_time = clock();
//     double execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
//     printf("%s的时间是：%f 毫秒\n",desc, execution_time);
//     fflush(stdout);
// }

// 往文件中写入30万条数据，每条数据的维度是128维，每个数据都是1-1000的随机数
void creatData() {
    char* fileString = "/root/wty/rawData.txt";
    ofstream outfile(fileString);

    if (!outfile.is_open()) {
        cerr << "Error opening file for writing" << endl;
        return;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000);

    for (int i = 0; i < 1000000; ++i) {
        for (int j = 0; j < 128; ++j) {
            if (j != 0) {
                outfile << " ";
            }
            outfile << dis(gen);
        }
        outfile << "\n";
    }

    outfile.close();

}

// 读取文件内容并输出
void readFile(char* fileString) {
    vector<vector<BIGNUM*>> data = readBIGNUMsFromFile111(fileString);

    cout << "data.size() = " << data.size() << endl;

    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < data[i].size(); ++j) {
            cout << BN_bn2dec(data[i][j]) << " ";
        }
        cout << endl;
    }
}

int main()
{


    // creatData();

    char* fileString = "/root/wty/rawData.txt";
    char* query = "/root/wty/query.txt";
    char* res = "/root/wty/result.txt";

    clock_t start1 = clock();
    dealData(fileString);
    printTime(start1,"数据读取");

    clock_t start2 = clock();
    secureCollaborationQ(query, res);
    printTime(start2,"查询");

    readFile(res);


    return 0;
}
