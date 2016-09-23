/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef my_cJSON__h
#define my_cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(GLOBAL_DEBUG)
#define CJSON_DEBUG(format, ...)  log_printf("CJSON: ", format,  ##__VA_ARGS__)
#else
#define CJSON_DEBUG(format, ...)
#endif
/* my_cJSON Types: */
#define my_cJSON_False 0
#define my_cJSON_True 1
#define my_cJSON_NULL 2
#define my_cJSON_Number 3
#define my_cJSON_String 4
#define my_cJSON_Array 5
#define my_cJSON_Object 6
	
#define my_cJSON_IsReference 256
#define my_cJSON_StringIsConst 512

/* The my_cJSON structure: */
typedef struct my_cJSON {
	struct my_cJSON *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct my_cJSON *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==my_cJSON_String */
	int valueint;				/* The item's number, if type==my_cJSON_Number */
	double valuedouble;			/* The item's number, if type==my_cJSON_Number */

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} my_cJSON;

typedef struct my_cJSON_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} my_cJSON_Hooks;

/* Supply malloc, realloc and free functions to my_cJSON */
extern void my_cJSON_InitHooks(my_cJSON_Hooks* hooks);


/* Supply a block of JSON, and this returns a my_cJSON object you can interrogate. Call cJSON_Delete when finished. */
extern my_cJSON *my_cJSON_Parse(const char *value);
/* Render a my_cJSON entity to text for transfer/storage. Free the char* when finished. */
extern char  *my_cJSON_Print(my_cJSON *item);
/* Render a my_cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *my_cJSON_PrintUnformatted(my_cJSON *item);
/* Render a my_cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
extern char *my_cJSON_PrintBuffered(my_cJSON *item,int prebuffer,int fmt);
/* Delete a my_cJSON entity and all subentities. */
extern void   my_cJSON_Delete(my_cJSON *c);

/* Returns the number of items in an array (or object). */
extern int	  my_cJSON_GetArraySize(my_cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern my_cJSON *my_cJSON_GetArrayItem(my_cJSON *array,int item);
/* Get item "string" from object. Case insensitive. */
extern my_cJSON *my_cJSON_GetObjectItem(my_cJSON *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
extern const char *my_cJSON_GetErrorPtr(void);
	
/* These calls create a my_cJSON item of the appropriate type. */
extern my_cJSON *my_cJSON_CreateNull(void);
extern my_cJSON *my_cJSON_CreateTrue(void);
extern my_cJSON *my_cJSON_CreateFalse(void);
extern my_cJSON *my_cJSON_CreateBool(int b);
extern my_cJSON *my_cJSON_CreateNumber(double num);
extern my_cJSON *my_cJSON_CreateString(const char *string);
extern my_cJSON *my_cJSON_CreateArray(void);
extern my_cJSON *my_cJSON_CreateObject(void);

/* These utilities create an Array of count items. */
extern my_cJSON *my_cJSON_CreateIntArray(const int *numbers,int count);
extern my_cJSON *my_cJSON_CreateFloatArray(const float *numbers,int count);
extern my_cJSON *my_cJSON_CreateDoubleArray(const double *numbers,int count);
extern my_cJSON *my_cJSON_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern void my_cJSON_AddItemToArray(my_cJSON *array, my_cJSON *item);
extern void	my_cJSON_AddItemToObject(my_cJSON *object,const char *string,my_cJSON *item);
extern void	my_cJSON_AddItemToObjectCS(my_cJSON *object,const char *string,my_cJSON *item);	/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the my_cJSON object */
/* Append reference to item to the specified array/object. Use this when you want to add an existing my_cJSON to a new my_cJSON, but don't want to corrupt your existing my_cJSON. */
extern void my_cJSON_AddItemReferenceToArray(my_cJSON *array, my_cJSON *item);
extern void	my_cJSON_AddItemReferenceToObject(my_cJSON *object,const char *string,my_cJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
extern my_cJSON *my_cJSON_DetachItemFromArray(my_cJSON *array,int which);
extern void   my_cJSON_DeleteItemFromArray(my_cJSON *array,int which);
extern my_cJSON *my_cJSON_DetachItemFromObject(my_cJSON *object,const char *string);
extern void   my_cJSON_DeleteItemFromObject(my_cJSON *object,const char *string);
	
/* Update array items. */
extern void my_cJSON_InsertItemInArray(my_cJSON *array,int which,my_cJSON *newitem);	/* Shifts pre-existing items to the right. */
extern void my_cJSON_ReplaceItemInArray(my_cJSON *array,int which,my_cJSON *newitem);
extern void my_cJSON_ReplaceItemInObject(my_cJSON *object,const char *string,my_cJSON *newitem);

/* Duplicate a my_cJSON item */
extern my_cJSON *my_cJSON_Duplicate(my_cJSON *item,int recurse);
/* Duplicate will create a new, identical my_cJSON item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
extern my_cJSON *my_cJSON_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated);

extern void my_cJSON_Minify(char *json);

/* Macros for creating things quickly. */
#define my_cJSON_AddNullToObject(object,name)		my_cJSON_AddItemToObject(object, name, my_cJSON_CreateNull())
#define my_cJSON_AddTrueToObject(object,name)		my_cJSON_AddItemToObject(object, name, my_cJSON_CreateTrue())
#define my_cJSON_AddFalseToObject(object,name)		my_cJSON_AddItemToObject(object, name, my_cJSON_CreateFalse())
#define my_cJSON_AddBoolToObject(object,name,b)	    my_cJSON_AddItemToObject(object, name, my_cJSON_CreateBool(b))
#define my_cJSON_AddNumberToObject(object,name,n)	my_cJSON_AddItemToObject(object, name, my_cJSON_CreateNumber(n))
#define my_cJSON_AddStringToObject(object,name,s)	my_cJSON_AddItemToObject(object, name, my_cJSON_CreateString(s))

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define my_cJSON_SetIntValue(object,val)			((object)?(object)->valueint=(object)->valuedouble=(val):(val))
#define my_cJSON_SetNumberValue(object,val)		    ((object)?(object)->valueint=(object)->valuedouble=(val):(val))

#ifdef __cplusplus
}
#endif

#endif
