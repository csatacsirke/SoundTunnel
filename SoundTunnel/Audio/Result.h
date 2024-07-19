#include <expected>


template<class T>
using Result = std::expected<T, HRESULT>;

