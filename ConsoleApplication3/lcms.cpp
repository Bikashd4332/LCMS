#include "lcms.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

SQLHENV henv_sql;
SQLHDBC hdbc_sql;
SQLHSTMT hstmt_sql;
SQLRETURN ret_sql;
SQLWCHAR* connectionString;




void clrscr() {
	system("cls");
}

//Init function -- This function is the function which is gonna be called to allocate the handles.
void INITHandles(SQLCHAR* ConnectionString) {
	
	gotoxy(5, 5);
	printf("\tAllocating environment handle:");
	ret_sql = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_sql);
	if (ret_sql == SQL_SUCCESS)
		printf("Success\n");
	else 
		printf("\tUnable to allocate handle!!");
		

	//Setting ODBC 3.0 functionality
	SQLSetEnvAttr(henv_sql, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	printf("\tAllocating connection handle: ");
	ret_sql = SQLAllocHandle(SQL_HANDLE_DBC, henv_sql, &hdbc_sql);
	if (ret_sql == SQL_SUCCESS)
		printf("Success\n");
	else
		show_error("sql handle connection", SQL_HANDLE_ENV, &henv_sql);
		

	//After allocation of the connection handle we try connect to the Data Source using the provided connection string.
	printf("\tConnecting to the Data Source: ");
	ret_sql = SQLDriverConnectA(hdbc_sql, NULL, ConnectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
	if (ret_sql == SQL_SUCCESS)
		printf("Success\n");
	else 
		show_error("try to connect to data source", SQL_HANDLE_DBC, hdbc_sql);
}

void alloc_stmt()
{
	//Then allocating sttement handle for future sql execution
	ret_sql = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_sql, &hstmt_sql);
	if (ret_sql != SQL_SUCCESS){
		show_error("try to connect to data source", SQL_HANDLE_DBC, hdbc_sql);
		exit(-2);
	}
}


//Function which is going to be used frequently
void gotoxy(short col, short row)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { col,row };
	SetConsoleCursorPosition(h, position);
}


//This functions is for extractig error string simply said "The reason of occuring this error"
void show_error(char* purp, SQLSMALLINT type, SQLHANDLE handle) {

	SQLSMALLINT i = 0;
	SQLINTEGER native;
	SQLWCHAR state[7];
	SQLWCHAR text[256];
	SQLSMALLINT size;

	fprintf(stderr, "\tError have been occured after this operation : %s", purp);

	while (SQLGetDiagRec(type, handle, ++i, state, &native, text, sizeof(text), &size) == SQL_SUCCESS) {
		fwprintf(stderr, L"%s:%ld:%ld:%s\n", state, i, native, text);
	}
	exit(-1);
}

char greeting(tm * localtime)
{
	if (((localtime->tm_hour) > 5) && ((localtime->tm_hour) <= 11))
		return 'm';
	else if (((localtime->tm_hour) >= 12) && (((localtime->tm_hour) <= 16)))
		return 'a';
	else
		return 'e';
}

//Student Interface for the project
void student_func(){

	char choice;
again:
	printf("\n\t1. Lodge a complaint about any computer that you experiencing trouble using it.");
	printf("\n\t2. Show the weekly status of every computer.");
	printf("\n\t3. Exit from the menu.\n");
	printf("\n\tEnter your choice:");
wrong:
	scanf_s(" %c", &choice);

	switch (choice)
	{
	case '1':
		lodge_complaints();
		goto again;
	case '2':
		weekly_status();
		goto again;
	case '3':
		printf("\n\tExiting...");
		Sleep(2000);
		exit(0);
	default:
		printf("\n\tnot a valid option!");
		printf("\n\tEnter choice again: ");
		goto wrong;
	}

}

