/*
** File:	unicode_app.cpp
**
** This application will insert a JPN string into table test_clob_col(clobcol clob)
** NLS_LANG = AMERICAN_AMERICA.WE8ISO8859P1 AMERICAN_AMERICA.UTF8
**
** DISCLAIMER: The sample code is not supported under any DataDirect Technologies support program or service.
** The sample code is provided on an "AS IS" basis. DataDirect Technologies makes no warranties, express or implied,
** and disclaims all implied warranties including, without limitation, the implied warranties of merchantability or
** of fitness for a particular purpose. The entire risk arising out of the use or performance of the sample code is
** borne by the user. In no event shall DataDirect Technologies, its employees, or anyone else involved in the creation,
** production, or delivery of the code be liable for any damages whatsoever (including, without limitation, damages for
** loss of business profits, business interruption, loss of business information, or other pecuniary loss) arising out 
** of the use of or inability to use the sample code, even if DataDirect Technologies has been advised of the possibility
** of such damages.
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

	wchar_t hexstring[]= {0X3042,0x3044,0x3046,0x3048,0x304A};    // UTF-16 Encoding 0x00
   wchar_t a[] = L" ";
/*
** Define Table and Driver
*/	
	
	if( argc < 4 ){
		printf("USAGE:repro <dsn> <uid> <password>\n");
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
/*
** Build the SQL statement
*/
   /**Test case 1 clobcol jpn truncation**/
   /*wcscpy(sql, L"insert into TEST_CLOB_COL (CLOBCOL ) values (?)");

   wprintf(L"Binding parameter...\n");
   LONG cbRowId = 10;
   rc= SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_LONGVARCHAR, sizeof(hexstring), 0, hexstring, sizeof(hexstring), 
      &cbRowId);
   */

   /**Test case 2 surrogate character**/
   wcscpy(sql, L"insert into claire_dock_txn_log values(?)");

   wprintf(L"Binding parameter...\n");
   WCHAR log_data_wide[511];
  //the arrow replacing '?' of 'Probelauf der Anlage ?? o.k.'
  wchar_t arrow[] = {0XDBC0, 0XDCC6,0x00};
  //the surrogate charactor after 'Wingding:'
  wchar_t winding1[] ={0xDBC0, 0XDC7A, 0X0020, 0XDBC0, 0XDC81, 0X0020, 0XDBC0, 0XDC84, 0X0020, 0XDBC0, 0XDC86, 0x00};
  //the surrogate charactor of last line
  wchar_t winding2[] = {0XDBC0, 0XDC87, 0X0020, 0XDBC0, 0XDC89, 0X0020, 0XDBC0, 0XDC88, 0X0020, 0XDBC0, 0XDC8C, 0X0020, 0XDBC0, 0XDC8D, 0X0020, 0XDBC0, 0XDC8B, 0X0020, 0XDBC0, 0XDC99, 0X0020, 0XDBC0, 0XDC8A, 0X00};
  //the arrow before 'Airal: a b...'
  wchar_t doublearrow[] = {0X21D2,0x00};

  wsprintfW(log_data_wide, L"2*C110*CDESC_TEXT2*N1171*Probelauf der Anlage %s o.k.\r\nText added in Comment field:\r\n%s Arial: a b c d e f g h I j k l m n o p q r s t u v w x y z\r\n%s Wingding: %s\r\n%s%s2*O128*Probelauf der Anlage %s o.k.x", arrow, doublearrow, doublearrow, winding1, doublearrow, winding2, arrow);
  int len = (wcslen(log_data_wide) +1 );
  
   LONG cbRowId = SQL_NTS;
   rc= SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 511, 0, log_data_wide, 511, 
      &cbRowId);
   
	if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO)) {
		wprintf (L"SQLBindParameter Failed:\n");
		ODBC_error (henv, hdbc, hstmt);
		EnvClose (henv, hdbc);
		exit (255);
	}

	wprintf(L"Prepairing the insert statement...\n");
   rc = SQLPrepareW(hstmt, sql, SQL_NTS);

	wprintf(L"Executing parameterized insert statement...\n");
   rc = SQLExecute((SQLHSTMT)hstmt);

   if(rc == SQL_NEED_DATA){
      wchar_t *buf = hexstring;
      rc = SQLParamData(hstmt, (SQLPOINTER *)&buf);
      rc = SQLPutData(hstmt, buf, sizeof(hexstring));
      if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO)) {
		   wprintf (L"SQLPutData failed:\n");
		   ODBC_error (henv, hdbc, hstmt);
		   EnvClose (henv, hdbc);
		   exit (255);
	   }

      rc = SQLParamData(hstmt, (SQLPOINTER *)&buf);
   }else if ((rc != SQL_SUCCESS) && (rc != SQL_SUCCESS_WITH_INFO)) {
		wprintf (L"SQLExecute failed:\n");
		ODBC_error (henv, hdbc, hstmt);
		EnvClose (henv, hdbc);
		exit (255);
	}


/*
** Free Bind Buffers
*/
	wprintf(L"Freeing handles...\n");
	rc = SQLFreeStmt (hstmt, SQL_UNBIND);
	EnvClose(henv, hdbc);
	return 0;
}