#ifndef _JET_TABLE_H
#define _JET_TABLE_H

//#include <assert.h>
#include <esent.h>
//#include <iostream>
#include <map> 
//#include <memory> // for shared_ptr
#include <string> 
#include <vector> 
//#include <windows.h>
#include "jet_database.h"

#pragma comment( lib, "esent.lib" )


using namespace std;

#pragma warning (disable : 4533) 

class eseColumn
{
public:
	eseColumn() = delete;
	eseColumn(eseColumn&) = delete;

	eseColumn(char* columnName)
	{
		name = columnName;
		coldef = { 0 }; // zero all fields.
		coldef.cbStruct = sizeof(JET_COLUMNDEF);
		coldef.coltyp = JET_coltypNil;
	}

	eseColumn(char* columnName, int coltype, int grbit=0, int cbMax = 0, int cp = 0)
	{ 
		colcreate = { 0 }; // zero all fields.
		colcreate.cbStruct = sizeof(JET_COLUMNCREATE);
		colcreate.szColumnName = columnName;
		colcreate.coltyp = coltype;
		colcreate.grbit = grbit;
		colcreate.err = JET_errSuccess;
		colcreate.cbMax = cbMax;
		colcreate.cp = cp;

		name = columnName;

		coldef = { 0 }; // zero all fields.
		coldef.cbMax = cbMax;
		coldef.cbStruct = sizeof(JET_COLUMNDEF);
		coldef.coltyp = coltype;
		coldef.grbit = grbit;
		coldef.cp = cp;
		//coldef.columnid = JET_columnidNil;
	}
	eseColumn(char* columnName, JET_COLUMNDEF& def)
	{
		name = columnName;
		columnid = def.columnid;
		coldef = def;
	}

	~eseColumn() { }

	JET_ERR getLastError() { return err; }
	JET_COLUMNID id() { return columnid; }
	const char* getName() { return name.c_str(); }
	JET_COLUMNDEF& getColumnDef() { return coldef; }
	JET_COLUMNCREATE& getColumnCreate() { return colcreate; }
	JET_COLUMNCREATE colcreate;
	JET_COLUMNDEF coldef;

	JET_COLUMNID columnid;
protected:
	JET_ERR err;
	string name;
}; // class eseColumn


class IndexKey
{
	std::vector<char> m_data;
public:
	IndexKey(const char* cs) : m_data(cs, cs + ::strlen(cs)) { m_data.push_back(0);  m_data.push_back(0); }
	IndexKey& operator <<(const char c) { m_data.push_back(c); return *this; }
	IndexKey& operator <<(const std::string &s) { m_data.insert(m_data.end(), s.begin(), s.end()); return *this; }
	IndexKey& operator <<(const char *cs) { m_data.insert(m_data.end(), cs, cs + ::strlen(cs)); return *this; }
	char *data() { return &m_data[0]; }
	const size_t size() const { return m_data.size(); }
	void clear() { m_data.clear(); }
};


class eseIndex
{
public:
	eseIndex() = delete;
	eseIndex(eseIndex&) = delete;
	eseIndex(char* name, IndexKey& indexkey, int grBit) : key(NULL)
	{
		index = { 0 }; // zero all fields.
		index.cbStruct = sizeof(JET_INDEXCREATE);
		index.szIndexName = name;
		key = (char*)malloc(indexkey.size());
		index.cbKey = indexkey.size();
		memcpy(key, indexkey.data(), index.cbKey);
		index.szKey = key;
		index.grbit = grBit;
		index.err = JET_errSuccess;
	}
	~eseIndex() { if (key) free(key); }

	char* getName() { return index.szIndexName; }
	char* getKey() { return index.szKey; }
	int getGrbit() { return index.grbit; }
	int getDensity() { return index.ulDensity; }

	JET_INDEXCREATE& getIndex()
	{
		return index;
	}

	JET_ERR getLastError() { return err; }
	//	JET_SESID id() { return columnid; }

protected:
	JET_ERR err;
	JET_INDEXCREATE index;
	char* key;
}; // class eseIndex

class eseTable
{
public:
	typedef std::vector<uint8_t> bookmark;
	//

	eseTable() = delete;
	eseTable(const eseTable&) = delete;
	eseTable(eseTable&&) = delete;

	eseTable(int _dbid, int _sessionid, int _tableid = JET_tableidNil) :sessionid(_sessionid), dbid(_dbid), tableid(_tableid), nbCols(0) {}
	~eseTable() { close(); }

