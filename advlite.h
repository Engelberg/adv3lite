#charset "us-ascii"

/* 
 *   Copyright (c) 2000, 2006 Michael J. Roberts.  All Rights Reserved. 
 *   
 *   TADS 3 Library - main header
 *   
 *   This file provides definitions of macros, properties, and other
 *   identifiers used throughout the library and in game source.
 *   
 *   Each source code file in the library and in a game should generally
 *   #include this header near the top of the source file.  
 */

#ifndef ADV3_H
#define ADV3_H

/* ------------------------------------------------------------------------ */
/*
 *   Include the system headers that we depend upon.  We include these here
 *   so that each game source file will pick up the same set of system
 *   headers in the same order, which is important for intrinsic function
 *   set definitions.  
 */
#include <tads.h>
#include <tok.h>
#include <t3.h>
#include <vector.h>
#include <strbuf.h>
#include <file.h>
#include <dict.h>
#include <bignum.h>
#include <gramprod.h>
#include <strcomp.h>

/* ------------------------------------------------------------------------ */
/*
 *   Turn on sourceTextGroup property generation in the compiler.  (This lets
 *   us determine which module defined an object, and the order of the module
 *   in the overall project build.  We use this for determining the
 *   precedence of certain items based on their definition order in the
 *   source code.)  
 */
#pragma sourceTextGroup(on)
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/*
 *   The '+' property sets an object's location to the lexically preceding
 *   object. 
 */
+ property location;


/*
 *   The VerbRule macro starts the definition of a verb grammar rule.  The
 *   tag is just an identifying name for the rule, so that you can refer to
 *   it with 'replace' or 'modify'.  
 */
#define VerbRule(tag)  grammar predicate(tag):

/*
 *   Verb rule noun phrase macros.  These are convenience macros for
 *   specifying the most common noun phrase slots in the grammar templates
 *   for verb rules.  
 */
#define singleDobj     singleNoun->dobjMatch
#define singleIobj     singleNoun->iobjMatch
#define singleAcc      singleNoun->accMatch
#define multiDobj      nounList->dobjMatch
#define multiIobj      nounList->iobjMatch
#define accList        nounList->accMatch
#define numberDobj     numberPhrase->dobjMatch
#define topicDobj      topicPhrase->dobjMatch
#define topicIobj      topicPhrase->iobjMatch
#define topicAcc       topicPhrase->accMatch
#define literalDobj    literalPhrase->dobjMatch
#define literalIobj    literalPhrase->iobjMatch
#define literalAcc     literalPhrase->accMatch
#define singleDir      directionName->dirMatch


/* ------------------------------------------------------------------------ */
/*
 *   Establish the default dictionary for the game's player command parser
 *   vocabulary.
 */
dictionary cmdDict;

/*
 *   The standard parts of speech for the dictionary.
 */
dictionary property noun, nounApostS;


/* ------------------------------------------------------------------------ */
/*
 *   Flags for matching noun phrases during parsing.  These are bit flags, so
 *   they can be combined with '|' (bitwise OR).
 *   
 *   Note that the arithmetic values also matter: the values are in order of
 *   priority for noun phrase matches.  That is, a higher arithmetic value is
 *   a better match.  The best match is one with no truncation and no
 *   approximation, so that will have the highest arithmetic value when you
 *   combine those two bit flags.  The next best is character approximation
 *   but no truncation - truncation is more important than approximation, so
 *   it has a higher arithmetic value.  
 */

/* matched a preposition (the phrase contains at least one preposition) */
#define MatchPrep      0x0001

/* matched an adjective (the phrase contains at least one adjective) */
#define MatchAdj       0x0002

/* matched a noun (the phrase contains at least one noun word) */
#define MatchNoun      0x0004

/* matched a plural (the phrase contains at least one plural word) */
#define MatchPlural    0x0008

/* mask to select only the part-of-speech flags */
#define MatchPartMask  0x0FFF


/* mask to select only the part-of-speech flags */
#define MatchPartMask  0x0FFF

/* 
 *   all words were matched WITHOUT character approximations (such as
 *   matching 'a' to 'a-umlaut') 
 */
#define MatchNoApprox  0x1000

/* all words were matched WITHOUT truncation */
#define MatchNoTrunc   0x2000

/* mask to select only the match-strength flags */
#define MatchStrengthMask  0xF000

/* ------------------------------------------------------------------------ */
/*
 *   Flags for object selection during parsing.  These flags reflect how well
 *   an object matched a noun phrase during the resolution process.  
 */

