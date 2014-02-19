// mapnik
#include <mapnik/grid/grid_view.hpp>
#include <mapnik/grid/grid.hpp>
#include <mapnik/value.hpp>

// std
#include <vector>
#include <string>
#include <assert.h>

// boost
#include <boost/python.hpp>
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>
#include <boost/variant.hpp>

//unicode
#include <unicode/unistr.h>

//local
#include "JsonObject.hpp"

void grid_encode(JNIEnv *env, JsonObject& json, mapnik::grid &grid, int resolution, jstring layerobj);

void put_utf_grid(JNIEnv *env, JsonObject& json, mapnik::grid &grid, std::vector<std::string> &key_order, int resolution);
void put_grid_keys(JNIEnv *env, JsonObject& json, std::vector<std::string> &key_order);
void put_grid_features(JNIEnv *env, JsonObject& json, mapnik::grid &grid, std::vector<std::string> &key_order);

std::vector<std::string> extractArrayListOfString(JNIEnv* env, jobject list);

/*
 * Class:     mapnik_Grid
 * Method:    alloc
 * Signature: (IILjava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_mapnik_Grid_alloc__IILjava_lang_String_2Ljava_util_ArrayList_2I
  (JNIEnv *env, jclass cls, jint width, jint height, jstring keyobj, jobject fieldsobj, jint resolution){
  	PREAMBLE;

  	std::vector<std::string> fields = extractArrayListOfString(env, fieldsobj);

  	const char *key = env->GetStringUTFChars(keyobj, static_cast<jboolean*>(NULL));
  	if (key == static_cast<char*>(NULL)) return static_cast<jlong>(NULL);

	mapnik::grid* grid = new mapnik::grid(width, height, key, resolution);

	for(unsigned int i = 0; i < fields.size(); i++){
		grid->add_property_name(fields[i]);
	}

	return FROM_POINTER(grid);
	TRAILER(0);
}


/*
 * Class:     mapnik_Grid
 * Method:    alloc
 * Signature: (Lmapnik/Grid;)J
 */
JNIEXPORT jlong JNICALL Java_mapnik_Grid_alloc__Lmapnik_Grid_2
  (JNIEnv *env, jclass c, jobject obj){
	PREAMBLE;
	if (!obj) {
		throw_runtime_exception(env, "Grid cannot be null in call to constructor");
		return 0;
	}
	mapnik::grid* other=LOAD_GRID_POINTER(obj);

	mapnik::grid* grid=new mapnik::grid(*other);
	return FROM_POINTER(grid);

	TRAILER(0);
}

