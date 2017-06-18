#ifndef _JET_DATABASE_H
#define _JET_DATABASE_H

//#include <assert.h>
#include <esent.h>
//#include <iostream>
#include <map> 
//#include <memory> // for shared_ptr
#include <string> 
#include <vector> 
#include <windows.h> // for MAX_PATH

#pragma comment( lib, "esent.lib" )

#pragma warning (disable : 4533) 

//read : https://stackoverflow.com/questions/tagged/extensible-storage-engine

// esentutl.exe ese.edb
// https://www.codeproject.com/Articles/52715/Extensible-Storage-Engine
// https://msdn.microsoft.com/en-us/library/5c485eff-4329-4dc1-aa45-fb66e6554792.aspx


// https://msdn.microsoft.com/en-us/library/gg294069(v=exchg.10).aspx -- ese file types

class eseInstance
{
public:
	eseInstance() : instance(JET_instanceNil) {}
	~eseInstance() { close(); } // cleanupEngine

	JET_ERR close()
	{
		if (instance != JET_instanceNil)
		{
			err = ::JetTerm(instance);
			instance = JET_instanceNil;
		}
		return err;
	}

	JET_ERR init(JET_INSTANCE _instance = 0)
	{
		instance = _instance;
		err = ::JetInit(0); // 0 as ESE instance
		return err;
	}
	JET_ERR getLastError() { return err; }
	int id() { return instance;}

protected:
	JET_INSTANCE instance;
	JET_ERR err;
}; //class eseInstance

class eseSession
{
public:
	eseSession() : sessionid(JET_sesidNil), instanceid(JET_instanceNil) { }
	~eseSession()
	{
		if (sessionid != JET_sesidNil)
		{
			::JetEndSession(sessionid, 0);
		} // cleanupSession
	}

	JET_ERR init(JET_INSTANCE instance)
	{
		instanceid = instance;
		err = ::JetBeginSession(instanceid, &sessionid, 0, 0); // 0 as ESE instance, session.id() - return value, reserved, reserved 
		return err;
	}
	JET_ERR getLastError() { return err; }
	JET_SESID id() { return sessionid; }


	template <class T>
	inline JET_ERR set_param(unsigned long paramId, const T paramValue)
	{
		JET_API_PTR param = paramValue;
		return ::JetSetSystemParameter(&instanceid, sessionid, paramId, param, 0);
	}

	template <class T>
	inline JET_ERR get_param(int paramId, T& paramValue)
	{
		JET_API_PTR param = paramValue;
		err = ::JetGetSystemParameter(instanceid, sessionid, paramId, &param, 0, 0);
		paramValue = param;
		return err;
	}
	JET_ERR get_param(int paramId, std::string &s)
	{
		s = "";
		char path[MAX_PATH];
		err = ::JetGetSystemParameterA(instanceid, sessionid, paramId, 0, path, MAX_PATH);
		s = path;
		return err;
	}

	JET_ERR set_param(unsigned long paramId, const std::string &s)
	{
		return err = ::JetSetSystemParameterA(&instanceid, sessionid, paramId, 0, s.c_str());
	}


	inline int get_param(unsigned long paramId)
	{
		JET_API_PTR inst = 0;
		err = get_param(paramId, inst);
		return static_cast<int>(inst);
	}

	inline std::string error(const JET_ERR ec)
	{
		char cs[512];
		JET_API_PTR eCode = ec;
		JET_ERR jErr = ::JetGetSystemParameterA(0, 0, JET_paramErrorToString, &eCode, cs, sizeof(cs));
		return std::string(cs);
	}



	const std::string& system_path()
	{
		std::string system_path;
		get_param(JET_paramSystemPath, system_path);
		return system_path;
	}
	//bool system_path(const std::string path);

	const std::string& log_path()
	{
		std::string log_file_path;
		get_param(JET_paramLogFilePath, log_file_path);
		return log_file_path;
	}
	//bool log_path(const std::string path);

	uint16_t max_sessions()
	{
		return get_param(JET_paramMaxSessions);
	}
	JET_ERR set_max_sessions(const uint16_t max)
	{ 
		return set_param(JET_paramMaxSessions, max);
	}

	uint16_t set_log_size() { return get_param(JET_paramLogFileSize); }
	JET_ERR get_log_size(uint16_t max) { return set_param(JET_paramLogFileSize, max); }

	uint32_t get_max_transaction_pages() { return get_param(JET_paramMaxVerPages); }
	JET_ERR set_max_transaction_pages(uint32_t max) { return set_param(JET_paramMaxVerPages, max); }

	bool get_circular_log() { return (get_param(JET_paramCircularLog) == 1); }
	JET_ERR set_circular_log(bool isSet)
	{
		return set_param(JET_paramCircularLog, isSet);
	}

	JET_ERR begin_transaction()
	{ 
		return err= JetBeginTransaction(sessionid);
	}

	JET_ERR commit(JET_GRBIT grbit = JET_bitCommitLazyFlush)
	{ 
		return err = JetCommitTransaction(sessionid, grbit);
	}
	JET_ERR rollback(JET_GRBIT grbit = JET_bitRollbackAll)
	{ 
		return err = JetRollback(sessionid, grbit);
	}




protected:
	JET_SESID sessionid;
	JET_INSTANCE instanceid;

	JET_ERR err;
}; // class eseSession

// https://stackoverflow.com/questions/2337043/enumerate-indexes-on-a-extensible-storage-engine-esent-table

class eseDatabase
{
public:
	eseDatabase(JET_SESID _sessionid) : databaseid(JET_dbidNil), sessionid(_sessionid) { }

	~eseDatabase() { close();}

	bool close(JET_GRBIT grbits = 0)
	{
		if (databaseid == JET_dbidNil)
		{
			return false;
		}

		char out[MAX_PATH];

		err = ::JetGetDatabaseInfoA(sessionid, databaseid, out, MAX_PATH, JET_DbInfoFilename);
		if (err != JET_errSuccess)
		{
			return false;
		}

		err = ::JetCloseDatabase(sessionid, databaseid, grbits);
		if (err != JET_errSuccess)
		{
			return false;
		}

		err = JetDetachDatabase(sessionid, dbname.c_str());
		return err == JET_errSuccess;
	} // bool close(JET_GRBIT grbits = 0)


	JET_ERR open(const char* _dbname, JET_GRBIT grbits = 0)
	{
		err = JetAttachDatabase(sessionid, _dbname, grbits);
		if (err != JET_errSuccess) return err;
		err = ::JetOpenDatabase(sessionid, _dbname, 0, &databaseid, 0);
		if (err != JET_errSuccess)
		{
			JetDetachDatabase(sessionid, _dbname);
		}
		else
		{
			dbname = _dbname;
		}
		return err;
	}

	JET_ERR create(const char* _dbname, JET_GRBIT grbits = 0)
	{
		err = ::JetCreateDatabase(sessionid, _dbname, 0, &databaseid, grbits); // session id, data base file name, reserved, dbID - return value, grbit 
		if (err == JET_errSuccess)
		{
			dbname = _dbname; 
		}
		return err;
	}



	JET_ERR getLastError() { return err; }
	JET_DBID id() { return databaseid; }

protected:
	JET_DBID databaseid;
	JET_SESID sessionid;
	std::string dbname;
	JET_ERR err;
}; // class eseDatabase


#endif