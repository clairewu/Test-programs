#include <sschar.h>
#include <stdlib.h>

#include <utlcmd.h>
#include <utlodbc.h>
#include <utlname.h>
#include <utldb.h>
#include <utldbddl.h>
#include <sistr.h>
#include <logapi.h>
#include <thrstate.h>
#include <ssversion.h>
//Hemmingway DPI: insert necessary includes
#include <ssfile.h>

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <ssstring.h>
#include <ssstrenc.h>

#define MAX_CHAR_LEN 50

void ODBC_error (			/* Obtain ODBC Error */
		HENV henv,			/* ODBC Environment */
		HDBC hdbc,			/* ODBC Connection Handle */
		HSTMT hstmt)			/* ODBC SQL Handle */
{
	SQLTCHAR	sqlstate[10];
	SQLTCHAR	errmsg[SQL_MAX_MESSAGE_LENGTH];
	SDWORD	nativeerr;
	SWORD	actualmsglen;
	RETCODE	rc;

loop:  	rc = SQLError((SQLHENV)henv, (SQLHDBC)hdbc, (SQLHSTMT)hstmt,
			sqlstate, &nativeerr, errmsg,
			SQL_MAX_MESSAGE_LENGTH - 1, &actualmsglen);

	if (rc == SQL_ERROR) {
		printf ("SQLError failed!\n");
		return;
	}

	if (rc != SQL_NO_DATA_FOUND) {
		printf ("SQLSTATE = %s\n",sqlstate);
		printf ("NATIVE ERROR = %d\n",nativeerr);
		errmsg[actualmsglen] = '\0';
		printf ("MSG = %s\n\n",errmsg);
		goto loop;
	}
}

/*Function to connect a DB using ODBC*/
void connectDB(HENV *henv,HDBC *hdbc){
	SSchar  ODBC_database_name[80];
	SSchar  ODBC_database_user_id[80];
	SSchar  ODBC_database_passwd[80];
	SQLRETURN  return_code;

	return_code = SQLAllocEnv ((SQLHENV *)henv);
	if ((return_code == SQL_SUCCESS) || (return_code == SQL_SUCCESS_WITH_INFO))
		return_code = SQLAllocConnect ((SQLHENV)*henv, (SQLHDBC *)hdbc);

	printf("This program attempts to connect to an ODBC database.\n");
	printf("Enter the ODBC database name: ");
	SSgets (ODBC_database_name);
	if (ODBC_database_name[0] == NULL_CHAR)
	{
		printf("ODBC_database_name can't be null\n");
		exit(1);
	}
	printf("Enter the user ID: ");
	SSgets(ODBC_database_user_id);
	printf("Enter the DB password: ");
	SSgets(ODBC_database_passwd);


	return_code = SQLConnect((SQLHDBC)*hdbc, (SSuchar*)ODBC_database_name, 
			SQL_NTS, (SSuchar*)ODBC_database_user_id, SQL_NTS, (SSuchar*)ODBC_database_passwd, SQL_NTS);

	if ((return_code == SQL_SUCCESS)
			|| (return_code == SQL_SUCCESS_WITH_INFO))
	{
		printf("Successful connection to database\n");
	}
	else{
		printf("Connection to database failed.\n");
		ODBC_error (NULL, (HDBC)hdbc, NULL) ;	
		exit(1);
	}

}

int SSmain(int    argc, 
		SSchar* argv[])
{
	HDBC	hdbc;
	HENV	henv;
	HSTMT	hstmt;
	SQLRETURN  return_code;


	UTLDbUtlProgStart (argc, argv, SSText("log.txt"), DB_PD_DDL_EXPORT_PROCNAME);

	/*1. Connect to DB*/
	connectDB(&henv, &hdbc);

    /* HSTMT   hstmt_session;
    return_code = SQLAllocStmt ((SQLHDBC)hdbc, (SQLHSTMT *)&hstmt_session);
	if (return_code != SQL_SUCCESS)
	{
		printf("ERROR: The SQL statement did not execute successfully.\n");
		exit(1);
	}

   return_code = SQLExecDirect(hstmt_session, SSText("alter session set NLS_LANGUAGE='SIMPLIFIED CHINESE'"), -3);
   SQLFreeStmt(hstmt_session, SQL_DROP);*/

	/*3. Execute the SQL statement*/
	return_code = SQLAllocStmt ((SQLHDBC)hdbc, (SQLHSTMT *)&hstmt);
	if (return_code != SQL_SUCCESS)
	{  
		printf("ERROR: The SQL statement did not execute successfully.\n");
		exit(1);
	}
	
	SSchar*     pFilterStmt =  SSText("INSERT INTO TEST_CLOB_COL VALUES (?)");
	//SSchar*     pFilterStmt =  SSText("INSERT INTO test(LOG_DATA_1) VALUES (?)");
	return_code = SQLPrepare(hstmt, (SSuchar*)(pFilterStmt), SQL_NTS);   
	SSchar clobcol[] = {0x3042,0x3044,0x3046,0x3048,0x304A, 0x0000};
   /* test 1:
	LONG        cbRowId = 10;
	//return_code= SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_VARCHAR, 4000, 0, clobcol, sizeof(clobcol), &cbRowId); //This is ok!
	return_code= SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_LONGVARCHAR, 40, 0, clobcol, 10, &cbRowId);
    
	return_code = SQLExecute((SQLHSTMT)hstmt); 
   */

   /*test 2*/
   //test a: LONG cbRowId = SQL_DATA_AT_EXEC;
   LONG cbRowId = SQL_LEN_DATA_AT_EXEC(11);
   return_code= SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_LONGVARCHAR, 40, 0, clobcol, 10, &cbRowId);
   return_code = SQLExecute((SQLHSTMT)hstmt);
   SSchar *buf = clobcol;
   return_code = SQLParamData(hstmt, (SQLPOINTER *)&buf);
   return_code = SQLPutData(hstmt, buf, 10);
   return_code = SQLParamData(hstmt, (SQLPOINTER *)&buf);

	if (return_code == SQL_SUCCESS)
	{
		printf("The SQL statement executed successfully.\n");
	}
	else{
		printf("The SQL statement did not execute successfully.\n");
		ODBC_error ((HENV)henv, (HDBC)hdbc, (HSTMT)hstmt) ;
	}

   SSchar*     pSelectStmt =  SSText("select userenv('language') from dual");
	//SSchar*     pFilterStmt =  SSText("INSERT INTO test(LOG_DATA_1) VALUES (?)");
	return_code = SQLPrepare(hstmt, (SSuchar*)(pSelectStmt), SQL_NTS);

	return_code = SQLExecute((SQLHSTMT)hstmt); 
   

	SQLDisconnect ((SQLHDBC)hdbc);
	SQLFreeConnect ((SQLHDBC)hdbc);
	SQLFreeEnv ((SQLHENV)henv);

	return(return_code);
}



