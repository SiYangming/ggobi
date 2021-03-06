/* Generated by GOB (v2.0.18)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>
#ifndef __GGOBI_RENDERER_FACTORY_H__
#define __GGOBI_RENDERER_FACTORY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



  #include "ggobi-renderer.h"


/*
 * Type checking and casting macros
 */
#define GGOBI_TYPE_RENDERER_FACTORY	(ggobi_renderer_factory_get_type())
#define GGOBI_RENDERER_FACTORY(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ggobi_renderer_factory_get_type(), GGobiRendererFactory)
#define GGOBI_RENDERER_FACTORY_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ggobi_renderer_factory_get_type(), GGobiRendererFactory const)
#define GGOBI_RENDERER_FACTORY_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), ggobi_renderer_factory_get_type(), GGobiRendererFactoryClass)
#define GGOBI_IS_RENDERER_FACTORY(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), ggobi_renderer_factory_get_type ())

#define GGOBI_RENDERER_FACTORY_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), ggobi_renderer_factory_get_type(), GGobiRendererFactoryClass)

/*
 * Main object structure
 */
#ifndef __TYPEDEF_GGOBI_RENDERER_FACTORY__
#define __TYPEDEF_GGOBI_RENDERER_FACTORY__
typedef struct _GGobiRendererFactory GGobiRendererFactory;
#endif
struct _GGobiRendererFactory {
	GObject __parent__;
};

/*
 * Class definition
 */
typedef struct _GGobiRendererFactoryClass GGobiRendererFactoryClass;
struct _GGobiRendererFactoryClass {
	GObjectClass __parent__;
	GGobiRenderer * (* create) (GGobiRendererFactory * self, GdkDrawable * parent);
};


/*
 * Public methods
 */
GType	ggobi_renderer_factory_get_type	(void) G_GNUC_CONST;
#line 13 "renderer-factory.gob"
GGobiRendererFactory * 	ggobi_renderer_factory_new	(void);
#line 56 "ggobi-renderer-factory.h"
#line 20 "renderer-factory.gob"
GGobiRenderer * 	ggobi_renderer_factory_create	(GGobiRendererFactory * self,
					GdkDrawable * parent);
#line 60 "ggobi-renderer-factory.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
