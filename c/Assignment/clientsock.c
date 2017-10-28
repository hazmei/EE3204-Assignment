/*
 * clientsock.c: sender sends 1 DU, wait for ack then send 2 DU's , waits for ACK and repeat
 *
 * Measure msg transfer time and throughput for various sizes of data units
 * and compare it with stop-and-wait protocol where batch size is fixed to 1.
 *
 * Choose appropriate values for data unit size and measure performance
 * Repeat experiment several times and plot the average values in report with
 * brief description of results, assumptions made, etc.
*/

#include "headsock.h"