/* 
 *   The noun phrase required disambiguation, because more than one object
 *   was in scope that matched the noun phrase.  We were able to figure out
 *   which one(s) the player meant from context, without having to ask the
 *   player for help.
 *   
 *   For example, there are two doors in the room, one open and one closed.
 *   The player types OPEN DOOR.  It's fairly obvious that they must be
 *   talking about the closed door.  So, we choose that object and set the
 *   Disambig flag.
 *   
 *   (Note that this flag specifically does NOT mean that we had to ask the
 *   user for help with the dreaded "Which do you mean..." question.  It's
 *   kind of the opposite: it means that we had a noun phrase that was
 *   initially ambiguous, but that we managed to disambiguate it on our own.
 *   When we get the user involved, there's ambiguity *before* we ask the
 *   question, but the user's response removes the ambiguity by telling us
 *   exactly which alternative they intended.  This flag indicates that we
 *   made an educated guess about what the user must have intended, without
 *   asking.  The flag lets the parser tell the player about the guess, which
 *   is desirable because the guess is sometimes wrong.  
 */
#define SelDisambig         0x0001

/*
 *   This object was chosen arbitrarily from a larger set, because the noun
 *   phrase construction says we should do so.  This flag is set when the
 *   noun phrase is something TAKE A BOOK or TAKE ANY OF THE BOOKS.  
 */
#define SelArbitrary        0x0002

/*
 *   The noun phrase that we matched was a manifestly plural construction,
 *   such as TAKE ALL or TAKE THE BOOKS.  
 */
#define SelPlural           0x0004

/*
 *   We selected an object as a default.  This is set when the player leaves
 *   out a noun phrase, but we can guess what was probably meant based on
 *   context: e.g., ASK ABOUT THE HOUSE is probably directed to Bob if Bob is
 *   the only person nearby.  
 */
#define SelDefault          0x0008

/*
 *   This object was set internally by the program; it did not come from
 *   parsing any player input.  This generally means that the whole command
 *   was constructed by the program, to handle an event or other internal
 *   processing, rather than by parsing player input.  
 */
#define SelProg             0x1000


/* ------------------------------------------------------------------------ */
/* 
 *   A couple of utility macros we use internally for turning macro values
 *   into strings.  STRINGIZE(x) expands any macros in its argument and then
 *   turns the result into a single-quoted string, which can then be used in
 *   regular program text or in directives that evaluate constant
 *   expressions, such as #if.  (STRINGIZE is the real macro; _STRINGIZE is
 *   needed to force expansion of any macros in the argument, which is
 *   required because of the weird ANSI C expansion-order rules, and which
 *   works because of same.)  
 */
#define _STRINGIZE(x) #@x
#define STRINGIZE(x)  _STRINGIZE(x)




/* ------------------------------------------------------------------------ */
/*
 *   Parser global variables giving information on the command currently
 *   being performed.  These are valid through doAction processing.  These
 *   should never be changed except by the parser.
 */

/* the actor performing the current command */
#define gActor (libGlobal.curActor)

/*
 *   For convenience, define some macros that return the current direct and
 *   indirect objects from the current action.  The library only uses direct
 *   and indirect objects, so games that define additional command objects
 *   will have to add their own similar macros for those.  
 */
#define gDobj (gAction.getDobj())
#define gIobj (gAction.getIobj())
#define gLiteral (gAction.literal)


/* the Action object of the command being executed */
#define gAction (libGlobal.curAction)
#define gCommand (libGlobal.curCommand)

/*
 *   Determine if the current global action is the specified action.  Only
 *   the action prefix is needed - so use "Take" rather than "TakeAction"
 *   here.
 *   
 *   This tests to see if the current global action is an instance of the
 *   given action class - we test that it's an instance rather than the
 *   action class itself because the parser creates an instance of the
 *   action when it matches the action's syntax.  
 */
#define gActionIs(action) \
    (gAction != nil && gAction.ofKind(action))

