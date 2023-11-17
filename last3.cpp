#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
const long double epsilan = 0.0000001;
bool lfEqual            (long double a, long double b, long double eps);
bool lfWithoutDecimal   (long double a,                long double eps);

unsigned long long NOD(unsigned long long a, unsigned long long b);
unsigned long long NOK(unsigned long long a, unsigned long long b);

typedef struct fraction {
    unsigned long long numerator;
    unsigned long long denominator;
} fraction;

typedef struct number {
    long double integer;
    size_t      exponent;
    fraction    period;

} number;

number      numberConstuctor            (const char *data);
number      numberAdding                (number a, number b);
fraction    fractionConvertPeriodToF    (const char *period);
void        fractionReduction           (fraction *fn);
void        fractionOut                 (fraction fn);
void        numbernOut                  (number num);
char*       numberToDecimal             (number num);

void deleteRecurringPeriods(char* str, char* period, size_t point_pos, size_t period_pos);

char*   getPeriod               (const char *data);
size_t  calculateDecimalDegree  (const char* data);

void charInsert(char subject[], const char insert[], size_t pos);

int main(void) {
    const size_t MAX_DATA_SIZE = 500;
    char data[MAX_DATA_SIZE];

    memset(data, 0, MAX_DATA_SIZE);
    scanf("%s", data);
    number first_number = numberConstuctor(data);
    //numbernOut(first_number);

    memset(data, 0, MAX_DATA_SIZE);
    scanf("%s", data);
    number second_number = numberConstuctor(data);
    //numbernOut(second_number);

    number result = numberAdding(first_number, second_number);
    //numbernOut(result);
    char* outputresult = numberToDecimal(result);
    printf("%s\n", outputresult);
    free(outputresult);
    return 0;
}

unsigned long long NOD(unsigned long long a, unsigned long long b) {
    if (b == 0)
        return a;
    return NOD(b, a % b);
}

unsigned long long NOK(unsigned long long a, unsigned long long b) {
    return (a * b) / NOD(a, b);
}

number numberConstuctor(const char *data) {

    assert(data != NULL);

    number num = {0, 0, {0, 0}};
    num.integer = atof(data);
    num.exponent = calculateDecimalDegree(data);

    char* period = getPeriod (data);
    assert(period != NULL);

    num.period = fractionConvertPeriodToF(period);

    free(period);
    return num;
}

void fractionReduction(fraction *fn) {
    assert(fn != NULL);

    unsigned long long gcdVal = NOD(fn->numerator, fn->denominator);
    fn->numerator   /= gcdVal;
    fn->denominator /= gcdVal;
}

void fractionOut(fraction fn) {
    printf("%llu / %llu\n", fn.numerator, fn.denominator);
}

void numbernOut(number num) {
    printf("%Lg ", num.integer);
    fractionOut(num.period);
}

number numberAdding(number a, number b)
{
    number output = {a.integer + b.integer, ((a.exponent > b.exponent) ? a.exponent : b.exponent), {0 ,0}};
    if(a.exponent > b.exponent){
        b.period.numerator *= (unsigned long long)pow(10ull, a.exponent - b.exponent);
    }
    else if (a.exponent < b.exponent){
        a.period.numerator *= (unsigned long long)pow(10ull, b.exponent - a.exponent);       
    }

    unsigned long long nok = NOK(a.period.denominator, b.period.denominator);

    a.period.numerator *= nok / a.period.denominator;
    b.period.numerator *= nok / b.period.denominator;
    output.period.numerator = a.period.numerator + b.period.numerator;
    output.period.denominator = nok;
    fractionReduction(&output.period);

    if(output.period.numerator >= output.period.denominator){
        output.integer += ((double)(output.period.numerator / output.period.denominator)) * pow(0.1, output.exponent);
        output.period.numerator %= output.period.denominator;
    }

    return output;
}

fraction fractionConvertPeriodToF(const char *period)
{
    assert(period != NULL);

    size_t periodLength = strlen(period);
    fraction output = {};
    for (size_t i = 0; i < (0 + periodLength); i++)
        output.numerator = output.numerator * 10 + (period[i] - '0');
    for (size_t i = 0; i < periodLength; i++)
        output.denominator = output.denominator * 10 + 9;
    fractionReduction(&output);
    return output;
}

char *getPeriod(const char *str)
{
    assert(str != NULL);

    while(*str != '(' && *str != '\0')
        str++;
    if(*str == '\0')
        return strdup("0");
    /* *str == '(' */
    str++;
    size_t period_size = 0;
    while(str[period_size] != ')' && str[period_size] != '\0')
        period_size++;
    if(str[period_size] == '\0')
        assert(*(str + period_size) != '\0');
    return period_size > 0 ? 
            strndup(str, period_size) :
            strdup("0");
}