void admin_func(){
	
	
	char choice;
again:
	printf("\n\t1. Add a trusted admin for maintenance");
	printf("\n\t2. Add a student who can lodge complaints");
	printf("\n\t3. Add a computer lab");
	printf("\n\t4. Delete an existing student");
	printf("\n\t5. Delete an existing lab");
	printf("\n\t6. Delete an existing admin");
	printf("\n\t7. Display the issues of all the computers");
	printf("\n\t8. resolved an issue");
	printf("\n\t9. Show the weekly status of every computer.");
	printf("\n\t0. Exit from main menu");
	printf("\n\n\t Enter you choice: ");
wrong:
	scanf_s(" %c", &choice);

	switch (choice)
	{
	case '1':
		add_admin();
		goto again;
	case '2':
		add_student();
		goto again;
	case '3':
		add_lab();
		goto again;
	case '4':
		del_student();
		goto again;
	case '5':
		del_lab();
		goto again;
	case '6':
		del_admin();
		goto again;
	case '7':
		display_complaints();
		goto again;
	case '8':
		resolved_issues();
		goto again;
	case '9':
		weekly_status();
		goto again;
	case '0':
		printf("\n\tExiting!...");
		Sleep(2000);
		exit(0);
	default:
		printf("\nnot a valid option!");
		printf("\nEnter choice again: ");
		goto again;
	}

}


//Single function for lodging complaints in the complaints
void lodge_complaints() {

	//required data for complaint table
	SQLCHAR* problemType = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 20);
	SQLCHAR* description = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 45);
	SQLINTEGER student_id, machine_id;
	SQLLEN problem_len = 20, description_len = 45;
	SQLCHAR* syntax = (SQLCHAR*)"INSERT INTO lodged_issue (stuId,machineId,problemType,description) VALUES (?,?,?,?) ";


//Getting all the inputs from the users
	printf("Enter Studnet ID: ");
	scanf_s("%d", &student_id);
	printf("Enter Machine ID: ");
	scanf_s("%d", &machine_id);
	printf("Enter ProblemType: ");
	scanf_s("%s", problemType,44);
	printf("Enter description: ");
	scanf_s(" %[^\n]", description,44);

	//preparing statement handle
	alloc_stmt();

	//Preparing sql syntax to execute
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	
	if (ret_sql != SQL_SUCCESS) {
		show_error("Preparing sql state", SQL_HANDLE_STMT, hstmt_sql);
		
		exit(-1);
	}

	//After preparing the statemant next work is to bind the param
		ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 3, 0, &student_id, 0, NULL);

		if (ret_sql != SQL_SUCCESS) 
			show_error("binding student_id", SQL_HANDLE_STMT, hstmt_sql);
			
		ret_sql = SQLBindParameter(hstmt_sql, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &machine_id, 0, NULL);
		
		if (ret_sql != SQL_SUCCESS) 
			show_error("binding machine_id", SQL_HANDLE_STMT, hstmt_sql);
		
		ret_sql = SQLBindParameter(hstmt_sql,3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0,problemType,problem_len,NULL);
		
		if (ret_sql != SQL_SUCCESS) 
			show_error("binding problemtype", SQL_HANDLE_STMT, hstmt_sql);
			
			ret_sql = SQLBindParameter(hstmt_sql, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 45, 0,description, description_len,NULL);

			if (ret_sql != SQL_SUCCESS) 
				show_error("binding description", SQL_HANDLE_STMT, hstmt_sql);

//Executing prepared sql statements


		ret_sql = SQLExecute(hstmt_sql);

		if (ret_sql != SQL_SUCCESS) 
			show_error("inserting data to complaint", SQL_HANDLE_STMT, hstmt_sql);
			

	printf("Successfully lodged.");
//freeing statement handle
	free_stmt();
}

