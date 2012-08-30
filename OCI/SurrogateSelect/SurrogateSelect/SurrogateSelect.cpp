/*
** File:	SurrogateSelect.cpp
**
** This application will extract the hexdecimal of the surrogate character stored in the long column.
** 
** Before executing this program, please do following steps:
**  1. Create a table with a long column using SQL 'create table claire_test_long(long_col long);'
    2. Insert the string from surrogate_select_error.txt which contains surrogate characters.
    3. Get the rowid of the inserted row.
    4. Please make sure NLS_LANG = AMERICAN_AMERICA.UTF8 for oracle home register key under 'My Computer'\HKEY_LOCAL_MACHINE\SOFTWARE\ORACLE\<ORACLE_HOME>
** Command line: <dsn> <uid> <password>
** The expected extracted hexdecimal should be '0xdbc0 0xdc83', but the actual result using DD Driver is '0xfffd 0xfffd'.
**
**/

#include "stdafx.h"
#define UNICODE
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlext.h>
#include <wchar.h>
#include <tchar.h>
#include <locale.h>
/*
** Define Some useful defines
*/
#if !defined (NULL)
#define	NULL	0
#endif


/*
** function: ODBC_error
**
** Purpose:	Display to stdout current ODBC Errors
**
** Arguments:	henv    _ ODBC Environment handle.
**		hdbc    - ODBC Connection Handle error generated on.
**		hstmt	- ODBC SQL Handle error generated on.
**
** Returns:	void
**
*/

void ODBC_error (		/* Get and print ODBC error messages	*/
	HENV henv,			/* ODBC Environment						*/
	HDBC hdbc,			/* ODBC Connection Handle				*/
	HSTMT hstmt)		/* ODBC SQL Handle						*/
{
	wchar_t	sqlstate[10];
	wchar_t	errmsg[SQL_MAX_MESSAGE_LENGTH];
	SDWORD	nativeerr;
	SWORD	actualmsglen;
	RETCODE	rc;	

loop:  	rc = SQLErrorW(henv, hdbc, hstmt,
		sqlstate, &nativeerr, errmsg,
		SQL_MAX_MESSAGE_LENGTH - 1, &actualmsglen);

	if (rc == SQL_ERROR) {
		printf ("SQLError failed!\n");
		return;
	}

	if (rc != SQL_NO_DATA_FOUND) {
		printf ("SQLSTATE = %S\n",sqlstate);
		printf ("NATIVE ERROR = %d\n",nativeerr);
		errmsg[actualmsglen] = '\0';
		printf ("MSG = %S\n\n",errmsg); 
		goto loop;
	}
}

/*
** function: ODBC_Connect
**
** Purpose:	Allocates ODBC HENV and HDBC.
**
** Arguments:	henv    _ Pointer to environment handle
**		hdbc    - Pointer to connection handle
**
** Returns:	RETCODE - Return status from last ODBC Function.
**
*/

RETCODE ODBC_Connect(			/* Perform Driver Connection	*/
	HENV henv,					/* ODBC Environment Handle		*/
	HDBC hdbc,					/* ODBC Connection Handle		*/
	wchar_t *driver,			/* Data Source Name				*/
	wchar_t *uid,				/* User ID						*/
	wchar_t *pwd)				/* User Password				*/
{
	RETCODE	rc;
	int	retries;

#if defined(TRACING)
	rc = SQLSetConnectOption(hdbc, SQL_OPT_TRACE, 1);
	if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO))
		return (rc);
#endif
/*
** try connecting up to 3 times
*/
	for (retries = 1; retries <= 1; retries++) {
		rc = SQLConnectW (hdbc, driver, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS); 
		if ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO))
    	return (rc);
    else {
			wprintf(L"SQLConnect: Failed...\n");
			ODBC_error (henv, hdbc, SQL_NULL_HSTMT);
			wprintf(L"SQLConnect: Retrying Connect.\n");
		}
	}
/*
** Attempt to obtain a meaningful error as to why connect failed.
*/
  wprintf(L"No connection could be established.\n");
	ODBC_error (henv, hdbc, SQL_NULL_HSTMT);
	return (SQL_INVALID_HANDLE);
}

/*
** function:	EnvInit
**
** Purpose:	Allocates ODBC HENV and HDBC.
**
** Arguments:	henv    _ Pointer to environment handle
**		hdbc    - Pointer to connection handle
** Returns:	RETCODE status from ODBC Functions.
*/
RETCODE EnvInit(HENV *henv, HDBC *hdbc)
{
	RETCODE rc;

	rc = SQLAllocEnv (henv);
	if ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO))
		rc = SQLAllocConnect (*henv, hdbc);
	return (rc);
}

/*
** function:	EnvClose
**
** Purpose:	Frees environment and connection handles.
**
** Arguments:	henv    _ environment handle
**		hdbc    - connection to handle
*/
void EnvClose(HENV henv, HDBC hdbc)
{
	SQLDisconnect (hdbc);
	SQLFreeConnect (hdbc);
	SQLFreeEnv (henv);

}
/*
** Defines Used by ArgParse and Parent.
*/
#define	PWD_ARG		1
#define	UID_ARG		2
#define	DSN_ARG		3
#define	HELP_ARG	4

/*
** function:	ArgParse
**
** Purpose:	To parse runtime arguments.
**
** Arguments:	argv which is a character string to be parsed.
**
*/
int ArgParse(char *argv)
{
	if (!strcmp(argv, "-help"))
		return (HELP_ARG);
	if (!strcmp(argv, "-pwd"))
		return (PWD_ARG);
	if (!strcmp(argv, "-uid"))
		return (UID_ARG);
	else
		return (DSN_ARG);
}

