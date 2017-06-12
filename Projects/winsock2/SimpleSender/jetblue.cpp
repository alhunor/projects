
#include <esent.h>
#include <iostream>
#include <map> 
#include <memory> // for shared_ptr
#include <string> 
#include <vector> 
#include <windows.h>
#include "jet_database.h"
#include "jet_table.h"

#pragma comment( lib, "esent.lib" )



int jb()
{
	JET_ERR err;

	eseInstance instance;
	if (instance.init(0) != JET_errSuccess)
	{
		return instance.getLastError();
	}

	eseSession session;
	if (session.init(instance.id()) != JET_errSuccess)
	{
		return session.getLastError();;
	}

	eseDatabase db(session.id());
	err = db.open("test2.edb");
	if (err == JET_errDatabaseDuplicate )
	{
		err = db.create("test2.edb");
	}
	if (err != JET_errSuccess)
	{
		return err;
	}

	eseColumn* col0 = new eseColumn("PK", JET_coltypLong, JET_bitColumnAutoincrement); // will be destroyed by table's destructor
	eseColumn* col1 = new eseColumn("Value", JET_coltypText, 0, 1024, 1200); // will be destroyed by table's destructor

	eseIndex indexKey("PK_index", IndexKey("+PK"), JET_bitIndexPrimary);
	eseIndex indexValue("Value_index", IndexKey("+Value"), JET_bitIndexUnique);

	eseTable table(db.id(), session.id());


	switch (err = table.open("TestTable"))
	{
		case JET_errSuccess:
			break;
		case JET_errObjectNotFound: // Table does not exists, open it
			err = table.create("TestTable", col0, col1, indexKey, indexValue);
			if (err != JET_errSuccess) return err;
			break;
		default:
			return err;
	}

// Inserting Values
	wchar_t* stringsTolnsert[] = { L"FirstInsertedRow2", L"InSecondInsertedRow2", L"ThirdInsertedRow2", L"FourinsertedRow2", L"Fourinse" };
	for (size_t n = 0; n != sizeof(stringsTolnsert) / sizeof(stringsTolnsert[0]); ++n)
	{
		err = table.insert(1, stringsTolnsert[n]); // insert to the second column (numbering starts from zero)
	}

// List content of the table
	err = table.setCurrentIndex(indexValue);
	if (err != JET_errSuccess)  return err;
	
	for (err = table.setCursor(JET_MoveFirst); JET_errSuccess == err; err = table.setCursor(JET_MoveNext))
	{
		int nPK = 0;
		err = table.get_column(0, &nPK);
		if (JET_errSuccess != err) break;

		std::wcout << nPK;

		wchar_t buffer[1024] = { 0 };
		err = table.get_column(1, buffer, 1024);
		std::wcout << L'\t' << buffer << std::endl;
	}
	if (JET_errNoCurrentRecord != err) return err;

	std::wcout << std::endl;

// Search value in the table
	err = table.setCursor(JET_MoveFirst); // first movce to begining using the current index
	if (JET_errSuccess != err) return err;
	
	wchar_t bufferSearchCriteria[] = L"Fo"; //  then set the search criteria
	err = ::JetMakeKey(session.id(), table.id(), &bufferSearchCriteria, sizeof(bufferSearchCriteria), JET_bitNewKey);
	if (JET_errSuccess != err) return err;

	for (err = ::JetSeek(session.id(), table.id(), JET_bitSeekGE); !(err < 0); err = table.setCursor(JET_MoveNext) )
	{
		int nPK = 0;
		err = table.get_column(0, &nPK);
		if (JET_errSuccess != err) break;
		std::wcout << nPK;

		wchar_t buffer[1024] = { 0 };
		err = table.get_column(1, buffer, 1024);
		std::wcout << L'\t' << buffer << std::endl;
	}
	if (JET_errNoCurrentRecord != err) return err;
	std::wcout << std::endl;

// Another search in the table
	err = table.setCursor(JET_MoveFirst);
	if (JET_errSuccess != err) return err;

	err = ::JetMakeKey(session.id(), table.id(), &bufferSearchCriteria, sizeof(bufferSearchCriteria), JET_bitNewKey | JET_bitPartialColumnStartLimit);
	if (JET_errSuccess != err) return err;

	err = ::JetSeek(session.id(), table.id(), JET_bitSeekGE);
	if (err < 0) return err;

	err = ::JetMakeKey(session.id(), table.id(), &bufferSearchCriteria, sizeof(bufferSearchCriteria), JET_bitNewKey | JET_bitPartialColumnEndLimit);
	if (JET_errSuccess != err) return err;

	for (err = ::JetSetIndexRange(session.id(), table.id(), JET_bitRangeInclusive | JET_bitRangeUpperLimit);
		!(err < 0); 
		table.setCursor(JET_MoveNext) )
	{
		int nPK = 0;
		err = table.get_column(0, &nPK);
		if (JET_errSuccess != err) break;

		std::wcout << nPK;

		wchar_t buffer[1024] = { 0 };
		err = table.get_column(1, buffer, 1024);
		std::wcout << L'\t' << buffer << std::endl;
	}
	return 0;
}