/* is the current global action ANY of the specified actions? */
#define gActionIn(action...) \
    (gAction != nil \
     && (action#foreach/gAction.ofKind(action)/||/))

/* is the current action a Travel action going dirn */
#define gTravelActionIs(dirn) \
  (gAction != nil && gAction.ofKind(Travel) && gAction.direction == dirn ## Dir)

/* the list of objects involved in the action just completed */
#define gActionList (nilToList(gCommand.action.actionList))

/* a displaying string version of the above */
#define gActionListStr makeListStr(gCommand.action.reportList, &theName)

/* ------------------------------------------------------------------------ */
/*
 *   Miscellaneous macros
 */

/* get the current player character Actor object */
#define gPlayerChar (libGlobal.playerChar)

#define objForCheck(which, action)

#define objFor(which, action) \
    objForCheck(which, action) \
    propertyset '*' ## #@which ## #@action

#define dobjFor(action) objFor(Dobj, action)
#define iobjFor(action) objFor(Iobj, action)

#define reportAfter(msg) gCommand.afterReports += msg

/*
 *   Treat an object definition as equivalent to another object definition.
 *   These can be used immediately after a dobjFor() or iobjFor() to treat
 *   the first action as though it were the second.  So, if the player types
 *   "search box", and we want to treat the direct object the same as for
 *   "look in box", we could make this definition for the box:
 *   
 *   dobjFor(Search) asDobjFor(LookIn)
 *   
 *   Note that no semicolon is needed after this definition, and that this
 *   definition is completely in lieu of a regular property set for the
 *   object action.
 *   
 *   In general, a mapping should NOT change the role of an object:
 *   dobjFor(X) should not usually be mapped using asIobjFor(Y), and
 *   iobjFor(X) shouldn't be mapped using asDobjFor(Y).  The problem with
 *   changing the role is that the handler routines often assume that the
 *   object is actually in the role for which the handler was written; a
 *   verify handler might refer to '{dobj}' in generating a message, for
 *   example, so reversing the roles would give the wrong object in the role.
 *   
 *   Role reversals should always be avoided, but can be used if necessary
 *   under conditions where all of the code involved in the TARGET of the
 *   mapping can be carefully controlled to ensure that it doesn't make
 *   assumptions about object roles, but only references 'self'.  Reversing
 *   roles in a mapping should never be attempted in general-purpose library
 *   code, because code based on the library could override the target of the
 *   role-reversing mapping, and the override could fail to observe the
 *   restrictions on object role references.
 *   
 *   Note that role reversals can almost always be handled with other
 *   mechanisms that handle reversals cleanly.  Always consider remapTo()
 *   first when confronted with a situation that seems to call for a
 *   role-reversing asObjFor() mapping, as remapTo() specifically allows for
 *   object role changes.  
 */
#define asObjFor(obj, Action) \
    { \
        preCond { return preCond##obj##Action; } \
        verify() { verify##obj##Action; } \
        remap() { return remap##obj##Action; } \
        check() { check##obj##Action; } \
        action() { action##obj##Action; } \
        report() { report##obj##Action; } \
    }

#define asDobjFor(action) asObjFor(Dobj, action)
#define asIobjFor(action) asObjFor(Iobj, action)

/* 
 *   Define mappings of everything except the action.  This can be used in
 *   cases where we want to pick up the verification, preconditions, and
 *   check routines from another handler, but not the action.  This is often
 *   useful for two-object verbs where the action processing is entirely
 *   provided by one or the other object, so applying it to both would be
 *   redundant.  
 */
#define asObjWithoutActionFor(obj, Action) \
    { \
        preCond { return preCond##obj##Action; } \
        verify() { verify##obj##Action; } \
        remap() { return remap##obj##Action(); } \
        check() { check##obj##Action; } \
        action() {  } \
    }

#define asDobjWithoutActionFor(action) asObjWithoutActionFor(Dobj, action)
#define asIobjWithoutActionFor(action) asObjWithoutActionFor(Iobj, action)

/*
 *   "Remap" an action.  This effectively rewrites the action in the given
 *   form.  Each of the object slots can be filled either with a specific
 *   object, or with a noun phrase role name (DirectObject, IndirectObject);
 *   in the latter case, the object or objects from the named noun phrase
 *   role in the *current* action (i.e., before the rewrite) will be used.
 *   
 *   If the new action has two or more objects (for example, if it's a
 *   TIAction), then EXACTLY ONE of the slots must be filled with a specific
 *   object, and all of the other slots must be filled with role names.  The
 *   specific object is the one that corresponds to the original object
 *   that's doing the remapping in the first place - this can simply be
 *   'self' if the new action will operate on the same object, or it can be
 *   a different object.  The important thing is that the 'verify' method
 *   for the defining object will be forwarded to the corresponding 'verify'
 *   method on the corresponding object for the new action.
 *   
 *   This macro must be used as the ENTIRE definition block for a dobjFor()
 *   or iobjFor().  For example, to remap a "put in" command directed to a
 *   desk so that the command is instead applied to a drawer in the desk, we
 *   could define the following on the desk object:
 *   
 *   iobjFor(PutIn) remapTo(PutIn, DirectObject, deskDrawer) 
 */
#define remapTo(action, objs...) { remap = [action, ##objs] }

/*
 *   Conditionally remap an action.  If 'cond' (a conditional expression)
 *   evaluated to true, we'll remap the action as directed; otherwise, we'll
 *   inherit the default handling 
 */
#define maybeRemapTo(cond, action, objs...) \
    { remap = ((cond) ? [action, ##objs] : inherited()) }


/*
 *   For two-object push-travel actions, such as "push sled into cave",
 *   define a special mapping for both the direct and indirect objects:
 *   
 *   - Map the direct object (the object being pushed) to a simple
 *   PushTravel action.  So, for "push sled into cave," map the direct
 *   object handling to PushTravel for the sled.  This makes the handling of
 *   the command equivalent to "push sled north" and the like.
 *   
 *   - Map the indirect object (the travel connector) to use the PushTravel
 *   action's verify remapper.  This is handled specially by the PushTravel
 *   action object to handle the verification as though it were verifying
 *   the corresponding ordinary (non-push) travel action on the indirect
 *   object.  Beyond verification, we do nothing, since the direct object of
 *   a pushable object will handle the whole action using a nested travel
 *   action.
 *   
 *   This effectively decomposes the two-object action into two coupled
 *   single-object actions: a regular PushTravel action on the object being
 *   pushed, and a regular whatever-kind-of-travel on the connector being
 *   traversed.  This handling has the appeal that it means that we don't
 *   need a separate PUSH version of every kind of allowed travel on a
 *   connector, and we don't need a special handler version for each kind of
 *   travel on a pushable object; instead, we just use the basic PushTravel
 *   and kind-of-travel handlers to form the combined form.  Note that this
 *   still allows separate treatment of the combined form wherever desired,
 *   just by overriding these default handlers for the two-object action.  
 */
#define mapPushTravelHandlers(pushAction, travelAction) \
    dobjFor(pushAction) asDobjFor(PushTravel) \
    mapPushTravelIobj(pushAction, travelAction)

#define mapPushTravelIobj(pushAction, travelAction) \
    iobjFor(pushAction) \
    { \
        verify() \
            { gAction.verifyPushTravelIobj(self, travelAction##Action); } \
    }


#define asExit(dir) : UnlistedProxyConnector { direction = dir##Dir }

/* ------------------------------------------------------------------------ */
/*
 *   Define an action with the given base class.  This adds the *Action
 *   suffix to the given root name, and defines a class with the given base
 *   class.  We also define the baseActionClass property to refer to myself;
 *   this is the canonical class representing the action for all subclasses.
 *   This information is useful because a language module might define
 *   several grammar rule subclasses for the given class; this lets us
 *   relate any instances of those various subclasses back to this same
 *   canonical class for the action if necessary.  
 */
//#define DefineAction(name, baseClass...) \
//    class name##Action: ##baseClass \
//    baseActionClass = name##Action
//


/* 
 *   Define an action OBJECT with the given name inheriting from the given base
 *   class, for use with the Mercury parser.
 */

#define DefineAction(name, baseClass...) \
    name: ##baseClass 


/*
 *   Define a "system" action.  System actions are meta-game commands, such
 *   as SAVE and QUIT, that generally operate the user interface and are not
 *   part of the game world.  
 */
#define DefineSystemAction(name) \
    DefineAction(name, SystemAction)

/*
 *   Define a concrete IAction, given the root name for the action.  We'll
 *   automatically generate a class with name XxxAction. 
 */
#define DefineIAction(name) \
    DefineAction(name, IAction)

/*
 *   Define a concrete TAction, given the root name for the action.  We'll
 *   automatically generate a class with name XxxAction, a verProp with name
 *   verXxx, a checkProp with name checkXxx, and an actionProp with name
 *   actionDobjXxx.  
 */
#define DefineTAction(name) \
    DefineTActionSub(name, TAction)

/*
 *   Define a concrete TAction with a specific base class.  
 */
#define DefineTActionSub(name, cls) \
    DefineAction(name, cls) \
    verDobjProp = &verifyDobj##name \
    remapDobjProp = &remapDobj##name \
    preCondDobjProp = &preCondDobj##name \
    checkDobjProp = &checkDobj##name \
    actionDobjProp  = &actionDobj##name \
    reportDobjProp = &reportDobj##name \

#define DefineLiteralTAction(name)\
    DefineTActionSub(name, LiteralTAction)

#define DefineLiteralAction(name)\
    DefineAction(name, LiteralAction)

#define DefineTopicTAction(name)\
    DefineTActionSub(name, TopicTAction)

#define DefineTopicAction(name)\
    DefineAction(name, TopicAction)

/*
 *   Define a concrete TIAction, given the root name for the action.  We'll
 *   automatically generate a class with name XxxAction, a verDobjProp with
 *   name verDobjXxx, a verIobjProp with name verIobjxxx, a checkDobjProp
 *   with name checkDobjXxx, a checkIobjProp with name checkIobjXxx, an
 *   actionDobjProp with name actionDobjXxx, and an actionIobjProp with name
 *   actionIobjXxx.  
 */
#define DefineTIAction(name) \
    DefineTIActionSub(name, TIAction)

/*
 *   Define a concrete TIAction with a specific base class.  
 */
#define DefineTIActionSub(name, cls) \
    DefineAction(name, cls) \
    verDobjProp = &verifyDobj##name \
    verIobjProp = &verifyIobj##name \
    remapDobjProp = &remapDobj##name \
    remapIobjProp = &remapIobj##name \
    preCondDobjProp = &preCondDobj##name \
    preCondIobjProp = &preCondIobj##name \
    checkDobjProp = &checkDobj##name \
    checkIobjProp = &checkIobj##name \
    actionDobjProp  = &actionDobj##name \
    actionIobjProp = &actionIobj##name \
    reportDobjProp = &reportDobj##name \
    reportIobjProp = &reportIobj##name \

#define gVerifyList = gAction.verifyList

#define logical gAction.addVerifyResult (new VerifyResult(100, '', true, self))
    
#define illogical(msg) \
    gAction.addVerifyResult(new VerifyResult(30, msg, nil, self))

#define illogicalNow(msg) \
    gAction.addVerifyResult(new VerifyResult(40, msg, nil, self))

#define illogicalSelf(msg) \
    gAction.addVerifyResult(new VerifyResult(20, msg, nil, self))

#define logicalRank(score) \
    gAction.addVerifyResult(new VerifyResult(score, '', true, self))

#define inaccessible(msg) \
    gAction.addVerifyResult(new VerifyResult(10, msg, nil, self))

#define implausible(msg) \
    gAction.addVerifyResult(new VerifyResult(35, msg, nil, self))

/* ------------------------------------------------------------------------ */
/*
 *   Command interruption signal macros.  
 */

/* a concise macro to throw an ExitSignal */
#define exit throw new ExitSignal()

/* a concise macro to throw an ExitActionSignal */
#define exitAction throw new ExitActionSignal()

/* a concise macro to throw an AbortImplicitSignal */
#define abortImplicit throw new AbortImplicitSignal()

/* a concise macro to throw an Abort signal */

#define abort throw new AbortActionSignal()


/* ------------------------------------------------------------------------ */
/*
 *   Try performing a command implicitly.  The action is the root name of
 *   the action, without the 'Action' suffix - we'll automatically add the
 *   suffix.  'objs' is a varying-length list of the resolved objects in the
 *   new action - the direct object, indirect object, and any others needed
 *   for the action.  
 */
//#define tryImplicitAction(action, objs...) \
//    _tryImplicitAction(action##Action, ##objs)

#define tryImplicitAction(action, objs...) \
    _tryImplicitAction(action, ##objs)

/*
 *   Replace the current action with a new action.  The new action will be
 *   performed, and the original action will be terminated with 'exit'.
 *   
 *   'action' is the root name of the action, without the 'Action' suffix
 *   (we'll add the suffix automatically).  'objs' is a varying-length list
 *   of the resolved objects - direct object, indirect object, etc.  
 */
//#define replaceAction(action, objs...) \
//    _replaceAction(gActor, action##Action, ##objs)

#define replaceAction(action, objs...) \
    _replaceAction(gActor, action, ##objs)

/*
 *   Run a nested action.
 */
//#define nestedAction(action, objs...) \
//    _nestedAction(nil, gActor, action##Action, ##objs)

#define nestedAction(action, objs...) \
    _nestedAction(nil, gActor, action, ##objs)


/* ------------------------------------------------------------------------ */
/*
 *   aHref() flags 
 */
#define AHREF_Plain  0x0001    /* plain text hyperlink (no underline/color) */







/* ------------------------------------------------------------------------ */
/*
 *   An achievement defines its descriptive text.  It can also optionally
 *   define the number of points it awards.  
 */
Achievement template +points? "desc";



/* ------------------------------------------------------------------------ */
/*
 *   Templates for style tags 
 */
StyleTag template 'tagName' 'openText'? 'closeText'?;

/* ------------------------------------------------------------------------ */
/*
 *   Object definition templates 
 */

Thing template 'vocab' @location? "desc"?;
Topic template 'vocab' @familiar?;

Room template 'roomTitle' 'vocab' "desc"?;
Room template 'roomTitle' "desc"?;

TopicEntry template
   +matchScore?
   @matchObj | [matchObj] | 'matchPattern'
   "topicResponse" | [eventList] ?;

/* a ShuffledEventList version of the above */
TopicEntry template
   +matchScore?
   @matchObj | [matchObj] | 'matchPattern'
   [firstEvents] [eventList];

/* we can also include *both* the match object/list *and* pattern */
TopicEntry template
   +matchScore?
   @matchObj | [matchObj]
   'matchPattern'
   "topicResponse" | [eventList] ?;

/* a ShuffledEventList version of the above */
TopicEntry template
   +matchScore?
   @matchObj | [matchObj]
   'matchPattern'
   [firstEvents] [eventList]; 

QueryTopic template
   +matchScore? 'qtype'
   @matchObj | [matchObj] | 'matchPattern'
   "topicResponse" | [eventList] ?;

/* a ShuffledEventList version of the above */
QueryTopic template
   +matchScore? 'qtype'
   @matchObj | [matchObj] | 'matchPattern'
   [firstEvents] [eventList];

/* we can also include *both* the match object/list *and* pattern */
QueryTopic template
   +matchScore? 'qtype'
   @matchObj | [matchObj]
   'matchPattern'
   "topicResponse" | [eventList] ?;

/* a ShuffledEventList version of the above */
QueryTopic template
   +matchScore? 'qtype'
   @matchObj | [matchObj]
   'matchPattern'
   [firstEvents] [eventList];


DefaultTopic template "topicResponse" | [eventList];
DefaultConsultTopic template "topicResponse" | [eventList];
DefaultThought template "topicResponse" | [eventList];

/* miscellanous topics just specify the response text or list */
MiscTopic template "topicResponse" | [eventList];
MiscTopic template [firstEvents] [eventList];
NodeContinuationTopic template "topicResponse" | [eventList];
NodeContinuationTopic template [firstEvents] [eventList];

Doer template 'cmd';



/* ------------------------------------------------------------------------ */
/*
 *   Command interruption signal macros.  
 */

/*
 *   Terminate execution of the command line.  This aborts the current
 *   command, including any remaining object iterations for the current
 *   action, and discards anything else on the command line.  
 */
#define exitCommandLine  throw new ExitCommandLineSignal()
/*----------------------------------------------------------------------------*/
/*
 *   enums for different types of lock:
 */    
   
enum notLockable, lockableWithoutKey, lockableWithKey, indirectLockable;

enum masculine, feminine, neuter;

/* ------------------------------------------------------------------------ */
/*
 *   The current library messages object.  This is the source object for
 *   messages that don't logically relate to the actor carrying out the
 *   comamand.  It's mostly used for meta-command replies, and for text
 *   fragments that are used to construct descriptions.
 *   
 *   This message object isn't generally used for parser messages or action
 *   replies - most of those come from the objects given by the current
 *   actor's getParserMessageObj() or getActionMessageObj(), respectively.
 *   
 *   By default, this is set to libMessages.  The library never changes this
 *   itself, but a game can change this if it wants to switch to a new set of
 *   messages during a game.  (If you don't need to change messages during a
 *   game, but simply want to customize some of the default messages, you
 *   don't need to set this variable - you can simply use 'modify
 *   libMessages' instead.  This variable is designed for cases where you
 *   want to *dynamically* change the standard messages during the game.)  
 */
#define gLibMessages (libGlobal.libMessageObj)

/* 
 *   the exit lister object - if the exits module isn't included in the
 *   game, this will be nil 
 */
#define gExitLister (libGlobal.exitListerObj)

/*
 *   the hint manager object - if the hints module isn't included in the
 *   game, this will be nil 
 */
#define gHintManager (libGlobal.hintManagerObj)


/* ------------------------------------------------------------------------ */
/*
 *   Convenience macros for controlling the narrative tense.
 */

/*
 *   Set the current narrative tense.  Use val = true for past and
 *   val = nil for present.
 */
#define setPastTense(val) (gameMain.usePastTense = (val))

/*
 *   Shorthand macro for selecting one of two values depending on the
 *   current narrative tense.
 */
#define tSel(presVal, pastVal) \
    (gameMain.usePastTense ? (pastVal) : (presVal))

/*
 *   Temporarily override the current narrative tense and invoke a callback
 *   function.
 */
#define withPresent(callback) (withTense(nil, (callback)))
#define withPast(callback)    (withTense(true, (callback)))

#define gInPrep (gLibMessages.inPrep)
#define gOnPrep (gLibMessages.onPrep)
#define gUnderPrep (gLibMessages.underPrep)
#define gBehindPrep (gLibMessages.behindPrep)

/* ------------------------------------------------------------------------ */
/*
 *   Object role identifiers.  These are used to identify the role of a noun
 *   phrase in a command.
 *   
 *   The library provides base classes for actions of zero, one, and two noun
 *   phrases in their grammars: "look", "take book", "put book on shelf".  We
 *   thus define role identifiers for direct and indirect objects.  Note that
 *   even though we stop there, this doesn't preclude games or library
 *   extensions from adding actions that take more than two noun phrases
 *   ("put coin in slot with tongs"); any such extensions must simply define
 *   their own additional role identifiers for the third or fourth (etc) noun
 *   phrase.
 */
//enum ActorObject, DirectObject, IndirectObject;

/*
 *   A special role for the "other" object of a two-object command.  This
 *   can be used in certain contexts (such as remapTo) where a particular
 *   object role is implied by the context, and where the action involved
 *   has exactly two objects; OtherObject in such contexts means
 *   DirectObject when the implied role is IndirectObject, and vice versa. 
 */
enum OtherObject;


/* ------------------------------------------------------------------------ */
/* 
 *   A couple of utility macros we use internally for turning macro values
 *   into strings.  STRINGIZE(x) expands any macros in its argument and then
 *   turns the result into a single-quoted string, which can then be used in
 *   regular program text or in directives that evaluate constant
 *   expressions, such as #if.  (STRINGIZE is the real macro; _STRINGIZE is
 *   needed to force expansion of any macros in the argument, which is
 *   required because of the weird ANSI C expansion-order rules, and which
 *   works because of same.)  
 */
#define _STRINGIZE(x) #@x
#define STRINGIZE(x)  _STRINGIZE(x)


/* ------------------------------------------------------------------------ */
/*
 *   Msg() - define a custom message to override a library message.  'id' is
 *   the message ID, which is the same ID used for the DMsg() message that
 *   you wish to override.  Do NOT use quotes around the ID - just enter it
 *   as though it were a variable name.  'txt' is the message text, as a
 *   single-quoted string.  
 *   
 *   This is used in CustomMessages objects to define message overrides.  See
 *   CustomMessages for full details.  
 */
#define Msg(id, txt)  #@id, txt



/* ------------------------------------------------------------------------ */
/*
 *   DMsg() - default English library message cover macro.
 *   
 *   Whenever the library displays a message, it uses the DMsg() macro.  The
 *   arguments are a message ID, and the default English message text to
 *   display.  The message ID is a string that identifies the message; this
 *   is used to look for overriding customizations of the message.  Refer to
 *   the CustomMessages class for information on customizing the standard
 *   library messages.
 *   
 *   In our approach, the library defines the default English text of the
 *   messages in-line, directly in the code.  On the surface, this is
 *   contrary to standard practices in most modern programming projects,
 *   which strive to make translations easier by separating the message text
 *   from the program code, gathering all of the text into a central message
 *   file that can be replaced for each language.  Despite appearances, we're
 *   accomplishing the same thing - but in our system, we have the advantage
 *   that we *also* define the default English message text in-line as part
 *   of the code it applies to.  This makes it easier to read the code by
 *   keeping a message and its full context in one place; this way you don't
 *   have to shuttle between the code and message file.
 *   
 *   Here's how we accomplish the message separation required for
 *   translations, and also for games that wish to customize the library
 *   defaults.  The DMsg() macro requires both the default English message
 *   text *and* an ID key for the message.  The message display function
 *   receives both.  The display function proceeds to look up the ID key in a
 *   translation table; if it finds an entry, it uses the version of the
 *   message in the translation table instead of the English default passed
 *   in via DMsg().  A language module can provide a message table that
 *   defines the language translations, and a game can provide a table that
 *   further customizes the library messages to fit its narrative style.
 *   
 *   There's one additional element.  Translators and game authors need to be
 *   able to see all of the messages in one place, so they can create their
 *   tables.  We would seem to lack that central list of English messages.
 *   Fortunately, by using a standard macro for each message, we can extract
 *   a comprehensive English message list automatically via a special tool.
 *   We use this as part of the library release process to create the English
 *   message file for reference.
 *   
 *   Note that the macro expansion includes the default English text only in
 *   English builds.  It omits the text in non-English builds.  This is to
 *   save space - we assume that the English messages will all be overridden
 *   anyway by each translated library version, so there's no point in
 *   including their text in the final compiled program.  
 */
#if STRINGIZE(LANGUAGE) == 'english'
#define DMsg(id, txt, args...)  message(#@id, txt, ##args)
#define BMsg(id, txt, args...)  buildMessage(#@id, txt, ##args)
#else
#define DMsg(id, txt, args...)  message(#@id, nil, ##args)
#define BMsg(id, txt, args...)  buildMessage(#@id, nil, ##args)
#endif

/* ------------------------------------------------------------------------ */
/*
 *   Debugging.  When we compile in development mode, we'll include a number
 *   of functions and methods that display information for debugging
 *   purposes.  We omit these in release builds to keep the compiled file
 *   size smaller, and to avoid making it too easy for end users to
 *   snoop around in the program internals.
 */
#ifdef __DEBUG
# define IfDebug(key, code) \
    if (DebugCtl.enabled[#@key]) { code; } else { }
#else
# define IfDebug(key, code)
#endif

#define gOutStream (outputManager.curOutputStream)

#ifdef __DEBUG
#include <dynfunc.h>
#endif

/*
 *   Some message processors add their own special parameters to messages,
 *   because they want to use expansion parameters (in the "{the dobj/him}"
 *   format) outside of the set of objects directly involved in the command.
 *   
 *   The Action method setMessageParam() lets you define such a parameter,
 *   but for convenience, we define this macro for setting one or more
 *   parameters whose names exactly match their local variable names.  In
 *   other words, if you call this macro like this:
 *   
 *   gMessageParams(obj, cont)
 *   
 *   then you'll get one parameter with the text name 'obj' whose expansion
 *   will be the value of the local variable obj, and another with text name
 *   'cont' whose expansion is the value of the local variable cont.  
 */
#define gMessageParams(var...) \
    (gAction.setMessageParams(var#foreach/#@var, var/,/))

/* ------------------------------------------------------------------------ */
/*
 *   Definitions for the menu system
 */

/* 
 *   The indices for the key values used to navigate menus, which are held
 *   in the keyList array of MenuItems.  
 */
#define M_QUIT      1
#define M_PREV      2
#define M_UP        3
#define M_DOWN      4
#define M_SEL       5

/* some templates for defining menu items */
MenuItem template 'title' 'heading'?;
MenuTopicItem template 'title' 'heading'? [menuContents];
MenuLongTopicItem template 'title' 'heading'? 'menuContents';

/* templates for hint system objects */
Goal template ->closeWhenAchieved? 'title' 'heading'? [menuContents];
Hint template 'hintText' [referencedGoals]?;




/* ------------------------------------------------------------------------ */
/*  
 *   Property synonyms
 */

/* 
 *   The library uses isEdible rather than isEatable, since edible is the more
 *   natural word to use, but strict consistency might have dictated isEatable,
 *   so we make it an effective synonym in case some game authors use it.
 */
#define isEatable isEdible


/*  
 *   Conversely, authors alive to the Latin root of edible might try the
 *   latinate isPotable instead of isDrinable.
 */
#define isPotable isDrinkable

/* ------------------------------------------------------------------------ */
/*
 *   String templates for room descriptions etc.
 */

string template <<mention a * >> mentionA;
string template <<mention an * >> mentionA;
string template <<mention the * >> mentionThe;
string template << list of * is >> listStrIs;
string template << list of * >> makeListInStr;
string template << is list of * >> isListStr;
string template << exclude * >> makeMentioned;



/* ------------------------------------------------------------------------ */
/*
 *   Size classes.  An object is large, medium, or small with respect to
 *   each sense; the size is used to determine how well the object can be
 *   sensed at a distance or when obscured.
 *   
 *   What "size" means depends on the sense.  For sight, the size
 *   indicates the visual size of the object.  For hearing, the size
 *   indicates the loudness of the object.  
 */

/* 
 *   Large - the object is large enough that its details can be sensed
 *   from a distance or through an obscuring medium.
 */
enum large;

/* 
 *   Medium - the object can be sensed at a distance or when obscured, but
 *   not in any detail.  Most objects fall into this category.  Note that
 *   things that are parts of large objects should normally be medium.  
 */
enum medium;

/*
 *   Small - the object cannot be sensed at a distance at all.  This is
 *   appropriate for detailed parts of medium-class objects.  
 */
enum small;

/* ------------------------------------------------------------------------ */
/*
 *   Conversation manager macros
 */

/* has a topic key been revealed through <.reveal>? */
#define gRevealed(key)  (libGlobal.revealedNameTab[key] != nil)

/* reveal a topic key, as though through <.reveal> */
#define gReveal(key) (libGlobal.setRevealed(key))

/* mark a Topic/Thing as known/seen by the player character */
#define gSetKnown(obj) (gPlayerChar.setKnowsAbout(obj))
#define gSetSeen(obj) (gPlayerChar.setHasSeen(obj))

/* has a topic key been revealed to an NPC through <.inform>? */
#define gInformed(key) (getActor.informedAbout(key))





#endif