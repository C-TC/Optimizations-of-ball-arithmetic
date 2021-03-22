/*
** big_integer.c
** Code largely based on this repo: <https://github.com/andreazevedo/biginteger>
** Modifications: support for arbitrary large numbers, bigint multiplication
**/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include "big_integer.h"

#ifndef MAX
    #define MAX( a, b ) ( (a)>(b) ? (a) : (b) )
#endif
#ifndef MIN
#	define MIN( a, b ) ( (a)<(b) ? (a) : (b) )
#endif

const int UINT_NUM_BYTES = (sizeof(unsigned int));
const int UINT_NUM_BITS = (sizeof(unsigned int) * 8);

/* PRIVATE FUNCTIONS DECLARATIONS */

/* ---------- Data Creation & Memory Management ---------- */
/* create a big_integer_data with size, capacity 0 & bits NULL */
BigIntegerData big_integer_empty_data( );
/* create a BigIntegerData with size `size`, capacity 2*`size`, and bits copied */
BigIntegerData big_integer_create_data( const unsigned int bits[], const int size );
/* create a BigInteger with sign `sign` & data `data` */
BigInteger big_integer_create_internal( const int sign, const BigIntegerData data );
/* free the allocated bits array and set size/sign/capacity to 0 */
void big_integer_destroy(BigInteger* pBigInteger);
/* free the allocated bits array and set size/capacity to 0 */
void big_integer_data_destroy(BigIntegerData* pBigIntegerData);
/* allocate a new bits array with `new_capacity` and copy the old data into it */
void big_integer_data_resize( BigIntegerData *pBigIntData, const int new_capacity );
/* create a deep copy of a bigint/bigintdata */
BigInteger big_integer_deepcopy(const BigInteger other);
BigIntegerData big_integer_data_deepcopy(const BigIntegerData other);
/* set the size according to the largest index i s.t. bits[i] is not zero */
void big_integer_normalize( BigIntegerData *pBigIntData );
void big_integer_normalize_from( BigIntegerData *pBigIntData, const int from );
/* set the memory from bits[size] to bits[capacity-1] to  0 */
void big_integer_clear_trash_data( BigIntegerData *pBigIntData );
// void big_integer_report_overflow();

/* ---------- Comparision ---------- */
/* compare two unsigned bigints, return 1 if left > right, 0 if =, and -1 if < */
int big_integer_compare_data( const BigIntegerData *pLeft, const BigIntegerData *pRight );
/* compare a unsigned bigint with an uint */
int big_integer_compare_data_uint( const BigIntegerData *pBigIntData, unsigned int value );

/* ----------Arithmetic ---------- */
/* add/subtract two unsigned bigints */
BigIntegerData big_integer_add_data( const BigIntegerData left, const BigIntegerData right );
void big_integer_add_data_inplace(const BigIntegerData left, const BigIntegerData right, BigIntegerData *pResult);
/* subtraction assumes that left > right always holds */
BigIntegerData big_integer_subtract_data( const BigIntegerData left, const BigIntegerData right );
void big_integer_subtract_data_inplace(const BigIntegerData left, const BigIntegerData right, BigIntegerData *pResult);
/* add/subtract a bigint with a uint value */
void big_integer_increment_data( BigIntegerData *pBigIntData, const unsigned int value );
void big_integer_decrement_data( BigIntegerData *pBigIntData, const unsigned int value );

void big_integer_multiply_data_with_uint( const BigIntegerData left, unsigned int right, BigIntegerData* pResult );
BigIntegerData big_integer_multiply_data( BigIntegerData left,  BigIntegerData right );
void big_integer_data_left_shift( BigIntegerData *pBigIntData, int d );
BigInteger big_integer_multiply( const BigInteger left, const BigInteger right );

/* PRIVATE FUNCTIONS IMPLEMENTATION */
BigIntegerData big_integer_empty_data( )
{
    BigIntegerData bigIntData;
    bigIntData.size = 0;
    bigIntData.capacity = 0;
    bigIntData.bits = NULL;
    big_integer_clear_trash_data( &bigIntData );
    return bigIntData;
};

BigIntegerData big_integer_create_data( const unsigned int bits[], const int size )
{
    assert(size > 0);

    BigIntegerData bigIntData;
    // allocate memory
    bigIntData.size = size;
    // by default we allocate memory twice as needed
    // TODO: this is a designed choice to be discussed
    bigIntData.capacity = 2 * size;
    bigIntData.bits = (unsigned int*) malloc( bigIntData.capacity * UINT_NUM_BYTES );
    if (bits && size > 0)
        memcpy( bigIntData.bits, bits, UINT_NUM_BYTES * size );

    big_integer_clear_trash_data( &bigIntData );

    return bigIntData;
};

