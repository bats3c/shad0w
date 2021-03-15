// jRead.cpp 
// Version 1v6
//
// jRead - an in-place JSON element reader
// =======================================
//
// Instead of parsing JSON into some structure, this maintains the input JSON as unaltered text
// and allows queries to be made on it directly.
//
// e.g. with the simple JSON:
//		{
//			"astring":"This is a string",
//			"anumber":42,
//			"myarray":[ "one", 2, {"description":"element 3"}, null ],
//			"yesno":true,
//			"HowMany":"1234",
//			"foo":null
//		}
//
// calling:
//		jRead( json, "{'myarray'[0", &jElem );
//
// would return:
//		jElem.dataType= JREAD_STRING;
//		jElem.elements= 1
//		jElem.bytelen= 3
//		jElem.pValue -> "one"
//
// or you could call the helper functions:
//		jRead_string( json, "{'astring'", destString, MAXLEN );
//		jRead_int( json, "{'anumber'", &myint );
//		jRead_string( json, "{'myarray'[3", destString, MAXLEN );
//		etc.
//
// Note that the helper functions do type coersion and always return a value
// (on error an empty string is returned or value of zero etc.)
//
// The query string simply defines the route to the required data item
// as an arbitary list of object or array specifiers:
//		object element=			"{'keyname'"
//		array element=			"[INDEX"
//
// The jRead() function fills a jReadElement structure to describe the located element
// this can be used to locate any element, not just terminal values
// e.g.
//		jRead( json, "{'myarray'", &jElem );
//
// in this case jElem would contain:
//		jElem.dataType= JSON_ARRAY
//		jElem.elements= 4
//		jElem.bytelen= 46
//		jElem.pValue -> [ "one", 2, {"descripton":"element 3"}, null ] ...
//
// allowing jRead to be called again on the array:
// e.g.
//		jRead( jElem.pValue, "[3", &jElem );		// get 4th element - the null value
//
//		.oO!	see main.c runExamples() for a whole bunch of examples		!Oo.
//             -------------------------------------------------------
//
// Note that jRead never modifies the source JSON and does not allocate any memory.
// i.e. elements are returned as pointer and length into the source text.
//
// Functions
// =========
// Main JSON reader:
//		int		jRead( char * JsonSource, char *query, jReadElement &pResult );
//
// Extended function using query parameters for indexing:
//		int		jRead( char * JsonSource, char *query, jReadElement &pResult, int *queryParams );
//
// Function to step thru JSON arrays instead of indexing:
//      char *jReadArrayStep( char *pJsonArray, struct jReadElement *pResult );
//
// Optional Helper functions:
//		long	jRead_long( char *pJson, char *pQuery );
//		int		jRead_int( char *pJson, char *pQuery );
//		double	jRead_double( char *pJson, char *pQuery );
//		int		jRead_string( char *pJson, char *pQuery, char *pDest, int destlen );
//
// Optional String output Functions
//		char *	jReadTypeToString( int dataType );		// string describes dataType
//		char *	jReadErrorToString( int error );		// string descibes error code
//
// *NEW* in 1v2
// - "{NUMBER" returns the "key" value at that index within an object
// - jReadParam() adds queryParams which can be used as indexes into arrays (or into 
//   objects to return key values) by specifying '*' in the query string
//   e.g. jReadParam( pJson, "[*", &result, &index ) 
// *NEW in 1v4
// - fixed a couple of error return values
// - added #define JREAD_DOUBLE_QUOTE_IN_QUERY
// *NEW* in 1v5  (11mar2015)
// - fixed null ptr if '[*' used when null param passed
// *NEW* in 1v6  (24sep2016)
// - fixed handling of empty arrays and objects
//
// TonyWilk, 24sep2016 
// mail at tonywilk . co .uk
//
// License: "Free as in You Owe Me a Beer"
// - actually, since some people really worry about licenses, you are free to apply
//   whatever licence you want.
//
// Note: jRead_atol() and jRead_atof() are modified from original routines
//       fast_atol() and fast_atof() 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
//
//       You may want to replace the use of jRead_atol() and jRead_atof() in helper functions
//       of your own. Especially note that my atof does not handle exponents.
//
//
#include <stdio.h>

