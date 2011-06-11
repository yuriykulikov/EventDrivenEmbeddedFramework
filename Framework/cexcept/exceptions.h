/*
 * exceptions.h
 *
 *  Created on: 07.06.2011
 *      Author: Yuriy
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

/* The following declarations would normally go in a separate .h file: */

#include "cexcept.h"

enum exception_flavor { okay, oops, screwup, barf };

struct exception {
  enum exception_flavor flavor;
  const char *msg;
  union {
    int oops;
    long screwup;
    char barf[8];
  } info;
};

define_exception_type(struct exception);

/* End of separate .h file. */

#endif /* EXCEPTIONS_H_ */
