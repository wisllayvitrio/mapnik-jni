#include <mapnik/grid/grid.hpp>
#include <mapnik/grid/grid_renderer.hpp>

/*
 * Class:     mapnik_Renderer
 * Method:    renderAgg
 * Signature: (Lmapnik/MapDefinition;Lmapnik/Image;DII)V
 */
JNIEXPORT void JNICALL Java_mapnik_Renderer_renderAgg
  (JNIEnv *env, jclass c, jobject mapobj, jobject imobj, jdouble scale_factor, jint offset_x, jint offset_y)
{
	PREAMBLE;
	if (!mapobj || !imobj || offset_x<0 || offset_y<0) {
		throw_runtime_exception(env, "Illegal arguments in call");
		return;
	}

	mapnik::Map* map=LOAD_MAP_POINTER(mapobj);
	mapnik::image_32* im=LOAD_IMAGE_POINTER(imobj);

	mapnik::agg_renderer<mapnik::image_32> ren(*map, *im, scale_factor, offset_x, offset_y);
	ren.apply();

	TRAILER_VOID;
}



/*
 * Class:     mapnik_Renderer
 * Method:    renderGrid
 * Signature: (Lmapnik/MapDefinition;Lmapnik/Grid;DII)V
 */
JNIEXPORT void JNICALL Java_mapnik_Renderer_renderGrid
  (JNIEnv *env, jclass c, jobject mapobj, jobject gridobj, jdouble scale_factor, jint offset_x, jint offset_y){
    PREAMBLE;
    if (!mapobj || !gridobj || offset_x < 0 || offset_y < 0) {
        throw_runtime_exception(env, "Illegal arguments in call");
        return;
    }

    mapnik::Map* map=LOAD_MAP_POINTER(mapobj);
    mapnik::grid* grid=LOAD_GRID_POINTER(gridobj);

    //pegando apenas um layer
    std::vector<mapnik::layer> const& layers = map->layers();
    mapnik::layer const& layer = layers[0];
    std::set<std::string> attributes = grid->property_names();

    mapnik::grid_renderer<mapnik::grid> renderer(*map, *grid, scale_factor, offset_x, offset_y);
    renderer.apply(layer, attributes, 0.0);

    TRAILER_VOID;
}