#include "jread.h"

// By default we use single quote in query strings so it's a lot easier
// to type in code i.e.  "{'key'" instead of "{\"key\""
//
#ifdef JREAD_DOUBLE_QUOTE_IN_QUERY
#define QUERY_QUOTE	'\"'
#else
#define QUERY_QUOTE '\''
#endif

//------------------------------------------------------
// Internal Functions

char *	jReadSkipWhitespace( char *sp );
char *	jReadFindTok( char *sp, int *tokType );
char *	jReadGetString( char *pJson, struct jReadElement *pElem, char quote );
int   	jReadTextLen( char *pJson );
int   	jReadStrcmp( struct jReadElement *j1, struct jReadElement *j2 );
char *	jReadCountObject( char *pJson, struct jReadElement *pResult, int keyIndex );
char *	jReadCountArray( char *pJson, struct jReadElement *pResult );
char *	jRead_atoi( char *p, unsigned int *result );
char *	jRead_atol( char *p, long *result );
char *	jRead_atof( char *p, double *result);

//=======================================================

char *jReadSkipWhitespace( char *sp )
{
	while( (*sp != '\0') && (*sp <= ' ') )
		sp++;
	return sp;
};


// Find start of a token
// - returns pointer to start of next token or element
//   returns type via tokType
//
char *jReadFindTok( char *sp, int *tokType )
{
	char c;
	sp= jReadSkipWhitespace(sp);
	c= *sp;
	if( c == '\0' )	*tokType= JREAD_EOL;
	else if((c == '"') || (c == QUERY_QUOTE))*tokType= JREAD_STRING;
	else if((c >= '0') && (c <= '9')) *tokType= JREAD_NUMBER;
	else if( c == '-') *tokType= JREAD_NUMBER;
	else if( c == '{') *tokType= JREAD_OBJECT;
	else if( c == '[') *tokType= JREAD_ARRAY;
	else if( c == '}') *tokType= JREAD_EOBJECT; 
	else if( c == ']') *tokType= JREAD_EARRAY;
	else if((c == 't') || (c == 'f')) *tokType= JREAD_BOOL;
	else if( c == 'n') *tokType= JREAD_NULL;
	else if( c == ':') *tokType= JREAD_COLON;
	else if( c == ',') *tokType= JREAD_COMMA;
	else if( c == '*') *tokType= JREAD_QPARAM;
	else *tokType= JREAD_ERROR;
	return sp;
};

// jReadGetString
// - assumes next element is "string" which may include "\" sequences
// - returns pointer to -------------^
// - pElem contains result ( JREAD_STRING, length, pointer to string)
// - pass quote = '"' for Json, quote = '\'' for query scanning
//
// returns: pointer into pJson after the string (char after the " terminator)
//			pElem contains pointer and length of string (or dataType=JREAD_ERROR)
//
char * jReadGetString( char *pJson, struct jReadElement *pElem, char quote )
{
	short skipch;
	pElem->dataType= JREAD_ERROR;
	pElem->elements= 1;
	pElem->bytelen= 0;
	pJson= jReadSkipWhitespace( pJson );
	if( *pJson == quote )
	{
		pJson++;
		pElem->pValue= pJson;				// -> start of actual string
		pElem->bytelen=0;
		skipch= 0;
		while( *pJson != '\0' )
		{
			if( skipch )
				skipch= 0;
			else if( *pJson == '\\' )		//  "\" sequence
				skipch= 1;
			else if( *pJson == quote )
			{
				pElem->dataType= JREAD_STRING;
				pJson++;
				break;
			}
			pElem->bytelen++;
			pJson++;
		};
	};
	return pJson;
};