size_t calculateDecimalDegree(const char* data) {
    assert(data != NULL);
    while(*data != '.' && *data != '\0')
        data++;
    if(*data == '\0')
        return 0;
    size_t degree = 0;
    while(data[degree] != '(' && data[degree] != '\0')
        degree++;
    /*including '.'*/
    degree--;
    return degree;
}

void charInsert(char subject[], const char insert[], size_t pos)
{
    #define MAX_BUF_SIZE 1000
    static char buf[MAX_BUF_SIZE];
    memset(buf, 0, MAX_BUF_SIZE);

    strncpy(buf, subject, pos);
    size_t len = strlen(buf);
    strcpy(buf + len, insert);
    len += strlen(insert);
    strcpy(buf + len, subject + pos);

    strcpy(subject, buf);
    #undef MAX_BUF_SIZE
}

#define HashMaxSize 200
typedef unsigned long long KeyType;
typedef unsigned long long ValueType;
typedef KeyType (*HashFunc)(KeyType key);
typedef struct KeyValue
{
    KeyType key;
    ValueType value;
    struct KeyValue *next;
} KeyValue;

typedef struct HashTable
{
    struct KeyValue *data[HashMaxSize];
    size_t size;
    HashFunc func;
} HashTable;

void        DestroyNode     (KeyValue*  node                                            );
KeyType     HashFunction    (KeyType    key                                             );
void        _HashDestroy    (KeyValue*  to_destroy                                      );
void        HashDestroy     (HashTable* ht                                              );
KeyValue*   CreateNode      (KeyType    key,        ValueType   value                   );
void        HashInit        (HashTable* ht,         HashFunc    func                    );
void        HashRemove      (HashTable* ht,         KeyType     key                     );
KeyValue*   HashFind        (HashTable* ht,         KeyType     key                     );
void        HashInsert      (HashTable* ht,         KeyType     key,    ValueType value );

KeyType HashFunction(KeyType key) {
    return key % HashMaxSize;
}

void HashInit(HashTable *ht, HashFunc func) {
    if (ht == NULL) {
        return;
    }
    ht->size = 0;
    ht->func = func;
    size_t i = 0;
    for (; i < HashMaxSize; i++) {
        ht->data[i] = NULL;
    }
}

void _HashDestroy(KeyValue *to_destroy) {
    KeyValue *cur = to_destroy->next;
    free(to_destroy);
    if (cur != NULL) {
        _HashDestroy(cur);
    }
}

void HashDestroy(HashTable *ht) {
    if (ht == NULL) {
        return;
    }
    size_t i = 0;
    for (; i < HashMaxSize; i++) {
        if (ht->data[i] != NULL) {
            _HashDestroy(ht->data[i]);
            ht->data[i] = NULL;
        }
    }
}

KeyValue *CreateNode(KeyType key, ValueType value) {
    KeyValue *new_node = (KeyValue *)malloc(sizeof(KeyValue));
    assert(new_node != NULL);

    if (new_node == NULL) {
        return NULL;
    }
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;
    return new_node;
}

void HashInsert(HashTable *ht, KeyType key, ValueType value) {
    if (ht == NULL) {
        return;
    }
    if (ht->size >= HashMaxSize * 10) {
        return;
    }
    size_t offset = (size_t)ht->func(key);
    KeyValue *cur = ht->data[offset];
    while (cur != NULL) {
        if (cur->key == key) {
            return;
        }
        cur = cur->next;
    }
    KeyValue *new_node = CreateNode(key, value);
    new_node->next = ht->data[offset];
    ht->data[offset] = new_node;
    ++ht->size;
}

void DestroyNode(KeyValue *node) {
    assert(node != NULL);
    free(node);
}