BigInteger big_integer_create_internal( const int sign, const BigIntegerData data )
{
    BigInteger bigInt;
    bigInt.sign = sign;
    bigInt.data = data;

    return bigInt;
};

// TODO: maybe we can just leave sign/size/capacity untouched?
void big_integer_destroy(BigInteger* pBigInteger)
{
    pBigInteger->sign = 0;
    pBigInteger->data.capacity = 0;
    pBigInteger->data.size = 0;
    free(pBigInteger->data.bits);
}

// TODO: maybe we can just leave sign/size/capacity untouched?
void big_integer_data_destroy( BigIntegerData* pBigIntegerData )
{
    pBigIntegerData->capacity = 0;
    pBigIntegerData->size = 0;
    free(pBigIntegerData->bits);
}

void big_integer_data_resize( BigIntegerData *pBigIntData, const int new_capacity )
{
    pBigIntData->capacity = new_capacity;
    unsigned int* bits = (unsigned int*) malloc(pBigIntData->capacity*UINT_NUM_BYTES);
    memcpy(bits, pBigIntData->bits, pBigIntData->size*UINT_NUM_BYTES);
    free(pBigIntData->bits);
    pBigIntData->bits = bits;
    big_integer_clear_trash_data(pBigIntData);
    return;
}

BigInteger big_integer_deepcopy(const BigInteger other)
{
    BigInteger this;
    this.sign = other.sign;
    this.data.capacity = other.data.capacity;
    this.data.size = other.data.size;
    this.data.bits = (unsigned int*) malloc( this.data.capacity * UINT_NUM_BYTES );
    memcpy( this.data.bits, other.data.bits, this.data.capacity * UINT_NUM_BYTES );
    return this;
}

BigIntegerData big_integer_data_deepcopy(const BigIntegerData other)
{
    BigIntegerData this;
    this.capacity = other.capacity;
    this.size = other.size;
    this.bits = (unsigned int*) malloc( this.capacity * UINT_NUM_BYTES );
    memcpy( this.bits, other.bits, this.capacity * UINT_NUM_BYTES );
    return this;
}

void big_integer_normalize( BigIntegerData *pBigIntData )
{
    big_integer_normalize_from( pBigIntData, pBigIntData->capacity-1 );
};

void big_integer_normalize_from( BigIntegerData *pBigIntData, const int from )
{
    int i;
    for ( i = from; i >= 0; --i )
    {
        if ( pBigIntData->bits[i] != 0 )
        {
            pBigIntData->size = i + 1;
            break;
        }
    }
};

void big_integer_clear_trash_data( BigIntegerData *pBigIntData )
{	
    if(pBigIntData==NULL)
    return;

    int i;
    for ( i = pBigIntData->size; i < pBigIntData->capacity; ++i )
        pBigIntData->bits[i] = 0;
    
    // TODO: is memset more efficient?
    // memset(&(pBigIntData->bits[pBigIntData->size]), 0, (pBigIntData->capacity-pBigIntData->size)*UINT_NUM_BYTES);
};

// void big_integer_report_overflow()
// {
//    fprintf(stderr, "BigInteger reported overflow!\n");
// };

int big_integer_compare_data( const BigIntegerData *pLeft, const BigIntegerData *pRight )
{
    /* if the sizes are different */
    if ( pLeft->size > pRight->size )
        return 1;
    if ( pLeft->size < pRight->size )
        return -1;

    int size = pLeft->size;
    int i;
    for ( i = (size-1); i >= 0; --i)
    {
        if ( pLeft->bits[i] > pRight->bits[i] )
            return 1;
        if ( pLeft->bits[i] < pRight->bits[i] )
            return -1;
    }

    return 0;
};

int big_integer_compare_data_uint( const BigIntegerData *pBigIntData, unsigned int value )
{
    if ( pBigIntData->size == 0 )
        return -1;
    if ( pBigIntData->size > 1 )
        return 1;

    if ( pBigIntData->bits[0] > value )
        return 1;
    else if ( pBigIntData->bits[0] < value )
        return -1;

    return 0;
};