//Fuction for gettind data from user about a student
void add_student() {

	//Rrequired data from user about student
	SQLCHAR* firstname = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 20);
	SQLCHAR* lastname = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 20);
	SQLCHAR* middlename = (SQLCHAR*)malloc(sizeof(SQLCHAR*) * 20);
	SQLCHAR* registrationNumber = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 20);
	SQLINTEGER semester;
	SQLCHAR syntax[] = "INSERT INTO STUDENT(firstName,lastName,middleName,registrationNumber,semester) VALUES (?,?,?,?,?)";


	//Getting data 
	printf("Enter student first name: ");
	scanf_s("%s",firstname,20);
	printf("\nEnter student middle name: ");
	scanf_s("%s",middlename,20);
	printf("\nEnter student last name: ");
	scanf_s("%s", lastname,20);
	printf("\nEnter student registration : ");
	scanf_s("%s", registrationNumber,20);
	printf("\nEnter student semester: ");
	scanf_s("%d", &semester);



	//preparing statement handle
	alloc_stmt();
	
	//preparing sql syntax
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS) 
		show_error("studnet sql syntax", SQL_HANDLE_STMT, hstmt_sql);
		

	//Setting input parameter binding
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 20, 0, firstname, 20, NULL);
	
	if (ret_sql != SQL_SUCCESS)
		show_error("binding fn paramter student", SQL_HANDLE_STMT, hstmt_sql);
		

		ret_sql = SQLBindParameter(hstmt_sql, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 20, 0, lastname, 20, NULL);

		if (ret_sql != SQL_SUCCESS)
			show_error("binding ln paramter student", SQL_HANDLE_STMT, hstmt_sql);
			

		ret_sql = SQLBindParameter(hstmt_sql, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 20, 0, middlename, 20, NULL);

		if (ret_sql != SQL_SUCCESS) 
			show_error("binding mn paramter student", SQL_HANDLE_STMT, hstmt_sql);
			

		ret_sql = SQLBindParameter(hstmt_sql, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 20, 0, registrationNumber, 20, NULL);

		if (ret_sql != SQL_SUCCESS)
			show_error("binding rn paramter student", SQL_HANDLE_STMT, hstmt_sql);
		

		
		ret_sql = SQLBindParameter(hstmt_sql, 5, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER,11, 0, &semester, 11, NULL);

		if (ret_sql != SQL_SUCCESS) 
			show_error("binding rn paramter student", SQL_HANDLE_STMT, hstmt_sql);
		

		
			ret_sql = SQLExecute(hstmt_sql);

			if (ret_sql != SQL_SUCCESS)
				show_error("inserting data to complaint", SQL_HANDLE_STMT, hstmt_sql);
				
			
		printf("Successfully inserted!");
		//freeing statement handle
		free_stmt();
	}

//This is for inserting admins
void add_admin() {

	//required data to insert
	SQLINTEGER  adminId;
	SQLCHAR* registrationNumber = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 20);
	SQLCHAR* name = (SQLCHAR*)malloc(sizeof(SQLCHAR*) * 40);
	SQLCHAR* syntax = (SQLCHAR*)"INSERT INTO admin values (?,?,?)";

	//Getting data
	printf("Enter admin ID: ");
	scanf_s("%d", &adminId);

	printf("Enter admin name:");
	scanf_s(" %[^\n]", name,40);

	printf("Enter registration number: ");
	scanf_s(" %s", registrationNumber, 20);

	//preparing statement handle
	alloc_stmt();

	//preparing sql statement
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);

	if (ret_sql != SQL_SUCCESS) 
		show_error("preparing admin sql syntax", SQL_HANDLE_STMT, hstmt_sql);
		

	//binding the parameter
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &adminId, 0, NULL);

	if (ret_sql != SQL_SUCCESS) 
		show_error("binding admin id", SQL_HANDLE_STMT, hstmt_sql);
		

	ret_sql = SQLBindParameter(hstmt_sql, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 40, 0, name,40 , NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding admin name", SQL_HANDLE_STMT, hstmt_sql);
		

	ret_sql = SQLBindParameter(hstmt_sql, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 20, 0, registrationNumber, 20, NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding admin registration", SQL_HANDLE_STMT, hstmt_sql);
		

	//Executing sql syntax
		ret_sql = SQLExecute(hstmt_sql);

		if (ret_sql != SQL_SUCCESS)
			show_error("Error in executing sql", SQL_HANDLE_STMT, hstmt_sql);
			

	printf("Successfully Executed!");
	//freeing statement handle
	free_stmt();
}

//This functions is for releasing all the allocated handles before terminating the program
void free_stmt()
{

	//freeing statement handle
	ret_sql = SQLFreeStmt(hstmt_sql, SQL_RESET_PARAMS);
	if (ret_sql != SQL_SUCCESS) 
		show_error("Reseting parameters of statement handle", SQL_HANDLE_DBC, hdbc_sql);
		

	ret_sql = SQLFreeStmt(hstmt_sql,SQL_UNBIND);

	if (ret_sql != SQL_SUCCESS)
		show_error("unbinding statement handle", SQL_HANDLE_DBC, hdbc_sql);
		
}

//this methdo required 
void add_lab()
{
	//Required data for lab
	SQLINTEGER labId;
	SQLCHAR* dept = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 10);
	SQLCHAR* syntax = (SQLCHAR*)"INSERT INTO lab values(?,?)";

	//getting data
	printf("Enter labId: ");
	scanf_s("%d", &labId);
	printf("Enter dept: ");
	scanf_s("%s", dept,10);

	//preparing statement handle
	alloc_stmt();

	//preparing sql syntax
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS)
		show_error("preparing sql querry lab", SQL_HANDLE_STMT, hstmt_sql);
		

	//binding paramters
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 10, 0, &labId,
		10, NULL);
	
	if (ret_sql != SQL_SUCCESS)
		show_error("binding labid", SQL_HANDLE_STMT, hstmt_sql);
		
	
	ret_sql = SQLBindParameter(hstmt_sql, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_CHAR, 10, 0, &dept,
		10, NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding dept", SQL_HANDLE_STMT, hstmt_sql);
		

	//Executing stament
	
		ret_sql = SQLExecute(hstmt_sql);
		if (ret_sql != SQL_SUCCESS)
			show_error("inserting lab data", SQL_HANDLE_STMT, hstmt_sql);
			

	printf("Successfully Executed!");
	//freeing statement handle
	free_stmt();
}