// jReadTextLen
// - used to identify length of element text
// - returns no. of chars from pJson upto a terminator
// - terminators: ' ' , } ]
//
int jReadTextLen( char *pJson )
{
	int len= 0;
	while(	(*pJson >  ' ' ) &&		// any ctrl char incl '\0'
	      	(*pJson != ',' ) &&
	      	(*pJson != '}' ) &&
	      	(*pJson != ']' ) )
	{
		len++;
		pJson++;
	}
	return len;
}

// compare two json elements
// returns: 0 if they are identical strings, else 1
//
int jReadStrcmp( struct jReadElement *j1, struct jReadElement *j2 )
{
	int i;
	if( (j1->dataType != JREAD_STRING) || 
		(j2->dataType != JREAD_STRING) ||
		(j1->bytelen != j2->bytelen ) )
		return 1;

	for( i=0; i< j1->bytelen; i++ )
		if( ((char *)(j1->pValue))[i] != ((char *)(j2->pValue))[i] )
			return 1;
	return 0; 
}

// read unsigned int from string
char *	jRead_atoi( char *p, unsigned int *result )
{
    unsigned int x = 0;
    while (*p >= '0' && *p <= '9') {
        x = (x*10) + (*p - '0');
        ++p;
    }
	*result= x;
    return p;
}

// read long int from string
//
char * jRead_atol( char *p, long *result )
{
    long x = 0;
    int neg = 0;
    if (*p == '-') {
        neg = 1;
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        x = (x*10) + (*p - '0');
        ++p;
    }
    if (neg) {
        x = -x;
    }
	*result= x;
	return p;
}


#define valid_digit(c) ((c) >= '0' && (c) <= '9')

// read double from string
// *CAUTION* does not handle exponents
//
//
char * jRead_atof( char *p, double *result)
{
    double sign, value;

    // Get sign, if any.
    sign = 1.0;
    if (*p == '-') {
        sign = -1.0;
        p += 1;

    } else if (*p == '+') {
        p += 1;
    }

    // Get digits before decimal point or exponent, if any.
    for (value = 0.0; valid_digit(*p); p += 1) {
        value = value * 10.0 + (*p - '0');
    }

    // Get digits after decimal point, if any.
    if (*p == '.') {
        double pow10 = 10.0;
        p += 1;
        while (valid_digit(*p)) {
            value += (*p - '0') / pow10;
            pow10 *= 10.0;
            p += 1;
        }
    }
	*result= sign * value;
	return p;
}

// read element into destination buffer and add '\0' terminator
// - always copies element irrespective of dataType (unless it's an error)
// - destBuffer is always '\0'-terminated (even on zero lenght returns)
// - returns pointer to destBuffer
//
char *jRead_strcpy( char *destBuffer, int destLength, struct jReadElement *pElement )
{
	int i;
	int len= pElement->bytelen;
	char *pdest= destBuffer;
	char *psrc= (char *)pElement->pValue;
	if( pElement->error == 0 )
	{
		if( len >= destLength )
			len= destLength;
		for( i=0; i<destLength; i++ )
			*pdest++= *psrc++;
	}
	*pdest= '\0';
	return destBuffer;
}

// jReadCountObject
// - used when query ends at an object, we want to return the object length
// - on entry pJson -> "{... "
// - used to skip unwanted values which are objects
// - keyIndex normally passed as -1 unless we're looking for the nth "key" value
//   in which case keyIndex is the index of the key we want
//
char * jReadCountObject( char *pJson, struct jReadElement *pResult, int keyIndex )
{
	struct jReadElement jElement;
	int jTok;
	char *sp;
	pResult->dataType= JREAD_OBJECT;
	pResult->error= 0;
	pResult->elements= 0;
	pResult->pValue= pJson;
	sp= jReadFindTok( pJson+1, &jTok ); // check for empty object
	if( jTok == JREAD_EOBJECT )		
	{
		pJson= sp+1;
	}else
	{
		while( 1 )
		{
			pJson= jReadGetString( ++pJson, &jElement, '\"' );
			if( jElement.dataType != JREAD_STRING )
			{
				pResult->error= 3;		// Expected "key"
				break;
			}
			if( pResult->elements == keyIndex )		// if passed keyIndex
			{
				*pResult= jElement;		// we return "key" at this index
				pResult->dataType= JREAD_KEY;
				return pJson;
			}
			pJson= jReadFindTok( pJson, &jTok );
			if( jTok != JREAD_COLON )
			{
				pResult->error= 4;		// Expected ":"
				break;
			}
			pJson= jRead( ++pJson, "", &jElement );
			if( pResult->error )
				break;
			pJson= jReadFindTok( pJson, &jTok );
			pResult->elements++;
			if( jTok == JREAD_EOBJECT )
			{
				pJson++;
				break;
			}
			if( jTok != JREAD_COMMA )
			{
				pResult->error= 6;		// Expected "," in object
				break;
			}
		}
	}
	if( keyIndex >= 0 )
	{
		// we wanted a "key" value - that we didn't find
		pResult->dataType= JREAD_ERROR;
		pResult->error= 11;			// Object key not found (bad index)
	}else{
		pResult->bytelen= pJson - (char *)pResult->pValue;
	}
	return pJson;
}



