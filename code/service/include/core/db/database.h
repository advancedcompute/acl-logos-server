#pragma once

#include "core/db/idatabase.h"
/*
#include "db/io/currency.h"
#include "db/io/currency.h"
#include "db/io/db_version.h"
#include "db/io/investor.h"
#include "db/io/order.h"
#include "db/io/product.h"
#include "db/io/transaction.h"
#include "db/io/user.h"

namespace dte::db {

    class DatabaseModel: public IDatabaseSchema {
    public:
        explicit DatabaseModel();
        explicit DatabaseModel(QSqlDatabase& database);

        IDatabaseEntity<db::ProductPtr>& Products() override;

        IDatabaseEntity<db::Investor_ptr>& Investors() override;

        IDatabaseEntity<db::User_ptr>& Users() override;

        IDatabaseEntity<db::Currency_ptr>& Currencies() override;

        IDatabaseEntity<db::Order_ptr>& Orders() override;

        IDatabaseEntity<db::Transaction_ptr>& Transactions() override;

        IDatabaseEntity<db::DatabaseVersion_ptr>& DatabaseVersion() override;

        bool CreateSchema();

        //bool UpdateSchema() override;

        QSqlDatabase& DatabaseObject() override;

    protected:

        template <class S>
        bool CreateSchemaTable(const QString& pkColumnName, IDatabaseEntity<S>& tableEntity, QSqlDatabase * db) {
            S item;
            long count;
            qx::QxSqlQuery query;
            auto countError = qx::dao::count<S>(count, query, db);
            bool returnResult = tableEntity.HandleQueryResponse(countError);

            if(!returnResult) {
                // Table doesn't exist. Attempt to create it.
                syslog(LOG_INFO, "Unable to find table. Attempting to create it...");
                auto createTableResponse = qx::dao::create_table<S>(db);
                return tableEntity.HandleQueryResponse(createTableResponse);
            } else {
                // Table already exists
                return true;
            }
        }

        bool ExecuteSqlScript(const QString& filepath) override;

    private:
        ProductEntity  _cryptoProducts = ProductEntity(&_database);
        OrderEntity _cryptoOrders = OrderEntity(&_database);
        TransactionEntity _cryptoTransactions = TransactionEntity(&_database);
        CurrencyEntity  _currencies = CurrencyEntity(&_database);
        DatabaseVersionEntity   _databaseVersions = DatabaseVersionEntity(&_database);
        InvestorEntity _investors = InvestorEntity(&_database);
        UserEntity      _users = UserEntity(&_database);

        QSqlDatabase _database;

        static int TableCount() { return 7; }       // Number of tables
    };

}

*/