/*
** Defines used by main program.
*/
#define	PWD_LEN		32
#define	UID_LEN		32
#define	OPT1_LEN	255
#define	OPT2_LEN	255
#define	DSN_LEN		34
#define	PWD_MSG1 L"Requested password exceeds compiled limit of %d.\n"
#define	PWD_ERR1 L"Password not found after keyword -pwd on command line.\n"
#define	UID_MSG1 L"Requested username exceeds compiled limit of %d.\n"
#define	UID_ERR1 L"Username not found after keyword -uid on command line.\n"
#define	USAGE_MSG1 L"Usage: %s [-uid username] [-pwd password] [-xml] DSN.\n"
#define	BANNER L"%s DataDirect Technologies, Inc. unicode_app Application.\n"

/*
** Program:	demo
*/
int main(int argc, char * argv[])
{
	HDBC	hdbc;
	HENV	henv;
	HSTMT	hstmt;
	RETCODE	rc;
	wchar_t	uid[UID_LEN] = {L'\0'};
	wchar_t	pwd[PWD_LEN] = {L'\0'};
	wchar_t	driver[DSN_LEN] = {L'\0'};
	wchar_t	sql[255];
		
	int count=0;
	SQLINTEGER ind_ptr1=-3;
	SQLINTEGER ind_ptr2=0;

/*
** Define Table and Driver
*/	
	
	if( argc < 4 ){
		printf("USAGE: <dsn> <uid> <password>\n");
		exit(0);
	}  //end if

	mbstowcs(driver, (char*)argv[1], strlen(argv[1]));
	mbstowcs(uid, (char*)argv[2], strlen(argv[2]));
	mbstowcs(pwd, (char*)argv[3], strlen(argv[3]));	

	printf ("Connecting to data source '%S' as user '%S/%S'.\n\n", driver, uid, pwd);

#if !defined (__cplusplus) && defined (hppa)
/*
** C programs must call the HP C++ Object initializer function.
*/
	_main ();
#endif
	EnvInit (&henv, &hdbc);	


	rc = ODBC_Connect (henv, hdbc, driver, uid, pwd);
	if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO))
		exit(255);	/* Exit with failure */

        if (hdbc != NULL)
	{
		if (SQL_SUCCESS != (rc = SQLSetConnectOption(hdbc, 1041, (UDWORD) "IVSE.LIC")))
		{
			printf("Problem in Setting the IVSE.LIC");
		}
		if (SQL_SUCCESS != (rc = SQLSetConnectOption(hdbc, 1042, (UDWORD) "SVc5NzA3a01nSnNSdEJ3VHNK" )))
		{
			printf("Problem in Setting the LICENCE key");
		}

	}

/*
** Allocate a HSTMT to communicate with ODBC DB Driver.
*/
	rc = SQLAllocStmt (hdbc, &hstmt);
	if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO)) {
		wprintf (L"Unable to Allocate a HSTMT:\n");
		ODBC_error (henv, hdbc, hstmt);
		EnvClose (henv, hdbc);
		//exit (255);
	}

   printf("Please input the rowid for the record in claire_test_long: ");
   char rowid[100];
   gets(rowid);
/*
** Build the SQL statement
*/

   wcscpy(sql, L"select long_col from claire_test_long where rowid = ?");

   wprintf(L"Binding parameter...\n");

   LONG cbRowId = SQL_NTS;
   rc= SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, rowid, 100, 
      &cbRowId);
   
	if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO)) {
		wprintf (L"SQLBindParameter Failed:\n");
		ODBC_error (henv, hdbc, hstmt);
		EnvClose (henv, hdbc);
		exit (255);
	}

	wprintf(L"Prepairing the select statement...\n");
   rc = SQLPrepareW(hstmt, sql, SQL_NTS);

	wprintf(L"Executing parameterized select statement...\n");
   rc = SQLExecute((SQLHSTMT)hstmt);

   if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO)) {
		wprintf (L"SQLExecute failed:\n");
		ODBC_error (henv, hdbc, hstmt);
		EnvClose (henv, hdbc);
		exit (255);
	}

   wchar_t long_col[2000];
   SQLINTEGER rlength = 0;

   while ((rc = SQLFetch(hstmt)) == SQL_SUCCESS)
   {
      rc = SQLGetData(hstmt, 1, SQL_C_WCHAR, &long_col, sizeof(long_col), &rlength);
      if (rc != SQL_SUCCESS ) {
		   wprintf (L"SQLGetData failed:\n");
		   ODBC_error (henv, hdbc, hstmt);
		   EnvClose (henv, hdbc);
		   exit (255);
	   }

      printf("The hexdecimal of the surrogate character is 0x%x 0x%x\n", long_col[133], long_col[134]);
   }

   printf("Please input enter key to exit");
   char  temp[10];
   gets(temp);
      
   if (rc != SQL_NO_DATA_FOUND ){
		wprintf (L"SQLFetch failed:\n");
		ODBC_error (henv, hdbc, hstmt);
		EnvClose (henv, hdbc);
		exit (255);
	}
   if(rlength == 0)
      printf("The input rowid is not a correct rowid\n");
   

/*
** Free Bind Buffers
*/
	wprintf(L"Freeing handles...\n");
	rc = SQLFreeStmt (hstmt, SQL_UNBIND);
	EnvClose(henv, hdbc);
	return 0;
}