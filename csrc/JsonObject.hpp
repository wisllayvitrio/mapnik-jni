/*
 * JsonObject.h
 *
 *  Created on: 24/01/2014
 *      Author: wisllay
 */

#ifndef JSONOBJECT_H_
#define JSONOBJECT_H_

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <mapnik/value.hpp>
#include <mapnik/value_types.hpp>
#include <boost/variant.hpp>

class JsonObject;
class JsonArray;

typedef boost::variant<mapnik::value, std::string, JsonObject, JsonArray> json_type;

class JsonObject {
private:
	std::map<std::string, json_type> json;

public:
	void put(const std::string& key, json_type value);

	json_type get(std::string key);

	json_type& operator[](std::string key);

	std::string to_string();
};

class JsonArray {
private:
	std::vector<json_type> json_array;

public:
	void add(json_type value);

	json_type get(int index);

	json_type& operator[](int index);

	int size();

	std::string to_string();
};

class to_string_visitor : public boost::static_visitor<std::string> {
private:
	static to_string_visitor *instance;
	to_string_visitor() {};

public:
	static to_string_visitor& get_instance();
	std::string operator()(mapnik::value& value) const;
	std::string operator()(JsonObject& value) const;
	std::string operator()(JsonArray& value) const;
	std::string operator()(std::string& value) const;
};

#endif /* JSONOBJECT_H_ */
