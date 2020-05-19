/**
 * @file
 *
 * Module used to generate an automatic man page
 *
 * The module has been inspired by <a href="https://www.cyberciti.biz/faq/linux-unix-creating-a-manpage/">this nixCraft page</a>.
 *
 * @date Aug 25, 2017
 * @author koldar
 */

#ifndef MAN_PRODUCER_H_
#define MAN_PRODUCER_H_

#include "arg_parser.h"

typedef struct man_producer man_producer;

/**
 * Creates a new structure allowing you to automatically create a man page
 *
 * @param[in] name the name of the application
 * @param[in] version the version of the application
 * @param[in] parser the parser structure used to manually parse the arguments. If NULL it means we want to build the man_producer without such parser
 * @return a structure allowing you to automatically create a man page
 */
man_producer* cuManProducerNewManProducer(CU_NOTNULL const char* name, CU_NOTNULL const char* version, CU_NOTNULL const cli_parser* parser);

/**
 * Destroy the structure allowing you to automatically create the manpage
 *
 * @param[in] mp the structure to remove from memory
 */
void cuManProducerDestroy(man_producer* mp, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuManProducerDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Set the application name your' building
 *
 * @pre
 *  @li @c name is an alphanumeric string with no spaces
 *
 * @param[inout] mp the structure involved
 * @param[in] name the name of the software you're building. Copied by reference.
 */
void cuManProducerSetApplicationName(man_producer* mp, const char* name);

/**
 * Set a brief description of the software you're building
 *
 * @param[inout] mp the structure involved
 * @param[in] name a brief description (one line) of the software you're building. Copied by reference.
 */
void cuManProducerSetBriefDescription(man_producer* mp, const char* name);

/**
 * Set a long description of the software you're building
 *
 * @param[inout] mp the structure involved
 * @param[in] name the long description. Copied by reference.
 */
void cuManProducerSetLongDescription(man_producer* mp, const char* name);

/**
 * A set of string of related software you want to put in the man page
 *
 * @param[inout] mp the structure involved
 * @param[in] count the number of parameters in the variadic
 * @param[in] ... each parameter in the variadic is a string. An example of string might be <tt>basename(3)</tt>
 */
void cuManProducerAddSeeAlso(man_producer* mp, int count, ...);

/**
 * A set of known bugs in the software
 *
 * @param[inout] mp the structure involved
 * @param[in] count the number of elements in the variadic
 * @param[in] ... each parameter in the variadic is a string. An example of such string might be <tt>input file needs to exist, otherwise the program crashes</tt>
 */
void cuManProducerAddBugs(man_producer* mp, int count, ...);

/**
 * Adds an author of this software
 *
 * @param[inout] mp the structure involved
 * @param[in] fullName the name of the author. Copied by reference.
 * @param[in] email the email of the author. Copied by reference.
 */
void cuManProducerAddAuthor(man_producer* mp, const char* fullName, const char* email);

/**
 * Generate a groff file that can be used to contains an automatically generaated man page
 *
 * @param[in] mp the structure involved
 */
void cuManProducerProduceGroffFile(const man_producer* mp);

#endif /* MAN_PRODUCER_H_ */
