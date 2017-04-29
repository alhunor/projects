//unitest
#include "currency.h"
#include <assert.h>


bool test_spotDate()
{
	assert(spotDate(EUR, USD, 40084) == 40086);
	assert(spotDate(USD, TRY, 39856) == 39857);
	assert(spotDate(GBP, USD, 39984) == 39986);
	assert(spotDate(EUR, USD, 39932) == 39937);

	assert(spotDate(USD, CAD, 40025) == 40029);
	assert(spotDate(AUD, NZD, 40094) == 40099);
	assert(spotDate(USD, BRL, 40127) == 40129);
	assert(spotDate(USD, MXN, 40127) == 40130);
	return true;
}