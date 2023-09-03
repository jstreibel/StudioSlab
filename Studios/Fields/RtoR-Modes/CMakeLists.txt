add_executable ( RtoR-Modes
        Main-RtoR-Modes.cpp
        Builder.cpp
        Builder.h
)
target_link_libraries ( RtoR-Modes ${SlabLibs} Slab-Maps-RtoR_GPU )
#set_property(TARGET Fields-RtoR PROPERTY CUDA_ARCHITECTURES 72)

message(NOTICE "Added executable RtoR-Modes")
