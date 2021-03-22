#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

/*
** big_integer.h
** Code largely based on this repo: <https://github.com/andreazevedo/biginteger>
** Modifications: support for arbitrary large numbers, add multiplication & division
**/

typedef struct BigIntegerData
{
	unsigned int* bits;
    // assume the number would not be larger than 2^INT_MAX
	int size;
    int capacity;
} BigIntegerData;

typedef struct BigInteger 
{
	int sign;
	BigIntegerData data;
} BigInteger;

/* creates a big integer number */
BigInteger big_integer_create( long long value );

/* creates a big integer number from file in the specific format:
 *     sign size bits[0] bits[1] ... bits[size-1]
 */
BigInteger big_integer_create_from_file( FILE** ppFile );

/* destroy(free) a big integer number */
void big_integer_destroy( BigInteger* pBigInt );

/* returns the big integer as int */
int big_integer_to_int( const BigInteger bigInt );

/* returns the big integer as long long */
long long big_integer_to_long_long( const BigInteger bigInt );

/* compare big integers */
int big_integer_compare( const BigInteger left, const BigInteger right );

/* adds two big integers together ( left + right ) */
BigInteger big_integer_add( const BigInteger left, const BigInteger right );
void big_integer_add_inplace( const BigInteger left, const BigInteger right, BigInteger* pResult );

/* subtracts one big integer from another ( left - right ) */
BigInteger big_integer_subtract( const BigInteger left, const BigInteger right );
void big_integer_subtract_inplace( const BigInteger left, const BigInteger right, BigInteger* pResult );

/* increments the bigInteger by the amount specified */
void big_integer_increment( BigInteger *bigInt, const unsigned int value );

/* decrements the bigInteger by the amount specified */
void big_integer_decrement( BigInteger *bigInt, const unsigned int value );

/* multiply two big integers ( left * right ) */
BigInteger big_integer_multiply( const BigInteger left, const BigInteger right );
void big_integer_multiply_inplace( const BigInteger left, const BigInteger right, BigInteger* pResult );


#ifdef DEBUG
void big_integer_dump( const BigInteger bigInt );
#endif

#endif /* BIG_INTEGER_H */