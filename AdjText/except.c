/* $title:      'EXCEPT.C ===== Exceptions for hyphen.c =====' */
/* $subtitle:   'Elipse Ltd.  [jms]      Revised: 1993 Jan 03' */


/*--- Listing 3 --------------------- EXCEPT.C -----------
 * Exception processing for the hyphenation algorithm.
 * Compiler:    Borland C/C++
 * Author:      Allen I. Holub
 *
 * (C) 1988, 1991, Allen I. Holub. All rights reserved.
 *-------------------------------------------------------*/

#define TABSIZE ( sizeof( Hyexcept ) / sizeof(char *) )

#include <stdio.h>
#include <stdlib.h>
#include "hyphen.h"

/* All following words are exceptions to the hyphenation
 * rules. The words in this list will also be recognized if 
 * they are followed by an s or es.
 */

static char *Hyexcept[] =
{
    "ab-sorb-ent",     "ac-cept-able",      "ac-ceptor",
    "ac-cord-ance",    "ac-count-ant",      "ac-know-ledge",
    "adapt-able",      "adapt-er",          "af-firm-a-tive",
    "al-go-rithm",     "an-nouncer",        "ant-acid",
    "ant-eater",       "ant-hill",          "an-tiq-uity",
    "any-thing",       "apart-heid",        "arch-an-gel",
    "arch-ery",        "ar-mi-stice",       "art-ist",
    "askance",         "astig-ma-tism",     "astir",
    "aston-ish-ment",  "at-mos-phere",      "bal-ding",
    "bar-on-ess",      "beach-comber",      "beck-on",
    "bes-tial",        "be-tween",          "bib-li-og-raphy",
    "bind-ery",        "bi-no-mial",        "blast-off",
    "board-er",        "bomb-er",           "bouncer",
    "bound-ary",       "buff-er",           "bull-ish",
    "buzz-er",         "by-stand-er",       "candle-stick",
    "carb-on",         "cast-away",         "cast-off",
    "cat-ion",         "cav-ern-ous",       "cen-ter",
    "cen-tre",
    "change-over",     "child-ish",         "chordal",
    "civ-i-lize",      "class-ify",         "class-room",
    "climb-er",        "clinch-er",
    "coars-en",        "cognac",
    "cole-slaw",       "com-a-tose",        "com-bat-ive",
    "come-back",       "co-me-dian",        "com-men-da-tory",
    "comp-troller",    "com-put-abil-ity",
    "con-cat-e-nate",  "con-cat-e-na-tion",
    "con-de-scend",    "cone-flower",
    "con-form-ity",    "con-sult-ant",
    "con-test-ant",    "con-trol-lable",    "co-nun-drum",
    "con-vert-ible",   "co-star",           "count-ess",
    "court-house",     "court-yard",        "cre-scendo",
    "crest-fallen",    "cross-over",        "crypt-an-alysis",
    "crys-tal-lize",   "curl-i-cue",        "damp-en",
    "damp-est",        "dar-ling",          "debt-or",
    "dec-la-ra-tion",  "de-cre-scendo",     "de-duct-ible",
    "de-form-ity",     "de-gree",           "de-pend-able",
    "de-pend-ent",     "de-scend-ent",      "de-scent",
    "de-test-able",    "di-gest-ible",      "dis-cern-ible",
    "dis-miss-al",     "dis-till-ery",      "dump-ling",
    "earth-en-ware",   "east-ern",          "egg-head",
    "egg-nog",         "eld-est",           "else-where",
    "eq-uable",        "equipped",          "err-ing",
    "es-tab-lish",     "eu-logy",           "eve-ning",
    "every-thing",     "ex-ac-ti-tude",     "ex-ist-ence",
    "ex-pend-able",    "ex-press-ible",     "fall-out",
    "false-hood",      "far-thing",         "fencer",
    "fiend-ish",       "for-eign-er",       "fore-short-en",
    "fore-stall",      "found-ling",        "gen-er-ator",
    "gold-en",         "handle-bar",        "hang-out",
    "hang-over",       "hap-hazard",        "ha-rangue",
    "hard-en",         "hard-ened",         "hard-est",
    "hard-ware",
    "harp-ist",        "haz-ard-ous",       "heart-ache",
    "heart-ily",       "hence-forth",       "her-bal",
    "hogs-head",       "hold-out",          "hold-over",
    "hold-up",
    "idler",           "im-mo-bi-lize",
    "im-pass-able",    "im-per-turb-able",  "inch-worm",
    "in-clem-ent",     "in-con-test-able",  "in-de-pend-ent",
    "in-di-gest-ible", "ineq-uity",         "in-ex-acti-tude",
    "in-ex-haust-ible", "in-form-ant",       "iniq-uity",
    "ink-blot",        "ink-ling",          "inn-keeper",
    "in-sa-tiable",    "in-te-rior",        "in-ter-rupt-ible",
    "ir-re-vers-ible", "jeop-ard-ize",      "kib-itzer",
    "land-owner",      "launch-er",         "left-ist",
    "left-over",       "less-en",           "life-style",
    "lift-off",        "lime-stone",        "li-on-ess",
    "liq-uefy",        "liq-uid",           "liq-ui-date",
    "liq-ui-da-tion",  "liq-uor",           "live-stock",
    "lull-aby",        "lunch-eon",         "lus-cious",
    "main-spring",     "mast-head",         "me-ringue",
    "me-ta-bo-lize",   "met-al",            "mile-stone",
    "mince-meat",      "min-is-ter",        "min-is-try",
    "mo-bi-lize",      "mod-ern-ize",       "mo-nop-o-lize",
    "morgue",          "needle-work",       "neg-li-gible",
    "ne-go-tiable",    "nerv-ous",          "nest-ling",
    "non-con-form-ist", "none-the-less",     "non-ex-ist-ent",
    "non-metal",       "north-east",        "north-ern",
    "nurse-maid",      "nurs-ery",          "ob-serv-able",
    "ob-server",       "off-beat",          "off-hand",
    "off-print",       "off-shoot",         "off-shore",
    "off-spring",      "orange-ade",        "out-land-ish",
    "pal-ate",         "pass-able",         "ped-a-gogy",
    "pent-house",      "per-cent-age",      "pe-ri-odic",
    "per-sist-ent",    "pet-al",            "pho-to-stat",
    "play-thing",      "pleb-i-scite",      "plumb-er",
    "poly-no-mial",    "port-hole",         "post-al",
    "post-hu-mous",    "pre-dict-able",     "pre-req-ui-site",
    "pre-school",      "pre-serv-ative",    "pre-vious",
    "priest-hood",     "prob-abil-ity",     "prob-able",
    "pro-ce-dure",     "pro-gram",          "pro-gram-mer",
    "pro-grams",       "psalm-ist",         "pub-li-ca-tion",
    "pub-lish",        "qua-drille",        "ranch-er",
    "rattle-snake",    "re-corder",         "re-hears-al",
    "rent-al",         "re-place-ment",     "rep-re-sent-a-tive",
    "req-ui-si-tion",  "re-scind",          "re-search-er",
    "re-solv-able",    "re-spect-able",     "re-start-ed",
    "re-state-ment",   "re-store",          "re-vers-ible",
    "re-volv-er",      "roll-away",         "round-about",
    "sap-ling",        "sea-scape",         "self-ish",
    "sell-out",        "send-off",          "sense-less",
    "serv-er",         "serv-ice-able",     "sharpen",
    "shoe-string",     "short-en",          "shy-ster",
    "sib-ling",        "side-step",         "side-swipe",
    "si-lencer",
    "smoke-stack",
    "snake-skin",      "so-ciable",         "soft-hearted",
    "soft-ware",
    "solv-able",       "som-er-sault",      "some-thing",
    "sta-bi-lize",     "stand-ard-ize",     "stand-out",
    "star-ling",       "stat-ure",          "ster-ling",
    "stew-ard-ess",    "stiff-en",          "straight-for-ward",
    "sub-se-quence",
    "sug-gest-ible",   "su-pe-rior",        "surf-er",
    "tan-ta-lize",     "thermo-stat",       "tongue",
    "torque",          "toss-up",           "trench-ant",
    "turn-about",      "turn-over",         "turn-table",
    "ubiq-ui-tous",    "una-nim-ity",       "u-nan-i-mous",
    "un-civ-i-lized",  "un-class-ified",    "un-con-trol-la-ble",
    "unc-tuous",       "un-der-stand-able", "un-err-ing",
    "un-gov-ern-able", "un-pre-dict-able",  "un-search-able",
    "un-so-ciable",    "un-solv-able",      "up-swing",
    "venge-ance",      "vict-ual",          "vignette",
    "volt-age",        "wall-eye",          "waste-bas-ket",
    "waste-land",      "watt-meter",        "weak-ling",
    "west-ern-ize",    "when-ever",         "whisk-er",
    "wors-en",         "yard-age",          "year-ling"
};

