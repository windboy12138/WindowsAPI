#pragma once
#include <iostream>

class LocalStreamPublication
{
public:
    virtual void StartPublish() {};
    virtual void StopPublish() {};

    virtual bool SetVideoDeviceID(const char* device_id) { return false; };
};

class VideoCaptureDevicePublication : public LocalStreamPublication
{
    void StartPublish() 
    { 
        std::cout << "Call StartPublish() of VideoCaptureDevicePublication" << std::endl; 
        std::cout << "Excute Start() Function of VideoCaptureDevicePublication indeed" << std::endl;
    }
    void StopPublish() 
    { 
        std::cout << "Call StoptPublish() of VideoCaptureDevicePublication" << std::endl;
        std::cout << "Excute Stop() Fcuntion of VideoCaptureDevicePublication indeed" << std::endl;
    }

    bool SetVideoDeviceID(const char* device_id) 
    { 
        std::cout << "Call SetVideoDeviceID() of VideoCaptureDevicePublication" << std::endl;
        std::cout << "Excute SetCaptureDeivceID of VideoCaptureDevicePublication indeed" << std::endl; 
        return false;
    };
};

class ScreenCastPublication : public LocalStreamPublication
{
    void StartPublish() { std::cout << "Call StartPublish() of ScreenCastPublication" << std::endl; }
    void StopPublish() { std::cout << "Call StoptPublish() of ScreenCastPublication" << std::endl; }
};