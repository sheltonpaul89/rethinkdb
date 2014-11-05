// Copyright 2010-2014 RethinkDB, all rights reserved.
#ifndef CLUSTERING_ADMINISTRATION_TABLES_TABLE_STATUS_HPP_
#define CLUSTERING_ADMINISTRATION_TABLES_TABLE_STATUS_HPP_

#include <string>
#include <vector>

#include "errors.hpp"
#include <boost/shared_ptr.hpp>

#include "clustering/administration/reactor_driver.hpp"
#include "clustering/administration/tables/table_common.hpp"
#include "concurrency/watchable.hpp"
#include "rdb_protocol/context.hpp"

class server_name_client_t;
class table_status_artificial_table_backend_t;

// Utility function to wait for table readiness through the `table_status` backend
enum class table_wait_result_t {
    WAITED,    // The table is ready after waiting for it
    IMMEDIATE, // The table was already ready
    DELETED,   // The table has been deleted
};
table_wait_result_t wait_for_table_readiness(
        const namespace_id_t &table_id,
        table_readiness_t readiness,
        table_status_artificial_table_backend_t *table_status_backend,
        signal_t *interruptor);

class table_status_artificial_table_backend_t :
    public common_table_artificial_table_backend_t
{
public:
    table_status_artificial_table_backend_t(
            boost::shared_ptr< semilattice_readwrite_view_t<
                cluster_semilattice_metadata_t> > _semilattice_view,
            watchable_map_t<std::pair<peer_id_t, namespace_id_t>,
                namespace_directory_metadata_t> *_directory_view,
            server_name_client_t *_name_client) :
        common_table_artificial_table_backend_t(_semilattice_view),
        directory_view(_directory_view),
        name_client(_name_client) { }

    bool write_row(
            ql::datum_t primary_key,
            bool pkey_was_autogenerated,
            ql::datum_t *new_value_inout,
            signal_t *interruptor,
            std::string *error_out);

private:
    bool format_row(
            namespace_id_t table_id,
            name_string_t table_name,
            name_string_t db_name,
            const namespace_semilattice_metadata_t &metadata,
            signal_t *interruptor,
            ql::datum_t *row_out,
            std::string *error_out);

    friend table_wait_result_t wait_for_table_readiness(
        const namespace_id_t &, table_readiness_t,
        table_status_artificial_table_backend_t *, signal_t *);

    watchable_map_t<std::pair<peer_id_t, namespace_id_t>,
        namespace_directory_metadata_t> *directory_view;
    server_name_client_t *name_client;
};

#endif /* CLUSTERING_ADMINISTRATION_TABLES_TABLE_STATUS_HPP_ */

