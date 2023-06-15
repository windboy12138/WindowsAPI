// DefaultFunctionParameter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
void Function1(int a, int b, int c = 1, int d = 2)
{
    std::cout << a + b + c + d << std::endl;
}

void Function2(int a, int b, int c = 1, int d = 2, bool use_default = false)
{
    int res = 0;
    if (use_default)
    {
        res += a + b + c + d;
    }
    else
    {
        res += a + b;
    }
    std::cout << res << std::endl;
}

int main()
{
    Function1(1, 2);
    Function2(3, 4);
    
    Function2(3, 4, 5, 6, true);
    getchar();
}
