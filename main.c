#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 1024
#define user_size 32
#define user_mail 255

#define MAX_ROWS 100
#define ID_SIZE sizeof(uint32_t)

#define USERNAME_SIZE user_size
#define EMAIL_SIZE user_mail
#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)

#define ID_OFFSET 0
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)

typedef struct {
  uint32_t id;
  char username[user_size];
  char email[user_mail];
} Row;

enum StatementType { INSERT, SELECT, DELETE };

typedef enum {
  EXECUTE_SUCCESS,
  EXECUTE_TABLE_FULL,
  EXECUTE_NOT_FOUND,
  EXECUTE_SYNTAX_ERROR
} ExecuteResult;

typedef struct {
  enum StatementType type;
  Row row_to_insert;
  uint32_t id_to_delete;
} Statement;

typedef struct {

  Row rows[MAX_ROWS];

  uint32_t num_rows;
} Table;



int meta_commands(char *input) {
  if (strcmp(input, ".exit") == 0) {
    exit(0);
    return 1;
  }

  if (strcmp(input, ".help") == 0) {
    printf("Available commands : \n .exit \n .help \n insert \n select \n delete\n");
    return 1;
  } else {
    printf("Unknown command : %s\n", input);
    return 0;
  }

  return 0;
}


int prepare_statement(char *input, Statement *statement) {

  if (strncmp(input, "insert", 6) == 0) {
    statement->type = INSERT;
    int args = sscanf(input, "insert %u %31s %254s", &(statement->row_to_insert.id),
                      statement->row_to_insert.username,
                      statement->row_to_insert.email);
    if (args < 3) {
      printf("Invalid or missing arguments \n");
      return 0;
    }

    return 1;

  } else if (strcmp(input, "select") == 0) {
    statement->type = SELECT;
    return 1;
  }

  else if(strncmp(input, "delete", 6) ==0) {
    statement -> type = DELETE;
    int args = sscanf(input, "delete %u", &(statement -> id_to_delete));

    if(args < 1){
      printf("Error: Please provide an ID to delete. \n");
      return 0;
    }

    return 1;
  }

  else{
    printf("Unrecognized command '%s' \n", input);
    return 0;
  }
  return 0;
}

ExecuteResult execute_insert(Statement *statement, Table *table) {

  if (table->num_rows >= MAX_ROWS) {
    // printf("WARNING !!! Table is full\n");
    return EXECUTE_TABLE_FULL;
  }

  table -> rows[table -> num_rows++] = statement -> row_to_insert;

  // printf("Executed successfully !! \n (%u) rows affected.\n ", table->num_rows);

  return EXECUTE_SUCCESS;

}

ExecuteResult execute_select(Table *table) {
  
  for (int i = 0; i < table->num_rows; i++) {

    Row *row = &(table->rows[i]);
    printf("(%u, %s, %s)\n", row->id, row->username, row->email);
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_delete(Statement *statement, Table *table){
  int count = 0;
  for(uint32_t i = 0; i < table -> num_rows; i++){

    if(table -> rows[i].id == statement -> id_to_delete){
      table -> rows[i] = table->rows[table-> num_rows - 1];
      table -> num_rows--;
      count++;
      // printf("Deleted successfully !! \n (%u) rows affected.\n ", count);
      
      return EXECUTE_SUCCESS;
    }
  }
  
  return EXECUTE_NOT_FOUND;
}


ExecuteResult execute_statement(Statement *statement, Table *table) {

  ExecuteResult result ;
  switch (statement->type) {
    case INSERT:
      return execute_insert(statement, table);

    case SELECT:
      return execute_select(table);

    case DELETE:
      return execute_delete(statement, table);
      
  }

  return EXECUTE_SYNTAX_ERROR;
}


int main() {
  char input[MAX_SIZE];
  Statement statement;

  Table table;
  table.num_rows = 0;



  while (1) {
    printf(">>> ");

    fgets(input, MAX_SIZE, stdin);
    input[strcspn(input, "\r\n")] = '\0';

    if (input[0] == '.') {
      meta_commands(input);
      continue;
    } else {
      int check = prepare_statement(input, &statement);

      if (check == 0) {
        continue;
      } else {
        
          ExecuteResult result = execute_statement(&statement, &table);

          switch(result) {
            case EXECUTE_SUCCESS:
              break;
            case EXECUTE_TABLE_FULL:
              printf("Error: Table is full\n");
              break;
            case EXECUTE_NOT_FOUND:
              printf("Error: Not found\n");
              break;
            case EXECUTE_SYNTAX_ERROR:
              printf("Error: Syntax error\n");
              break;
          }

      }
    }
  }

  return 0;
}