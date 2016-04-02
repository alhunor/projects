// Source is from Jon Bentley Programming Pearls http://googleresearch.blogspot.co.uk/2006/06/extra-extra-read-all-about-it-nearly.html
template <typename T>
int binsearch(T key, T* data, int lo, int hi)
{
	int mid;
	T midval;
	while (lo <= hi)
	{
		//mid = (lo + hi) / 2; -- can overflow
		mid = ((unsigned int)lo + (unsigned int)hi)) >> 1;
		midVal = data[mid];
		if (midVal < key)
		{
			lo = mid + 1
		} else if (midVal > key)
		{
			hi = mid - 1;
		} else
		{
			return mid; // key found
		}
		return -1;  // key not found.
	}
} // int binsearch(T key, T* data, int lo, int hi)