	JET_ERR create(char* tableName, int pagesCount = 1, int density = 50)
	{
		int Density = ((density < 20) || (density > 100)) ? 0 : density;
		err = ::JetCreateTableA(sessionid, dbid, tableName, pagesCount, Density, &tableid);
		if (JET_errSuccess == err)
		{
			name = tableName;
		}
		return err;
	}

	JET_ERR create(char* tableName, eseColumn* colPK, eseColumn* colValue, eseIndex& idx0, eseIndex& idx1, int pagesCount = 1, int density = 50)
	{
		JET_TABLECREATE table;

		table = { 0 };
		table.cbStruct = sizeof(JET_TABLECREATE);
		table.szTableName = tableName;

		JET_COLUMNCREATE ColumnCreate[2];
		ColumnCreate[0] = colPK->getColumnCreate();
		ColumnCreate[1] = colValue->getColumnCreate();

		JET_INDEXCREATE IndexCreate[2];
		IndexCreate[0] = idx0.getIndex();
		IndexCreate[1] = idx1.getIndex();

		table.rgcolumncreate = ColumnCreate;
		table.cColumns = 2;
		table.rgindexcreate = IndexCreate;
		table.cIndexes = 2;
		table.tableid = JET_tableidNil;
		table.ulDensity = ((density < 20) || (density > 100)) ? 0 : density;
		table.ulPages = pagesCount;

		err = ::JetCreateTableColumnIndex(sessionid, dbid, &table);
		if (err == JET_errSuccess)
		{
			tableid = table.tableid;
			colPK->columnid = ColumnCreate[0].columnid;
			colValue->columnid = ColumnCreate[1].columnid;
			nbCols = 2;
			name = tableName;
			columns_by_name["PK"] = 0;
			columns_by_name["Value"] = 1;
			columns.push_back(colPK);
			columns.push_back(colValue);
		}
		return err;
	}

	JET_ERR open(char* tableName)
	{
		err = ::JetOpenTable(sessionid, dbid, tableName, 0, 0, 0, &tableid);
		err = detect_columns();
		return err;
	}
	JET_ERR close()
	{
		if (tableid == JET_tableidNil)
		{
			return 0;
		}

		err = ::JetCloseTable(sessionid, tableid);
		tableid = JET_tableidNil;
		for (int i = 0; i < nbCols; ++i)
		{
			delete columns[i];// destroys all columns associated with the table
		}
		nbCols = 0;

		name = "";
		return (err);
	}

	JET_ERR insert(int column, wchar_t* value)
	{
		err = ::JetBeginTransaction(sessionid);
		if (JET_errSuccess != err) return err;

		err = ::JetPrepareUpdate(sessionid, tableid, JET_prepInsert);
		if (JET_errSuccess != err) return err;

		err = ::JetSetColumn(sessionid, tableid, columns[column]->columnid, value, ::wcslen(value) * sizeof(wchar_t), 0, NULL);

		if (JET_errSuccess != err) return err;

		err = ::JetUpdate(sessionid, tableid, 0, 0, 0);
		if (JET_errSuccess != err)
		{
			err = ::JetRollback(sessionid, 0);
		}
		else err = ::JetCommitTransaction(sessionid, 0);
		return err;
	} // JET_ERR insert(JET_COLUMNID columnidl, wchar_t* value)

	JET_ERR insertMultiple(int nbColumns, int* columns, wchar_t** value)
	{ // this function has not been tested
		err = ::JetBeginTransaction(sessionid);
		if (JET_errSuccess != err) return err;

		err = ::JetPrepareUpdate(sessionid, tableid, JET_prepInsert);
		if (JET_errSuccess != err) return err;

		JET_SETCOLUMN* setColumns = (JET_SETCOLUMN*)malloc(nbColumns * sizeof(JET_SETCOLUMN));

		for (int i = 0; i < nbColumns; ++i)
		{
			setColumns[i].columnid = columns[columns[i]];
			setColumns[i].pvData = value;
			setColumns[i].cbData = ::wcslen(value[i]) * sizeof(wchar_t);
			setColumns[i].err = JET_errSuccess;
		}
		err = ::JetSetColumns(sessionid, tableid, setColumns, nbColumns);
		free(setColumns);

		if (JET_errSuccess != err) return err;

		err = ::JetUpdate(sessionid, tableid, 0, 0, 0);
		if (JET_errSuccess != err)
		{
			err = ::JetRollback(sessionid, 0);
		}
		else err = ::JetCommitTransaction(sessionid, 0);
		return err;
	} // JET_ERR insertMultiple(JET_COLUMNID columnidl, wchar_t* value)

