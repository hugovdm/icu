/*
*******************************************************************************
*
*   Copyright (C) 2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uarrsort.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003aug04
*   created by: Markus W. Scherer
*
*   Internal function for sorting arrays.
*/

#ifndef __UARRSORT_H__
#define __UARRSORT_H__

#include "unicode/utypes.h"

/**
 * Function type for comparing two items as part of sorting an array or similar.
 * Callback function for uprv_sortArray().
 *
 * @param context Application-specific pointer, passed through by uprv_sortArray().
 * @param left    Pointer to the "left" item.
 * @param right   Pointer to the "right" item.
 * @return 32-bit signed integer comparison result:
 *                <0 if left<right
 *               ==0 if left==right
 *                >0 if left>right
 *
 * @internal
 */
typedef int32_t U_CALLCONV
UComparator(const void *context, const void *left, const void *right);

/**
 * Array sorting function.
 * Uses a UComparator for comparing array items to each other, and simple
 * memory copying to move items.
 *
 * @param array      The array to be sorted.
 * @param length     The number of items in the array.
 * @param itemSize   The size in bytes of each array item.
 * @param cmp        UComparator function used to compare two items each.
 * @param context    Application-specific pointer, passed through to the UComparator.
 * @param sortStable If true, a stable sorting algorithm must be used.
 * @param pErrorCode ICU in/out UErrorCode parameter.
 *
 * @internal
 */
U_CAPI void U_EXPORT2
uprv_sortArray(void *array, int32_t length, int32_t itemSize,
               UComparator *cmp, const void *context,
               UBool sortStable, UErrorCode *pErrorCode);

/**
 * Convenience UComparator implementation for uint16_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_uint16Comparator(const void *context, const void *left, const void *right);

/**
 * Convenience UComparator implementation for int32_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_int32Comparator(const void *context, const void *left, const void *right);

/**
 * Convenience UComparator implementation for uint32_t arrays.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
uprv_uint32Comparator(const void *context, const void *left, const void *right);

#endif
