/******************************************************************************
 *
 *  Synchronization Control Protocol. Copyright (C) 2013 Volodymyr Bendiuga
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version version 2.1 of the License,
 * or (at your option) any later version.
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

#ifndef SCP_IMPL_H
#define SCP_IMPL_H

#include <vector>
#include <map>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>		/* Probably not needed here */

class Concurrent
{
public:

	/*
	 * All parameters should be declared globally and the passed to 
	 * constructor
	 */
    Concurrent( pthread_mutex_t& hmutex, std::map<int, sem_t>& event_table,\
	std::map<int, std::vector<int> >& access_table, std::map<int,\
	std::map<int, int> >& tables_of_users, std::map<int, int>& count_table,\
	std::map<int, int>& table_of_flags, std::map<int, int>& missed_wakeups,\
	std::map<int, int>& resource_state, int id );
    
    ~Concurrent (void );

	/* 
	 * initialise startup structures
	 */
    int init( bool bcount );

	/* 
	 * deinitialise on exiting
	 */
    int deinit();

	/* 
	 * mark resources prior to using them
	 */
    int mark_resource(int res_id);

	/*
	 * unmark resources if they are not going to be used soon
	 */
    int unmark_resource(int res_id);

	/* 
	 * register resource
	 */
    int register_resource( int res_id, std::map<int, int> );

	/* 
	 * unregister resource
	 */
    int unregister_resource(int res_id );
	
	/* 
	 * acquire all marked resources
	 */
    int acquire_resources();

	/* 
	 * release all marked resources
	 */
    int release_resources();
};

#endif // SCP_IMPL_H