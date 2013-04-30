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
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 US
 *
 * Please find full license text in License.txt
 *
 * Contact info: vova.bendiuga@gmail.com
 *
 *****************************************************************************/

#include "../include/scp_impl.h"

SCP_concurrent::SCP_concurrent( pthread_mutex_t& hmutex, std::map<int, sem_t>&\
 event_table, std::map<int, std::vector<int> >& access_table,\
    std::map<int, std::map<int, int> >& tables_of_users, std::map<int, int>&\
	count_table, std::map<int, int>& table_of_flags, std::map<int, int>&\
	missed_wakeups, std::map<int, int>& resource_state, int id )
    :mrh_mutex( hmutex ), mr_event_table( event_table ),\
	mr_access_table( access_table ), mr_tables_of_users( tables_of_users ),\
	mr_count_table( count_table ), mr_table_of_flags( table_of_flags ),\
	mr_missed_wakeups( missed_wakeups ), mr_resource_state( resource_state ),\
	mn_id( id )
{
}

SCP_concurrent::~SCP_concurrent( void )
{
}

/****************************** INITIALIZATION *******************************/
int SCP_concurrent::init( bool bcount = false )
{
	/* lock global mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	int ret, ret_init;
	switch (result)
	{
	case 0:		// OK
		/* initialize binary semaphore with 0 */
		ret_init = sem_init( &(mr_event_table[ mn_id ]), 0, 0 );
		if(bcount)
		{
			/* iterator for count table */
			std::map<int, int>::iterator it;
			/* pick first element in the map */
			it = mr_count_table.begin();
			/* check if its count is valid number */
			if( (*it).second >= 0 )
			{
				/* runtime; init count variable with count
				 * variable of any other object (let it
				 * be first) */
				mr_count_table[ mn_id ] = (*it).second;
			}
			else
			{
				/* assign 0 in case of bad element */
				mr_count_table[ mn_id ] = 0;
			}
		}
		else
		{
			/* startup; init count variable with 0 */
			mr_count_table[ mn_id ] = 0;
		}
		/* init missedwakeups variable with 0 */
		mr_missed_wakeups[ mn_id ] = 0;
		/* release global mutex */
		ret = pthread_mutex_unlock( &mrh_mutex );
		switch(ret)
		{
		case 0:		/* success */
			/* if not zero,return semaphore initialization error */
			if( ret_init != 0 )
				return -3;
			else
				break;
		case EINVAL:
			/* mutex has not been properly initialized */
			return -2;
		default:
			/* other mutex problem */
			return -1;
		}

		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* other mutex problem */
		return -1;
	}
	return 1;	/* return SUCCESS */
}
/*****************************************************************************/


/******************************** DEINITIALIZATION ***************************/
int SCP_concurrent::deinit()
{
	/* lock global mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	int ret, del_ret;
	switch (result)
	{
	case 0:
		/* destroy binary semaphore */
		del_ret = sem_destroy( &(mr_event_table[ mn_id ]) );
		/* erase relevant record from event table */
		mr_event_table.erase( mr_event_table.find( mn_id ) );
		/* erase relevant revord from count table */
		mr_count_table.erase( mr_count_table.find( mn_id ) );
		/* erase relevant revord from missed wakeups table */
		mr_missed_wakeups.erase( mr_missed_wakeups.find( mn_id ) );
		/* unlock global mutex */
		ret = pthread_mutex_unlock( &mrh_mutex );
		switch(ret)
		{
		case 0:		/* success */
			/* return if semaphore is not valid */
			if( del_ret == -1 )
			  return -5;
			else
			  break;
		case EINVAL:
			/* mutex has not been properly initialized */
			return -2;
		default:
			/* other mutex problem */
			return -1;
		}

		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* other mutex problem */
		return -1;
	}
	return 1;		/* return SUCCESS */
}
/*****************************************************************************/


