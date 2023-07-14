// PolymorphismsC++.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

#include "Inherit.h"


class Dad
{
public:
    virtual void Fun1() { std::cout << "I'm Fun1 of Dad" << std::endl; }

    void Fun2() { std::cout << "I'm Fun2 of Dad" << std::endl; }

    void Fun3(int input) { std::cout << "I'm Fun3 of Dad" << std::endl; }

    virtual void Fun4() {};
};

class Son : public Dad
{
public:
    virtual void Fun1() { std::cout << "I'm Fun1 of Son" << std::endl; } // override

    bool Fun2() { std::cout << "I'm Fun2 of Son" << std::endl;  return true; } // hide

    void Fun3() { std::cout << "I'm Fun3 of Son" << std::endl; } // hide
};
// override 覆盖 派生类的同名函数覆盖了基类函数，无论使用派生类指针还是基类指针均只能访问到派生类的函数实现

// hide     隐藏 派生类的同名函数将基类的函数隐藏了，使用基类指针则访问基类的函数，使用派生类的指针则访问派生类的函数
//               即便参数不同、返回值不同，只要函数名字相同，派生类就会隐藏基类的函数

// 加基类的类名限定作用域，可以直接访问基类的函数使用

int main()
{
    LocalStreamPublication* publication = new VideoCaptureDevicePublication;
    publication->StartPublish();
    publication->StopPublish();
    publication->SetVideoDeviceID(nullptr);

    std::cout << std::endl;
    LocalStreamPublication* publication1 = new ScreenCastPublication;
    publication1->StartPublish();
    publication1->StopPublish();
    publication1->SetVideoDeviceID(nullptr);
    getchar();
    return 0;

    Dad* ptr = nullptr;
    Son* ptr2 = nullptr;
    Son son;
    ptr = &son;
    ptr2 = &son;
    ptr->Fun1();
    ptr->Fun2();
    ptr->Fun3(3);
    ptr->Fun4();

    std::cout << std::endl;
    ptr2->Fun1();
    ptr2->Fun2();
    ptr2->Fun3();

    std::cout << std::endl;
    ptr2->Dad::Fun1();
    ptr2->Dad::Fun2();
    ptr2->Dad::Fun3(4);
    ptr2->Dad::Fun4();
    getchar();
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
