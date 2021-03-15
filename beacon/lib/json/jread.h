// jRead.h
//
// see jRead.c for more information
//

// uncomment this if you really want to use double quotes in query strings instead of '
//#define JREAD_DOUBLE_QUOTE_IN_QUERY

//
// return dataTypes:
#define JREAD_ERROR		0		// general error, eof etc.
#define JREAD_OBJECT	1		// "{"
#define JREAD_ARRAY		2		// "["
#define JREAD_STRING	3		// "string" 
#define JREAD_NUMBER	4		// number (may be -ve) int or float
#define JREAD_BOOL		5		// true or false
#define JREAD_NULL		6		// null
#define JREAD_KEY		7		// object "key"
// internal values:
#define JREAD_COLON		8		// ":"
#define JREAD_EOL		9		// end of input string (ptr at '\0')
#define JREAD_COMMA		10		// ","
#define JREAD_EOBJECT	11		// "}"
#define JREAD_EARRAY	12		// "]"
#define JREAD_QPARAM	13		// "*" query string parameter

//------------------------------------------------------
// jReadElement
// - structure to return JSON elements
// - error=0 for valid returns
//
// *NOTES* 
//    the returned pValue pointer points into the passed JSON
//    string returns are not '\0' terminated.
//    bytelen specifies the length of the returned data pointed to by pValue
//
struct jReadElement{
	int dataType;		// one of JREAD_...
	int elements;		// number of elements (e.g. elements in array or object)
	int bytelen;		// byte length of element (e.g. length of string, array text "[ ... ]" etc.)
	void * pValue;		// pointer to value string in JSON text
	int error;			// error value if dataType == JREAD_ERROR
};

//------------------------------------------------------
// The JSON reader function
//
// - reads a '\0'-terminated JSON text string from pJson
// - traverses the JSON according to the pQuery string
// - returns the result value in pResult
//
// returns: pointer into pJson after the queried value
//
// e.g.
//    With JSON like: "{ ..., "key":"value", ... }"
//
//    jRead( pJson, "{'key'", &result );
// returns with: 
//    result.dataType= JREAD_STRING, result.pValue->'value', result.bytelen=5
//
char *	jRead( char *pJson, char *pQuery, struct jReadElement *pResult );

// version of jRead which allows one or more queryParam integers to be substituted
// for array or object indexes marked by a '*' in the query
//
// e.g. jReadParam( pJson, "[*", &resultElement, &arrayIndex );
//
// *!* CAUTION *!*
// You can supply an array of integers which are indexed for each '*' in pQuery
// however, horrid things will happen if you don't supply enough parameters
// 
char * jReadParam( char *pJson, char *pQuery, struct jReadElement *pResult, int *queryParams );

// Array Stepping function
// - assumes pJsonArray is JSON source of an array "[ ... ]"
// - returns next element of the array in pResult
// - returns pointer to end of element, to be passed to next call of jReadArrayStep()
// - if end of array is encountered, pResult->error = 13 "End of array found"
//
// e.g.
//   With JSON like:   "{ ...  "arrayInObject":[ elem1,elem2,... ], ... }"
//
//   pJson= jRead( pJson, "{'arrayInObject'", &theArray );
//   if( theArray.dataType == JREAD_ARRAY )
//   {
//       char *pArray= (char *)theArray.pValue;
//       jReadElement arrayElement;
//       int index;
//       for( index=0; index < theArray.elements; index++ )
//       {
//           pArray= jReadArrayStep( pArray, &arrayElement );
//           ...
//
// Note: this significantly speeds up traversing arrays.
//
char *jReadArrayStep( char *pJsonArray, struct jReadElement *pResult );


#define EXPORT_OPTIONAL_FUNCTIONS
#ifdef EXPORT_OPTIONAL_FUNCTIONS 

//------------------------------------------------------
// Optional Helper Functions
//
long jRead_long( char *pJson, char *pQuery, int *queryParams );
int jRead_int( char *pJson, char *pQuery, int *queryParams );
double jRead_double( char *pJson, char *pQuery, int *queryParams );
int jRead_string( char *pJson, char *pQuery, char *pDest, int destlen, int *queryParams );

//------------------------------------------------------
// Optional String output Functions
//
char *jReadTypeToString( int dataType );	   	// string describes dataType
char * jReadErrorToString( int error );		   	// string descibes error code

//------------------------------------------------------
// Other jRead utilities which may be useful...
//
char * jRead_atoi( char *p, unsigned int *result );	// string to unsigned int
char * jRead_atol( char *p, long *result );			// string to signed long
char * jRead_atof( char *p, double *result);		// string to double (does not do exponents)
int jReadStrcmp( struct jReadElement *j1, struct jReadElement *j2 ); // compare STRING elements

// copy element to '\0'-terminated buffer
char * jRead_strcpy( char *destBuffer, int destLength, struct jReadElement *pElement );

#endif
// end of jRead.h