/*
 * Class:     mapnik_Grid
 * Method:    dealloc
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mapnik_Grid_dealloc
  (JNIEnv *env, jobject obj, jlong ptr){
	PREAMBLE;
	mapnik::grid* grid=static_cast<mapnik::grid*>(TO_POINTER(ptr));
	if (grid) {
		delete grid;
	}
	TRAILER_VOID;
}

/*
 * Class:     mapnik_Grid
 * Method:    getWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_mapnik_Grid_getWidth
  (JNIEnv *env, jobject gridobj){
	PREAMBLE;
	mapnik::grid* grid=LOAD_GRID_POINTER(gridobj);
	return grid->width();
	TRAILER(0);
}

/*
 * Class:     mapnik_Grid
 * Method:    getHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_mapnik_Grid_getHeight
  (JNIEnv *env, jobject gridobj){
	PREAMBLE;
	mapnik::grid* grid=LOAD_GRID_POINTER(gridobj);
	return grid->width();
	TRAILER(0);
}

/*
 * Class:     mapnik_Grid
 * Method:    saveToMemory
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_mapnik_Grid_saveToMemory
  (JNIEnv *env, jobject gridobj, jstring layerobj) {
	PREAMBLE;

	JsonObject json;
	mapnik::grid* grid=LOAD_GRID_POINTER(gridobj);

	grid_encode(env, json, *grid, 4, layerobj);
	std::string json_string = json.to_string();

	return env->NewStringUTF(json_string.c_str());
	TRAILER(0);
}

void grid_encode(JNIEnv *env, JsonObject& json, mapnik::grid &grid, int resolution, jstring layerobj) {
	std::vector<std::string> key_order;

	put_utf_grid(env, json, grid, key_order, resolution);
	put_grid_keys(env, json, key_order);
	put_grid_features(env, json, grid, key_order);
}

void put_utf_grid(JNIEnv *env, JsonObject& json,
		mapnik::grid &grid, std::vector<std::string> &key_order, int resolution){
	JsonArray lines;

	typedef std::map< std::string, mapnik::grid::value_type> keys_map;
	typedef keys_map::const_iterator keys_iterator;

	mapnik::grid::feature_key_type const& feature_keys = grid.get_feature_keys();
	mapnik::grid::feature_key_type::const_iterator feature_pos;

	keys_map keys;

	// start counting at utf8 codepoint 32, aka space character
	uint16_t codepoint = 32;
	std::vector<std::string> utf_grid;
	std::vector<UChar> line;

	for (unsigned y = 0; y < grid.height(); y=y+resolution) {
		mapnik::grid::value_type const* row = grid.getRow(y);
		line.clear();

		for (unsigned x = 0; x < grid.width(); x=x+resolution){
			feature_pos = feature_keys.find(row[x]);
			if (feature_pos != feature_keys.end()) {
				std::string val = feature_pos->second;
				keys_iterator key_pos = keys.find(val);

				if (key_pos == keys.end()) {
					// Create a new entry for this key. Skip the codepoints that
					// can't be encoded directly in JSON.
					if (codepoint == 34) ++codepoint;      				// Skip "
					else if (codepoint == 92) ++codepoint; 				// Skip backslash

					keys[val] = codepoint;
					key_order.push_back(val);

					line.push_back(static_cast<UChar>(codepoint));
					++codepoint;
				}
				else {
					line.push_back(static_cast<UChar>(key_pos->second));
				}
			}
			// else, shouldn't get here...
		}

		//convert line to unicode string and append to utf_grid
		UnicodeString ustr(&line[0], line.size());
		lines.add(ustr);
	}

	json["grid"] = lines;
}

void put_grid_keys(JNIEnv *env, JsonObject& json, std::vector<std::string> &key_order) {
	JsonArray keys;
	for(int i = 0, size = key_order.size(); i < size; i++) {
		keys.add(key_order[i]);
	}
	json["keys"] = keys;
}

void put_grid_features(JNIEnv *env, JsonObject& json, mapnik::grid &grid, std::vector<std::string> &key_order) {
	mapnik::grid::feature_type const& g_features = grid.get_grid_features();

	if (g_features.size() <= 0){
		return;
	}

	std::set<std::string> const& attributes = grid.property_names();
	mapnik::grid::feature_type::const_iterator feat_end = g_features.end();

	JsonObject data;

	BOOST_FOREACH (std::string const& key_item, key_order) {
		if (key_item.empty()){
			continue;
		}

		mapnik::grid::feature_type::const_iterator feat_itr = g_features.find(key_item);
		if (feat_itr == feat_end){
			continue;
		}

		mapnik::feature_ptr feature = feat_itr->second;
		JsonObject attrJson;

		BOOST_FOREACH (std::string const& attr, attributes){
			if (feature->has_key(attr)){
				const mapnik::value& value = feature->get(attr);
				attrJson[attr] = value;
			}
		}

		data[key_item] = attrJson;
	}

	json["data"] = data;
}

std::vector<std::string> extractArrayListOfString(JNIEnv* env, jobject list){
	jclass array_list = env->FindClass( "java/util/ArrayList" );
	jmethodID caster = env->GetMethodID(array_list, "toString", "()Ljava/lang/String;");
	jmethodID array_list_get = env->GetMethodID( array_list, "get", "(I)Ljava/lang/Object;" );
	jmethodID array_list_size = env->GetMethodID( array_list, "size", "()I" );

	int size = env->CallIntMethod(list, array_list_size);
	std::vector<std::string> string_list(size);

	for(int i = 0; i < size; i++){
	    jobject strobj = env->CallObjectMethod(list, array_list_get, i);

	    jstring jstr = (jstring)env->CallObjectMethod(strobj, caster);
	    const char *utf = env->GetStringUTFChars(jstr, NULL);

	    string_list[i] = utf;
	    env->ReleaseStringUTFChars(jstr, utf);
	}

	return string_list;
}
