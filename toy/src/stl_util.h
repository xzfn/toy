
#include <cstdint>
#include <cstddef>

namespace stlutil {

template <typename T>
std::size_t vector_buffer_size(const T& container) {
	return container.size() * sizeof(container[0]);
}


}