BigIntegerData big_integer_add_data( const BigIntegerData left, const BigIntegerData right )
{
    BigIntegerData result = big_integer_empty_data( );

    int size = MAX( left.size, right.size );
    int capacity = MAX( left.capacity, right.capacity );

    result.bits = (unsigned int*) malloc( capacity * UINT_NUM_BYTES );
    result.capacity = capacity;

    unsigned long long sum = 0;
    int i;
    for ( i = 0; i < size; ++i )
    {
        sum += (unsigned long long) left.bits[i] + right.bits[i];
        result.bits[i] = (unsigned int) sum;
        sum >>= UINT_NUM_BITS;
    }

    if ( sum > 0 )
    {
        result.bits[i] = (unsigned int) sum;
        i++;
    }

    result.size = i;
    big_integer_clear_trash_data( &result );

    assert(result.size <= result.capacity);
    if (result.size == result.capacity) {
        big_integer_data_resize( &result, result.capacity*2 );
    }

    return result;
};

// left or right can be the same data as *pResult
void big_integer_add_data_inplace( const BigIntegerData left, const BigIntegerData right, BigIntegerData *pResult )
{
    // assume pResult's bits is allocated and the capacity is properly set
    // the contents in bits & size won't matter as they will be written again.

    int size = MAX( left.size, right.size );
    int capacity = MAX( left.capacity, right.capacity );
    
    if (pResult->capacity < size+1) {
        // TODO: this is a design choice: make the result as large as its operands.
        // make sure overflow won't happen
        // if (capacity < size) {
        //     printf("%u %u\n", left.bits[0], right.bits[0]);
        //     printf("%d %d; %d %d\n", left.size, left.capacity, right.size, right.capacity);
        // }
        assert(capacity >= size);
        big_integer_data_resize(pResult, capacity);
    }

    unsigned long long sum = 0;
    int i;
    for ( i = 0; i < size; ++i )
    {
        sum += (unsigned long long) left.bits[i] + right.bits[i];
        pResult->bits[i] = (unsigned int) sum;
        sum >>= UINT_NUM_BITS;
    }

    if ( sum > 0 )
    {
        pResult->bits[i] = (unsigned int) sum;
        i++;
    }

    pResult->size = i;
    big_integer_clear_trash_data(pResult);

    assert(pResult->size <= pResult->capacity);
    if (pResult->size == pResult->capacity) {
        // hope this won't happen much in a addition
        big_integer_data_resize( pResult, pResult->capacity*2 );
    }

    return;
}

/* left > right always */
BigIntegerData big_integer_subtract_data( const BigIntegerData left, const BigIntegerData right )
{
    BigIntegerData result = big_integer_empty_data( );

    int size = MAX( left.size, right.size );
    int capacity = MAX( left.capacity, right.capacity );

    result.bits = (unsigned int*) calloc( capacity, UINT_NUM_BYTES );
    result.capacity = capacity;

    unsigned long long borrow = 0;
    int i;
    for ( i = 0; i < size; ++i )
    {
        /* what happens here is that, if left is less than right, borrow will become 
           "negative" (not really because it is unsigned), and the bit pattern for that is 
           the 1's complement (complementing it to get to 0), which is exactly the remainder
           of this term in the subtraction. */
        borrow = (unsigned long long) left.bits[i] - right.bits[i] - borrow;

        result.bits[i] = (unsigned int) borrow;

        /* here we just want the first 1 after removing the lower order term */
        borrow = (borrow >> UINT_NUM_BITS) & 1; 
    }

    big_integer_normalize_from( &result, i );

    return result;
};

void big_integer_subtract_data_inplace( const BigIntegerData left, const BigIntegerData right, BigIntegerData *pResult )
{
    int size = MAX( left.size, right.size );
    int capacity = MAX( left.capacity, right.capacity );
    
    if (pResult->capacity < size+1) {
        // TODO: this is a design choice: make the result as large as its operands.
        // make sure overflow won't happen
        assert( capacity > size );
        big_integer_data_resize( pResult, capacity );
    }

    unsigned long long borrow = 0;
    int i;
    for ( i = 0; i < size; ++i )
    {
        /* what happens here is that, if left is less than right, borrow will become 
           "negative" (not really because it is unsigned), and the bit pattern for that is 
           the 1's complement (complementing it to get to 0), which is exactly the remainder
           of this term in the subtraction. */
        borrow = (unsigned long long) left.bits[i] - right.bits[i] - borrow;

        pResult->bits[i] = (unsigned int) borrow;

        /* here we just want the first 1 after removing the lower order term */
        borrow = (borrow >> UINT_NUM_BITS) & 1; 
    }

    // assume bits[size] to bits[capacity-1] is clean (set to 0)
    big_integer_normalize_from( pResult, i );

    return;
}

