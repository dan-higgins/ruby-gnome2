/* -*- c-file-style: "ruby" -*- */
/************************************************

  rbgtkcolorsel.c -

  $Author: mutoh $
  $Date: 2002/05/19 13:59:10 $

  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#include "global.h"

/*
 * ColorSelection
 */

static VALUE
colorsel_initialize(self)
    VALUE self;
{
    set_widget(self, gtk_color_selection_new());
    return Qnil;
}

static VALUE
colorsel_set_update_policy(self, policy)
    VALUE self, policy;
{
    gtk_color_selection_set_update_policy(GTK_COLOR_SELECTION(get_widget(self)),
					  (GtkUpdateType)NUM2INT(policy));
    return self;
}

#if GTK_MAJOR_VERSION < 2
static VALUE
colorsel_set_opacity(self, opacity)
    VALUE self, opacity;
{
    gtk_color_selection_set_opacity(GTK_COLOR_SELECTION(get_widget(self)),
				    RTEST(opacity));
    return self;
}
#endif

static VALUE
colorsel_set_color(self, color)
    VALUE self, color;
{
    double buf[4];
    int arylen;
    GtkColorSelection *colorsel;

    Check_Type(color, T_ARRAY);
    colorsel = GTK_COLOR_SELECTION(get_widget(self));
#if GTK_MAJOR_VERSION >= 2
    if (gtk_color_selection_get_has_opacity_control(colorsel)) {
#else
    if (colorsel->use_opacity) {
#endif
	arylen = 4;
    } else {
	arylen = 3;
    }
    if (RARRAY(color)->len < arylen) {
	rb_raise(rb_eArgError, "color array too small");
    }
    buf[0] = NUM2DBL(RARRAY(color)->ptr[0]);
    buf[1] = NUM2DBL(RARRAY(color)->ptr[1]);
    buf[2] = NUM2DBL(RARRAY(color)->ptr[2]);
    if (arylen == 4) {
	buf[3] = NUM2DBL(RARRAY(color)->ptr[3]);
    }

    gtk_color_selection_set_color(colorsel, buf);
    return self;
}

static VALUE
colorsel_get_color(self)
    VALUE self;
{
    double buf[4];
    VALUE ary;
    int arylen;
    GtkColorSelection *colorsel;

    colorsel = GTK_COLOR_SELECTION(get_widget(self));
    gtk_color_selection_get_color(colorsel, buf);
#if GTK_MAJOR_VERSION == 2
    if (gtk_color_selection_get_has_opacity_control(colorsel)) {
#else
    if (colorsel->use_opacity) {
#endif
	arylen = 4;
    } else {
	arylen = 3;
    }
    ary = rb_ary_new2(arylen);
    rb_ary_push(ary, rb_float_new(buf[0]));
    rb_ary_push(ary, rb_float_new(buf[1]));
    rb_ary_push(ary, rb_float_new(buf[2]));
    if (arylen == 4) {
	rb_ary_push(ary, rb_float_new(buf[3]));
    }
    return ary;
}

void Init_gtk_color_selection()
{
    gColorSel = rb_define_class_under(mGtk, "ColorSelection", gVBox);

    rb_define_const(gColorSel, "SIGNAL_COLOR_CHANGED", rb_str_new2("color_changed"));

    rb_define_method(gColorSel, "initialize", colorsel_initialize, 0);
    rb_define_method(gColorSel, "set_update_policy", colorsel_set_update_policy, 1);
#if GTK_MAJOR_VERSION != 2
    rb_define_method(gColorSel, "set_opacity", colorsel_set_opacity, 1);
#endif
    rb_define_method(gColorSel, "set_color", colorsel_set_color, 1);
    rb_define_method(gColorSel, "get_color", colorsel_get_color, 0);
}

/*
 * ColorSelectionDialog
 */
static VALUE
cdialog_initialize(self, title)
    VALUE self, title;
{
    set_widget(self, gtk_color_selection_dialog_new(STR2CSTR(title)));
    return Qnil;
}

static VALUE
cdialog_get_colorsel(self)
    VALUE self;
{
    VALUE sel = rb_iv_get(self, "colorsel");

    if (NIL_P(sel)) {
	GtkWidget *w = GTK_COLOR_SELECTION_DIALOG(get_widget(self))->colorsel;
	sel = make_widget(gColorSel, w);
	rb_iv_set(self, "colorsel", sel);
    }

    return sel;
}

static VALUE
cdialog_get_ok_button(self)
    VALUE self;
{
    VALUE b = rb_iv_get(self, "ok_button");

    if (NIL_P(b)) {
	GtkWidget *w = GTK_COLOR_SELECTION_DIALOG(get_widget(self))->ok_button;
	b = make_widget(gButton, w);
	rb_iv_set(self, "ok_button", b);
    }

    return b;
}

static VALUE
cdialog_get_cancel_button(self)
    VALUE self;
{
    VALUE b = rb_iv_get(self, "cancel_button");

    if (NIL_P(b)) {
	GtkWidget *w
	    = GTK_COLOR_SELECTION_DIALOG(get_widget(self))->cancel_button;
	b = make_widget(gButton, w);
	rb_iv_set(self, "cancel_button", b);
    }

    return b;
}

static VALUE
cdialog_get_help_button(self)
    VALUE self;
{
    VALUE b = rb_iv_get(self, "help_button");

    if (NIL_P(b)) {
	GtkWidget *w
	    = GTK_COLOR_SELECTION_DIALOG(get_widget(self))->help_button;
	b = make_widget(gButton, w);
	rb_iv_set(self, "help_button", b);
    }

    return b;
}

void Init_gtk_color_selection_dialog()
{
    gColorSelDialog = rb_define_class_under(mGtk, "ColorSelectionDialog", gWindow);

    rb_define_method(gColorSelDialog, "initialize", cdialog_initialize, 1);
    rb_define_method(gColorSelDialog, "colorsel", cdialog_get_colorsel, 0);
    rb_define_method(gColorSelDialog, "ok_button", cdialog_get_ok_button, 0);
    rb_define_method(gColorSelDialog, "cancel_button", cdialog_get_cancel_button, 0);
    rb_define_method(gColorSelDialog, "help_button", cdialog_get_help_button, 0);
}
