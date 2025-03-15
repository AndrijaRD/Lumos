#pragma once
#ifndef MySDL_DB
#define MySDL_DB

#include "../lib.h"

#define SQL_SELECT      0
#define SQL_INSERT      1
#define SQL_CREATE      2
#define SQL_UPDATE      3
#define SQL_ALTER       4
#define SQL_DROP        5
#define SQL_DELETE      6
#define SQL_TRUNCATE    7
#define SQL_GRANT       8
#define SQL_REVOKE      9
#define SQL_VACUUM      10



struct Statement {
    string name;
    string command;
    int nParams;
    int type;
    bool prepared;

    Statement(
        const string& _name = "",
        const string& _command = "",
        const int _numOfParams = 0,
        const int _commandType = SQL_SELECT
    ):  name(_name), 
        command(_command),
        nParams(_numOfParams),
        type(_commandType), 
        prepared(false) 
    {}
};

inline ostream& operator<<(ostream& os, const Statement& s){
    os << "Statement(";
        os << s.name;
        os << ", \"";
        os << s.command;
        os << "\", Type: ";
        os << s.type;
        os << ", Prepared: ";
        os << s.prepared;
        os << ", Params: ";
        os << s.nParams;
    os << ")";
    return os;
}




class DBResult {
    friend class DB;
private:
    PGresult* result;

public:
    DBResult(PGresult* res = nullptr) : result(res) {}
    ~DBResult() {
        if (result) PQclear(result);
    }

    bool isValid() const { return result != nullptr; };

    int rowCount() const    { return (isValid() ? PQntuples(result) : -1); };
    int columnCount() const { return (isValid() ? PQnfields(result) : -1); };

    string getValue(int row, int column) const {
        if ( !isValid() || row >= rowCount() || column >= columnCount()) return "";
        return PQgetvalue(result, row, column);
    };

    ExecStatusType status() const    { return PQresultStatus(result); };
    const char* errorMessage() const { return PQresultErrorMessage(result); };
};

inline ostream& operator<<(ostream& os, const DBResult& res){
    os << "Result(";
        os << "Valid: "     << res.isValid();
        os << ", Rows: "    << res.rowCount();
        os << ", Columns: " << res.columnCount();
    os << ")";
    return os;
}




class DB{
    private:
    static inline string dbName;
    static inline string dbUser;
    static inline string dbPass;
    static inline string dbAddr;
    static inline int    dbPort;

    static inline PGconn* dbConn = nullptr;

    public:
    static inline PGconn* const& conn = dbConn; // PGconn pointer exposed to the global code, but as read-only

    static int init(
        const string& dbName,
        const string& username = "postgres",
        const string& password = "password",
        const string& hostAddr = "127.0.0.1",
        const int& port = 5432
    );

    static int prepareStatement(Statement& statement);
    static int execPrepared(Statement& statement, const vector<string>& params, DBResult& result);

    private:
    static bool checkResult(const PGresult* s, const int type);
};


#endif
// Creator: @AndrijaRD