// jReadCountArray
// - used when query ends at an array, we want to return the array length
// - on entry pJson -> "[... "
// - used to skip unwanted values which are arrays
//
char * jReadCountArray( char *pJson, struct jReadElement *pResult )
{
	struct jReadElement jElement;
	int jTok;
	char *sp;
	pResult->dataType= JREAD_ARRAY;
	pResult->error= 0;
	pResult->elements= 0;
	pResult->pValue= pJson;
	sp= jReadFindTok( pJson+1, &jTok ); // check for empty array
	if( jTok == JREAD_EARRAY )		
	{
		pJson= sp+1;
	}else
	{
		while( 1 )
		{
			pJson= jRead( ++pJson, "", &jElement );	// array value
			if( pResult->error )
				break;
			pJson= jReadFindTok( pJson, &jTok );	// , or ]
			pResult->elements++;
			if( jTok == JREAD_EARRAY )
			{
				pJson++;
				break;
			}
			if( jTok != JREAD_COMMA )
			{
				pResult->error= 9;		// Expected "," in array
				break;
			}
		}
	}
	pResult->bytelen= pJson - (char *)pResult->pValue;
	return pJson;
}

// jReadArrayStep()
// - reads one value from an array
// - assumes pJsonArray points at the start of an array or array element
//
char *jReadArrayStep( char *pJsonArray, struct jReadElement *pResult )
{
	int jTok;

	pJsonArray= jReadFindTok( pJsonArray, &jTok );
	switch( jTok )
	{
	case JREAD_ARRAY:	// start of array
	case JREAD_COMMA:	// element separator
		return jRead( ++pJsonArray, "", pResult );

	case JREAD_EARRAY:	// end of array
		pResult->error= 13;		// End of array found
		break;
	default:			// some other error
		pResult->error= 9;		// expected comma in array
		break;
	}
	pResult->dataType= JREAD_ERROR;
	return pJsonArray;
}


// jRead
// - reads a complete JSON <value>
// - matches pQuery against pJson, results in pResult
// returns: pointer into pJson
//
// Note: is recursive
//
char * jRead( char *pJson, char *pQuery, struct jReadElement *pResult )
{
	return jReadParam( pJson, pQuery, pResult, NULL );
}

