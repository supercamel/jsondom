#ifndef JSON_DOM_QUERY_INSTRUCTIONS_H
#define JSON_DOM_QUERY_INSTRUCTIONS_H

enum {
    QI_NULL,            // invalid instruction / terminate
    QI_SPLAT,           // match all documents
    QI_PUSH_NODE,       // pushes a pointer to a node onto the stack
    QI_LOAD,            // load a value from the DOM to the stack
    QI_EQ,              // perform EQ operation. pops 2 values, compares them. pushes boolean result.
    QI_NEQ,             // same as EQ, only it pushes TRUE if they do not match
    QI_GT,              // pops two values. pushed TRUE if the first is greater than the second
    QI_LT,              // pops two values. pushes FALSE if the first is greater than the second
    QI_RANGE,           // pops two values that represent a range, pops a third value. 
                        // pushes true is 3rd is within the range, otherwise false
    QI_IN,              //  expects an array at the top of the stack, and a key name under it. loads the value of the key and pushes true if it matches any of the values in the array
    QI_NIN,             // same as in only pushes false if it matches any values in the array
    QI_EXISTS,          // pops a string value, pushes true if the document has this as a key
    QI_STARTSWITH,      // pops a string value, pops a key value. pushes true if the string matches the value of the key entry
    QI_REGEX,           // idk
    QI_AND,             // umm
    QI_OR,
    QI_NOT
} typedef QueryInstruction;


#endif

