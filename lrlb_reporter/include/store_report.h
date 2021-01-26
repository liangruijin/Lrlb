#pragma once

#include "mysql.h"
#include "lrlb.pb.h"


class StoreReport{
public:
	StoreReport();
	void store(lrlb::ReportStatusReq req);
private:
	MYSQL _db_conn;
};


