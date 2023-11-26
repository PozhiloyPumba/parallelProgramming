set(OpenCL_Lib
    opencl_simple_interface
)

add_library(${OpenCL_Lib} IOpenCLApp.cpp)
# linking with OpenCL
target_link_libraries(${OpenCL_Lib} PUBLIC OpenCL::OpenCL)