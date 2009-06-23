/*
 *  ABarChart.h
 *  Acinonyx
 *
 *  Created by Simon Urbanek on 5/12/09.
 *  Copyright 2009 Simon Urbanek. All rights reserved.
 *
 */

#ifndef A_BAR_CHART_H
#define A_BAR_CHART_H

#include "AStatVisual.h"
#include "APlot.h"

class ABarChart : public APlot {
protected:
	ADiscreteXAxis *xa;
	AYAxis *ya;
	AFloat mLeft, mTop, mBottom, mRight, ptSize, ptAlpha;
	vsize_t bars;
	bool spines;
	
	vsize_t movingBar, movingBarTarget;
	AFloat movingBarX, movingBarDelta;
	
public:
	ABarChart(AContainer *parent, ARect frame, int flags, AFactorVector *x) : APlot(parent, frame, flags), movingBar(0), spines(false) {
		mLeft = 30.0f; mTop = 10.0f; mBottom = 20.0f; mRight = 10.0f;

		nScales = 2;
		marker = x->marker();
		if (marker) {
			marker->retain();
			marker->add(this);
		}
		bars = 0;
		_scales = (AScale**) malloc(sizeof(AScale*) * nScales);
		AUnivarTable *tab = x->table();
		_scales[0] = new AScale(x, AMkRange(_frame.x + mLeft, _frame.width - mLeft - mRight), bars = x->levels());
		_scales[1] = new AScale(NULL, AMkRange(_frame.y + mBottom, _frame.height - mBottom - mTop), tab->maxCount());
		xa = new ADiscreteXAxis(this, AMkRect(_frame.x + mLeft, _frame.y, _frame.width - mLeft - mRight, mBottom), AVF_FIX_BOTTOM|AVF_FIX_HEIGHT|AVF_FIX_LEFT, _scales[0]);
		add(*xa);
		ya = new AYAxis(this, AMkRect(_frame.x, _frame.y + mBottom, mLeft, _frame.height - mBottom - mTop), AVF_FIX_LEFT|AVF_FIX_WIDTH, _scales[1]);
		add(*ya);
		createPrimitives();
		OCLASS(ABarChart)
	}
	
	virtual ~ABarChart() {
		xa->release();
		ya->release();
		DCLASS(ABarChart);
	}

	ARect rectForBar(AUnivarTable *tab, group_t group) {
		ARange xr = _scales[0]->discreteRange((int)group - 1);
		if (!spines && xr.length > 40.0) { xr.begin = xr.begin + xr.length / 2 - 20.0; xr.length = 40.0; }
		if (xr.length > 5.0) { xr.begin += xr.length * 0.1; xr.length *= 0.8; }
		if (spines) {
			double prop = tab->maxCount() ? (((double) tab->count(group)) / ((double) tab->maxCount())) : 0;
			xr.begin += (1.0 - prop) / 2 * xr.length;
			xr.length *= prop;
			ARange gr = _scales[1]->range();
			return AMkRect(xr.begin, gr.begin, xr.length, gr.length);
		} else
			return AMkRect(xr.begin, _scales[1]->position(0), xr.length, _scales[1]->position(tab->count(group)) - _scales[1]->position(0));
	}
	
	void createPrimitives() {
		AFactorVector *data = (AFactorVector*) _scales[0]->data();
		AUnivarTable *tab = data->table();
		if (pps && pps->length() != bars) {
			pps->release();
			pps = NULL;
		}
		if (!pps)
			pps = new ASettableObjectVector(bars);
		for(vsize_t i = 0; i < bars; i++) {
			group_t group = (group_t) i + 1;
			ABarStatVisual *b = new ABarStatVisual(this, rectForBar(tab, group), Up, marker, (vsize_t*) data->asInts(), data->length(), group, false);
			((ASettableObjectVector*)pps)->replaceObjectAt(i, b);
			b->release(); // we passed the ownership to pps
		}
	}
	
	void update() {
		_scales[0]->setRange(AMkRange(_frame.x + mLeft, _frame.width - mLeft - mRight));
		_scales[1]->setRange(AMkRange(_frame.y + mBottom, _frame.height - mBottom - mTop));

		ya->setHidden(spines);
		
		if (!pps)
			createPrimitives();
		else {
			AFactorVector *data = (AFactorVector*) _scales[0]->data();
			AUnivarTable *tab = data->table();
			vsize_t i = 0, bars = pps->length();
			while (i < bars) {
				ABarStatVisual *b = (ABarStatVisual*) pps->objectAt(i++);
				ARect br = rectForBar(tab, (group_t) i);
				if (i == movingBar)
					br.x = movingBarX;
				b->setRect(br);
			}
		}
	}
	
	virtual bool keyDown(AEvent e) {
		switch (e.key) {
			case 1: spines = !spines; update(); redraw(); break;
			default:
				return false;
		}
		return true;
	}
	
	virtual bool mouseDown(AEvent e) {
		// handle bar dragging
		if ((e.flags & AEF_ALT) && (e.flags & AEF_BUTTON1) && pps) {
			vsize_t i = 0, bars = pps->length();
			while (i < bars) {
				ABarStatVisual *b = (ABarStatVisual*) pps->objectAt(i++);
				if (b->containsPoint(e.location)) {
					ALog("moving bar %d - start", i);
					movingBar = i;
					ABarStatVisual *b = (ABarStatVisual*) pps->objectAt(i - 1);
					movingBarX = b->rect().x;
					movingBarDelta = movingBarX - e.location.x;
					movingBarTarget = scale(0)->discreteValue(e.location.x);
					if (b) b->setAlpha(0.5);
					update(); redraw();
					return true;
				}
			}
		}
		return APlot::mouseDown(e);
	}

	virtual bool event(AEvent e) {
		if (movingBar && e.event == AE_MOUSE_MOVE) {
			movingBarX = e.location.x + movingBarDelta;
			movingBarTarget = scale(0)->discreteIndex(e.location.x);
			vsize_t currentTarget = scale(0)->permutationOf(movingBar - 1);
			ALog("moving bar target = %d (current = %d)", movingBarTarget, currentTarget);
			if (movingBarTarget != currentTarget)
				scale(0)->moveToIndex(movingBar - 1, movingBarTarget);
			ALog("after move: %d", scale(0)->permutationOf(movingBar - 1));
			update();
			redraw();
			return true;
		} else return APlot::event(e);
	}
	
	virtual bool mouseUp(AEvent e) {
		if (movingBar) {
			ABarStatVisual *b = (ABarStatVisual*) pps->objectAt(movingBar - 1);
			if (b) b->setAlpha(1.0);
			movingBar = 0;
			update();
			redraw();
			return true;
		}
		return APlot::mouseUp(e);
	}
	
};

#endif