void big_integer_increment_data( BigIntegerData *pBigIntData, const unsigned int value )
{
    unsigned long long carry = value;
    int i = 0;
    while ( carry > 0 )
    {
        carry += (unsigned long long) pBigIntData->bits[i];
        pBigIntData->bits[i] = (unsigned int) carry;
        carry >>= UINT_NUM_BITS;
        ++i;
    }

    if ( i > pBigIntData->size )
        pBigIntData->size = i;

    assert(pBigIntData->size <= pBigIntData->capacity);
    if (pBigIntData->size == pBigIntData->capacity) {
        big_integer_data_resize( pBigIntData, pBigIntData->capacity*2 );
    }
};

/* pBigIntData > value */
void big_integer_decrement_data( BigIntegerData *pBigIntData, const unsigned int value )
{
    unsigned long long borrow = value;
    int i = 0;
    while ( borrow > 0 )
    {
        borrow = (unsigned long long) pBigIntData->bits[i] - borrow;
        pBigIntData->bits[i] = (unsigned int) borrow;
        borrow = (borrow >> UINT_NUM_BITS) & 1;
        ++i;
    }

    big_integer_normalize_from( pBigIntData, i );
    assert(pBigIntData->size <= pBigIntData->capacity);
    if (pBigIntData->size == pBigIntData->capacity) {
        big_integer_data_resize( pBigIntData, pBigIntData->capacity*2 );
    }
};

void big_integer_multiply_data_with_uint( const BigIntegerData left, unsigned int right, BigIntegerData* pResult ) {
    // use ulong to store the value to avoid implicit conversion 
    unsigned long ulright = right, product = 0;
    int i;

    // make sure pResult have enough space to store the result
    if (pResult->capacity < left.size+1) {
        // avoid copy
        pResult->size = 0;
        big_integer_data_resize( pResult, left.size*2 );
    }

    for (i = 0; i < left.size; ++i) {
        product += ulright * left.bits[i];
        // TODO: not sure if this is correct and the most efficient way to extract the lower 32 bits
        // pResult->bits[i] = product & 0x00000000ffffffff;
        pResult->bits[i] = (unsigned int) product;
        // get upper 32 bits
        product >>= UINT_NUM_BITS;
    }

    // i is just left.size
    if (product > 0) {
        pResult->bits[i] = (unsigned int) product;
        pResult->size = i + 1;
    }
    else {
        pResult->size = i;
    }

    return;
}

BigIntegerData big_integer_multiply_data( const BigIntegerData left, const BigIntegerData right)
{	
    BigIntegerData result;
    BigIntegerData left_copy = big_integer_data_deepcopy( left );
    int capacity = MAX( left.capacity, right.capacity );
    // calloc is fater than malloc+memset
    // good for initialization
    result.bits = (unsigned int*) calloc( capacity*2, UINT_NUM_BYTES );
    result.capacity = capacity * 2;
    result.size = 0;

    int i, j;
    for( i = 0; i < right.size; i++ )
    {
        // big_integer_multiply_data_with_uint(left_copy, right.bits[i], &result);
        // TODO: remove this stupid loop
        // printf("%d %d\n", left_copy.size, left_copy.capacity);
        // printf("\t%d %d\n", right.size, right.capacity);
        for(j=1;j<=right.bits[i];j++)
        {
            // this is memory leaking!
            // result=big_integer_add_data(result,left);
            big_integer_add_data_inplace(result, left_copy, &result);
        }
        big_integer_data_left_shift(&left_copy, 1);
    }
    big_integer_data_destroy(&left_copy);
    return result;
}

void big_integer_data_left_shift(BigIntegerData *pBigIntData, int d)
{
    assert( d > 0 );
    int i;
    // if( pBigIntData->size == 1 && pBigIntData -> bits[0] == 0) return ;
    // 0 has the size of 0
    if ( pBigIntData->size == 0 ) return;
    if ( pBigIntData->size + d > pBigIntData->capacity )
        big_integer_data_resize( pBigIntData, 2*(pBigIntData->size+d) );
    for ( i = pBigIntData->size ; i >= 0 ; i-- )
    {
        pBigIntData->bits[i+d] = pBigIntData->bits[i];
    }
    for( i = 0; i < d; i++ )
    {
        pBigIntData->bits[i] = 0;
    }
    pBigIntData->size+=d;
}

