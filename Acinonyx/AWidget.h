/*
 *  AWidget.h
 *  Acinonyx
 *
 *  Created by Simon Urbanek on 5/31/11.
 *  Copyright 2011 Simon Urbanek. All rights reserved.
 *
 */

#ifndef A_WIDGET_H
#define A_WIDGET_H

#include "ATypes.h"
#include "AVisual.h"
#include "APlot.h" // for delegation

class AWidget : public AContainer {
protected:
	APlot *delegate_;
	// the following flags can be used to track the behavior of the widget
	bool mouse_inside;      // this flag monitors enter/leave
	bool mouse_locked;      // if this flag is set, then moving the mouse outside doesn't trigger dismissal
	bool mouse_down_inside; // true if the last mouseDown even was inside the widget
	bool hovering;          // true if the mouse is currently hovering over the widget
public:
	const char *click_action;
#pragma mark --- contructor ---
	AWidget(AContainer *parent, ARect frame, unsigned int flags) : AContainer(parent, frame, flags), mouse_inside(false), mouse_locked(false), mouse_down_inside(false), hovering(false), delegate_(NULL), click_action("clicked") { OCLASS(AWidget) }

#pragma mark --- delegation ---
	APlot *delegate() { return delegate_; }

	void setDelegate(APlot *new_delegate) { delegate_ = new_delegate; } // FIXME: we currently don't use retain to avoid loops for common use of objects owned by the plot

	void delegateAction(const char *action, AObject *aux = NULL) {
		if (delegate_)
			delegate_->delegateAction(this, action, aux);
	}
	
#pragma mark --- mouse handling ---
	virtual bool mouseEnter(AEvent event) {
		hovering = true;
		redrawWidget();
		return false;
	}

	virtual bool mouseLeave(AEvent event) {
		hovering = false;
		redrawWidget();
		return false;
	}

	virtual bool mouseMove(AEvent event) {
		// ALog("mouseMove: %g,%g -> %g,%g-%g,%g", event.location.x, event.location.y, _frame.x, _frame.y, _frame.width, _frame.height);
		if (!mouse_inside && containsPoint(event.location)) {
			mouse_inside = true;
			return mouseEnter(event);
		} else if (mouse_inside && !containsPoint(event.location)) {
			mouse_inside = false;
			return mouseLeave(event);
		}
		return false;
	}

	// this method is called when the widget is clicked on (as in both up and down clicks are inside; chain into mouseDown if you want to allow drag-click)
	virtual bool clicked(AEvent event) {
		if (click_action)
			delegateAction(click_action, NULL);
		return false;
	}
	
	virtual bool mouseDown(AEvent event) {
		// ALog("%s mouseDown: %g,%g [%g,%g-%g,%g]", describe(), event.location.x, event.location.y, _frame.x, _frame.y, _frame.width, _frame.height);
		if ((mouse_down_inside = containsPoint(event.location))) return true;
		return false;
	}
	
	virtual bool mouseUp(AEvent event) {
		// ALog("%s mouseUp: %g,%g [%g,%g-%g,%g]", describe(), event.location.x, event.location.y, _frame.x, _frame.y, _frame.width, _frame.height);
		if (containsPoint(event.location) && mouse_down_inside) {
			mouse_down_inside = false;
			return clicked(event);
		}
		mouse_down_inside = false;
		return false;
	}
	
	// for speed redraw() is not virtual, so widgets use thier own method
	// FIXME: we could make redraw layer IV of each renderer such that redraw() could account for it automatically ...
	void redrawWidget() {
		if (_window) {
			_window->setRedrawLayer(LAYER_WIDGETS);
			_window->redraw();
		}
	}
	
	virtual bool drawWidget() { return true; } // widgets drawing chain -- each subclass should call super.drawWidget() and only draw if it returns true; widgets have no layers, they are in their own layer

	virtual void draw(vsize_t layer) {
		if (layer == LAYER_WIDGETS) drawWidget();
		AContainer::draw(layer);
	}
};

#endif
