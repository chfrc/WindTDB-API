#include "stdafx.h"
#include <string>
#include <iostream>
#include <queue>

#include <boost\python\module.hpp>
#include <boost\python\def.hpp>
#include <boost\python\dict.hpp>
#include <boost\python\list.hpp>
#include <boost\python\object.hpp>
#include <boost\python.hpp>

#include <boost\any.hpp>
#include <boost\thread.hpp>
#include <boost\bind.hpp>

#include "TDBAPI.h"

using namespace std;

#define ELEMENT_COUNT(arr) (sizeof(arr)/sizeof(*arr))

void getInt(boost::python::dict d, string key, int *value) {
/* Get int field from python dict d with key key, 
*  then paste it into int pointer *value.
*/
	if (d.has_key(key)) 
	{
		boost::python::object o = d[key];
		boost::python::extract<int> x(o);

		if (x.check()) {
			*value = x();
		}
	}
};

void getDouble(boost::python::dict d, string key, double *value) {
/* Get float field from python dict d with key key,
*  then paste it into float pointer *value.
*/
	if (d.has_key(key))
	{
		boost::python::object o = d[key];
		boost::python::extract<double> x(o);
		if (x.check())
		{
			*value = x();
		}
	}
};

void getChar(boost::python::dict d, string key, char *value) {
/* Get string field from python dict d with key key,
*  then paste it into char pointer *value.
*/
	if (d.has_key(key))
	{
		boost::python::object o = d[key];
		boost::python::extract<string> x(o);
		if (x.check())
		{
			string s = x();
			const char *buffer = s.c_str();
			strcpy_s(value, strlen(buffer) + 1, buffer);
		}
	}
};

#pragma once
class zTDBApi
{
public:
	zTDBApi() : hTdb_(NULL), is_connected_(FALSE) { };
	virtual ~zTDBApi() { };

protected:
	void Connect(OPEN_SETTINGS &settings) {
	// Connect to TDB server.
		THANDLE hTdb_pre = NULL;
		TDBDefine_ResLogin LoginRes = { 0 };
		hTdb_pre = TDB_Open(&settings, &LoginRes);

		if (hTdb_pre){
			hTdb_ = hTdb_pre;
			is_connected_ = TRUE;
		}
	};

	boost::python::list GetKLines(THANDLE hTdb, char* szCode, int* nBeginDate, 
		int* nEndDate, int nCycle, int nUserDef, int nCQFlag, int nAutoComplete) {
	// Require klines data.
		// set req_kline requirement object. 
		TDBDefine_ReqKLine* req = new TDBDefine_ReqKLine;
		// paste in values.
		strncpy_s(req->chCode, szCode, ELEMENT_COUNT(req->chCode));
		req->nCQFlag = (REFILLFLAG)nCQFlag;
		req->nBeginDate = *nBeginDate; req->nEndDate = *nEndDate;
		req->nBeginTime = 0; req->nEndTime = 0;
		req->nCycType = (CYCTYPE)nCycle;
		req->nCycDef = 0;
		// Initialize Kline data container.
		TDBDefine_KLine* kLine = NULL;
		int pCount;
		// Get klines via original API function call.
		TDB_GetKLine(hTdb, req, &kLine, &pCount);

		// Paste data into python list
		boost::python::list k_data;
		for (int i = 0; i < pCount; i++) {
			boost::python::dict tmp;
			tmp["windCode"] = kLine[i].chWindCode;
			tmp["Date"] = kLine[i].nDate; tmp["Time"] = kLine[i].nTime;
			tmp["Close"] = kLine[i].nClose;
			k_data.append(tmp);
		}
		// release memory
		TDB_Free(kLine); delete req; req = NULL;
		// return list
		return k_data;
	};

	
public:
	void connect(boost::python::dict req) {
	// Python interface.
	// req is a python dict, has key 'IP', 'port', 'usr' and 'pwd'
		OPEN_SETTINGS settings;
		getChar(req, "IP", settings.szIP);
		getChar(req, "port", settings.szPort);
		getChar(req, "usr", settings.szUser);
		getChar(req, "pwd", settings.szPassword);
		this->Connect(settings);
	};

	boost::python::list getKLines(boost::python::dict req) {
	// Get k lines data.'
		boost::python::list data;
		char* szCode = NULL;
		int *nBeginDate = NULL;
		int *nEndDate = NULL;
		int nCycle, nUserDef, nCQFlag, nAutoComplete;
		getChar(req, "code", szCode);
		getInt(req, "begin", nBeginDate);
		getInt(req, "end", nEndDate);
		
		nCycle = CYC_MINUTE;
		nUserDef = 0; nCQFlag = 0; nAutoComplete = 0;
		data = GetKLines(hTdb_, szCode, nBeginDate, nEndDate, 
			nCycle, nUserDef, nCQFlag, nAutoComplete);
		return data;
	}

	//virtual void onRtnKLines(boost::python::dict data);

private:
	THANDLE hTdb_;
	bool is_connected_;

};

