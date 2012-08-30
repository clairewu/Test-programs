#include <stdio.h>
#include <pthread.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "arbor_include.h"

void login_db_module(Arb_connection **connection)
{
  int argc = 3;
  int iserver_id;
  char *env_arbordata;
  char *env_ctrl_rpt_fname;
  int timer_on =  - 1;
  int trace_on =  - 1;
  int debug_flag = TRUE;
  char *argv[] =
  {
    "CAP", "CAP", "3"
  };

  if (0 == module_std_init(argc, argv, "CAP", debug_flag,  &iserver_id,
    &env_arbordata,  &env_ctrl_rpt_fname,  &trace_on,  &timer_on,  connection))
    printf("failed to connect the db");
    printf("Succeed to connect the db");
}

int login_db_my(Arb_connection **connection)
{

     /* Datalayer Thread initialization */
   if (init_datalayer()!=SUCCESS) {
	return FAILURE;
	}
  if(arb_database_login("SABP01MA","SABP01M1", "arborbmb","arbor123", "test_thread",ARB_STD_CONNECT,connection) != SUCCESS)
  {
  return FAILURE;
  }
  
  
}

void *test_multi_thread(Arb_connection *dbp)
{
  int Apply_unit_cr_by_date;
  if (get_system_parameter_int(dbp,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
  printf("Apply_unit_cr_by_date: %d\n", Apply_unit_cr_by_date);
  pthread_exit(NULL);
}

void *test_multi_db(void *msg)
{
printf("in test_multi_db\n ");
    Arb_connection *connection;
  int Apply_unit_cr_by_date;
  if(!login_db_my(&connection))
  {
    printf("Failed to connect db\n");
    return;
  }
  
  if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	 if (get_system_parameter_int(connection,"BIP","APPLY_UNIT_CR_BY_DATE",
				 &Apply_unit_cr_by_date) == FAILURE)
	Apply_unit_cr_by_date = 0;
	
  printf("Apply_unit_cr_by_date: %d\n", Apply_unit_cr_by_date);
  pthread_exit(NULL);
}



int main(void)
{
  pthread_attr_t threadAttr;
  pthread_t tid1;
  Arb_connection *connection;
  
  // initialize the thread attribute
  pthread_attr_init(&threadAttr);
  // Set the stack size of the thread
  pthread_attr_setstacksize(&threadAttr, 2 *1024 * 1024);
  
  //login_db(&connection);
  
  //pthread_create(&tid1, &threadAttr, test_multi_thread, connection);
  pthread_create(&tid1, &threadAttr, test_multi_db, NULL);

  sleep(5);
  printf("wake up");
  pthread_attr_destroy(&threadAttr);


}

int proc_wrap_up(){}