	JET_ERR setCursor(long pos)
	{ // move is done by using current index
		return err = ::JetMove(sessionid, tableid, pos, 0);
	}

	JET_ERR create_index(eseIndex index)
	{
		// ckey points to a double null - terminated string of null - delimited tokens
		create_index(index.getName(), IndexKey(index.getKey()), index.getGrbit(), index.getDensity());

		return err;
	}
	JET_ERR create_index(const char* name, const char *ckey, const JET_GRBIT grbit, unsigned long density = 0)
	{
		return create_index(name, IndexKey(ckey), grbit, density);
	}
	JET_ERR create_index(const char* name, IndexKey &key, const JET_GRBIT grbit, unsigned long density)
	{
		err = JetCreateIndex(sessionid, tableid, name, grbit, key.data(), static_cast<unsigned long>(key.size()), density);
		return err;
	}

	JET_ERR setCurrentIndex(eseIndex& idx)
	{
		return setCurrentIndex(idx.getName());
	}
	JET_ERR setCurrentIndex(char* indexName)
	{
		return err = ::JetSetCurrentIndex(sessionid, tableid, indexName);
	}

	//JET_ERR getCurrentIndex(eseIndex idx)

	// columns
	const size_t column_count() const { return columns.size();}
	eseColumn* column(int ix) { return columns[ix];}
	eseColumn* column(const std::string& name)
	{
		return columns[columns_by_name[name]];
	}

	
	JET_ERR create_column(eseColumn* column)
	{
		err = ::JetAddColumn(sessionid, tableid, column->getName(), &column->getColumnDef(), nullptr, 0, &column->columnid);
		if (err == JET_errSuccess)
		{
			column->getColumnDef().columnid = column->columnid;
		}
		return err;
	}

	JET_ERR getLastError() { return err; }
	JET_TABLEID id() { return tableid; }
	JET_COLUMNID colid(int c) { return columns[c]->columnid;}


	JET_ERR get_column(int column, int* val)
	{
		unsigned long nReadBytes = 0;
		if (columns[column]->coldef.coltyp != JET_coltypLong) return JET_errInvalidColumnType;
		err = ::JetRetrieveColumn(sessionid, tableid, colid(column), val, sizeof(int), &nReadBytes, 0, 0);
		if (JET_errSuccess != err) return err;
		return err;
	}

	JET_ERR get_column(int column, float* val)
	{
		unsigned long nReadBytes = 0;
		if (columns[column]->coldef.coltyp != JET_coltypIEEESingle) return JET_errInvalidColumnType;
		err = ::JetRetrieveColumn(sessionid, tableid, colid(column), val, sizeof(float), &nReadBytes, 0, 0);
		if (JET_errSuccess != err) return err;
		return err;
	}

	JET_ERR get_column(int column, double* val)
	{
		unsigned long nReadBytes = 0;
		if (columns[column]->coldef.coltyp != JET_coltypIEEEDouble) return JET_errInvalidColumnType;
		err = ::JetRetrieveColumn(sessionid, tableid, colid(column), val, sizeof(double), &nReadBytes, 0, 0);
		if (JET_errSuccess != err) return err;
		return err;
	}


	JET_ERR get_column(int column, wchar_t* buffer, int bufflen)
	{
		unsigned long nReadBytes = 0;
		if (columns[column]->coldef.coltyp != JET_coltypText) return JET_errInvalidColumnType; // JET_coltypText  is max 255 chars
		err = ::JetRetrieveColumn(sessionid, tableid, colid(column), buffer, bufflen, &nReadBytes, 0, 0);
		if (JET_errSuccess != err) return err;
		return err;
	}