void add_machine() {

	//reqruired data
	SQLINTEGER labId, machineId;
	SQLCHAR* syntax = (SQLCHAR*) "INSERT INTO machine VALUES (?,?)";

	//getting data
	printf("Enter labId to which that computer belongs: ");
	scanf_s("%d", &labId);
	printf("Enter the computerId: ");
	scanf_s("%d", &machineId);

	//preparing statement handle
	alloc_stmt();
	
	//preparing sql syntax
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS) 
		show_error("preparing syntax for machine", SQL_HANDLE_STMT, hstmt_sql);
		


	// binding sql paramter
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &machineId,
		11, NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding machineId for machine", SQL_HANDLE_STMT, hstmt_sql);
		

	ret_sql = SQLBindParameter(hstmt_sql, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &labId,
		11, NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding labId for machine", SQL_HANDLE_STMT, hstmt_sql);
		


	//executing syntax
	ret_sql = SQLExecute(hstmt_sql);

	if (ret_sql != SQL_SUCCESS)
		show_error("Executing syntax for machine", SQL_HANDLE_STMT, hstmt_sql);
		

	printf("Successfully executed!");
	//freeing statement handle
	free_stmt();
}


//This function is for admins eg- when an admin finishes solving error then he usses this to track solved errors
void resolved_issues()
{
	//requierd data
	SQLINTEGER issueId , adminId;
	SQLCHAR* dateOfSolve = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 15);
	SQLCHAR* syntax = (SQLCHAR*)"INSERT INTO resolved_issue values (?,?,?)";


	//getting user data
	printf("Enter issueId: ");
	scanf_s("%d", &issueId);
	printf("Enter the adminId who solved the issue: ");
	scanf_s("%d", &adminId);
    printf("Enter the date in the following format (YY-MM-DD) : ");
	scanf_s("%s", dateOfSolve,12);

	//preparing statement handle
	alloc_stmt();

	//preparing sql statement
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);

	if (ret_sql != SQL_SUCCESS) 
		show_error("preparing synatx for resolved issue", SQL_HANDLE_STMT, hstmt_sql);
		

	//binding paramters
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &issueId, 11, NULL);
	
	if (ret_sql != SQL_SUCCESS) 
		show_error("Binding paramter issueId for resolved_issue", SQL_HANDLE_STMT, hstmt_sql);
		

	ret_sql = SQLBindParameter(hstmt_sql, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &adminId, 11, NULL);

	if (ret_sql != SQL_SUCCESS) 
		show_error("Binding paramter adminId for resolved_issue", SQL_HANDLE_STMT, hstmt_sql);
		
	
	ret_sql = SQLBindParameter(hstmt_sql, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DATE, SQL_DATE_LEN, 0, dateOfSolve, SQL_DATE_LEN, NULL);

	if (ret_sql != SQL_SUCCESS) 
		show_error("Binding paramter issueId for resolved_issue", SQL_HANDLE_STMT, hstmt_sql);
		


	//executing syntax
	ret_sql = SQLExecute(hstmt_sql);

	if (ret_sql != SQL_SUCCESS)
		show_error("Executing syntax for relosved_issue", SQL_HANDLE_STMT, hstmt_sql);
		

	printf("Successfully executed!");
	//freeing statement handle
	free_stmt();
}

