
#include "blockQueue.h"
#include <memory.h>


bool blockQueueTest()
{
	blockQueue<memWriter> bq(4,2);
	int state;

	int a[32], tmp;

	for (int i = 0; i < 32; ++i) a[i] = i;

	state = bq.insert(&a[0], 4); // E->FE
	if (state != FullEmpty) return false;

	state = bq.remove(&tmp, 4); //FE->E
	if (state != Empty || tmp!=0) return false;

	state = bq.insert(&a[0], 4);// E->FE, already tested
	state = bq.insert(&a[1], 4);// FE->F
	if (state != Full ) return false;

	state = bq.remove(&tmp, 4); //F->EF
	if (state != EmptyFull || tmp != 0) return false;

	state = bq.insert(&a[0], 4); // EF ->F
	if (state != Full) return false;

	state = bq.insert(&a[2], 4); //F->FE
	if (state != FullEmpty) return false;

	state = bq.remove(&tmp, 4); //FE->EFE
	if (state != EmptyFullEmpty || tmp != 0) return false;

	state = bq.remove(&tmp, 4); //EFE->EF
	if (state != EmptyFullEmpty || tmp != 1) return false;

	state = bq.insert(&a[3], 4); //EFE->EF
	if (state != EmptyFull) return false;


	state = bq.remove(&tmp, 4); //EF->EF
	if (state != EmptyFull || tmp!=2 ) return false;

	state = bq.remove(&tmp, 4); //EF->E
	if (state != Empty || tmp != 3) return false;


	state = bq.insert(&a[0], 4);// E->FE, already tested
	state = bq.insert(&a[1], 4);// FE->FE
	state = bq.insert(&a[2], 4);// FE->FE
	state = bq.insert(&a[3], 4);// FE->F
	state = bq.remove(&tmp, 4); //F->EF
	state = bq.remove(&tmp, 4); //EF->EF
	if (state != EmptyFull || tmp != 1) return false;

	state = bq.insert(&a[0], 4); // EF->FEF
	if (state != FullEmptyFull) return false;

	state = bq.insert(&a[1], 4);// FE->FE
	if (state != Full) return false;

	state = bq.remove(&tmp, 4); //F->EF
	state = bq.remove(&tmp, 4); //EF->EF
	state = bq.insert(&a[0], 4);// EF->FEF
	state = bq.remove(&tmp, 4); //FEF->EFF
	if (state != FullEmptyFull || tmp != 2) return false;

	state = bq.remove(&tmp, 4); //FEF->FF
	if (state != FullEmpty || tmp != 3) return false;

	return true;
} // bool blockQueueTest()