char * jReadParam( char *pJson, char *pQuery, struct jReadElement *pResult, int *queryParams )
{
	int qTok, jTok, bytelen;
	unsigned int index, count;
	struct jReadElement qElement, jElement;

	pJson= jReadFindTok( pJson, &jTok );
	pQuery= jReadFindTok( pQuery, &qTok );

	pResult->dataType= jTok;
	pResult->bytelen= pResult->elements= pResult->error= 0;
	pResult->pValue= pJson;

	if( (qTok != JREAD_EOL) && (qTok != jTok) )
	{
		pResult->error= 1;	// JSON does not match Query
		return pJson;
	}

	switch( jTok )
	{
	case JREAD_ERROR:		// general error, eof etc.
		pResult->error= 2;	// Error reading JSON value
		break;

	case JREAD_OBJECT:		// "{"
		if( qTok == JREAD_EOL )
			return jReadCountObject( pJson, pResult, -1 );	// return length of object 

		pQuery= jReadFindTok( ++pQuery, &qTok );			// "('key'...", "{NUMBER", "{*" or EOL
		if( qTok != JREAD_STRING )
		{
			index= 0;
			switch( qTok )
			{
			case JREAD_NUMBER:
				pQuery= jRead_atoi( (char *)pQuery, &index );	// index value
				break;
			case JREAD_QPARAM:
				pQuery++;
				index= (queryParams != NULL) ? *queryParams++ : 0;	// substitute parameter
				break;
			default:
				pResult->error= 12;								// Bad Object key
				return pJson;
			}
			return jReadCountObject( pJson, pResult, index );
		}

		pQuery= jReadGetString( pQuery, &qElement, QUERY_QUOTE );	// qElement = query 'key'
		//
		// read <key> : <value> , ... }
		// loop 'til key matched
		//
		while( 1 )
		{
			pJson= jReadGetString( ++pJson, &jElement, '\"' );
			if( jElement.dataType != JREAD_STRING )
			{
				pResult->error= 3;		// Expected "key"
				break;
			}
			pJson= jReadFindTok( pJson, &jTok );
			if( jTok != JREAD_COLON )
			{
				pResult->error= 4;		// Expected ":"
				break;
			}
			// compare object keys
			if( jReadStrcmp( &qElement, &jElement ) == 0 )
			{
				// found object key
				return jReadParam( ++pJson, pQuery, pResult, queryParams );
			}
			// no key match... skip this value
			pJson= jRead( ++pJson, "", pResult );
			pJson= jReadFindTok( pJson, &jTok );
			if( jTok == JREAD_EOBJECT )
			{
				pResult->error= 5;		// Object key not found
				break;
			}
			if( jTok != JREAD_COMMA )
			{
				pResult->error= 6;		// Expected "," in object
				break;
			}
		}
		break;
	case JREAD_ARRAY:		// "[NUMBER" or "[*"
		//
		// read index, skip values 'til index
		//
		if( qTok == JREAD_EOL )
			return jReadCountArray( pJson, pResult );	// return length of object 

		index= 0;
		pQuery= jReadFindTok( ++pQuery, &qTok );		// "[NUMBER" or "[*"
		if( qTok == JREAD_NUMBER )		
		{
			pQuery= jRead_atoi( pQuery, &index );		// get array index	
		}else if( qTok == JREAD_QPARAM )
		{
			pQuery++;
			index= (queryParams != NULL) ? *queryParams++ : 0;	// substitute parameter
		}

		count=0;
		while( 1 )
		{
			if( count == index )
				return jReadParam( ++pJson, pQuery, pResult, queryParams );	// return value at index
			// not this index... skip this value
			pJson= jRead( ++pJson, "", &jElement );
			if( pResult->error )
				break;
			count++;				
			pJson= jReadFindTok( pJson, &jTok );			// , or ]
			if( jTok == JREAD_EARRAY )
			{
				pResult->error= 10;		// Array element not found (bad index)
				break;
			}
			if( jTok != JREAD_COMMA )
			{
				pResult->error= 9;		// Expected "," in array
				break;
			}
		}
		break;
	case JREAD_STRING:		// "string" 
		pJson= jReadGetString( pJson, pResult, '\"' );
		break;
	case JREAD_NUMBER:		// number (may be -ve) int or float
	case JREAD_BOOL:		// true or false
	case JREAD_NULL:		// null
		bytelen= jReadTextLen( pJson );
		pResult->dataType= jTok;
		pResult->bytelen= bytelen;
		pResult->pValue= pJson;
		pResult->elements= 1;
		pJson += bytelen;
		break;
	default:
		pResult->error= 8;	// unexpected character (in pResult->dataType)
	}
	// We get here on a 'terminal value'
	// - make sure the query string is empty also
	pQuery= jReadFindTok( pQuery, &qTok );
	if( !pResult->error && (qTok != JREAD_EOL) )
		pResult->error= 7;	// terminal value found before end of query
	if( pResult->error )
	{
		pResult->dataType= JREAD_ERROR;
		pResult->elements= pResult->bytelen= 0;
		pResult->pValue= pJson;		// return pointer into JSON at error point
	}
	return pJson;
}


