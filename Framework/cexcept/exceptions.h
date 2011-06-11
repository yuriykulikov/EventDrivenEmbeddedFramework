/*
 * exceptions.h
 * To be used inside tasks only, not within interrupts or main.
 *  Created on: 07.06.2011
 *      Author: Yuriy
 */

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_


#include "cexcept.h"

enum exception_type { warning, null, error, outOfHeap, stackOverflow };

struct exception {
  enum exception_type type;
  const char *msg;
};

define_exception_type(struct exception);

#endif /* EXCEPTIONS_H_ */
