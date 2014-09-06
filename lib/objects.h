#ifndef OBJECTS_H
#define OBJECTS_H

#include <memory>
#include <vector>

// This is a C++11 thing - if we can't use this then we're looking
// at a define or some other horrifying construct
template<typename T>
using shared_vector = std::vector<std::shared_ptr<T> >;

namespace stadic{





}
#endif // OBJECTS_H
