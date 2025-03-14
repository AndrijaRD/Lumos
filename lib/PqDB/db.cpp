#include "./db.h"



int DB::init(
    const string& db_name,
    const string& username,
    const string& password,
    const string& hostAddr,
    const int& port
){
    dbName = db_name;
    dbUser = username;
    dbPass = password;
    dbAddr = hostAddr;
    dbPort = port;
    
    string connInfo = "dbname=" + dbName +
                      " user=" + dbUser +
                      " password=" + dbPass + 
                      " hostaddr=" + dbAddr +
                      " port=" + to_string(port);
    
    dbConn = PQconnectdb(connInfo.c_str());
    if (PQstatus(dbConn) != CONNECTION_OK) {
        PQfinish(dbConn);
	cout << "connInfo: " << connInfo << endl;
        return DB_CONNECTION_ERROR;
    }

    return NO_ERROR;
}




int DB::prepareStatement(Statement& s){
    if(s.name == "" or s.command == "") return DB_EMPTY_STATEMENT_PARAM;
    if(s.prepared) return DB_REPREPARATION;

    PGresult* res = PQprepare(
        dbConn, 
        s.name.c_str(), 
        s.command.c_str(), 
        s.nParams, 
        nullptr
    );
    
    if(!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
        if(res) PQclear(res);
        return DB_PREPARE_ERROR;
    }

    s.prepared = true;
    PQclear(res);
    return NO_ERROR;
}



/** Check Result
 * 
 * Checks if result is fine. RETURNS TRUE.
 * 
 * @returns True if everything is fine and False if there is problem
 */
bool DB::checkResult(const PGresult* res, const int type){
    if (res == nullptr) {
        return true; // Null result is always an error
    }

    switch (type)
    {
        case SQL_SELECT:
            return PQresultStatus(res) == PGRES_TUPLES_OK;

        case SQL_INSERT:
        case SQL_UPDATE:
        case SQL_DELETE:
        case SQL_TRUNCATE:
            return PQresultStatus(res) == PGRES_COMMAND_OK;

        case SQL_CREATE:
        case SQL_ALTER:
        case SQL_DROP:
            return PQresultStatus(res) == PGRES_COMMAND_OK;

        case SQL_GRANT:
        case SQL_REVOKE:
            return PQresultStatus(res) == PGRES_COMMAND_OK;

        case SQL_VACUUM: // Vacuum does not return a result
            return PQresultStatus(res) == PGRES_COMMAND_OK;

        default:
            return false; // Unknown query type
    }
}




int DB::execPrepared(Statement& s, const vector<string>& params, DBResult& result){
    if(!s.prepared) return DB_EXEC_NOT_PREPARED_ERROR;

    //const char* formatedParams[s.nParams];
    std::vector<const char*> formatedParams;
    formatedParams.reserve(s.nParams);  // Preallocate memory to avoid reallocation


    for (const auto& param : params) {
        formatedParams.push_back(param.c_str());
    }

    result.result = PQexecPrepared(
        dbConn, 
        s.name.c_str(), 
        s.nParams, 
        formatedParams.data(), 
        nullptr, 
        nullptr, 
        0
    );

    if(!checkResult(result.result, s.type)) {
        PQclear(result.result);
        result.result = nullptr;
        return DB_EXEC_RESULT_ERROR;
    };
    
    return NO_ERROR;
}


