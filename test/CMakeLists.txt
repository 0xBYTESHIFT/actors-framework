#available tests

include_directories(.)
include_directories(../header/)

add_subdirectory(any)
add_subdirectory(handler)
add_subdirectory(intrusive_ptr)
add_subdirectory(memory_resource)
add_subdirectory(message)
add_subdirectory(metaprogramming)
add_subdirectory(polymorphic_allocator)

add_subdirectory(resend_order)
add_subdirectory(send)
add_subdirectory(spawn)