void display_complaints()
{
	SQLINTEGER issueId, stuId, machineId;
	SQLCHAR dateOfIssue[SQL_TIMESTAMP_LEN];
	SQLCHAR* problemType = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 10), *description = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 45);
	SQLCHAR* syntax = (SQLCHAR*) "select * from lodged_issue where issueId not in (select issueId from resolved_issue);";
	int len;

	//preparing statement handle
	alloc_stmt();

	ret_sql= SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS) 
		show_error("Preparing sql syntax inside shoe lodged complaint", SQL_HANDLE_STMT, hstmt_sql);
		
	
	ret_sql = 	SQLExecute(hstmt_sql);
	if (ret_sql != SQL_SUCCESS) 
		show_error("Executing sql syntax inside shoe lodged complaint", SQL_HANDLE_STMT, hstmt_sql);
	
	SQLBindCol(hstmt_sql, 1,SQL_INTEGER,&issueId, 11, NULL);
	SQLBindCol(hstmt_sql, 2, SQL_INTEGER, &stuId, 11, NULL);
	SQLBindCol(hstmt_sql, 3, SQL_INTEGER, &machineId, 11, NULL);
	SQLBindCol(hstmt_sql, 4, SQL_C_CHAR, problemType, 25, NULL);
	SQLBindCol(hstmt_sql, 5, SQL_C_CHAR, description, 45, NULL);
	SQLBindCol(hstmt_sql, 6, SQL_C_CHAR, dateOfIssue,SQL_TIMESTAMP_LEN,NULL);


	printf("%s\t%s\t%s\t%s\t%s\t%s\n", "ISSUE ID", "STU ID", "MACHINE ID", "PROBLEM TYPE", "DESCRIPTION", "DATE OF ISSUE");
	do {
			ret_sql = SQLFetch(hstmt_sql);
		
			if (ret_sql != SQL_NO_DATA) {
					printf("%d\t\t%d\t\t%d\t\t%s\t\t%s\t\t%s", issueId, stuId, machineId, problemType, description, dateOfIssue);
				}

				printf("\n");
	
	} while (ret_sql != SQL_NO_DATA);

	//freeing statement handle
	free_stmt();
}

void weekly_status()
{
	SQLINTEGER issueId, stuId, adminId;
	SQLCHAR* description = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 20), *problemType = (SQLCHAR*)malloc(sizeof(SQLCHAR)*45);
	SQLCHAR dateOfIssue[20], dateOfSolve[20];
	SQLCHAR* syntax = (SQLCHAR*)"select * from weekly_status";

	//preparing statement handle
	alloc_stmt();

	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS) 
		show_error("preparing sql for weekly status", SQL_HANDLE_STMT, hstmt_sql);
	
	
	SQLBindCol(hstmt_sql,1,SQL_INTEGER,&issueId,11,NULL);
	SQLBindCol(hstmt_sql, 2, SQL_INTEGER, &stuId, 11, NULL);
	SQLBindCol(hstmt_sql, 3, SQL_C_CHAR, dateOfIssue,20, NULL);
	SQLBindCol(hstmt_sql, 4, SQL_C_CHAR, problemType, 20, NULL);
	SQLBindCol(hstmt_sql, 5, SQL_INTEGER, description, 45, NULL);
	SQLBindCol(hstmt_sql, 6, SQL_C_CHAR, dateOfSolve, 20, NULL);


	ret_sql = SQLExecute(hstmt_sql);
	if (ret_sql != SQL_SUCCESS) 
		show_error("executing sql for weekly status", SQL_HANDLE_STMT, hstmt_sql);
		
	
	printf("%s\t%s\t%s\t%s\t%s\t%s\n","ISSUE ID","STU ID","DATE OF ISSUE","PROBLEM TYPE","DESCRIPTION","DATE OF SOLVE");
	while (SQLFetch(hstmt_sql) == SQL_SUCCESS) {
		printf("%d\t\t%d\t\t%s\t\t%s\t%s\t%s",issueId,stuId,dateOfIssue,problemType,description,dateOfSolve);
		printf("\n");
	}
	//freeing statement handle
	free_stmt();
}




