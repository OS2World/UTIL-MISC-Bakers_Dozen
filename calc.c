/* $title:      'CALC.C  =====  Algebraic Calculator  =====' */
/* $subtitle:   'Elipse Ltd. [jms]     Revised: 1992 Jul 31' */
/*               Based upon Advanced C (Schildt), pp253-258  */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define DELIMITER   1
#define VARIABLE    2
#define NUMBER      3
#define NUMVARS    26
#define HEAD        ": CALC. (C) Elipse Ltd.  Jul, 1992"

char tok_type;                          /* One of the above categories */
char token[80];                         /* One item from prog */
char *prog;                             /* The expression to be analyzed */

double vars[NUMVARS];                   /* vars = holding place for variables */


int main(int argc, char *argv[]);
int calc(char *p, char *hd);
double evaluate(char *expr);
double *get_exp(double *result);
void level1(double *result);
void level2(double *result);
void level3(double *result);
void level4(double *result);
void level5(double *result);
void level6(double *result);
void primitive(double *result);
void arith(char o, double *r, double *h);
void unary(char o, double *r);
void putback(void);
double find_var(char *s);
void serror(int error);
void get_token(void);
int isdelim(char c);
int is_in(char ch, char *s);
void clear(void);
void show_help(void);
void read_vars(void);
void save_vars(void);


/* MAIN ===== Starting Point ===== $pageif:6 */

int main(int argc, char *argv[])
    {                                   /* main */
    int i = 1;
    char p[256];
    memset( p, '\0', sizeof(p) );
    memset( vars, '\0', sizeof(vars) );
    read_vars( );

    if ( argc > 1 )
        {                               /* command line calculation */
        for ( i = 1;  i < argc;  ++i )
            {
            strcat( p, argv[i] );
            strcat( p, " " );
            }
        calc( p, HEAD );
        }
    else
        {                               /* multiple line calculations */
        printf( "\n\n%s\n\n", HEAD );
        while ( i )
            {
            printf( ": " );
            fflush( stdout );
            if ( *gets( p ) )
                i = calc( p, " " );
            }
        printf( "* END \n\n" );
        }
    save_vars( );
    return 0;
    }                                   /* main */


/* CALC ===== Body of program ===== $pageif:6 */

int calc(char *p, char *hd)
    {                                   /* calc */
    strupr( p );
    if ( strncmp( p, "END", 3 ) == 0 )
        return 0;
    if ( strncmp( p, "CLEAR", 5 ) == 0 )
        {
        clear( );
        return 1;
        }
    if ( strncmp( p, "HELP", 4 ) == 0 )
        {
        show_help( );
        return 1;
        }
    printf( "  %s  :  %g   %s \n", p, evaluate( p ), hd );
    return 1;
    }                                   /* calc */


/* EVALUATE ===== $pageif:6 */

double evaluate(char *expr)
    {                                   /* evaluate */
    double answer;
    prog = expr;
    answer = 0.0;
    get_exp( &answer );
    vars[NUMVARS-1] = answer;           /* save in last position (Z) */
    return answer;
    }                                   /* evaluate */


/* GET_EXP ===== $pageif:6 */

double *get_exp(double *result)
    {                                   /* get_exp */
    get_token( );
    if ( !*token )
        {
        serror( 2 );
        return result;
        }
    level1( result );
    return result;
    }                                   /* get_exp */


/* LEVEL1 ===== $pageif:6 */

void level1(double *result)
    {                                   /* level1 */
    double hold;
    int slot, ttok_type;
    char temp_token[80];

    if ( tok_type == VARIABLE )         /* save old token */
        {
        strcpy( temp_token, token );
        ttok_type = tok_type;

        slot = *token - 'A';
        get_token( );
        if ( *token != '=' )
            {
            putback( );                 /* return current token */
            strcpy( token, temp_token );/* restore old - not assignment */
            tok_type = ttok_type;
            }
        else
            {
            get_token( );               /* get next part of exp */
            level2( result );
            vars[slot] = *result;       /* save in varaible */
            return;
            }
        }
    level2( result );
    }                                   /* level1 */


/* LEVEL2 ===== $pageif:6 */

void level2(double *result)
    {                                   /* level2 */
    register char op;
    double hold;

    level3( result );
    while ( (op = *token) == '+' || op == '-' )
        {
        get_token( );
        level3( &hold );
        arith( op, result, &hold );
        }
    }                                   /* level2 */


/* LEVEL3 ===== $pageif:6 */

void level3(double *result)
    {                                   /* level3 */
    register char op;
    double hold;

    level4( result );
    while ( (op = *token) == '*' || op == '/' || op == '%' )
        {
        get_token( );
        level4( &hold );
        arith( op, result, &hold );
        }
    }                                   /* level3 */


/* LEVEL4 ===== $pageif:6 */

void level4(double *result)
    {                                   /* level4 */
    double hold;

    level5( result );
    while ( *token == '^' )
        {
        get_token( );
        level4( &hold );
        arith( '^', result, &hold );
        }
    }                                   /* level4 */


/* LEVEL5 ===== $pageif:6 */

void level5(double *result)
    {                                   /* level5 */
    register char op;

    op = 0;
    if ( (tok_type == DELIMITER) && *token == '+' || *token == '-' )
        {
        op = *token;
        get_token( );
        }
    level6( result );
    if ( op )
        unary( op, result );
    }                                   /* level5 */


/* LEVEL6 ===== $pageif:6 */

void level6(double *result)
    {                                   /* level6 */
    if ( (*token == '(') && (tok_type == DELIMITER) )
        {
        get_token( );
        level1( result );
        if ( *token != ')' )
            serror( 1 );
        get_token( );
        }
    else
        primitive( result );
    }                                   /* level6 */


