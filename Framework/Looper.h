/*
 * Looper.h
 *
 *  Created on: Nov 20, 2011
 *      Author: Yuriy
 */

#ifndef LOOPER_H_
#define LOOPER_H_

typedef struct
{
	/** Queue on which handler posts messages */
	xQueueHandle messageQueue;
} Looper;

Looper * Looper_start(unsigned char messageQueueSize, char *name, unsigned short stackDepth, char priority, xTaskHandle taskHandle);

#endif /* LOOPER_H_ */
