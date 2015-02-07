#ifndef BGL_VALUE_ITERATOR
#define BGL_VALUE_ITERATOR

#include <iterator>
#include <type_traits>


template <typename Graph, typename BglIterator>
class BglValueIterator : public std::iterator<std::forward_iterator_tag, 
		typename std::remove_reference<decltype(std::declval<Graph>(
					)[typename BglIterator::value_type{}])>::type> {
 public:
	using parent_t = std::iterator<std::forward_iterator_tag, 
		  typename std::remove_reference<decltype(std::declval<Graph>(
					)[typename BglIterator::value_type{}])>::type>;

	explicit BglValueIterator(Graph& graph) : graph_{graph} {}
	BglValueIterator(Graph& graph, BglIterator iterator) : 
		graph_(graph), iterator_{iterator} {}

	BglValueIterator operator++(int) {
		BglIterator current_it{iterator_++};
		return BglValueIterator{graph_, current_it};
	}

	BglValueIterator& operator++() {
		++iterator_;
		return *this;
	}

	typename parent_t::pointer operator->() { return &graph_[*iterator_]; }
	typename parent_t::reference operator*() { return graph_[*iterator_]; }

	bool operator==(const BglValueIterator& other) const
	{ return &graph_ == &other.graph_ && iterator_ == other.iterator_; }

	bool operator!=(const BglValueIterator& other) const
	{ return !(iterator_ == other.iterator_); }

 private:
	Graph& graph_;
	BglIterator iterator_;
};


#endif  // BGL_VALUE_ITERATOR
