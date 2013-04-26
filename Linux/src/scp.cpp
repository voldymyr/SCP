/******************************************************************************
 *
 *  Synchronization Control Protocol. Copyright (C) 2013 Volodymyr Bendiuga
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 * Please find full license text in License.txt
 *
 * Contact info: vova.bendiuga@gmail.com
 *
 *****************************************************************************/

#include "scp.h"
#include "scp_impl.h"

Concurrent::Concurrent( pthread_mutex_t& hmutex, std::map<int, sem_t>&\
  event_table, std::map<int, std::vector<int> >& access_table,\
  std::map<int, std::map<int, int> >& tables_of_users,\
  std::map<int, int>& count_table, std::map<int, int>& table_of_flags,\
  std::map<int, int>& missed_wakeups, std::map<int, int>& resource_state,\
  int id )
{
  SCP_concurrent::SCP_concurrent( pthread_mutex_t& hmutex,\
    std::map<int, sem_t>& event_table, std::map<int, std::vector<int> >&\
    access_table, std::map<int, std::map<int, int> >& tables_of_users,\
    std::map<int, int>& count_table, std::map<int, int>& table_of_flags,\
    td::map<int, int>& missed_wakeups, std::map<int, int>&\
    resource_state, int id );
}

Concurrent::~Concurrent(void)
{
	SCP_concurrent::~SCP_concurrent();
}

/* INITIALIZATION */
int Concurrent::init( bool bcount = false )
{
	return SCP_concurrent::init(bcount);
}

/* DEINITIALIZATION */
int Concurrent::deinit()
{
	return SCP_concurrent::deinit(void);
}

/* MARK RESOURCES */
int Concurrent::mark_resource( int res_id )
{
	return SCP_concurrent::mark_resource(res_id);
}

/* UNMARK RESOURCES */
int Concurrent::unmark_resource( int res_id )
{
	return SCP_concurrent::unmark_resource(res_id);
}

/* REGISTER RESOURCE */
int Concurrent::register_resource( int res_id, std::map<int, int>\
 table_of_users )
{
	return SCP_concurrent::register_resource(res_id, table_of_users);
}

/* UNREGISTER RESOURCE */
int Concurrent::unregister_resource( int res_id )
{
	return SCP_concurrent::unregister_resource(res_id);
}

/* ACQUIRE RESOURCES */
int Concurrent::acquire_resources()
{
	return SCP_concurrent::acquire_resources();
}

/* RELINQUISH RESOURCES */
int Concurrent::release_resources()
{
	return SCP_concurrent::release_resources();
}