void del_lodge_complaints()
{


	//required data for complaint table
	SQLINTEGER issueId;
	SQLCHAR* syntax = (SQLCHAR*)"DELETE FROM lodged_issue WHERE issueId = ?";


	//Getting all the inputs from the users
	printf("Enter Issue ID: ");
	scanf_s("%d", &issueId);

	//preparing statement handle
	alloc_stmt();

	//Preparing sql syntax to execute
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);

	if (ret_sql != SQL_SUCCESS) 
		show_error("Preparing sql statement", SQL_HANDLE_STMT, hstmt_sql);

	//After preparing the statemant next work is to bind the param
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &issueId, 11, NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding student_id", SQL_HANDLE_STMT, hstmt_sql);
	
	//Executing prepared sql statements


	ret_sql = SQLExecute(hstmt_sql);

	if (ret_sql != SQL_SUCCESS)
		show_error("deleting data from complaint", SQL_HANDLE_STMT, hstmt_sql);
		

	printf("Successfully deleted!");
	//freeing statement handle
	free_stmt();
}

void del_student()
{
	SQLINTEGER stuId;
	SQLCHAR syntax[] = "DELETE FROM student WHERE stuId = ?)";


	//Getting data 
	printf("Enter the student id :");
	scanf_s("%d", &stuId);

	//preparing statement handle
	alloc_stmt();

	//preparing sql syntax
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS)
		show_error("studnet sql syntax", SQL_HANDLE_STMT, hstmt_sql);
		

	ret_sql = SQLBindParameter(hstmt_sql, 5, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &stuId, 11, NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding rn paramter student", SQL_HANDLE_STMT, hstmt_sql);
		


	ret_sql = SQLExecute(hstmt_sql);

	if (ret_sql != SQL_SUCCESS) 
		show_error("deleting data from complaint", SQL_HANDLE_STMT, hstmt_sql);
		

	printf("Successfully deleted!");

	//freeing statement handle
	free_stmt();
}


void del_admin()
{

	//required data to insert
	SQLINTEGER  adminId;
	SQLCHAR* syntax = (SQLCHAR*)"DELETE FROM admin WHERE adminId = ?";

	//Getting data
	printf("Enter admin ID: ");
	scanf_s("%d", &adminId);

	//preparing statement handle
	alloc_stmt();

	//preparing sql statement
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);

	if (ret_sql != SQL_SUCCESS) 
		show_error("preparing admin sql syntax", SQL_HANDLE_STMT, hstmt_sql);
		

	//binding the parameter
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &adminId, 11, NULL);

	if (ret_sql != SQL_SUCCESS) 
		show_error("binding admin id", SQL_HANDLE_STMT, hstmt_sql);
	

	//Executing sql syntax
	ret_sql = SQLExecute(hstmt_sql);

	if (ret_sql != SQL_SUCCESS) 
		show_error("deleting data from student", SQL_HANDLE_STMT, hstmt_sql);
	
	printf("Successfully deleted!");
	//freeing statement handle
	free_stmt();
}


void del_lab()
{

	//Required data for lab
	SQLINTEGER labId;
	SQLCHAR* syntax = (SQLCHAR*)"DELETE FROM lab WHERE labId = ?";

	//getting data
	printf("Enter labId: ");
	scanf_s("%d", &labId);

	//preparing statement handle
	alloc_stmt();

	//preparing sql syntax
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS) 
		show_error("preparing sql querry lab", SQL_HANDLE_STMT, hstmt_sql);
	
	//binding paramters
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 10, 0, &labId,
		10, NULL);

	if (ret_sql != SQL_SUCCESS)
		show_error("binding labid", SQL_HANDLE_STMT, hstmt_sql);
	

	//Executing stament

	ret_sql = SQLExecute(hstmt_sql);
	if (ret_sql != SQL_SUCCESS)
		show_error("deleting datat from lab", SQL_HANDLE_STMT, hstmt_sql);
		

	printf("Successfully deleted!");
	//freeing statement handle
	free_stmt();
}

void del_machine()
{

	SQLINTEGER  machineId;
	SQLCHAR* syntax = (SQLCHAR*) "DELETE FROM  machine WHERE machineId = ?";

	//getting data
	printf("Enter the computerId: ");
	scanf_s("%d", &machineId);

	//preparing statement handle
	alloc_stmt();

	//preparing sql syntax
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);
	if (ret_sql != SQL_SUCCESS) 
		show_error("preparing syntax for machine", SQL_HANDLE_STMT, hstmt_sql);
	


	// binding sql paramter
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &machineId,
		11, NULL);

	if (ret_sql != SQL_SUCCESS) 
		show_error("binding machineId for machine", SQL_HANDLE_STMT, hstmt_sql);
	

	//executing syntax
	ret_sql = SQLExecute(hstmt_sql);

	if (ret_sql != SQL_SUCCESS) 
		show_error("deleting data from machine", SQL_HANDLE_STMT, hstmt_sql);

	printf("Successfully deleted!");

	//freeing statement handle
	free_stmt();
}