/* PRIMITIVE ===== $pageif:6 */

void primitive(double *result)
    {                                   /* primitive */
    switch ( tok_type)
        {
    case VARIABLE:
        *result = find_var( token );
        get_token( );
        break;
    case NUMBER:
        *result = atof( token );
        get_token( );
        break;
    default:
        serror( 0 );
        break;
        }
    }                                   /* primitive */


/* ARITH ===== $pageif:6 */

void arith(char o, double *r, double *h)
    {                                   /* arith */
    switch ( o)
        {
    case '-':
        *r = *r - *h;
        break;
    case '+':
        *r = *r + *h;
        break;
    case '*':
        *r = *r * *h;
        break;
    case '/':
        if ( *h )
            *r = (*r) / (*h);
        else
            serror( 3 );
        break;
    case '%':                           /* redo using floats */
        break;
    case '^':
        *r = pow( *r, *h );
        break;
        }
    }                                   /* arith */


/* UNARY ===== $pageif:6 */

void unary(char o, double *r)
    {                                   /* unary */
    if ( o == '-' )
        *r = -(*r);
    }                                   /* unary */


/* PUTBACK ===== $pageif:6 */

void putback(void)                          /* return token to prog */
    {                                   /* putback */
    char *t;

    t = token;
    for ( ;*t;  t++ )
        prog--;
    }                                   /* putback */


/* FIND_VAR ===== $pageif:6 */

double find_var(char *s)
    {                                   /* find_var */
    if ( isalpha( *s ) )
        return vars[*token - 'A'];
    serror( 1 );
    return 0.0;
    }                                   /* find_var */


/* SERROR ===== $pageif:6 */

void serror(int error)
    {                                   /* serror */
    const static char *e[] = {"syntax error", "unbalanced parentheses",
          "no expression present", "divide by zero", "undefined variable"};
    printf( "%s\n", e[error] );
    }                                   /* serror */


/* GET_TOKEN ===== $pageif:6 */

void get_token(void)
    {                                   /* get_token */
    register char *temp;
    tok_type = 0;
    temp = token;

    while ( isspace( *prog ) )
        ++prog;                         /* skip white space */
    if ( is_in( *prog, "+-*/%^=()" ) )
        {
        tok_type = DELIMITER;
        *temp++ = *prog++;              /* advance to next position */
        }
    else if ( isalpha( *prog ) )
        {
        while ( !isdelim( *prog ) )
            *temp++ = *prog++;
        tok_type = VARIABLE;
        }
    else if ( isdigit( *prog ) || (*prog == '.') )
        {
        while ( !isdelim( *prog ) )
            *temp++ = *prog++;
        tok_type = NUMBER;
        }
    *temp = 0;
    }                                   /* get_token */


/* ISDELIM ===== $pageif:6 */

int isdelim(char c)
    {                                   /* isdelim */
    if ( is_in( c, "+-*/%^=()" ) || c == 9 || c == '\r' || c == 0 )
        return 1;
    return 0;
    }                                   /* isdelim */


/* IS_IN ===== $pageif:6 */

int is_in(char ch, char *s)
    {                                   /* is_in */
    while ( *s )
        if ( *s++ == ch )
            return 1;
    return 0;
    }                                   /* is_in */


/* CLEAR ===== $pageif:6 */

void clear(void)
    {                                   /* clear */
    int i;
    for ( i = 0;  i < NUMVARS;  ++i )
        vars[i] = 0.0;
    printf( "* CLEAR \n" );
    }                                   /* clear */


/* SHOW_HELP ===== $pageif:6 */

void show_help(void)
    {                                   /* show_help */
    printf( "* HELP \n" );
    printf( "\n  INSTRUCTIONS    %s \n\n", HEAD );
    printf( "  Type any arithmetic expression (with any level of nested brackets)\n" );
    printf( "  The answer will be displayed.\n" );
    printf( "  Answers can be stored in variables (or resigters) named A to Z\n" );
    printf( "  To store result in variable type (variable) = (expression)\n" );
    printf( "  Naturally, an expression can contain variables\n" );
    printf( "  Also, the variable Z is special; ALL results are stored there.\n" );
    printf( "\n  Special Commands: \n" );
    printf( "                    HELP to see this screen \n" );
    printf( "                    CLEAR to zero all variables \n" );
    printf( "                    END to terminate program \n" );
    printf( "\n  Operators allowed: \n" );
    printf( "                    +  Addition \n" );
    printf( "                    -  Subtraction \n" );
    printf( "                    *  Multiplication \n" );
    printf( "                    /  Division \n" );
    printf( "                    ^  Raise to power \n" );
    printf( "\n" );
    }                                   /* show_help */


/* READ_VARS ===== $pageif:6 */

void read_vars(void)
    {                                   /* read_vars */
    char *p, nam[192];
    FILE *fil;
    clear( );
    if ( (p = getenv( "TMP" )) != NULL )
        {
        strcpy( nam, p );
        strcat( nam, "calc.dat" );
        if ( (fil = fopen( nam, "rb" )) != NULL )
            {
            fread( vars, sizeof(double), NUMVARS, fil );
            fclose( fil );
            }
        }
    }                                   /* read_vars */


/* SAVE_VARS ===== $pageif:6 */

void save_vars(void)
    {                                   /* save_vars */
    char *p, nam[192];
    FILE *fil;
    if ( (p = getenv( "TMP" )) != NULL )
        {
        strcpy( nam, p );
        strcat( nam, "calc.dat" );
        if ( (fil = fopen( nam, "wb" )) != NULL )
            {
            fwrite( vars, sizeof(double), NUMVARS, fil );
            fclose( fil );
            }
        }
    }                                   /* save_vars */
