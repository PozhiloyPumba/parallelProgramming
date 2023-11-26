#ifndef IOPENCL_APP_HPP__
#define IOPENCL_APP_HPP__

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_QUEUE_PROFILING_ENABLE

#include <CL/opencl.hpp>

namespace OpenCLApp {

class Timer {
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;

public:
    void timerInit () { start_ = std::chrono::high_resolution_clock::now (); }
    void timerEnd () { end_ = std::chrono::high_resolution_clock::now (); }
    long getTimeNs ()
    {
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::nanoseconds> (end_ - start_);
        return elapsed_seconds.count ();
    }
    long getTimeMs ()
    {
        return getTimeNs () / 1000000;
    }
};

// clang-format off
template <typename T>
const std::string getTypeName ();

template <> inline const std::string getTypeName<int> ()                {   return "int"; }
template <> inline const std::string getTypeName<float> ()              {   return "float"; }
template <> inline const std::string getTypeName<double> ()             {   return "double"; }
template <> inline const std::string getTypeName<short> ()              {   return "short"; }
template <> inline const std::string getTypeName<char> ()               {   return "char"; }
template <> inline const std::string getTypeName<long long int> ()      {   return "long int"; }
template <> inline const std::string getTypeName<unsigned char> ()      {   return "unsigned char"; }
template <> inline const std::string getTypeName<unsigned short> ()     {   return "unsigned short"; }
template <> inline const std::string getTypeName<unsigned int> ()       {   return "unsigned int"; }
template <> inline const std::string getTypeName<unsigned long long> () {   return "unsigned long"; }

template<typename T> inline const std::string getKernelExtension() { return ""; }

template<> inline const std::string getKernelExtension<double>() {
    return "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n";
}

// clang-format on

// helper for get local group if you are lazy
inline cl::NDRange getLocalRangeOptimal(const cl::NDRange &globalRange, const cl::size_type maxLocalGroupSize) {
    cl::size_type globalSize = globalRange[0] * globalRange[1] * globalRange[2];

    switch(globalRange.dimensions()) {
        case 1: {
            cl::size_type trueLocalGroupSize = maxLocalGroupSize;
            for(; trueLocalGroupSize > 0; --trueLocalGroupSize) {
                if (globalSize % trueLocalGroupSize == 0)
                    break;
            }
            return cl::NDRange(trueLocalGroupSize);
        }
        case 2: {
            cl::size_type trueLocalGroupSize = maxLocalGroupSize;
            for(; trueLocalGroupSize > 0; --trueLocalGroupSize) {
                if (globalSize % trueLocalGroupSize == 0)
                    break;
            }
            cl::size_type localGroupSizeX = std::sqrt(trueLocalGroupSize);
            for(; localGroupSizeX > 0; --localGroupSizeX) {
                if ((trueLocalGroupSize % localGroupSizeX == 0) && (globalRange[0] % localGroupSizeX == 0))
                    break;
            }
            cl::size_type localGroupSizeY = trueLocalGroupSize / localGroupSizeX;
            for(; localGroupSizeY > 0; --localGroupSizeY) {
                if ((trueLocalGroupSize % localGroupSizeY == 0) && (globalRange[1] % localGroupSizeY == 0))
                    break;
            }
            return cl::NDRange(localGroupSizeX, localGroupSizeY);
        }
        default: 
            std::cerr << "unknown or unsupported dimensions number in global range" << std::endl;
    }
    return cl::NDRange();
}

class Platform {
    static inline cl::Platform platform_;
    static inline bool isCreated_ = false;

public:
    static cl::Platform &selectGPUplatform ();
};

class Context {
    static inline cl::Context context_;
    static inline bool isCreated_ = false;

public:
    static cl::Context &getGPUcontext (cl_platform_id p);
};

class Queue {
    static inline cl::CommandQueue queue_;
    static inline bool isCreated_ = false;

public:
    static cl::CommandQueue &getCommandQueue (cl::Context &ctx);
};

class IOpenclApp {
protected:
    cl::Platform &platform_;
    cl::Context &context_;
    cl::CommandQueue &queue_;
    std::string kernel_;

    std::vector<cl::Event> profiling_;

public:
    IOpenclApp ()
        : platform_ (Platform::selectGPUplatform ()), context_ (Context::getGPUcontext (platform_ ())), queue_ (Queue::getCommandQueue (context_))
    {
    }

    long getGPULastRunTimeNs ();
    inline long getGPULastRunTimeMs () { return getGPULastRunTimeNs () / 1000000; }
};

}  // namespace OpenCLApp

#endif