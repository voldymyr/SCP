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

#ifndef SCP_IMPL_H
#define SCP_IMPL_H		/* Might need to be modified */

#include <vector>
#include <map>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

class SCP_concurrent
{
public:

	/*
	 * Constructor that receives parameters defined globally
	 */
    SCP_oncurrent( pthread_mutex_t& hmutex, std::map<int, sem_t>& event_table,\
	std::map<int, std::vector<int> >& access_table,\
	std::map<int, std::map<int, int> >& tables_of_users,\
	std::map<int, int>& count_table, std::map<int, int>& table_of_flags,\
	std::map<int, int>& missed_wakeups, std::map<int, int>& resource_state,\
	int id );

    ~SCP_concurrent (void );

	/*
	 * initialise startup structures
	 */
    int init( bool bcount );

	/*
	 * deinitialise on exiting
	 */
    int deinit();

	/*
	 * mark resources prior to using them (only those one wants to access)
	 */
    int mark_resource(int res_id);

	/*
	 * unmark resources if they are not going to be used in near future
	 */
    int unmark_resource(int res_id);

	/*
	 * object can register its resource or global resource
	 */
    int register_resource( int res_id, std::map<int, int> );

	/*
	 * if object provides a resource, it should unregister it before exiting
	 */
    int unregister_resource(int res_id );

	/*
	 * acquires all marked resources
	 */
    int acquire_resources();

	/*
	 * releases all marked resources
	 */
    int release_resources();

protected:

    int mn_id;		/* ID of Object */

private:

	/*
	 * finds minimum access count
	 */
    int find_min( std::map<int, int>& inmap );

	/*
	 * finds maximum missedwakeups count
	 */
    int find_max( std::map<int, int>& inmap );


	/*
	 * global mutex, used to protect all shared resources
	 */
    pthread_mutex_t& mrh_mutex;

	/*
	 * reference to global event table; one event for each object
	 * (key: objectID; value: binary semaphore)
	 */
    std::map<int, sem_t>& mr_event_table;

	/*
	 * reference to global access table;(key: objectID;
	 * value: vector of resource IDs that object accesses)
	 */
    std::map<int, std::vector<int> >& mr_access_table;

	/*
	 * reference to global tables of users. contains tables
	 * of users; each table contains: (id: resourceID;
	 * key: ObjectID; value: accessType)
	 */
    std::map<int, std::map<int, int> >& mr_tables_of_users;

	/*
	 * reference to global count table; number of accesses to different
	 * shared resources by each object (key: objectID; value: count)
	 */
    std::map<int, int>& mr_count_table;

	/*
	 * reference to global table of flags; one flag for each resource;
	 * flag can be either BUSY or FREE
	 */
    std::map<int, int>& mr_table_of_flags;

	/*
	 * counts missed wake ups (key: object id; value: count variable)
	 */
    std::map<int, int>& mr_missed_wakeups;

	/*
	 * shows if resource is marked by some objects, and by how many
	 * (key: resource id; value: number of objects that marked resource)
	 */
    std::map<int, int>& mr_resource_state;

	/*
	 * iterator for mr_tables_of_users
	 */
    std::map<int, std::map<int, int> >::iterator m_it_tu;

    std::map<int, int>::iterator m_it;

	/*
	 * state of object when it is not interested in any resource
	 */
    static const int PASSIVE = 0;

	/*
	 * state of object when it is currently accessing shared resources
	 */
    static const int ACCESSING = 1;

	/* 
	 * state of object when it is interested in the resources,
	 * but they are taken
	 */
    static const int WAITING = 2;

	/*
	 * state of resource when it is free and can be taken by the
	 * first one who asks for it
	 */
    static const int FREE = 3;

	/*
	 * state of resource when it is being accessed by some object
	 */
    static const int BUSY = 4;
};

#endif // SCP_IMPL_H
