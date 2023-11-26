#include <IOpenCLApp.hpp>

namespace OpenCLApp {

cl::Platform &Platform::selectGPUplatform ()
{
    if (isCreated_)
        return platform_;

    cl::vector<cl::Platform> platforms;

    cl::Platform::get (&platforms);

    for (auto curIt = platforms.begin (), endIt = platforms.end (); curIt != endIt; ++curIt) {
        cl_uint numDevices = 0;
        ::clGetDeviceIDs ((*curIt) (), CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
        std::vector<cl::Device> devices;
        (*curIt).getDevices (CL_DEVICE_TYPE_ALL, &devices);
// finding NVIDIA or first platform with devices
#ifdef NVIDIA
        if ((*curIt).getInfo<CL_PLATFORM_NAME> () == "NVIDIA CUDA")
#else
        if (numDevices > 0)
#endif
        {
            platform_ = *curIt;
            isCreated_ = true;

            return platform_;
        }
    }
    throw std::runtime_error ("No platform with GPU devices(((");
}

cl::Context &Context::getGPUcontext (cl_platform_id p)
{
    if (isCreated_)
        return context_;

    cl_context_properties cprops[] = {
        CL_CONTEXT_PLATFORM,
        reinterpret_cast<cl_context_properties> (p),
        0};

    context_ = cl::Context (CL_DEVICE_TYPE_GPU, cprops);
    isCreated_ = true;
    return context_;
}

cl::CommandQueue &Queue::getCommandQueue (cl::Context &ctx)
{
    if (isCreated_)
        return queue_;

    std::vector<cl::Device> devices = ctx.getInfo<CL_CONTEXT_DEVICES> ();
// printing device name uncomment this if you want
#ifndef NDEBUG
    // std::cout << (devices[0]).getInfo<CL_DEVICE_NAME> () << std::endl;
#endif
    queue_ = cl::CommandQueue (ctx, devices[0], CL_QUEUE_PROFILING_ENABLE);
    isCreated_ = true;

    return queue_;
}

long IOpenclApp::getGPULastRunTimeNs ()
{
    long GPUtime = 0;
    for (auto evt : profiling_)
        GPUtime += evt.getProfilingInfo<CL_PROFILING_COMMAND_END> () - evt.getProfilingInfo<CL_PROFILING_COMMAND_START> ();

    return GPUtime;
}

}