/***************************** MARK RESOURCES ********************************/
int SCP_concurrent::mark_resource( int res_id )
{
	/*  check for negative numbers (only positive integers are allowed) */
	if( res_id < 0 )
		return -6;
	/* lock global mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	int ret;
	bool bExists = false;
	switch (result)
	{
	case 0:
		m_it_tu = mr_tables_of_users.find(res_id);
		/* set true if such resource exists */
		if ((*m_it_tu).first >= 0)
			bExists = true;

		if( bExists )
		{
			/* add resource ID to access table */
			mr_access_table[ mn_id ].push_back( res_id );
			/* initially object is PASSIVE to the resource */
			mr_tables_of_users[ res_id ][ mn_id ] = PASSIVE;
			/* increase usage counter for this resource */
			mr_resource_state[ res_id ] ++;
			/* release global mutex */
			ret = pthread_mutex_unlock( &mrh_mutex );
			switch(ret)
			{
			case 0:			/* success */
				break;
			case EINVAL:
				/* muteh has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
		}
		else
		{
			/* release mutex */
			ret = pthread_mutex_unlock( &mrh_mutex );
			switch(ret)
			{
			case 0:		/* success */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
			/* specified resource does not exist */
			return -7;
		}

		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* other mutex problem */
		return -1;
	}
	return 1;	/* return SUCCESS */
}
/*****************************************************************************/


/*************************** UNMARK RESOURCES ********************************/
int SCP_concurrent::unmark_resource( int res_id )
{
	/* check for negative numbers (only positive integers are allowed) */
	if( res_id < 0 )
		return -6;
	/* lock global mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	int res;
	std::vector<int> tmp_vec;
	switch (result)
	{
	case 0:
		/* check if such resource exists */
		if( mr_table_of_flags[ res_id ] == FREE ||\
		mr_table_of_flags[ res_id] == BUSY )
		{
			tmp_vec = mr_access_table[ mn_id ];
			mr_access_table[ mn_id ].clear();
			for( unsigned int i = 0; i < tmp_vec.size(); i ++ )
			{
				if( tmp_vec[ i ] != res_id )
					mr_access_table[ mn_id ].push_back( tmp_vec[ i ] );
			}
			/* erase relevant record */
			mr_tables_of_users[ res_id ].erase( mn_id );
			/* decrease usage counter */
			mr_resource_state[ res_id ] --;
			/* release global mutex */
			res = pthread_mutex_unlock( &mrh_mutex );
			switch(res)
			{
			case 0:		/* success */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
		}
		else
		{
			/* release mutex */
			res = pthread_mutex_unlock( &mrh_mutex );
			switch(res)
			{
			case 0:			/* success */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
			/* resource does not exist */
			return -7;
		}

		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* other mutex problem */
		return -1;
	}
	return 1;		/* return SUCCESS */
}
/*****************************************************************************/


/*********************************** REGISTER RESOURCE ***********************/
int SCP_concurrent::register_resource( int res_id, std::map<int, int>\
 table_of_users )
{
	/* check for negative numbers (only positive integers are allowed) */
	if( res_id < 0 )
		return -6;
	/* lock global mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	int retval;
	switch (result)
	{
	case 0:
		/* check whether such resource does not exist */
		if( mr_table_of_flags[ res_id ] != FREE ||\
		mr_table_of_flags[ res_id] != BUSY )
		{
			/* add table of users */
			mr_tables_of_users[ res_id ] = table_of_users;
			/* mark as FREE initially */
			mr_table_of_flags[ res_id ] = FREE;
			/* release mutex */
			retval = pthread_mutex_unlock( &mrh_mutex );
			switch(retval)
			{
			case 0:			/* success */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
		}
		else
		{
			/* release global mutex */
			retval = pthread_mutex_unlock( &mrh_mutex );
			switch(retval)
			{
			case 0:			/* success */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
			/* resource already axists */
			return -8;
		}

		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* other mutex problem */
		return -1;
	}
	return 1;		/* return SUCCESS */
}
/*****************************************************************************/


/****************************** UNREGISTER RESOURCE **************************/
int SCP_concurrent::unregister_resource( int res_id )
{
	/* check for negative numbers (only positive integers are allowed) */
	if( res_id < 0 )
		return -6;
	/* lock mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	int r;
	switch (result)
	{
	case 0:
		/* check whether such resource exist */
		if( mr_table_of_flags[ res_id ] == FREE ||\
		mr_table_of_flags[ res_id] == BUSY )
		{
			if( mr_resource_state[ res_id ] == 0 )
			{
				/* clear table of users */
				mr_tables_of_users[ res_id ].clear();
				/* erase table of users from map */
				mr_tables_of_users.erase( mr_tables_of_users.find( res_id ) );
				/* erase resource from table of flags */
				mr_table_of_flags.erase( mr_table_of_flags.find( res_id ) );
				/* unlock mutex */
				r = pthread_mutex_unlock( &mrh_mutex );
				switch(r)
				{
				case 0:			/* success */
					break;
				case EINVAL:
					/* mutex has not been properly initialized */
					return -2;
				default:
					/* other mutex problem */
					return -1;
				}
			}
			else
			{
				/* release mutex */
				r = pthread_mutex_unlock( &mrh_mutex );
				switch(r)
				{
				case 0:			/* success */
					break;
				case EINVAL:
					/* mutex has not been properly initialized */
					return -2;
				default:
					/* other mutex problem */
					return -1;
				}
				/* resource can not be unregistered as some objects are
				 * interested in it */
				return -9;
			}
		}
		else
		{
			/* release mutex */
			r = pthread_mutex_unlock( &mrh_mutex );
			switch(r)
			{
			case 0:			/* success */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
			/* resource does not exist */
			return -7;
		}

		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* other mutex problem */
		return -1;
	}
	return 1;
}
/*****************************************************************************/


/******************************** ACQUIRE RESOURCES **************************/
int SCP_concurrent::acquire_resources()
{
	/* Lock gobal mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	int semtake_ret;
	/* this flag is used to check if there is at least one object that is
	 * accessing shared resource */
	bool bFlag = false;
	/* signals that event has been passed successfully or failed to pass */
	bool bPassOK = false;
	/* objectID of starving object */
	int starvingOID;
	/* 1 - signals SetState; 2 - signals ResetState : 1 && 2 are the two
	 * states that this part of algorithm can be in */
	int nSetResetState;
	/* signals success or failure on sem_post() procedure for binary
	 * semaphore */
	bool bSetEvent = true;
	switch (result)
	{
	case 0:		/* success on acquisition of global mutex */
		/* find object who starved the most */
		starvingOID = find_max( mr_missed_wakeups );

		for (unsigned int i = 0; i < mr_access_table[ mn_id ].size(); i++)
		{
			/* check if starving object is waiting for any of resource that
			 * we are going to access */
			if ( mr_tables_of_users[ mr_access_table[ mn_id ][ i ] ]\
			[ starvingOID ] == WAITING )
			{
				/* signal if starving object is waiting for at least
				 * one resource */
				bFlag = true;
				break;
			}
		}
		/* if he is waiting, then ... */
		if (bFlag)
		{
			/* check if he starved for reasonable amount of missedwakeups
			 * (number of resources he accesses + 1) */
			if ( mr_missed_wakeups[ starvingOID ] <= mr_access_table\
				[ starvingOID ].size() + 1 )
			{
				bFlag = false;
				for (unsigned int i = 0; i < mr_access_table[ mn_id ].size();\
				i++)
				{
					/* check if resources that we want to acquire are free */
					if (mr_table_of_flags[ mr_access_table[ mn_id ][ i ] ]\
					== BUSY)
					{
						/* mark flag if at least one of wanted resources
						 * is busy */
						bFlag = true;
						break;
					}
				}
			}

		}
		else		/* if he is not waiting, then ... */
		{
			for (unsigned int i = 0; i < mr_access_table[ mn_id ].size(); i++)
			{
				/* check if resources that you want to acquire are free */
				if (mr_table_of_flags[ mr_access_table[ mn_id ][ i ] ] == BUSY)
				{
					/* mark flag if at least one of wanted resources is busy */
					bFlag = true;
					break;
				}
			}
		}
		/* if wanted resources are all FREE, we can access them soon */
		if (!bFlag)
		{
			/* signal that we are in state 1  */
			nSetResetState = 1;
			/* iterate through all your (the ones you want to access)
			 * resource IDs  */
			for (unsigned int i = 0; i < mr_access_table[ mn_id ].size(); i++)
			{
				/* mark all wanted resources' flags as BUSY, as you are going\
				to use them */
				mr_table_of_flags[ mr_access_table[ mn_id ][ i ] ] = BUSY;
				/* mark your status in all your tables of users as ACCESSING */
				mr_tables_of_users[ mr_access_table[ mn_id ][ i ] ][ mn_id ]\
				= ACCESSING;
			}
			/* make binary semaphore FULL, initialy it should be EMPTY */
			int semgive_ret = sem_post( &( mr_event_table[ mn_id ] ) );
			switch (semgive_ret)
			{
			case 0:
				bSetEvent = true;
				break;
			default:
				bSetEvent = false;
				/* iterate through all your (the ones you want to access)
				 * resource IDs  */
				for (unsigned int i = 0; i < mr_access_table[ mn_id ].size();\
				i++)
				{
					/* mark all wanted resources' flags as BUSY, as you are
					 * going to use them */
					mr_table_of_flags[ mr_access_table[ mn_id ][ i ] ] = FREE;
					/* mark your status in all your tables of users as
					 * ACCESSING */
					mr_tables_of_users[ mr_access_table[ mn_id ][ i ] ]\
					[ mn_id ] = PASSIVE;
				}
				break;
			}
			/* release global mutex */
			int ret = pthread_mutex_unlock( &mrh_mutex );
			/* error check */
			switch (ret)
			{
			case 0:			/* correct */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
		}
		else
		{
			/* if wanted resources are not free, we have to sleep until they
			 * become FREE ... */
			/* singal that we are in state 2 */
			nSetResetState = 2;
			/* iterate through all your (the ones you want to access)
			 * resource IDs */
			for (unsigned int i = 0; i < mr_access_table[ mn_id ].size(); i++)
			{
				/* mark your status as WAITING in all your tables of users */
				mr_tables_of_users[ mr_access_table[ mn_id ][ i ] ][ mn_id ]\
				= WAITING;
			}
			/* release global mutex */
			int ret = pthread_mutex_unlock( &mrh_mutex );
			/* error check */
			switch (ret)
			{
			case 0:
				/* correct */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
		}
		/* check in which state we were */
		switch (nSetResetState)
		{
		case 1:
			/* check if we succeeded on sem_post() */
			if (bSetEvent)
			{
				/* pass the event, take FULL binary semaphore */
				semtake_ret = sem_wait( &( mr_event_table[ mn_id ] ) );
				/* report that we successfully passed, even if sem_wait()
				 * before failed */
				bPassOK = true;
			}
			else
			{
				/* sem_post() failed, but function can be called again,
				 * because all necessary rollback has been done */
				return -10;
			}
			break;
		case 2:
			/* sleep on the event, take EMPTY binary semaphore */
			semtake_ret = sem_wait( &( mr_event_table[ mn_id ] ) );
			/* mark flag, it will be checked if sem_wait() before failed,
			 * otherwise it won't matter */
			bPassOK = false;
			break;
		}
		/* check return value from one of the sem_wait() before */
		switch (semtake_ret)
		{
		case 0:
			break;
		default:
			/* if we failed on the first sem_wait(), execution will still
			 * be correct */
			if (bPassOK)
			{
				break;
			}
			else
			{
				/* Failed on sem_wait(), but function can be safely called
				 * again */
				return -11;
			}
		}
		/* this break belongs to the very first OK belonging to first switch
		 * statement */
		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* something is wrong with the semaphore */
		return -1;
	}

	return 1;		/* return SUCCESS */
}
/*****************************************************************************/


/******************************* RELINQUISH RESOURCES ************************/
int SCP_concurrent::release_resources()
{
	/* Lock global mutex */
	int result = pthread_mutex_lock( &mrh_mutex );
	/* temporary map that will be filled with objects that are waiting to get
	 * access to shared resource */
	std::map<int, int> tmp_map;
	/* vector holding IDs of objects that are waiting to get access to shared
	 * resource */
	std::vector<int> tmp;
	/* a flag to check if there whas been anyone trying to access resource
	 * while we were using it */
	bool bNoOne = true;

	switch (result)
	{
	case 0:		/* mutex has been acquired successfully */
		/* iterate through all your (the ones you have accessed) resource IDs */
		for (unsigned int i = 0; i < mr_access_table[ mn_id ].size(); i++)
		{
		/* mark resources as FREE as you release them */
			mr_table_of_flags[ mr_access_table[ mn_id ][ i ] ] = FREE;
			/* mark you status as PASSIVE in your tables of users */
			mr_tables_of_users[ mr_access_table[ mn_id ][ i ] ][ mn_id ]\
			= PASSIVE;
		}
		/* increment your count variable related to the resource you have\
		 * been using */
		mr_count_table[ mn_id ]++;
		/* iterate through tables of users (the ones you are in) */
		for (unsigned int i = 0; i < mr_access_table[ mn_id ].size(); i++)
		{
			/* iterate through elements of each table of users */
			for ( m_it = mr_tables_of_users[ mr_access_table[ mn_id ]\
			[ i ] ].begin(); m_it != mr_tables_of_users[ mr_access_table\
			[ mn_id ][ i ] ].end(); m_it++)
			{
				/* select from relevant user table IDs of resources with\
				 * status WAITING */
				if ((*m_it).second == WAITING)
				{
					/* push selected object's ID to the vector */
					tmp.push_back( (*m_it).first );
					/* mark the flag indicating that there is at least one\
					 * object waiting to get access to shared resource */
					bNoOne = false;
				}
			}
		}
		/* if there is no one waiting to get access to resource that we have\
		 * been using, then ... */
		if (bNoOne)
		{
			/* unlock mutex */
			int ret = pthread_mutex_unlock( &mrh_mutex );
			/* error check */
			switch (ret)
			{
			case 0:		/* success */
				break;
			case EINVAL:
				/* mutex has not been properly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
		}
		else
		{
			/* otherwise give a chance to one of the waiting objects to access\
			 * shared resource */
			/* loop through object IDs in temporary vector */
			for (unsigned int i = 0; i < tmp.size(); i++)
			{
				/* fill in temporary map: key - Object ID; value - objects
				 * relevant count variable */
				tmp_map[ tmp[ i ] ] = mr_count_table[ tmp[ i ] ];
			}
			/* flag indicates whether objects resources are all free
			 * or taken */
			bool F = false;
			/* iterate untill temporary map has elements */
			for ( ;; )
			{
				if ( tmp_map.size() == 0)
					break;
				/* find object in temporary map with the lowest count
				 * variable and give him a chance to access resource */
				int oIdToWakeUp = find_min( tmp_map );
				/* iterate through number of resource IDs that oIdToWakeUp
				 * wants to access */
				for (unsigned int k = 0; k < mr_access_table\
				[ oIdToWakeUp ].size(); k++ )
				{
					/* check if all resources that oIdToWakeUp wants to
					 * access are FREE */
					if (mr_table_of_flags[ mr_access_table[ oIdToWakeUp ]\
					[ k ] ] == BUSY)
					{
						/* if at least one resource is BUSY, mark the flag */
						F = true;
						/* increment objects missed wake up count as some of
						 * his resources are BUSY */
						mr_missed_wakeups[ oIdToWakeUp ] ++;
						break;
					}
				}
				/* if all relevant resource flags are FREE, then ...  */
				if (!F)
				{
					/* loop through number of resource IDs that oIdToWakeUp
					 * wants to access */
					for ( unsigned int p = 0; p < mr_access_table\
					[ oIdToWakeUp ].size(); p++ )
					{
						/* mark all relevant resource flags of oIdToWakeUp
						 * as BUSY */
						mr_table_of_flags[ mr_access_table[ oIdToWakeUp ]\
						[ p ] ] = BUSY;
						/* mark oIdToWakeUp's status in his user tables as
						 * ACCESSING */
						mr_tables_of_users[ mr_access_table[ oIdToWakeUp ]\
						[ p ] ][ oIdToWakeUp ] = ACCESSING;
					}
					/* unlock mutex */
					int ret = sem_post( &( mr_event_table[ oIdToWakeUp ] ) );
					/* error check */
					switch (ret)
					{
					case 0:		/* SUCCESS */
						/* reset missed wake ups count as you are going to
						 * wake the object up */
						mr_missed_wakeups[ oIdToWakeUp ] = 0;
						/* erase object from temporary map */
						tmp_map.erase( oIdToWakeUp );
						break;
					default:
						/* ERROR, performing ROLLBACK */
						/* loop through number of resource IDs that
						 * oIdToWakeUp wants to access */
						for ( unsigned int p = 0; p < mr_access_table\
							[ oIdToWakeUp ].size(); p++ )
						{
							/* mark all relevant resource flags of \
							 * back as FREE */
							/* mark oIdToWakeUp's status in his status tables
							 * as WAITING */
							mr_table_of_flags[ mr_access_table[ oIdToWakeUp ]\
							[ p ] ] = FREE; mr_tables_of_users\
							[ mr_access_table[ oIdToWakeUp ][ p ] ]\
							[ oIdToWakeUp ] = WAITING;
						}
					}
				}
				else
				{
					/* if at least one of relevant resources is not free,
					 * then ... */
					/* check objects missed wake up count */
					if ( mr_missed_wakeups[ oIdToWakeUp ] >= mr_access_table\
						[ oIdToWakeUp ].size() + 1 )
					{
						/* if it is equal or greater then number of resources
						 * he accesses, then we should not wake anyone else
						 * up, we should delete everyone from tmp map; */
						tmp_map.clear();
					}
					else
					{
						/* erase oIdToWakeUp from temporary map, because
						 * we can not wake him up, other object will try
						 * to do this */
						tmp_map.erase( oIdToWakeUp );
					}
				}
			}
			/* unlock mutex */
			int ret = pthread_mutex_unlock( &mrh_mutex );
			/* rror check */
			switch (ret)
			{
			case 0:			/* SUCCESS */
				break;
			case EINVAL:
				/* mutex has not been properlly initialized */
				return -2;
			default:
				/* other mutex problem */
				return -1;
			}
		}

		break;
	case EINVAL:
		/* mutex has not been properly initialized */
		return -2;
	case EDEADLK:
		/* mutex is already owned by the calling thread */
		return -4;
	default:
		/* other mutex problem */
		return -1;
	}

	return 1;		/* return SUCCESS */
}
/*****************************************************************************/


/************************** FINDS MINIMUM VALUE FROM MAP<INT, INT> ***********/
int SCP_concurrent::find_min( std::map<int, int>& inmap )
{
	/* set map iterator to the beginning of the map */
	m_it = inmap.begin();
	/* set variable to first Objects ID */
	int minObj = (*m_it).first;
	/* set vcariable to first objects value */
	int minVal = (*m_it).second;
	/* loop through all elements in a map */
	for (m_it = inmap.begin(); m_it != inmap.end(); m_it++)
	{
		/* find object ID with minimum caunt variable */
		if ( (*m_it).second < minVal)
		{
			minVal = (*m_it).second;
			minObj = (*m_it).first;
		}
	}
	/* return object ID whos count variable is the lowest */
	return minObj;
}
/*****************************************************************************/


/************************** FINDS MAXIMUM VALUE FROM MAP<INT, INT> ***********/
int SCP_concurrent::find_max( std::map<int, int>& inmap )
{
  	m_it = inmap.begin();
	int maxObj = (*m_it).first;
	int maxVal = (*m_it).second;
	for (m_it = inmap.begin(); m_it != inmap.end(); m_it++)
	{
		if ( (*m_it).second > maxVal )
		{
			maxVal = (*m_it).second;
			maxObj = (*m_it).first;
		}
	}
	return maxObj;
}
/*****************************************************************************/
