
#include "core/db/database.h"

/*
#include <utils/string_utils.h>
#include <utils/file_utils.h>


namespace dte::db {

    DatabaseModel::DatabaseModel() {
        _database = QSqlDatabase();
    }

    DatabaseModel::DatabaseModel(QSqlDatabase& database) {
        _database = database;
    }

    IDatabaseEntity<db::ProductPtr>& DatabaseModel::Products() {
        return _cryptoProducts;
    }

    IDatabaseEntity<db::Investor_ptr>&  DatabaseModel::Investors() {
        return _investors;
    }

    IDatabaseEntity<db::User_ptr>& DatabaseModel::Users() {
        return _users;
    }

    IDatabaseEntity<db::Currency_ptr>& DatabaseModel::Currencies() {
        return _currencies;
    }

    IDatabaseEntity<db::Order_ptr>& DatabaseModel::Orders() {
        return _cryptoOrders;
    }

    IDatabaseEntity<db::Transaction_ptr>& DatabaseModel::Transactions() {
        return _cryptoTransactions;
    }

    IDatabaseEntity<db::DatabaseVersion_ptr>& DatabaseModel::DatabaseVersion() {
        return _databaseVersions;
    }

    bool DatabaseModel::CreateSchema() {
        bool success = false;

        if(DatabaseObject().isOpen() && DatabaseObject().isValid()) {
            int successCount = 0;
            bool supportsTransactions = DatabaseObject().driver()->hasFeature(QSqlDriver::Transactions);

            if(supportsTransactions) {
                DatabaseObject().transaction();
            } else {
                syslog(LOG_WARNING, "Transactions are not supported on this database");
            }

            successCount += CreateSchemaTable(Order::column_internal_id(), Orders(), &DatabaseObject());
            successCount += CreateSchemaTable(Transaction::column_id(), Transactions(), &DatabaseObject());
            successCount += CreateSchemaTable(Product::column_internal_id(), Products(), &DatabaseObject());
            successCount += CreateSchemaTable(Currency::column_id(), Currencies(), &DatabaseObject());
            successCount += CreateSchemaTable(Version::column_id(), DatabaseVersion(), &DatabaseObject());
            successCount += CreateSchemaTable(Investor::column_id(), Investors(), &DatabaseObject());
            successCount += CreateSchemaTable(User::column_id(), Users(), &DatabaseObject());

            if(supportsTransactions) {
                if(successCount == TableCount()) {
                    success = true;
                    syslog(LOG_INFO, "Committing database transaction. All tables created successfully");
                    DatabaseObject().commit();
                } else {
                    syslog(LOG_WARNING, "Rolling back database transaction: only %d/%d tables created successfully", successCount, TableCount());
                    DatabaseObject().rollback();
                }
            } else {
                success = true;
            }
        } else {
            syslog(LOG_ERR, "Unable to create database schema - Database connection is not open");
        }
        return success;
    }

    bool DatabaseModel::ExecuteSqlScript(const QString& filepath) {
        string dbInitScriptContent;
        bool dbInitScriptLoaded = utils::readFileContents(filepath.toStdString(), dbInitScriptContent);
        bool scriptExecuted = false;

        if(dbInitScriptLoaded) {
            QSqlQuery query;
            bool success = true;
            auto lines = utils::split(dbInitScriptContent, ";", true);
            auto transaction = DatabaseObject().transaction();

            syslog(LOG_INFO, "Applying database script '%s', please wait...", filepath.toStdString().c_str());

            for(const auto& line : lines) {
                if(!line.empty()) {
                    query = DatabaseObject().exec(line.c_str());
                    success = HandleQueryResponse(query.lastError());
                    if(!success) {
                        syslog(LOG_ERR, "SQL Query: %s", query.lastQuery().toStdString().c_str());
                    }
                }
            }

            if(transaction) {
                if(success) {
                    syslog(LOG_INFO, "Successfully applied database script '%s' to database '%s' at '%s:%d' - committing transaction",
                           filepath.toStdString().c_str(), DatabaseObject().databaseName().toStdString().c_str(),
                           DatabaseObject().hostName().toStdString().c_str(), DatabaseObject().port());

                    if(DatabaseObject().commit()) {
                        scriptExecuted = true;
                    } else {
                        syslog(LOG_ERR, "Error attempting to commit database transaction: %s", DatabaseObject().lastError().text().toStdString().c_str());
                    }
                } else {
                    syslog(LOG_ERR, "Unsuccessfully applied database script '%s' to database '%s' at '%s:%d' - rolling back transaction",
                           filepath.toStdString().c_str(), DatabaseObject().databaseName().toStdString().c_str(),
                           DatabaseObject().hostName().toStdString().c_str(), DatabaseObject().port());

                    if(!DatabaseObject().rollback()) {
                        syslog(LOG_ERR, "Error attempting to rollback database transaction: %s",
                               DatabaseObject().lastError().text().toStdString().c_str());
                    }
                }
            } else {
                if(success) {
                    scriptExecuted = true;
                    syslog(LOG_INFO, "Successfully applied database script '%s' to database '%s' at '%s:%d'",
                           filepath.toStdString().c_str(), DatabaseObject().databaseName().toStdString().c_str(),
                           DatabaseObject().hostName().toStdString().c_str(), DatabaseObject().port());
                } else {
                    syslog(LOG_ERR, "Unsuccessfully applied database script '%s' to database '%s' at '%s:%d' - rolling back transaction",
                           filepath.toStdString().c_str(), DatabaseObject().databaseName().toStdString().c_str(),
                           DatabaseObject().hostName().toStdString().c_str(), DatabaseObject().port());
                }
            }
        } else {
            syslog(LOG_ERR, "Unable to load script: %s", filepath.toStdString().c_str());
        }
        return scriptExecuted;
    }

    QSqlDatabase& DatabaseModel::DatabaseObject() {
        return _database;
    }

}

*/