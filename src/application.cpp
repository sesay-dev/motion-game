#include "rubymotion.h"

VALUE rb_cApplication = Qnil;
static VALUE mc_application_instance = Qnil;

class mc_Application : private cocos2d::Application {
    public:
	VALUE obj;

    mc_Application() {
	obj = Qnil;
    }

    virtual bool
    applicationDidFinishLaunching() {
	rb_send(obj, rb_selector("start"), 0, NULL);
	return true;
    }

    virtual void
    applicationDidEnterBackground() {

    }

    virtual void
    applicationWillEnterForeground() {

    }
};

static VALUE
application_instance(VALUE rcv, SEL sel)
{
    if (mc_application_instance == Qnil) {
	mc_application_instance = rb_class_wrap_new(
		(void *)cocos2d::Application::getInstance(), rb_cApplication);
	rb_obj_retain(mc_application_instance);
    }
    return mc_application_instance;
}

static VALUE
application_alloc(VALUE rcv, SEL sel)
{
    mc_Application *app = new mc_Application();
    VALUE obj = rb_class_wrap_new((void *)app, rcv);
    app->obj = obj;
    return obj;
}

static VALUE mc_application_orientation = Qnil;

static VALUE
application_orientation(VALUE rcv, SEL sel, int argc, VALUE *argv)
{
    if (argc > 1) {
	rb_raise(rb_eArgError, "expected 1 or 0 arguments");
    }
    if (argc == 1) {
	if (mc_application_orientation != argv[0]) {
	    // TODO: validate argument
	    mc_application_orientation = argv[0];
	}
    }
    return mc_application_orientation;
}

static VALUE
application_run(VALUE rcv, SEL sel)
{
    APPLICATION(rcv)->run();
    return rcv;
}

static VALUE
application_screen_size_changed(VALUE rcv, SEL sel)
{
    cocos2d::GLView *glview = cocos2d::Director::getInstance()->getOpenGLView();
    if (glview != NULL) {
	cocos2d::Size size = glview->getFrameSize();
	cocos2d::Application::getInstance()->applicationScreenSizeChanged(
		size.width, size.height);
    }
    return rcv;
}

extern "C"
void
Init_Application(void)
{
    rb_cApplication = rb_define_class_under(rb_mMC, "Application", rb_cObject);

    rb_define_singleton_method(rb_cApplication, "instance",
	    application_instance, 0);
    rb_define_singleton_method(rb_cApplication, "alloc", application_alloc, 0);
    rb_define_singleton_method(rb_cApplication, "orientation",
	    application_orientation, -1);
    rb_define_method(rb_cApplication, "run", application_run, 0);

    // Internal.
    rb_define_method(rb_cApplication, "_screen_size_changed",
	    application_screen_size_changed, 0);

    mc_application_orientation = rb_name2sym("all");
}