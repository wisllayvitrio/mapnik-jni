/// -- Geometry class
/*
 * Class:     mapnik_Geometry
 * Method:    getType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_mapnik_Geometry_getType
  (JNIEnv *env, jobject gobj)
{
	PREAMBLE;
	mapnik::geometry_type* g=LOAD_GEOMETRY_POINTER(gobj);
	return g->type();
	TRAILER(0);
}

/*
 * Class:     mapnik_Geometry
 * Method:    getNumPoints
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_mapnik_Geometry_getNumPoints
  (JNIEnv *env, jobject gobj)
{
	PREAMBLE;
	mapnik::geometry_type* g=LOAD_GEOMETRY_POINTER(gobj);
	#if MAPNIK_VERSION >= 200100
	return g->size();
	#else
	return g->num_points();
	#endif
	TRAILER(0);
}

/*
 * Class:     mapnik_Geometry
 * Method:    getVertex
 * Signature: (ILmapnik/Coord;)I
 */
JNIEXPORT jint JNICALL Java_mapnik_Geometry_getVertex
  (JNIEnv *env, jobject gobj, jint pos, jobject coord)
{
	PREAMBLE;
	mapnik::geometry_type* g=LOAD_GEOMETRY_POINTER(gobj);
	double x=0, y=0;
	#if MAPNIK_VERSION >= 200100
	unsigned ret=g->vertex(pos, &x, &y);
	#else
	unsigned ret=g->get_vertex(pos, &x, &y);
	#endif

	if (coord) {
		env->SetDoubleField(coord, FIELD_COORD_X, x);
		env->SetDoubleField(coord, FIELD_COORD_Y, y);
	}

	return ret;
	TRAILER(0);
}

/*
 * Class:     mapnik_Geometry
 * Method:    getEnvelope
 * Signature: ()Lmapnik/Box2d;
 */
JNIEXPORT jobject JNICALL Java_mapnik_Geometry_getEnvelope
  (JNIEnv *env, jobject gobj)
{
	PREAMBLE;
	mapnik::geometry_type* g=LOAD_GEOMETRY_POINTER(gobj);
	return box2dFromNative(env, g->envelope());
	TRAILER(0);
}