/* PUBLIC FUNCTIONS IMPLEMENTATION */
BigInteger big_integer_create( long long value )
{
    BigInteger bigInt;
    bigInt.data.bits = (unsigned int*) malloc(sizeof(long long) * 2);
    bigInt.data.capacity = sizeof(long long) * 2 / UINT_NUM_BYTES;
    int numBits = UINT_NUM_BITS;

    if ( value == 0 )
    {
        bigInt.sign = 0;
        bigInt.data.bits[0] = 0;
        bigInt.data.size = 1;
    }
    else
    {
        unsigned long long uValue;
        if ( value < 0 )
        {
            bigInt.sign = -1;
            uValue = (unsigned long long) -value;
        }
        else
        {
            bigInt.sign = 1;
            uValue = (unsigned long long) value;
        }

        bigInt.data.size = 0;
        while ( uValue > 0 )
        {
            bigInt.data.bits[bigInt.data.size++] = (unsigned int) uValue;
            uValue >>= numBits;
        }
    }

    big_integer_clear_trash_data( &bigInt.data );

    return bigInt;
};

BigInteger big_integer_create_from_file( FILE** ppFile ) {
    BigInteger bigInt;
    int sign, size;
    int ret;
    int i;
    ret = fscanf(*ppFile, "%d %d", &sign, &size);
    if (ret != 2) {
        printf("Error during reading a bigint's sign & size!");
        exit(EXIT_FAILURE);
    }
    assert(size > 0);
    bigInt.sign = sign;
    bigInt.data.size = size;
    bigInt.data.capacity = 2 * size;
    bigInt.data.bits = (unsigned int*) malloc( bigInt.data.capacity * UINT_NUM_BYTES );
    for (i = 0; i < size; ++i) {
        ret = fscanf(*ppFile, "%u", &(bigInt.data.bits[i]));
        assert(ret == 1);
    }
    big_integer_clear_trash_data( &(bigInt.data) );
    return bigInt;
}

int big_integer_to_int( const BigInteger bigInt )
{
    if ( bigInt.sign == 0 )
        return 0;

    /* overflow check */
    if ( bigInt.data.size > 1 ||
        (bigInt.sign == 1 && bigInt.data.bits[0] > INT_MAX) ||
        (bigInt.sign == -1 && -(bigInt.data.bits[0]) < INT_MIN) )		
    {
        // big_integer_report_overflow();
        abort();
        exit( EXIT_FAILURE );
    }

    if ( bigInt.sign == -1 )
        return -(int)bigInt.data.bits[0];

    return (int)bigInt.data.bits[0];
};

long long big_integer_to_long_long( const BigInteger bigInt )
{
    if ( bigInt.sign == 0 )
        return 0;

    int uIntNumBits = UINT_NUM_BITS;
    int maxLength = sizeof(long long) / UINT_NUM_BYTES;

    if ( bigInt.data.size > maxLength )
    {
        // big_integer_report_overflow();
        abort();
        exit( EXIT_FAILURE );
    }

    unsigned long long result = 0;
    int i = 0;
    for ( i = 0; i < bigInt.data.size; ++i )
    {
        result |= ((unsigned long long)bigInt.data.bits[i]) << (uIntNumBits * i);
    }

    if ( bigInt.sign == -1 )
        return -(long long)result;

    return result;
};

int big_integer_compare( const BigInteger left, const BigInteger right )
{
    /* if one is positive and the other negative */
    if ( left.sign > right.sign )
        return 1;
    if ( left.sign < right.sign )
        return -1;
    
    /* if they have the same sign */
    int sign = left.sign;
    return sign * big_integer_compare_data( &left.data, &right.data );
};

BigInteger big_integer_add( const BigInteger left, const BigInteger right )
{
    if ( left.sign == 0 )
        return big_integer_deepcopy(right);
    if ( right.sign == 0 )
        return big_integer_deepcopy(left);

    if ( left.sign == right.sign )
        return big_integer_create_internal( left.sign, big_integer_add_data( left.data, right.data ));

    /* compare the MOD of the numbers */
    int compRes = big_integer_compare_data( &left.data, &right.data );

    if ( compRes == 0 )
        return big_integer_create( 0 );

    if ( compRes > 0 ) /* left > right */
        return big_integer_create_internal( left.sign, big_integer_subtract_data( left.data, right.data ));

    return big_integer_create_internal( right.sign, big_integer_subtract_data( right.data, left.data ));
};