int jb2()
{
	JET_ERR err;

	eseInstance instance;
	if (instance.init(0) != JET_errSuccess)
	{
		return instance.getLastError();
	}

	eseSession session;
	if (session.init(instance.id()) != JET_errSuccess)
	{
		return session.getLastError();;
	}

	eseDatabase db(session.id());
	err = db.open("test2.edb");
	if (err == JET_errDatabaseDuplicate)
	{
		err = db.create("test2.edb");
	}
	if (err != JET_errSuccess)
	{
		return err;
	}

	// eseColumns will be destroyed by table's destructor
	eseColumn* col0 = new eseColumn("PK", JET_coltypLong, JET_bitColumnAutoincrement);
	eseColumn* col1 = new eseColumn("filename", JET_coltypText, 0, 1024); // include path
	eseColumn* col2 = new eseColumn("size", JET_coltypLongLong);
	eseColumn* col3 = new eseColumn("changedDate", JET_coltypDateTime);
	eseColumn* col4 = new eseColumn("status", JET_coltypLong);

	eseIndex indexKey("PK_index", IndexKey("+PK"), JET_bitIndexPrimary);
	eseIndex indexFileName("filename_index", IndexKey("+filename"), JET_bitIndexUnique);

	eseTable table(db.id(), session.id());


	switch (err = table.open("FileSystemLog"))
	{
	case JET_errSuccess:
		break;
	case JET_errObjectNotFound: // Table does not exists, open it
		err = table.create("FileSystemLog", col0, col1, indexKey, indexFileName);
		table.create_column(col2);
		table.create_column(col3);
		table.create_column(col4);

		if (err != JET_errSuccess) return err;
		break;
	default:
		return err;
	}

	// Inserting Values
	wchar_t* stringsTolnsert[] = { L"FirstInsertedRow2", L"InSecondInsertedRow2", L"ThirdInsertedRow2", L"FourinsertedRow2", L"Fourinse" };
	for (size_t n = 0; n != sizeof(stringsTolnsert) / sizeof(stringsTolnsert[0]); ++n)
	{
		err = table.insert(1, stringsTolnsert[n]); // insert to the second column (numbering starts from zero)
	}

// List content of the table
	err = table.setCurrentIndex(indexFileName);
	if (err != JET_errSuccess)  return err;

	for (err = table.setCursor(JET_MoveFirst); JET_errSuccess == err; err = table.setCursor(JET_MoveNext))
	{
		int nPK = 0;
		err = table.get_column(0, &nPK);
		if (JET_errSuccess != err) break;

		std::wcout << nPK;

		wchar_t buffer[1024] = { 0 };
		err = table.get_column(1, buffer, 1024);
		std::wcout << L'\t' << buffer << std::endl;
	}
	if (JET_errNoCurrentRecord != err) return err;

	std::wcout << std::endl;

	return 0;
} /// jb2
