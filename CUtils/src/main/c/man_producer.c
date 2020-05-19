/*
 * man_producer.c
 *
 *  Created on: Aug 25, 2017
 *      Author: koldar
 */

#include <stdarg.h>
#include "man_producer.h"
#include "macros.h"
#include "list.h"
#include "file_utils.h"
#include "errors.h"


struct man_producer {
	/**
	 * the name of the application
	 *
	 * The name is an alphanumeric sequence with no spaces
	 */
	CU_NOTNULL char* name;
	/**
	 * a very brief description of what the application does
	 */
	CU_NULLABLE char* briefDescription;
	/**
	 * A much more complete description of what the application does
	 */
	CU_NULLABLE char* description;
	/**
	 * version of the program
	 */
	CU_NULLABLE char* version;
	const cli_parser* parser;
	int sectionLevel;
	CU_NOTNULL string_list* seeAlsoList;
	CU_NOTNULL string_list* bugList;
	CU_NOTNULL string_list* authorList;
	CU_NOTNULL string_list* emailList;
};

man_producer* cuManProducerNew(CU_NOTNULL const char* name, CU_NULLABLE const char* version, CU_NOTNULL const cli_parser* parser) {
	man_producer* retVal = malloc(sizeof(man_producer));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->briefDescription = NULL;
	retVal->bugList = cuListNew();
	retVal->description = NULL;
	retVal->name = (char*) name;
	retVal->sectionLevel = 8;
	retVal->seeAlsoList = cuListNew();
	retVal->authorList = cuListNew();
	retVal->parser = parser;
	retVal->version = (char*) version;

	return retVal;
}

void cuManProducerDestroy(man_producer* mp, CU_NULLABLE const struct var_args* context) {
	cuListDestroy(mp->bugList, context);
	cuListDestroy(mp->seeAlsoList, context);
	free(mp);
}

void cuManProducerSetApplicationName(man_producer* mp, const char* name) {
	mp->name = (char*)name;
}

void cuManProducerSetSectionNumber(man_producer* mp, int sectionLevel) {
	if (sectionLevel<1 || sectionLevel>8) {
		ERROR_ON_APPLICATION("section level", "%d", sectionLevel, "man producer", "%p", mp);
	}

	mp->sectionLevel = sectionLevel;
}

void cuManProducerSetBriefDescription(man_producer* mp, const char* name) {
	mp->briefDescription = (char*)name;
}

void cuManProducerSetLongDescription(man_producer* mp, const char* name) {
	mp->description = (char*)name;
}

void cuManProducerAddSeeAlso(man_producer* mp, int count, ...) {
	va_list va;

	va_start(va, count);
	for (int i=0; i<count; i++) {
		char* seeAlso = va_arg(va, char*);
		cuListAddTail(mp->seeAlsoList, seeAlso);
	}
	va_end(va);
}

void cuManProducerAddBugs(man_producer* mp, int count, ...) {
	va_list va;

	va_start(va, count);
	for (int i=0; i<count; i++) {
		char* bug = va_arg(va, char*);
		cuListAddTail(mp->bugList, bug);
	}
	va_end(va);
}

void cuManProducerAddAuthor(man_producer* mp, const char* fullName, const char* email) {
	cuListAddTail(mp->authorList, fullName);
	cuListAddTail(mp->emailList, email);
}

void setVersionOfManProducer(man_producer* mp, char* version) {
	mp->version = version;
}


void populateManProducerFromCLIParser(man_producer* mp, const cli_parser* p, bool overwriteCurrentOptions) {

}

void populateManProducerFromGroffFile(man_producer* mp, const char* groffFileName, bool overwriteCurrentOptions) {

}

void cuManProducerProduceGroffFile(const man_producer* mp) {
	char fileNameBuffer[BUFFER_SIZE];

	sprintf(fileNameBuffer, "%s.1", mp->name);
	FILE* fout = fopen(fileNameBuffer, "w");
	if (fout == NULL) {
		ERROR_FILE(fileNameBuffer);
	}

	cuFileUtilsWriteln(fout, ".\\\" Manpage for nuseradd.");
	cuFileUtilsWriteln(fout, ".\\\" Contact m.bono@unibs.it to correct errors or typos.");

	cuFileUtilsWriteln(fout, ".TH man %d \"%s\" %s %s",
		mp->sectionLevel,
		__DATE__,
		mp->version,
		mp->name
	);

	//name
	cuFileUtilsWriteln(fout, ".SH NAME");
	cuFileUtilsWriteln(fout, "%s \\- %s", mp->name, mp->briefDescription);

	//synopsis
	char buffer[BUFFER_SIZE * 10];
	cuCLIParserGetSynopsis(mp->parser, buffer);
	cuFileUtilsWriteln(fout, ".SH SYNOPSIS");
	cuFileUtilsWriteln(fout, "./%s %s",
			mp->name,
			buffer
			);

	//description
	cuFileUtilsWriteln(fout, ".SH DESCRIPTION");
	cuFileUtilsWriteln(fout, "%s", mp->description);

	//options
	cuFileUtilsWriteln(fout, ".SH OPTIONS");
	char options[BUFFER_SIZE * 100];
	cuCLIParseGetOptions(mp->parser, options);
	cuFileUtilsWriteln(fout, "%s", options);

	//bugs
	cuFileUtilsWriteln(fout, ".SH BUGS");
	if (cuListIsEmpty(mp->bugList)) {
		cuFileUtilsWriteln(fout, "No Known bugs to declare.");
	} else {
		int i=0;
		CU_ITERATE_OVER_LIST(mp->bugList, cell, bug, char*) {
			i++;
			cuFileUtilsWriteln(fout, " %d. %s;", i, bug);
		}
	}
	cuFileUtilsWriteln(fout, "");

	//authors
	cuFileUtilsWriteln(fout, ".SH AUTHOR");
	for (int i=0; i<cuListGetSize(mp->authorList); i++) {
		cuFileUtilsWriteln(fout, " - %s contactable with %s\n",
				cuListGetNthItem(mp->authorList, i, char*),
				cuListGetNthItem(mp->emailList, i, char*)
		);
	}
	cuFileUtilsWriteln(fout, "");

	if (!cuListIsEmpty(mp->seeAlsoList)) {
		cuFileUtilsWriteln(fout, ".SH SEE ALSO");
		CU_ITERATE_OVER_LIST(mp->seeAlsoList, cell, seeAlso, char*) {
			cuFileUtilsWriteln(fout, " - %s", seeAlso);
		}
		cuFileUtilsWriteln(fout, "");
	}

	fclose(fout);
}
