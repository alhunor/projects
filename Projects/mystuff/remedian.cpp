

template<typename T>
T remedian(T* data, int nbData, int b, int k)
{
	int i, j, count[k], level;
	T medians[b*k], m;
	bool done;

	for (i=0; i<k; i++) count[i]=0;

	for (i=0; i<nbData; i++)
	{
		level=0;
		m=data[i];
		done=false;
		while (!done)
		{
			if (count[level]<b)
			{
				//insert m into medians
				j=count[level];
				medians[j]=m;
				while (j>0 && medians[j]>medians[j-1]) swap(medians[j], medians[j-1]);

				// increment count and done				
				count[level]++;
				done=true;
			} else
			{
				//insert new element				
				count[level]=1;
				medians[level*b]=m;
				// carry over median to next level
				m=medians[level*b+b/2];
				level++;
			}
		} // while(!done)
	} // for
// now get the median as the weighted median from all levels

}