//--------------------------------------------------------------------
// Optional helper functions
// - simple routines to extract values from JSON
// - does coercion of types where possible
// - always returns a value (e.g. 0 or "" on error)
//
// Note: by default, pass NULL for queryParams
//       unless you are using '*' in the query for indexing
// 

// jRead_long
// - reads signed long value from JSON 
// - returns number from NUMBER or STRING elements (if possible)
//   returns 1 or 0 from BOOL elements
//   otherwise returns 0
//
long jRead_long( char *pJson, char *pQuery, int *queryParams )
{
	struct jReadElement elem;
	long result;
	jReadParam( pJson, pQuery, &elem, queryParams );
	if( (elem.dataType == JREAD_ERROR) || (elem.dataType == JREAD_NULL))
		return 0;
	if( elem.dataType == JREAD_BOOL )
		return *((char *)elem.pValue)=='t' ? 1 : 0;

	jRead_atol( (char *)elem.pValue, &result );
	return result;
}

int jRead_int( char *pJson, char *pQuery, int *queryParams )
{
	return (int)jRead_long( pJson, pQuery, queryParams );
}

// jRead_double
// - returns double from JSON
// - returns number from NUMBER or STRING elements
//   otherwise returns 0.0
//
double jRead_double( char *pJson, char *pQuery, int *queryParams )
{
	struct jReadElement elem;
	double result;
	jReadParam( pJson, pQuery, &elem, queryParams );
	if( elem.dataType == JREAD_ERROR )
		return 0.0;
	jRead_atof( (char *)elem.pValue, &result );
	return result;
}

// jRead_string
// Copy string to pDest and '\0'-terminate it (upto destlen total bytes)
// returns: character length of string (excluding '\0' terminator)
//
// Note: any element can be returned as a string
//
int jRead_string( char *pJson, char *pQuery, char *pDest, int destlen, int *queryParams )
{
	struct jReadElement elem;
	int i;

	*pDest= '\0';
	jReadParam( pJson, pQuery, &elem, queryParams );
	if( elem.dataType == JREAD_ERROR )
		return 0;

	for( i=0; (i<elem.bytelen) && (i<destlen-1); i++ )
		*pDest++ = ((char *)elem.pValue)[i];
	*pDest= '\0';
	return elem.bytelen;
}

//-------------------------------------------------
// Optional String output Functions
//
char *jReadTypeStrings[]={
	"Error",			// 0
	"Object",			// 1
	"Array",			// 2
	"String",			// 3
	"Number",			// 4
	"Bool",				// 5
	"null",				// 6
	"Object key",		// 7
	"colon",			// 8
	"eol",				// 9
	"comma",			// 10
	"}",				// 11
	"]",				// 12
	"* parameter"		// 13
};

char *jReadTypeToString( int dataType )
{
	return jReadTypeStrings[ dataType ];
};

char * jReadErrorStrings[]={
	"Ok",                                       // 0
	"JSON does not match Query",                // 1
	"Error reading JSON value",                 // 2
	"Expected \"key\"",                         // 3
	"Expected ':'",                             // 4
	"Object key not found",                     // 5
	"Expected ',' in object",                   // 6
	"Terminal value found before end of query", // 7
	"Unexpected character",                     // 8
	"Expected ',' in array",                    // 9
	"Array element not found (bad index)",      // 10
	"Object key not found (bad index)",			// 11
	"Bad object key",							// 12
	"End of array found",						// 13
	"End of object found"						// 14
};
char * jReadErrorToString( int error )
{
	if( (error >=0 ) && (error <= 14))
		return jReadErrorStrings[ error ];
	return "Unknown error";
};

// end of jRead.c
