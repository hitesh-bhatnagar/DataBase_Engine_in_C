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

enum StatementType { INSERT, SELECT, DELETE , UPDATE};

typedef enum {
  EXECUTE_SUCCESS,
  EXECUTE_TABLE_FULL,
  EXECUTE_NOT_FOUND,
  EXECUTE_SUCCESS_UPDATE,
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
    
    statement-> type = SELECT;
    
    int args = sscanf(input, "select %u", &(statement-> id_to_delete));

    if(args < 1){
      statement ->id_to_delete = 0;
    }
    
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

  else if(strncmp(input, "update", 6) ==0){
    statement -> type = UPDATE;

    int args = sscanf(input, "update %u set %s %s", 
                  &(statement->row_to_insert.id), 
                  statement->row_to_insert.username, 
                  statement->row_to_insert.email);

    if(args != 3){
      printf("Error or Invalid arguments\n");
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

  uint32_t i = table-> num_rows;

  while(i > 0 && table-> rows[i-1].id > statement -> row_to_insert.id){
    table -> rows[i] = table-> rows[i-1];
    i--;
  }

  table -> rows[i] = statement -> row_to_insert;
  table -> num_rows++;

  // printf("Executed successfully !! \n (%u) rows affected.\n ", table->num_rows);

  return EXECUTE_SUCCESS;

}

int binary_search(Table *table, uint32_t id){
  int bottom= 0, top= table -> num_rows - 1;  
  while(bottom <= top){

    int mid = bottom + (top - bottom)/2;

    if(table->rows[mid].id == id){
      return mid;
    }
    if(table->rows[mid].id <id) bottom = mid + 1;
    else top = mid -1;
  }
  
  return -1;
}

ExecuteResult execute_select(Statement *statement, Table *table) {
  
  if(statement->id_to_delete != 0){
    int idx = binary_search(table, statement -> id_to_delete);
    if(idx == -1) return EXECUTE_NOT_FOUND;

    Row *row = &(table->rows[idx]);

    printf("(%u, %s, %s)\n", row-> id, row ->username, row -> email);
  }

  else{
    for(int i = 0; i < table-> num_rows; i++){

      Row *row = &(table-> rows[i]);
      printf("(%u, %s, %s)\n", row -> id, row ->username, row -> email);
    }
  }

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_delete(Statement *statement, Table *table){
  int idx = binary_search(table, statement -> id_to_delete);

  if(idx == -1) return EXECUTE_NOT_FOUND;

  for(int i = idx; i < table ->num_rows - 1; i++) table-> rows[i] = table-> rows[i+1];

  table -> num_rows--;
  
  return EXECUTE_SUCCESS;
}

ExecuteResult execute_update(Statement *statement, Table *table){

  int idx = binary_search(table, statement-> row_to_insert.id);

  if(idx == -1) return EXECUTE_NOT_FOUND;

  strcpy(table-> rows[idx].username, statement-> row_to_insert.username);
  strcpy(table -> rows[idx].email, statement->row_to_insert.email);

  return EXECUTE_SUCCESS_UPDATE;
}


ExecuteResult execute_statement(Statement *statement, Table *table) {

  ExecuteResult result ;
  switch (statement->type) {
    case INSERT:
      return execute_insert(statement, table);

    case SELECT:
      return execute_select(statement, table);

    case DELETE:
      return execute_delete(statement, table);

    case UPDATE:
      return execute_update(statement, table);
      
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
            
            case EXECUTE_SUCCESS_UPDATE:
              printf("Updated successfully !! \n");
              break;
          }

      }
    }
  }

  return 0;
}