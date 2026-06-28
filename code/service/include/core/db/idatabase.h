#pragma once

/*
#include <functional>


#include "db/models/product.h"
#include "db/models/order.h"
#include "db/models/transaction.h"
#include "db/models/currency.h"
#include "db/models/version.h"
#include "db/models/investor.h"
#include "db/models/user.h"

#include "utils/macros.h"

using namespace std;

namespace dte {

    template <class T>
    class IDatabaseEntity {

    public:
        IDatabaseEntity(QSqlDatabase * database = nullptr): _database(database) {}

        /////////////////////////
        virtual bool GetAllItems(QList<T>& items, bool forceUpdate=false) {
            if(forceUpdate) {
                QSqlError error;
                auto response = GetAllItems(items, error);
                if (response)
                    _cacheItems = items;
                return response;
            } else {
                items = _cacheItems;
                return true;
            }
        }

        virtual bool GetAllItems(QList<T>& items, QSqlError& error) {
            auto success = false;
            if(_database && _database->isOpen() && _database->isValid()) {
                QSqlError daoError = qx::dao::fetch_all(items, _database);
                success = HandleQueryResponse(daoError);
            } else {
                syslog(LOG_ERR, "Unable to perform database query because database connection is not set or valid");
            }
            return success;
        }

        virtual const QList<T>& GetAllItems(bool forceUpdate=false) {
            if(forceUpdate) {
                QSqlError error;
                bool updateSuccess = GetAllItems(_cacheItems, error);
                if(!updateSuccess) {
                    syslog(LOG_ERR, "Error performing operation: %s::%s", __CLASS_NAME_CSTR__, __METHOD_NAME_CSTR__);
                }
                return _cacheItems;
            } else {
                return _cacheItems;
            }
        }

        /////////////////////////
        virtual bool GetItem(T& item, qx::QxSqlQuery query) {
            auto success = false;
            if(_database && _database->isOpen() && _database->isValid()) {
                QSqlError daoError = qx::dao::fetch_by_query(query, item, _database);
                success = HandleQueryResponse(daoError);
            } else {
                syslog(LOG_ERR, "Unable to perform database query because database connection is not set or valid");
                syslog(LOG_ERR, "SQL Query: %s", query.query().toStdString().c_str());
            }
            return success;
        }

        virtual bool GetItem(T& item, function<bool(const T& item)> condition, bool forceUpdate=false) {
            if(forceUpdate) {
                QSqlError error;
                if(!GetAllItems(_cacheItems, error))
                    return false;
            }
            for(auto it : _cacheItems) {
                if(condition(it)) {
                    item = it;
                    return true;
                }
            }
            return false;
        }

        virtual bool GetItems(QList<T>& items, qx::QxSqlQuery query) {
            auto success = false;
            if(_database && _database->isOpen() && _database->isValid()) {
                QSqlError daoError = qx::dao::fetch_by_query(query, items, _database);
                success = HandleQueryResponse(daoError);
            } else {
                syslog(LOG_ERR, "Unable to perform database query because database connection is not set or valid");
                syslog(LOG_ERR, "SQL Query: %s", query.query().toStdString().c_str());
            }
            return success;
        }

        virtual bool GetItems(QList<T>& items, function<bool(const T& item)> condition, bool forceUpdate=false) {
            if(forceUpdate) {
                QSqlError error;
                if(!GetAllItems(_cacheItems, error))
                    return false;
            }
            for(auto item : _cacheItems) {
                if(condition(item))
                    items.push_back(item);
            }
            return true;
        }

        /////////////////////////
        virtual bool UpdateItem(T& item, bool forceUpdate=false) {
            auto success = false;
            auto itemPosition = _cacheItems.indexOf(item);
            if(itemPosition != -1) {
                _cacheItems.replace(itemPosition, item);
            }
            if(forceUpdate) {
                if (_database && _database->isOpen() && _database->isValid()) {
                    bool transaction = _database->transaction();
                    QSqlError daoError = qx::dao::update(item, _database);
                    success = HandleQueryResponse(daoError);
                    if(success) {
                        if(transaction) {
                            _database->commit();
                        }
                    } else {
                        if(transaction) {
                            _database->rollback();
                        }
                    }
                } else {
                    syslog(LOG_ERR, "Unable to perform database query because database connection is not set or valid");
                }
            } else {
                success = true;
            }
            return success;
        }

        virtual bool UpdateItems(QList<T>& items, bool forceUpdate=false) {
            auto successCount = 0;
            for(auto item : items) {
                successCount += UpdateItem(item, forceUpdate);
            }
            return (successCount == items.size());
        }

        /////////////////////////
        virtual bool InsertItem(T& item, bool forceUpdate=false) {
            auto success = false;
            if(!_cacheItems.contains(item))
                _cacheItems.push_back(item);
            if(forceUpdate) {
                if (_database && _database->isOpen() && _database->isValid()) {
                    bool transaction = _database->transaction();
                    QSqlError daoError = qx::dao::insert(item, _database);
                    success = HandleQueryResponse(daoError);
                    if(success) {
                        if(transaction) {
                            _database->commit();
                        }
                    } else {
                        if(transaction) {
                            _database->rollback();
                        }
                    }
                } else {
                    syslog(LOG_ERR, "Unable to perform database query because database connection is not set or valid");
                }
            } else {
                success = true;
            }
            return success;
        }

        virtual bool InsertItems(QList<T>& items, bool forceUpdate=false) {
            auto successCount = 0;
            for(auto item : items) {
                successCount += InsertItem(item, forceUpdate);
            }
            return (successCount == items.size());
        }

        /////////////////////////
        virtual bool DeleteItems(function<bool(const T& item)> condition, bool forceUpdate=false) {
            int successCount = 0;
            int conditionCount = 0;
            for(auto item : _cacheItems) {
                if(condition(item)) {
                    conditionCount++;
                    successCount += DeleteItem(item, forceUpdate);
                }
            }
            return (conditionCount == successCount);
        }

        virtual bool DeleteItems(bool forceUpdate=false) {
            auto success = false;
            _cacheItems.clear();
            if(forceUpdate) {
                if (_database && _database->isOpen() && _database->isValid()) {
                    bool transaction = _database->transaction();
                    QSqlError daoError = qx::dao::delete_all<T>(_database);
                    success = HandleQueryResponse(daoError);
                    if(success) {
                        if(transaction) {
                            _database->commit();
                        }
                    } else {
                        if(transaction) {
                            _database->rollback();
                        }
                    }
                } else {
                    syslog(LOG_ERR, "Unable to perform database query because database connection is not set or valid");
                }
            } else {
                success = true;
            }
            return success;
        }

        virtual bool DeleteItem(T& item, bool forceUpdate=false) {
            auto success = false;
            _cacheItems.removeOne(item);
            if(forceUpdate) {
                if (_database && _database->isOpen() && _database->isValid()) {
                    bool transaction = _database->transaction();
                    QSqlError daoError = qx::dao::delete_by_id(item, _database);
                    success = HandleQueryResponse(daoError);
                    if(success) {
                        if(transaction) {
                            _database->commit();
                        }
                    } else {
                        if(transaction) {
                            _database->rollback();
                        }
                    }
                } else {
                    syslog(LOG_ERR, "Unable to perform database query because database connection is not set or valid");
                }
            } else {
                success = true;
            }
            return success;
        }

        /////////////////////////

        virtual const QList<T>& Items() { return _cacheItems; }
        //virtual const QMap<QString, T>& ItemIndex() { return _cacheItemsIndex; }

        virtual bool HandleQueryResponse(const QSqlError& error) {
            switch(error.type()) {
                case QSqlError::NoError:
                    return true;
                case QSqlError::TransactionError:
                    syslog(LOG_ERR, "Transaction error retrieving data from datastore: %s", error.text().toStdString().c_str());
                    break;
                case QSqlError::ConnectionError:
                    syslog(LOG_ERR, "Connection error retrieving data from datastore: %s", error.text().toStdString().c_str());
                    break;
                case QSqlError::UnknownError:
                default:
                    syslog(LOG_ERR, "Error retrieving data from datastore: %s", error.text().toStdString().c_str());
                    break;
            }
            syslog(LOG_ERR, "Database error number: %d", error.number());
            syslog(LOG_ERR, "Database error text: %s", error.databaseText().toStdString().c_str());
            syslog(LOG_ERR, "Database driver error text: %s", error.driverText().toStdString().c_str());
            return false;
        }

    private:
        QList<T> _cacheItems;                   // Represents the main cache used
        //QMap<QString, T> _cacheItemsIndex;    // Map of the items (perhaps used for quicker look ups in future)
        QSqlDatabase  *  _database = nullptr;
    };

    class IDatabaseSchema {
    public:
        virtual IDatabaseEntity<db::ProductPtr>&            Products() = 0;
        virtual IDatabaseEntity<db::Investor_ptr>&          Investors() = 0;
        virtual IDatabaseEntity<db::User_ptr>&              Users() = 0;
        virtual IDatabaseEntity<db::Currency_ptr>&          Currencies() = 0;
        virtual IDatabaseEntity<db::Order_ptr>&             Orders() = 0;
        virtual IDatabaseEntity<db::Transaction_ptr>&       Transactions() = 0;
        virtual IDatabaseEntity<db::DatabaseVersion_ptr>&   DatabaseVersion() = 0;

        virtual QSqlDatabase& DatabaseObject() = 0;

        virtual bool CreateSchema() = 0;
        //virtual bool UpdateSchema() = 0;

        virtual bool ExecuteSqlScript(const QString& filepath) = 0;

        // TODO: Resolve this duplicate code. It is useful having this at DB level as well as entity level.
        virtual bool HandleQueryResponse(const QSqlError& error) {
            switch(error.type()) {
                case QSqlError::NoError:
                    return true;
                case QSqlError::TransactionError:
                    syslog(LOG_ERR, "Transaction error retrieving data from datastore: %s", error.text().toStdString().c_str());
                    break;
                case QSqlError::ConnectionError:
                    syslog(LOG_ERR, "Connection error retrieving data from datastore: %s", error.text().toStdString().c_str());
                    break;
                case QSqlError::UnknownError:
                default:
                    syslog(LOG_ERR, "Error retrieving data from datastore: %s", error.text().toStdString().c_str());
                    break;
            }
            syslog(LOG_ERR, "Database error number: %d", error.number());
            syslog(LOG_ERR, "Database error text: %s", error.databaseText().toStdString().c_str());
            syslog(LOG_ERR, "Database driver error text: %s", error.driverText().toStdString().c_str());
            return false;
        }
    };

}

*/