	JET_ERR get_column(int column, char* buffer, int bufflen)
	{
		unsigned long nReadBytes = 0;
		if (columns[column]->coldef.coltyp != JET_coltypText) return JET_errInvalidColumnType; // JET_coltypText  is max 255 chars
		err = ::JetRetrieveColumn(sessionid, tableid, colid(column), buffer, bufflen, &nReadBytes, 0, 0);
		if (JET_errSuccess != err) return err;
		return err;
	}

/*	JET_ERR getValue(int column, wchar_t* buffer, int bufflen)
	{
		unsigned long nReadBytes = 0;
		// XXX TODO check this
		if (columns[column]->coldef.coltyp != JET_coltypLongText) return 0; // JET_coltypText  is max 255 chars
		err = ::JetRetrieveColumn(sessionid, tableid, colid(column), buffer, bufflen, &nReadBytes, 0, 0);
		if (JET_errSuccess != err) return err;
		return err;
	}
	*/

protected:
	JET_ERR detect_columns()
	{
		columns.clear();
		columns_by_name.clear();
		JET_COLUMNLIST cl;

		cl.cbStruct = sizeof(cl);

		err = JetGetTableColumnInfoA(sessionid, tableid, nullptr, &cl, sizeof(cl), JET_ColInfoListSortColumnid);
		if (JET_errSuccess != err)
		{
			return err;
		}
		nbCols = cl.cRecord;

		eseTable t(dbid, sessionid, cl.tableid); // t is a psecial table, not all operations are suported...
		//t.detect_columns();
		columns.reserve(nbCols);

		//database().session().begin_transaction();
		try
		{
			char columnName[256]; // contains column names
			unsigned long size = 256;
			err = t.setCursor(JET_MoveFirst);
			int index = 0;
			while (err == JET_errSuccess)
			{
				// get the column name first
				//err = t.get_column(0, columnName, 256);
				err = ::JetRetrieveColumn(sessionid, t.id(), cl.columnidDefinitionName, columnName, 256, &size, 0, nullptr);
				if (err == JET_errSuccess)
				{
					columnName[size] = 0;
					JET_COLUMNDEF column;
					// then retrive all other info to describe the columns given its name (in buffer)
					err = ::JetGetTableColumnInfoA(sessionid, tableid, columnName, (void *)&column, sizeof(JET_COLUMNDEF), JET_ColInfo);
					if (err != JET_errSuccess) return err;
					eseColumn* col = new eseColumn(columnName, column);
					if (col)
					{
						columns.push_back(col);
						columns_by_name[columnName] = index++;
					}//
				}// if
				//err = ::JetMove(sessionid, t.id(), JET_MoveNext, 0);
				err = t.setCursor(JET_MoveNext);
			}// while
		}//try
		catch (...)
		{
		}// catch
		 // check that the loop went until the last record without errors
		if (err == JET_errNoCurrentRecord)
		{
			err = JET_errSuccess;
			// temporary table "t" will be closed by destructor as it goes out of scope
			//err = JetCloseTable(sessionid, t.id());
		}
		//database().session().commit();
		return err;
	} // JET_ERR detect_columns()

	JET_ERR set_column(int nrcolumn, const void *pData, const unsigned long cbData, JET_GRBIT grbit = 0, JET_SETINFO *info = nullptr)
	{
		err = JetSetColumn(sessionid, tableid, columns[nrcolumn]->columnid, pData, cbData, grbit, info);
		return err;
	}
	JET_ERR set_column(int nrcolumn, const std::string& s) { return set_column(nrcolumn, &s[0], static_cast<const unsigned long>(s.size())); }
	JET_ERR set_column(int nrcolumn, const std::wstring& s) { return set_column(nrcolumn, &s[0], static_cast<const unsigned long>(s.size()) * sizeof(wchar_t)); }
	JET_ERR set_column(int nrcolumn, const std::vector<uint8_t>& v) { return set_column(nrcolumn, &v[0], static_cast<const unsigned long>(v.size())); }
	template <class T> JET_ERR set_column(int nrcolumn,T t)
	{
		static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
		return set_column(int nrcolumn, &t, sizeof(T));
	}


	JET_ERR prepare(const unsigned long prepareOption)
	{
		return err = JetPrepareUpdate(sessionid, tableid, prepareOption);
	}
	JET_ERR prepare_insert() { return prepare(JET_prepInsert); }
	JET_ERR prepare_replace() { return prepare(JET_prepReplace); }
	JET_ERR update(bookmark& bk)
	{
		if (bk.empty())
		{
			bk.resize(4);
		}
		unsigned long size = static_cast<unsigned long>(bk.size());
		err = JetUpdate(sessionid, tableid, &bk[0], size, &size);
		if (err == JET_errBufferTooSmall)
		{
			bk.resize(size);
			size = static_cast<unsigned long>(bk.size());
			err = JetUpdate(sessionid, tableid, &bk[0], size, &size);
			bk.resize(size);
		}// if
		return err;
	}


	JET_TABLEID tableid;
	string name;
	JET_SESID sessionid;
	JET_DBID dbid;
	JET_ERR err;
	int nbCols;
	std::vector< eseColumn* > columns;
	std::map< std::string, int > columns_by_name; // maps column names to column position. (not columnID !!!)

}; // class eseTable

#endif