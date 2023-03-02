project(kengine)

set(KENGINE_LOG_MAX_SEVERITY verbose CACHE STRING "Maximum allowed severity (see log_severity in log.hpp)")
target_compile_definitions(${kengine_library_name} ${link_type} KENGINE_LOG_MAX_SEVERITY=${KENGINE_LOG_MAX_SEVERITY})

register_types_from_headers(
        helpers/event.hpp
)