void HashRemove(HashTable *ht, KeyType key) {
    if (ht == NULL){
        return;
    }
    size_t offset = (size_t)ht->func(key);
    KeyValue *prev = NULL;
    KeyValue *cur = ht->data[offset];
    while (cur != NULL) {
        if (cur->key == key && prev == NULL) {
            ht->data[offset] = cur->next;
            free(cur);
            return;
        }
        else if (cur->key == key && prev != NULL) {
            prev->next = cur->next;
            DestroyNode(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    return;
}

KeyValue *HashFind(HashTable *ht, KeyType key) {
    if (ht == NULL) {
        return NULL;
    }
    size_t offset = (size_t)ht->func(key);
    KeyValue *cur = ht->data[offset];
    while (cur != NULL) {
        if (cur->key == key) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

void cyclicShift(char* str, size_t size, size_t shift) {
    assert(str != NULL);
    assert(size > 0);
    // Нормализуем shift, чтобы он был в диапазоне [0, size)
    shift = shift % size;
    
    // Выполняем сдвиг
    char temp = 0;
    //printf("BAD str = %s\n", str);
    for (size_t i = 0; i < shift;  i++) {
        temp = str[size - 1];
        
        for (size_t j = size - 1; j > 0; j--) {
            str[j] = str[j - 1];
        }
        
        str[0] = temp;
    }
}

void deleteRecurringPeriods(char* str, char* period, size_t point_pos, size_t period_pos){

	assert(str      != NULL);
	assert(period   != NULL);

	const size_t period_offset = 3; //()\0
    period_pos++;
	size_t period_size = strlen(period);
	size_t str_size = strlen(str);
	size_t str_position = str_size - period_size - period_offset;
	size_t end_period_position = period_pos + period_size - 1;
	size_t readed_size = 0;
    /*
	printf("str 			    = %s\n", str);
	printf("period 			    = %s\n", period);
	printf("period_size 		= %lu\n", period_size);
	printf("str_size 		    = %lu\n", str_size);
	printf("str_position 		= %lu\n", str_position);
	printf("end_period_postion 	= %lu\n", end_period_position);
    */
	for(size_t str_offset = 0; str_position - str_offset > point_pos; str_offset++){
		//printf("str = %c per = %c\n", str[str_position - str_offset], str[end_period_position - (str_offset % period_size)]);
		if(str[str_position - str_offset] != str[end_period_position - (str_offset % period_size)])
			break;
		readed_size++;
	}
    //#warning delete_magic_numbers
	//printf("readed_size = %llu\n", readed_size);
	size_t newperiod_position = str_position - readed_size + 1;
	//printf("first symbol= %c\n", str[newperiod_position]);
	//printf("last  symbol= %c\n", str[newperiod_position + period_size + 1]);
	str[newperiod_position] 			= '(';
	str[newperiod_position + period_size + 1] 	= ')';
	str[newperiod_position + period_size + 2] 	= '\0';
	cyclicShift(period, period_size, readed_size);
	for(size_t str_offset = 0; str_offset < period_size; str_offset++){
		str[newperiod_position + str_offset + 1] = period[str_offset];
	}
	
}

bool lfEqual(long double a, long double b, long double eps){
    return fabsl(a - b) < eps;
}

bool lfWithoutDecimal(long double a, long double eps){
    return lfEqual(a, floorl(a), eps);
}

char *numberToDecimal(number num)
{
    const size_t MAX_RES_SIZE = 2000;
    char* res = (char*)calloc(MAX_RES_SIZE, sizeof(char));
    assert(res != NULL);
    memset(res, 0, MAX_RES_SIZE);

    size_t res_offset = 0;

    unsigned long long n = num.period.numerator;
    unsigned long long d = num.period.denominator;

    if (n % d == 0){
        if(lfWithoutDecimal(num.integer, epsilan))
            num.exponent = 0;
        res_offset += (size_t)sprintf(
                                        res + res_offset, 
                                        "%.*Lf", 
                                        (int)num.exponent, 
                                        num.integer
                                    );
        return res;
    }

    res_offset += (size_t)sprintf(
                                    res + res_offset, 
                                    "%.*Lf", 
                                    (int)num.exponent, 
                                    num.integer
                                );

    if(lfWithoutDecimal(num.integer, epsilan))
        res_offset += (size_t)sprintf(res + res_offset, ".");

    HashTable *mymap = (HashTable *)calloc(1, sizeof(HashTable));
    assert(mymap != NULL);
    HashInit(mymap, HashFunction);

    char* period = NULL;
    size_t period_position = 0;
    for (unsigned long long r = n % d; r; r %= d) {
        if (HashFind(mymap, r) != 0) {

            period_position = (size_t)HashFind(mymap, r)->value;

            charInsert(res, "(", period_position);
            res_offset++;

            period = strndup(
                                res + period_position + 1, 
                                strlen(res + period_position)
                            );
            assert(period != NULL);
            
            res_offset += (size_t)sprintf(res + res_offset, ")");

            break;
        }
        HashRemove(mymap, r);
        HashInsert(mymap, r, (unsigned long long)strlen(res));
        r *= 10;
        res_offset += (size_t)sprintf(res + res_offset, "%llu", r / d);
    }

    deleteRecurringPeriods(res, period, strstr(res, ".") - res, period_position);
    HashDestroy(mymap);
    free(period);
    free(mymap);

    return res;
}