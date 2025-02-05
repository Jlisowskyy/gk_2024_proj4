#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>

LIBGCP_DECL_START_
int RenderEngineMain(
    const setting_t &settings, const resource_t &resources, const dynamic_objects_t &dynamic_objects,
    const static_objects_t &static_objects, const std::string &default_shader_name
);

LIBGCP_DECL_END_

#endif  // MAIN_HPP_
