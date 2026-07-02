#pragma once
#include <soci/soci.h>
#include <string>
#include <vector>
#include <optional>

namespace acl { namespace logos { namespace core { namespace db {

template <typename T>
class EntityManager
{
public:
    explicit EntityManager(soci::session& sql)
        : sql_(sql)
    {}

    // -------------------------
    // CREATE
    // -------------------------
    template <typename InsertFn>
    void create(const T& entity, InsertFn insertFn)
    {
        sql_ << insertFn(entity);
    }

    // -------------------------
    // READ (by ID)
    // -------------------------
    template <typename RowMapper>
    std::optional<T> getById(const std::string& table,
                             const std::string& idField,
                             const std::string& idValue,
                             RowMapper mapper)
    {
        soci::row row;

        std::string query =
            "SELECT * FROM " + table +
            " WHERE " + idField + " = :id LIMIT 1";

        auto result = sql_.once << query, soci::into(row), soci::use(idValue);

        if (!sql_.got_data())
            return std::nullopt;

        return mapper(row);
    }

    // -------------------------
    // UPDATE
    // -------------------------
    template <typename UpdateFn>
    void update(const T& entity, UpdateFn updateFn)
    {
        sql_ << updateFn(entity);
    }

    // -------------------------
    // DELETE
    // -------------------------
    void remove(const std::string& table,
                const std::string& idField,
                const std::string& idValue)
    {
        sql_ << "DELETE FROM " + table +
                " WHERE " + idField + " = :id",
                soci::use(idValue);
    }

    // -------------------------
    // LIST ALL
    // -------------------------
    template <typename RowMapper>
    std::vector<T> listAll(const std::string& table,
                           RowMapper mapper)
    {
        soci::rowset<soci::row> rs =
            (sql_.prepare << "SELECT * FROM " + table);

        std::vector<T> results;

        for (auto const& row : rs)
        {
            results.push_back(mapper(row));
        }

        return results;
    }

private:
    soci::session& sql_;
};

} } } }