/*--------------------------------------------------------*/

static int wcmp( ctype *word, char **tabp )
    {
    /* True is returned if the "word" matches but is followed
     * by an 's' or 'es'. Note that the argument order is 
     * correct, here for the bsearch() that is in the Borland 
     * C/C++ run-time library: The key argument to bsearch() 
     * is passed as the first argument to the comparison 
     * function. You may have to swap the arguments for other 
     * compilers' bsearch()s.
     */

    char  *p;

    for ( p = *tabp;  *word && *p;  word++,  p++ )
        {
        if ( *p == '-' )
            p++;
        if ( CHAR(*word) != *p )
            break;
        }

    if ( !*p && (CHAR(word[0]) == 's' ||
        (CHAR(word[0]) == 'e' && CHAR(word[1]) == 's' ) ) )
        return 0;

    return (CHAR(*word) - *p );
    }

/*--------------------------------------------------------*/

int exception( ctype *word, ctype *end )
    {
    /* Looks up word in the exception list and returns
     * a pointer to the entry if it's there (0 if it
     * isn't). Soft-hyphen points are added to the
     * word before returning.
     */

    char  **pp, *p;
    int    rval = 0;
    ctype  oend = *++end;

    *end = 0;
    pp = (char **) bsearch( word, Hyexcept, TABSIZE, sizeof(char *),
        (int (*)(const void *, const void *)) wcmp );
    if ( pp )
        {
        for ( p = *pp;  *p;  word++,  p++ )
            if ( *p == '-' )
                {
                HYPHENATE(*word );
                p++;
                }
        rval = 1;
        }
    *end = oend;
    return rval;
    }