void del_resolved_issues()
{

	//requierd data
	SQLINTEGER issueId;
	SQLCHAR* syntax = (SQLCHAR*)"DELETE FROM resolved_issue WHERE issueId = ?";


	//getting user data
	printf("Enter issueId: ");
	scanf_s("%d", &issueId);

	//preparing statement handle
	alloc_stmt();

	//preparing sql statement
	ret_sql = SQLPrepareA(hstmt_sql, syntax, SQL_NTS);

	if (ret_sql != SQL_SUCCESS)
		show_error("preparing synatx for resolved issue", SQL_HANDLE_STMT, hstmt_sql);


	//binding paramters
	ret_sql = SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, 11, 0, &issueId, 11, NULL);

	if (ret_sql != SQL_SUCCESS) 
		show_error("Binding paramter issueId for resolved_issue", SQL_HANDLE_STMT, hstmt_sql);
		

	//executing syntax
	ret_sql = SQLExecute(hstmt_sql);

	if (ret_sql != SQL_SUCCESS) 
		show_error("deleting record from resolved_issue", SQL_HANDLE_STMT, hstmt_sql);


	printf("Successfully deleted!");
	//freeing statement handle
	free_stmt();
}


void authentiation()
{

	SQLCHAR* registrationNumber = (SQLCHAR*)malloc(sizeof(SQLCHAR) * 20);
	time_t t;
	char flag;
	SQLCHAR* name = (SQLCHAR*)calloc(40,sizeof(SQLCHAR));
	struct tm tm_time;
	SQLCHAR* syntax_stu = (SQLCHAR*)"SELECT CONCAT(firstName,\' \',lastName) from student WHERE  registrationNumber = ?";
	SQLCHAR* syntax_admin = (SQLCHAR*)"SELECT adminName FROM admin WHERE registrationNumber = ?";

	time(&t);
	localtime_s(&tm_time,&t);
	flag = greeting(&tm_time);

	printf("\nWould you please enter you Registration ID for logging in\n");
	printf("REGISTRATION ID: ");
	scanf_s("%s", registrationNumber,20);
	printf("\n");
	
	alloc_stmt();
	//binding the paramter the first syntaxs
	SQLPrepareA(hstmt_sql, syntax_stu, SQL_NTS);

	SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 20, 0, registrationNumber, 20, NULL);
	SQLBindCol(hstmt_sql, 1, SQL_C_CHAR, name, 40, NULL);
	ret_sql = SQLExecute(hstmt_sql);
	SQLFetch(hstmt_sql);

	//freeing statement handle
	free_stmt();

if (ret_sql == SQL_SUCCESS && _strcmpi((char*)name,"\0")) {
		switch (flag)
		{
		case 'm':
			printf("Good morning %s\n", name);
			break;
		case 'a':
			printf("Good afternoon %s\n", name);
			break;
		case 'e':
			printf("Good evening %s\n", name);
			break;
		}
		printf("Here are the choice that you would like to do: ");
		student_func();
		exit(0);
	}

	SQLPrepareA(hstmt_sql, syntax_admin, SQL_NTS);
	SQLBindParameter(hstmt_sql, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 20, 0, registrationNumber, 20, NULL);
	SQLBindCol(hstmt_sql, 1, SQL_C_CHAR, name, 40, NULL);
	ret_sql = SQLExecute(hstmt_sql);
	SQLFetch(hstmt_sql);
	
	//freeing statement handle
	free_stmt();
	
	if (ret_sql == SQL_SUCCESS && _strcmpi((char*)name, "\0")) {
	
		switch (flag)
		{
		case 'm':
			printf("Good morning %s\n", name);
			break;
		case 'a':
			printf("Good afternoon %s\n", name);
			break;
		case 'e':
			printf("Good evening %s\n", name);
			break;
		}
		
		printf("Here are the choice that you would like to do: \n");
		admin_func();
		exit(0);
	}		

	printf("\nSorry sir you are not listed as a autorized person to access the data.");
	printf("\n Reffer Mr. Bikash Das to get access.");
	Sleep(2000);
}
