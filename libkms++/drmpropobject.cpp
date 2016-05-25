#include <string.h>
#include <iostream>
#include <stdexcept>

#include <xf86drm.h>
#include <xf86drmMode.h>

#include "kms++.h"

using namespace std;

namespace kms
{

DrmPropObject::DrmPropObject(Card& card, uint32_t object_type)
	: DrmObject(card, object_type)
{
}

DrmPropObject::DrmPropObject(Card& card, uint32_t id, uint32_t object_type, uint32_t idx)
	: DrmObject(card, id, object_type, idx)
{
	refresh_props();
}

DrmPropObject::~DrmPropObject()
{

}

void DrmPropObject::refresh_props()
{
	auto props = drmModeObjectGetProperties(card().fd(), this->id(), this->object_type());

	if (props == nullptr)
		return;

	for (unsigned i = 0; i < props->count_props; ++i) {
		uint32_t prop_id = props->props[i];
		uint64_t prop_value = props->prop_values[i];

		m_prop_values[prop_id] = prop_value;
	}

	drmModeFreeObjectProperties(props);
}

uint64_t DrmPropObject::get_prop_value(uint32_t id) const
{
	return m_prop_values.at(id);
}

uint64_t DrmPropObject::get_prop_value(const string& name) const
{
	for (auto pair : m_prop_values) {
		auto prop = card().get_prop(pair.first);
		if (name == prop->name())
			return m_prop_values.at(prop->id());
	}

	throw invalid_argument("property not found: " + name);
}

int DrmPropObject::set_prop_value(uint32_t id, uint64_t value)
{
	return drmModeObjectSetProperty(card().fd(), this->id(), this->object_type(), id, value);
}

int DrmPropObject::set_prop_value(const string &name, uint64_t value)
{
	Property* prop = card().get_prop(name);

	if (prop == nullptr)
		throw invalid_argument("property not found: " + name);

	return set_prop_value(prop->id(), value);
}

}
