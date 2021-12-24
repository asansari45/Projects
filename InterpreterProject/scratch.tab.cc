/* A Bison parser, made by GNU Bison 2.7.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* First part of user declarations.  */

/* Line 279 of lalr1.cc  */
#line 38 "scratch.tab.cc"


#include "scratch.tab.hh"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 46 "scratch.tab.cc"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 46 "scratch.y"

#include <stdio.h>
#include <string.h>
#include "InterpreterDriver.hpp"
#include "InterpreterNode.hpp"

#undef yylex
#define yylex scanner.yylex


/* Line 286 of lalr1.cc  */
#line 61 "scratch.tab.cc"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace yy {
/* Line 353 of lalr1.cc  */
#line 156 "scratch.tab.cc"

  /// Build a parser object.
  InterpreterParser::InterpreterParser (InterpreterScanner& scanner_yyarg, InterpreterDriver& driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {
  }

  InterpreterParser::~InterpreterParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  InterpreterParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  InterpreterParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  InterpreterParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  InterpreterParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  InterpreterParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  InterpreterParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  InterpreterParser::debug_level_type
  InterpreterParser::debug_level () const
  {
    return yydebug_;
  }

  void
  InterpreterParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  inline bool
  InterpreterParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  InterpreterParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  InterpreterParser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
        YYCDEBUG << "Reading a token: ";
        yychar = yylex (&yylval, &yylloc);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 13:
/* Line 670 of lalr1.cc  */
#line 149 "scratch.y"
    {
        (yyval.m_pNode) = (yysemantic_stack_[(3) - (2)].m_pNode);
    }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 205 "scratch.y"
    {
            (yyval.m_pNode) = (yysemantic_stack_[(8) - (4)].m_pNode);
         }
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 210 "scratch.y"
    {
            (yyval.m_pNode) = (yysemantic_stack_[(7) - (6)].m_pNode);
         }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 221 "scratch.y"
    {
            (yyval.m_pNode) = new Interpreter::Node;
         }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 226 "scratch.y"
    {
            (yyval.m_pNode) = new Interpreter::Node;
         }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 232 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 238 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 245 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 250 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 255 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 266 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 275 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 280 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 285 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 290 "scratch.y"
    {
        (yyval.m_pNode) = new Interpreter::Node;
    }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 301 "scratch.y"
    {
        Interpreter::Node* pNode = (yysemantic_stack_[(2) - (1)].m_pNode);
        for (;pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext((yysemantic_stack_[(2) - (2)].m_pNode));
    }
    break;


/* Line 670 of lalr1.cc  */
#line 560 "scratch.tab.cc"
      default:
        break;
      }

    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */
        if (yychar <= yyeof_)
          {
            /* Return failure if at end of input.  */
            if (yychar == yyeof_)
              YYABORT;
          }
        else
          {
            yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
            yychar = yyempty_;
          }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (!yy_pact_value_is_default_ (yyn))
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	  YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
        yypop_ ();
      }

    return yyresult;
    }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  InterpreterParser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char InterpreterParser::yypact_ninf_ = -83;
  const short int
  InterpreterParser::yypact_[] =
  {
       154,   -83,    30,    -7,    34,    36,   -13,    39,   -83,   -83,
      -1,    45,   -83,   386,   -83,   -83,   154,   -83,   -83,   -83,
     -83,   -83,   -83,    24,    48,   347,    30,    30,    38,    58,
     -16,    30,    50,   -19,    30,    30,    30,    30,    30,    30,
      30,    30,    30,    30,   -83,   -83,   -83,   360,   373,    72,
      30,    -3,    66,    68,   386,   -83,   386,    52,   -83,   -83,
      40,   -83,    41,    41,   -83,   -83,   339,   339,   339,   339,
     118,   118,    70,    71,   317,    73,   -83,   294,   -83,   -83,
     -83,   386,   -83,   -83,   287,   154,    80,   287,    75,   386,
     -83,   386,   -83,   -83,   173,   -83,   -83,   -83,   116,    78,
     192,   287,    60,   -83,   -83,   154,   -83,   211,    30,    98,
     -83,    69,   135,   -83,   328,   287,   104,   -83,   -83,   287,
     230,   287,   249,   -83,   268,   -83,   -83
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  InterpreterParser::yydefact_[] =
  {
         0,    25,     0,     0,     0,     0,     0,     0,    54,    55,
      26,     0,     5,     6,     7,     4,     2,    51,     8,    24,
       9,    10,    11,     0,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     1,    13,     0,     0,     0,
       0,     0,     0,     0,    12,    42,    40,     0,    59,    60,
       0,    58,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,     0,     0,     0,     0,    36,     0,    53,    52,
      41,    39,    56,    57,     0,     0,     0,     0,     0,    35,
      29,    30,    31,    28,     0,    32,    33,    34,     0,     0,
       0,     0,    45,    27,    43,     0,    38,     0,     0,     0,
      49,     0,     0,    37,     0,     0,     0,    48,    44,     0,
       0,     0,     0,    46,     0,    50,    47
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  InterpreterParser::yypgoto_[] =
  {
       -83,     4,     0,     1,     3,   -82,   -83,   -83,    74,   -83,
     -83,   -48,   -57,   -83,     5,     6,     9,    21,   -83,   -83,
     -83
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  InterpreterParser::yydefgoto_[] =
  {
        -1,    90,    91,    92,    15,    16,    17,    60,    61,    18,
      77,    93,    94,    19,    95,    96,    97,   110,    23,    57,
     111
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char InterpreterParser::yytable_ninf_ = -1;
  const unsigned char
  InterpreterParser::yytable_[] =
  {
        13,    14,    25,    98,    12,    20,    21,    31,    26,    22,
      52,    58,    59,    75,    32,    53,    13,    14,    29,    44,
      12,    20,    21,   112,    45,    22,    47,    48,    76,    50,
     100,    54,    56,     1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,   107,     2,   103,    36,    37,    27,
      74,    28,   103,     1,    30,     1,    82,    81,   120,   103,
      33,    24,   122,    32,   124,     2,    55,     2,    80,    49,
      58,    59,   103,    51,   103,    86,   103,    89,   108,   109,
      31,    24,    78,    24,    79,    13,    14,   108,   116,    12,
      20,    21,    84,    85,    22,    87,    99,   101,    13,    14,
     105,    44,    12,    20,    21,    13,    14,    22,   114,    12,
      20,    21,    13,    14,    22,    44,    12,    20,    21,     1,
     115,    22,    34,    35,    36,    37,   121,    38,    39,    40,
      41,     2,   117,     3,    83,     0,     4,     5,     1,   104,
       6,     7,     0,     8,     9,     0,     0,    10,    11,     0,
       2,     0,     3,     0,     0,     4,     5,     1,   118,     6,
       7,     0,     8,     9,     0,     0,    10,    11,     0,     2,
       0,     3,     0,     0,     4,     5,     1,     0,     6,     7,
       0,     8,     9,     0,     0,    10,    11,     0,     2,     0,
       3,     0,     0,     4,     5,     1,   102,     0,     7,     0,
       8,     9,     0,     0,    10,    11,     0,     2,     0,     3,
       0,     0,     4,     5,     1,   106,     0,     7,     0,     8,
       9,     0,     0,    10,    11,     0,     2,     0,     3,     0,
       0,     4,     5,     1,   113,     0,     7,     0,     8,     9,
       0,     0,    10,    11,     0,     2,     0,     3,     0,     0,
       4,     5,     1,   123,     0,     7,     0,     8,     9,     0,
       0,    10,    11,     0,     2,     0,     3,     0,     0,     4,
       5,     1,   125,     0,     7,     0,     8,     9,     0,     0,
      10,    11,     0,     2,     0,     3,     0,     0,     4,     5,
       1,   126,     0,     7,     0,     8,     9,     1,     0,    10,
      11,     0,     2,     0,     3,     0,     0,     4,     5,     2,
      88,     0,     7,     0,     8,     9,     0,     0,    10,    11,
       0,    34,    35,    36,    37,    24,    38,    39,    40,    41,
      42,    43,    34,    35,    36,    37,     0,    38,    39,    40,
      41,    42,    43,    34,    35,    36,    37,     0,    49,     0,
     119,    34,    35,    36,    37,     0,    38,    39,    40,    41,
      42,    43,     0,    46,    34,    35,    36,    37,     0,    38,
      39,    40,    41,    42,    43,     0,    72,    34,    35,    36,
      37,     0,    38,    39,    40,    41,    42,    43,     0,    73,
      34,    35,    36,    37,     0,    38,    39,    40,    41,    42,
      43
  };

  /* YYCHECK.  */
  const signed char
  InterpreterParser::yycheck_[] =
  {
         0,     0,     2,    85,     0,     0,     0,     8,    15,     0,
      26,    30,    31,    16,    15,    31,    16,    16,    31,    16,
      16,    16,    16,   105,     0,    16,    26,    27,    31,    28,
      87,    31,    32,     3,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,   101,    15,    94,     6,     7,    15,
      50,    15,   100,     3,    15,     3,    16,    57,   115,   107,
      15,    31,   119,    15,   121,    15,    16,    15,    16,    31,
      30,    31,   120,    15,   122,    74,   124,    77,    18,    19,
       8,    31,    16,    31,    16,    85,    85,    18,    19,    85,
      85,    85,    22,    22,    85,    22,    16,    22,    98,    98,
      22,    98,    98,    98,    98,   105,   105,    98,   108,   105,
     105,   105,   112,   112,   105,   112,   112,   112,   112,     3,
      22,   112,     4,     5,     6,     7,    22,     9,    10,    11,
      12,    15,   111,    17,    60,    -1,    20,    21,     3,    23,
      24,    25,    -1,    27,    28,    -1,    -1,    31,    32,    -1,
      15,    -1,    17,    -1,    -1,    20,    21,     3,    23,    24,
      25,    -1,    27,    28,    -1,    -1,    31,    32,    -1,    15,
      -1,    17,    -1,    -1,    20,    21,     3,    -1,    24,    25,
      -1,    27,    28,    -1,    -1,    31,    32,    -1,    15,    -1,
      17,    -1,    -1,    20,    21,     3,    23,    -1,    25,    -1,
      27,    28,    -1,    -1,    31,    32,    -1,    15,    -1,    17,
      -1,    -1,    20,    21,     3,    23,    -1,    25,    -1,    27,
      28,    -1,    -1,    31,    32,    -1,    15,    -1,    17,    -1,
      -1,    20,    21,     3,    23,    -1,    25,    -1,    27,    28,
      -1,    -1,    31,    32,    -1,    15,    -1,    17,    -1,    -1,
      20,    21,     3,    23,    -1,    25,    -1,    27,    28,    -1,
      -1,    31,    32,    -1,    15,    -1,    17,    -1,    -1,    20,
      21,     3,    23,    -1,    25,    -1,    27,    28,    -1,    -1,
      31,    32,    -1,    15,    -1,    17,    -1,    -1,    20,    21,
       3,    23,    -1,    25,    -1,    27,    28,     3,    -1,    31,
      32,    -1,    15,    -1,    17,    -1,    -1,    20,    21,    15,
      16,    -1,    25,    -1,    27,    28,    -1,    -1,    31,    32,
      -1,     4,     5,     6,     7,    31,     9,    10,    11,    12,
      13,    14,     4,     5,     6,     7,    -1,     9,    10,    11,
      12,    13,    14,     4,     5,     6,     7,    -1,    31,    -1,
      22,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
      13,    14,    -1,    16,     4,     5,     6,     7,    -1,     9,
      10,    11,    12,    13,    14,    -1,    16,     4,     5,     6,
       7,    -1,     9,    10,    11,    12,    13,    14,    -1,    16,
       4,     5,     6,     7,    -1,     9,    10,    11,    12,    13,
      14
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  InterpreterParser::yystos_[] =
  {
         0,     3,    15,    17,    20,    21,    24,    25,    27,    28,
      31,    32,    34,    35,    36,    37,    38,    39,    42,    46,
      47,    48,    49,    51,    31,    35,    15,    15,    15,    31,
      15,     8,    15,    15,     4,     5,     6,     7,     9,    10,
      11,    12,    13,    14,    37,     0,    16,    35,    35,    31,
      36,    15,    26,    31,    35,    16,    35,    52,    30,    31,
      40,    41,    35,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    16,    16,    35,    16,    31,    43,    16,    16,
      16,    35,    16,    41,    22,    22,    36,    22,    16,    35,
      34,    35,    36,    44,    45,    47,    48,    49,    38,    16,
      45,    22,    23,    44,    23,    22,    23,    45,    18,    19,
      50,    53,    38,    23,    35,    22,    19,    50,    23,    22,
      45,    22,    45,    23,    45,    23,    23
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  InterpreterParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  InterpreterParser::yyr1_[] =
  {
         0,    33,    51,    38,    38,    37,    37,    37,    37,    37,
      37,    37,    36,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    45,    45,    44,
      44,    44,    44,    44,    44,    43,    43,    42,    42,    52,
      52,    46,    46,    47,    48,    49,    49,    49,    53,    53,
      50,    34,    34,    34,    34,    34,    39,    40,    40,    41,
      41
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  InterpreterParser::yyr2_[] =
  {
         0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     8,     7,     2,
       1,     4,     3,     7,     9,     7,    11,    12,     2,     1,
       5,     1,     4,     4,     1,     1,     4,     2,     1,     1,
       1
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const InterpreterParser::yytname_[] =
  {
    "$end", "error", "$undefined", "NUM_", "PLUS_", "MINUS_", "MULTIPLY_",
  "DIVIDE_", "EQUALS_", "LES_", "LEQ_", "GRT_", "GEQ_", "DEQ_", "NEQ_",
  "LPAREN_", "RPAREN_", "IF_", "ELIF_", "ELSE_", "WHILE_", "FOR_",
  "LBRACKET_", "RBRACKET_", "FUNCTION_", "LOAD_", "FILENAME_", "HELP_",
  "QUIT_", "FUNCNAME_", "STRING_", "NAME_", "PRINT_", "$accept", "command",
  "expression", "assignment", "line", "lines", "print", "namestring_list",
  "namestring", "funcdef", "param_list", "funcline", "funclines",
  "funccall", "while", "for", "if", "elif", "program", "expression_list",
  "elif_list", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const InterpreterParser::rhs_number_type
  InterpreterParser::yyrhs_[] =
  {
        51,     0,    -1,    38,    -1,    38,    37,    -1,    37,    -1,
      34,    -1,    35,    -1,    36,    -1,    42,    -1,    47,    -1,
      48,    -1,    49,    -1,    31,     8,    35,    -1,    15,    35,
      16,    -1,    35,     4,    35,    -1,    35,     5,    35,    -1,
      35,     6,    35,    -1,    35,     7,    35,    -1,    35,     9,
      35,    -1,    35,    10,    35,    -1,    35,    11,    35,    -1,
      35,    12,    35,    -1,    35,    13,    35,    -1,    35,    14,
      35,    -1,    46,    -1,     3,    -1,    31,    -1,    45,    44,
      -1,    44,    -1,    34,    -1,    35,    -1,    36,    -1,    47,
      -1,    48,    -1,    49,    -1,    43,    35,    -1,    31,    -1,
      24,    31,    15,    43,    16,    22,    45,    23,    -1,    24,
      31,    15,    16,    22,    45,    23,    -1,    52,    35,    -1,
      35,    -1,    31,    15,    52,    16,    -1,    31,    15,    16,
      -1,    20,    15,    35,    16,    22,    38,    23,    -1,    21,
      15,    36,    35,    36,    16,    22,    38,    23,    -1,    17,
      15,    35,    16,    22,    45,    23,    -1,    17,    15,    35,
      16,    22,    45,    23,    19,    22,    45,    23,    -1,    17,
      15,    35,    16,    22,    45,    23,    53,    19,    22,    45,
      23,    -1,    53,    50,    -1,    50,    -1,    18,    35,    22,
      45,    23,    -1,    39,    -1,    25,    15,    31,    16,    -1,
      25,    15,    26,    16,    -1,    27,    -1,    28,    -1,    32,
      15,    40,    16,    -1,    40,    41,    -1,    41,    -1,    30,
      -1,    31,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  InterpreterParser::yyprhs_[] =
  {
         0,     0,     3,     5,     8,    10,    12,    14,    16,    18,
      20,    22,    24,    28,    32,    36,    40,    44,    48,    52,
      56,    60,    64,    68,    72,    74,    76,    78,    81,    83,
      85,    87,    89,    91,    93,    95,    98,   100,   109,   117,
     120,   122,   127,   131,   139,   149,   157,   169,   182,   185,
     187,   193,   195,   200,   205,   207,   209,   214,   217,   219,
     221
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  InterpreterParser::yyrline_[] =
  {
         0,   118,   118,   122,   124,   128,   130,   132,   134,   136,
     138,   140,   144,   148,   153,   155,   157,   159,   161,   163,
     165,   167,   169,   171,   173,   175,   177,   180,   182,   186,
     188,   190,   192,   194,   196,   199,   201,   204,   209,   215,
     217,   220,   225,   231,   237,   244,   249,   254,   260,   262,
     265,   272,   274,   279,   284,   289,   296,   300,   307,   311,
     313
  };

  // Print the state stack on the debug stream.
  void
  InterpreterParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  InterpreterParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  InterpreterParser::token_number_type
  InterpreterParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int InterpreterParser::yyeof_ = 0;
  const int InterpreterParser::yylast_ = 400;
  const int InterpreterParser::yynnts_ = 21;
  const int InterpreterParser::yyempty_ = -2;
  const int InterpreterParser::yyfinal_ = 45;
  const int InterpreterParser::yyterror_ = 1;
  const int InterpreterParser::yyerrcode_ = 256;
  const int InterpreterParser::yyntokens_ = 33;

  const unsigned int InterpreterParser::yyuser_token_number_max_ = 287;
  const InterpreterParser::token_number_type InterpreterParser::yyundef_token_ = 2;


} // yy
/* Line 1141 of lalr1.cc  */
#line 1148 "scratch.tab.cc"
/* Line 1142 of lalr1.cc  */
#line 316 "scratch.y"

// Finally the error member function reports the errors.

void yy::InterpreterParser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
