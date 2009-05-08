/*
 *  ATypes.h - basic Acinonyx types
 *  Acinonyx
 *
 *  Created by Simon Urbanek
 *  Copyright 2008 Simon Urbanek. All rights reserved.
 *
 *  lang: C
 */

#ifndef A_TYPES_H
#define A_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
	
	/* basic types */

	typedef float AFloat;
	
	typedef unsigned int vsize_t;
	
	typedef unsigned int notifid_t;

	typedef struct ARect_s { AFloat x,y,width,height; } ARect;
	typedef struct APoint_s { AFloat x,y; } APoint;
	typedef struct ASize_s { AFloat width, height; } ASize;

	typedef struct ADataRange_s { double begin, length; } ADataRange;
	typedef struct ARange_s { AFloat begin, length; } ARange;

	typedef struct AColor_s { AFloat r,g,b,a; } AColor;
	
	typedef struct AEvent_s {
		int event;
		int flags;
		int key;
		APoint location;
	} AEvent;

#define AMkRect(x,y,w,h) ((ARect) { (x),(y),(w),(h) })
#define AMkPoint(x,y) ((APoint) { (x),(y) })
#define AMkSize(w,h) ((ASize) { (w),(h) })
#define AMkRange(b,l) ((ARange) { (b),(l) })
#define AMkDataRange(b,l) ((ADataRange) { (b),(l) })
#define AMkEvent(e,f,k,l) ((AEvent) { (e),(f),(k),(l) })
#define AMkColor(r,g,b,a) ((AColor) { (r),(g),(b),(a) })

#define AMAX(X,Y) (((X) > (Y))?(X):(Y))
#define AMIN(X,Y) (((X) < (Y))?(X):(Y))
	
#define AUndefSize AMkSize(-1.0f, -1.0f)
	// FIXME: AUndefRange needs a more reasonable definition
#define AUndefRange AMkRange(0.0f, -1.0f)
#define AUndefDataRange AMkDataRange(0.0, -1.0)

#define ANotFound ((vsize_t) -1)
	
#define APointsAreEqual(A,B) (((A).x == (B).x) && ((A).y == (B).y))
#define ASizesAreEqual(A,B) (((A).width == (B).width) && ((A).height == (B).height))
#define ARectsAreEqual(A,B) (APointsAreEqual(A,B) && ASizesAreEqual(A,B))
#define ADataRangesAreEqual(A,B) (((A).begin == (B).begin) && ((A).length == (B).length))
#define ARectContains(A,B) (((B).x >= (A).x) && ((B).x <= (A).x + (A).width) && ((B).y >= (A).y) && ((B).y <= (A).y + (A).width)) 
#define ARectsIntersect(A,B) (!((A).x + (A).width < (B).x) || ((A).x > (B).x + (B).width) || ((A).y + (A).height < (B).y) || ((A).y > (B).y + (B).height))
	
#define AEF_BUTTON1 0x001
#define AEF_BUTTON2 0x002
#define AEF_BUTTON3 0x004
#define AEF_SHIFT   0x010
#define AEF_CTRL    0x020
#define AEF_ALT     0x040
#define AEF_META    0x080

#define AE_MOUSE_DOWN   0x0101
#define AE_MOUSE_UP     0x0102
#define AE_MOUSE_CLICK  0x0103
#define AE_MOUSE_2CLICK 0x0104

#define AE_MOUSE_IN     0x0106
#define AE_MOUSE_OUT    0x0107

#define AE_MOUSE_MOVE   0x0108

#define AE_KEY_DOWN     0x0201
#define AE_KEY_UP       0x0202
#define AE_KEY_TYPED    0x0203

#define KEY_LEFT  123
#define KEY_RIGHT 124	
#define KEY_DOWN  125
#define KEY_UP    126
	
// This macro is used after memory allocation to check for out of memory issues
#define AMEM(x) // FIXME: replace with some error handling if x is NULL
	
extern double NA_double;
extern float NA_float;
#define NA_int INT_MIN

#ifdef __cplusplus
}
#endif	

#endif