BigInteger big_integer_subtract( const BigInteger left, const BigInteger right )
{
    if ( left.sign == 0 )
        return big_integer_create_internal( -right.sign, big_integer_data_deepcopy(right.data) );
    if ( right.sign == 0 )
        return big_integer_deepcopy(left);

    if ( left.sign != right.sign )
        return big_integer_create_internal( left.sign, big_integer_add_data(left.data, right.data) );

    /* compare the MOD of the numbers */
    int compRes = big_integer_compare_data( &left.data, &right.data );

    if ( compRes == 0 )
        return big_integer_create( 0 );

    if ( compRes > 0 ) /* left > right */
        return big_integer_create_internal( left.sign, big_integer_subtract_data(left.data, right.data) );

    return big_integer_create_internal( -right.sign, big_integer_subtract_data(right.data, left.data) );
};

void big_integer_increment( BigInteger *bigInt, const unsigned int value )
{
    if ( bigInt->sign >= 0 ) /* bigInt >= 0 */
    {
        if ( bigInt->sign == 0 && value > 0 )
            bigInt->sign = 1;
        big_integer_increment_data( &bigInt->data, value );
    }
    else /* bigInt < 0 */
    {
        int compRes = big_integer_compare_data_uint( &bigInt->data, value );

        if ( compRes == 0 ) /* |bigInt| == |value| */
        {
            bigInt->sign = 0;
            bigInt->data.size = 0;
            big_integer_clear_trash_data( &bigInt->data );
        }
        else if ( compRes > 0 ) /* |bigInt| > |value| */
        {
            big_integer_decrement_data( &bigInt->data, value );
        }
        else /* |bigInt| < |value| */
        {
#ifdef DEBUG
            /* |bigInt| < |value| implies that bigInt has length 1
               because value, if expressed as a BigInteger, would have length 1. */
            assert( bigInt->data.length == 1 );
#endif
            bigInt->sign = 1;
            bigInt->data.bits[0] = value - bigInt->data.bits[0];
        }
    }
};

void big_integer_decrement( BigInteger *bigInt, const unsigned int value )
{
    if ( bigInt->sign <= 0 )/* bigInt <= 0 */
    {
        if ( bigInt->sign == 0 && value > 0 )
            bigInt->sign = -1;
        big_integer_increment_data( &bigInt->data, value );
    }
    else /* bigInt > 0 */
    {
        int compRes = big_integer_compare_data_uint( &bigInt->data, value );

        if ( compRes == 0 ) /* |bigInt| == |value| */
        {
            bigInt->sign = 0;
            bigInt->data.size = 0;
            big_integer_clear_trash_data( &bigInt->data );
        }
        else if ( compRes > 0 ) /* |bigInt| > |value| */
        {
            big_integer_decrement_data( &bigInt->data, value );
        }
        else /* |bigInt| < |value| */
        {
#ifdef DEBUG
            /* |bigInt| < |value| implies that bigInt has length 1 or less 
               because value, if expressed as a BigInteger, would have length 1. */
            assert( bigInt->data.length == 1 );
#endif
            bigInt->sign = -1;
            bigInt->data.bits[0] = value - bigInt->data.bits[0];
        }
    }
};


BigInteger big_integer_multiply(const BigInteger left, const BigInteger right)
{
    if(left.sign==0 || right.sign==0)
        return big_integer_create(0);
    // printf("mul: %d %d %d %d\n", left.data.size, left.data.capacity, right.data.size, right.data.capacity);
    return big_integer_create_internal(left.sign*right.sign, big_integer_multiply_data(left.data, right.data));
}

#ifdef DEBUG
void big_integer_dump( const BigInteger bigInt )
{
    printf("BigInteger:\n");
    printf("Sign: %d\n", (int)bigInt.sign);
    printf("Data: { ");
    if ( bigInt.data.length > 0 )
    {
        int i;
        for ( i = 0; i < (bigInt.data.length - 1); i++ )
            printf("%u, ", bigInt.data.bits[i]);
        printf("%u ", bigInt.data.bits[bigInt.data.length-1]);
    }
    printf("}\n");
    printf("Length: %d\n", bigInt.data.length);
}
#endif