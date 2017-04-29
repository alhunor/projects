#ifndef _CIRCULATOR_H
#define _CIRCULATOR_H

template<class basicIterator>
class circulator //: public std::iterator<std::bidirectional_iterator_tag, T>
{
public:
	typedef typename basicIterator::value_type value_type;
	typedef typename circulator<basicIterator> self_type;
	value_type &operator*() {return *curr; }
	
//	circulator(const circulator& rhs) {}

	circulator(const basicIterator& _begin, const basicIterator& _end ) : curr(_begin), begin (_begin), end(_end), used(false)
	{
		prev=curr;
		dec(prev);
		next=curr;
		inc(next);
	}

	self_type& operator++() 
	{
		prev=curr;
		curr=next;
		inc(next);
		used = true;
		return *this;
	}
	
	self_type& operator--() 
	{
		next=curr;
		curr=prev;
		dec(prev);
		used = true;
		return *this;
	}

	bool operator==(const self_type &rhs)
	{
		return rhs.prev==prev && rhs.curr==curr && rhs.next==next;
	}

	bool operator!=(const self_type &rhs)
	{
		return ! operator==(rhs);
	}

	self_type& operator=(const self_type& rhs)
	{
		if (*this==rhs) return *this;
		prev=rhs.prev;
		curr=rhs.curr;
		next=rhs.next;
		used = rhs.used;
		return *this;
	}

	bool eof() {return curr==begin && used;}

//private:
	basicIterator prev, curr, next, begin, end;
private:
	bool used;
	void inline inc(basicIterator& bi ) {++bi; if (bi==end) bi=begin;}
	void inline dec(basicIterator& bi ) {if (bi==begin) bi=end; --bi;}
};

#endif