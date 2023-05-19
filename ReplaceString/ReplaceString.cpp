// ReplaceString.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    cout << argv[1] << "\n";
    cout << argv[2] << "\n";
    string file(argv[1]);

    fstream fixstream(file);
    string version(argv[2]);

    if (!fixstream)
    {
        cerr << "Could not open " << argv[1] << "\n";
        return 1;
    }

    string line;
    size_t cur; // 记录读指针位置
    size_t pos = 0;
    while (getline(fixstream, line))
    {
        pos = line.find("#define HIRTC_VERSION_STR");
        if (pos != string::npos)
        {
            cur = fixstream.tellg();
            size_t len = strlen(line.c_str()) + 2;
            fixstream.seekp(-1 * len, fstream::cur); // (读写指针本来在相同位置),此时写指针回退到上一行
            line.replace(pos, 3, ("#define HIRTC_VERSION_STR \"" + version + "\""));
            fixstream << line;
            fixstream.seekp(cur);
            break;
        }
    }

    return 0;
}
