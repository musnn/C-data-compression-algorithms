
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{ // Define a structure for a dictionary entry
  char *key;
  int value;
}Dictionary;

int binary_to_int(const char *binary_str){ // Function to convert a binary string to an integer
  int result=0;
  for(int i=0;binary_str[i]!='\0';++i) result=(result<<1)+(binary_str[i]-'0');
  return result;
}

// Function to find a key in the dictionary given its value
char *find_key_in_dictionary(Dictionary *dict,int size,int value){
  for(int i=0;i<size;++i) if(dict[i].value==value) return dict[i].key;
  return NULL;
}

// Function to find a value in the dictionary given its key
int find_in_dictionary(Dictionary *dict,int size,char *key){
  for (int i=0;i<size;++i) if(strcmp(dict[i].key,key)==0) return dict[i].value;
  return -1;
}

// Function to convert an integer to a binary string with a specified number of bits
void int_to_binary(int num,int bits,char *binary_str){
  for(int i=bits-1;i>=0;--i) {
    binary_str[i]=(num&1)?'1':'0';
    num>>=1;
  }
  binary_str[bits] = '\0';
}

int calculate_bits(int value){ // Function to calculate the number of bits required to represent a value
  int bits = 0;
  while(value>0) {
    bits++;
    value>>=1;
  }
  return bits;
}

int main() {
  int main_dict;
  const char *input="si ton tonton tond mon tonton";
  Dictionary dictionary[1000];
  int dict_index=0;
  for(int i = 0; input[i] != '\0'; ++i){  // Create a dictionary from unique characters in the input
    char key[2] = {input[i], '\0'};
    if(find_in_dictionary(dictionary,dict_index,key) == -1){ // if not found add to dictionary
      dictionary[dict_index].key=strdup(key);
      dictionary[dict_index].value=dict_index;
      dict_index++;
    }
  }
  main_dict=dict_index;
  int encoded[1000];
  int encoded_index=0;
  int i=0;
  while(input[i]!='\0'){   // Encode the input using the created dictionary
    char key[100]="";
    int j=i;
    int prev_value=-1;
    while(input[j]!='\0'){
      strncat(key,&input[j], 1);
      int value=find_in_dictionary(dictionary,dict_index,key);
      if(value==-1){ // if not found
        dictionary[dict_index].key=strdup(key); // add to dictionary
        dictionary[dict_index].value=dict_index;
        encoded[encoded_index++]=prev_value; // and encode with the existing one
        dict_index++;
        break;
      } else prev_value=value;
      j++;
    }
    i=j;
    // Fix: Add this condition to handle the last part of the input
    if(input[j]=='\0' && prev_value!=-1) encoded[encoded_index++]=prev_value;
  }
  for(int i=0;i<dict_index;++i) printf("%d - %s\n",dictionary[i].value,dictionary[i].key);   // Print the dictionary
  printf("\nEncoded: ");

  int bits=0;
  for(int i= 0;i<encoded_index;++i){   // Calculate and print the binary representation of the encoded input
    if(main_dict<8) bits=3;
    else if(main_dict<16) bits=4;
    else if(main_dict<32) bits=5;
    else if(main_dict<64) bits=6;
    char binary_str[bits];
    int_to_binary(encoded[i],bits,binary_str);
    printf("%s ",binary_str);
    main_dict++;
  }
  printf("\n");
  char decoded[1000] = "";
  for(int i=0;i<encoded_index;++i){   // Decoding
    int value=encoded[i];
    char *key=find_key_in_dictionary(dictionary,dict_index,value);
    strcat(decoded,key);
  }
  printf("Decoded: %s\n",decoded); // Print the decoded input

  for (int i=0;i<dict_index;++i) free(dictionary[i].key);   // Free the memory allocated for the dictionary keys
  return 0;
}