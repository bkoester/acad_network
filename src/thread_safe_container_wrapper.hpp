#ifndef THREAD_SAFE_CONTAINER_WRAPPER_H
#define THREAD_SAFE_CONTAINER_WRAPPER_H

#include <iterator>

#include "thread_safe_iterator.hpp"

/**
 * This class implements a subset of operations to provide thread safe iteration
 * over STL containers
 */

template <typename Container>
class ThreadSafeContainerWrapper {
 public:
	ThreadSafeContainerWrapper(Container& c) : container_{c} {}

	using iterator_t = ThreadSafeIterator<
		decltype(std::declval<Container>().begin()>

	iterator_t begin() { return iterator_t{std::begin(container_)}; }
	iterator_t begin() const { return iterator_t{std::begin(container_)}; }
	iterator_t cbegin() const { return iterator_t{std::begin(container_)}; }

	iterator_t end() { return iterator_t{std::end(container_)}; }
	iterator_t end() const { return iterator_t{std::end(container_)}; }
	iterator_t cend() const { return iterator_t{std::end(container_)}; }

 private:
	Container& container_;
};


#endif  // THREAD_SAFE_CONTAINER_WRAPPER_H
