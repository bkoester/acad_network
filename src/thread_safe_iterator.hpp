#ifndef THREAD_SAFE_ITERATOR_H
#define THREAD_SAFE_ITERATOR_H


/**
 * Class that wraps a forward iterator to provide thread safety when 
 * incrementing it.
 *
 * Note: Provides no thread safety when modifying data members.
 */

template <typename Iterator>
class ThreadSafeIterator : public std::iterator<std::forward_iterator_tag,
	Iterator::value_type> {
 public:
	explicit ThreadSafeIterator(Iterator it) : iterator_{it} {}

	ThreadSafeIterator operator++(int) {
		std::lock_guard<std::mutex> lg;
		Iterator current_it{iterator_++};
		return ThreadSafeIterator{current_it};
	}

	Iterator& operator++() {
		std::lock_guard<std::mutex> lg;
		++iterator_;
		return *this;
	}

	typename parent_t::pointer operator->() { return &(*iterator_); }
	typename parent_t::reference operator*() { return *iterator_; }

	bool operator==(const BglValueIterator& other) const
	{ return iterator_ == other.iterator_; }

	bool operator!=(const BglValueIterator& other) const
	{ return !(iterator_ == other.iterator_); }

 private:
	using parent_t = std::iterator<std::forward_iterator_tag,
		  Iterator::value_type>

	Iterator iterator_;
	std::mutex mutex_;
};
