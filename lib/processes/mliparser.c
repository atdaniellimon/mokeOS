/*
    Moke Lang Identifier parser
    Version 1.0
    Made by capmar

    See more https://github.com/capmar/sxml
*/

#include "mliparser.h"
#include "../string/string.h"
#include "../../malloc/mem.h"

// Extract the value of a MLI key (Moke Lang Identifier)
char* mli_get_value(const char* xml_content, const char* key){
    // We search the key tag: <key>KeyName</key>
    char key_tag[64];
    sprintf(key_tag, "<key>%s</key>", key);
    
    char* key_pos = strstr(xml_content, key_tag);
    if(!key_pos) return 0; //Could not find key

    // We look for the start of a value (<string>)
    char* open_string = strstr(key_pos, "<string>");
    if(!open_string) return 0;
    
    char* value_start = open_string + 8; // We skip the 8 characters of "<string>"

    // We look at the end of a value (</string>)
    char* close_string = strstr(value_start, "</string>");
    if(!close_string) return 0;
    int length = close_string - value_start;
    char* result = (char*)kmalloc(length + 1);
    
    for(int i = 0; i < length; i++){
        result[i] = value_start[i];
    }
    result[length] = '\0';

    return result;
}

//I'd like to thank a random GitHub repo for helping me with this code lol