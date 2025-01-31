message(STATUS "Downloading models...")

set(MODEL_OOT "${CMAKE_BINARY_DIR}/models")
file(MAKE_DIRECTORY ${MODEL_OOT})

set(model_links
        "https://learnopengl.com/data/models/backpack.zip"
)

foreach(link ${model_links})
    get_filename_component(file_name ${link} NAME)
    get_filename_component(base_name ${file_name} NAME_WE)
    set(output_path "${MODEL_OOT}/${file_name}")
    set(extract_path "${MODEL_OOT}/${base_name}")

    if(NOT EXISTS ${extract_path})
        message(STATUS "Downloading model: ${file_name}")

        file(DOWNLOAD ${link} ${output_path}
                STATUS download_status
                SHOW_PROGRESS)

        list(GET download_status 0 download_result)
        if(NOT download_result EQUAL 0)
            message(FATAL_ERROR "Failed to download ${link}")
        endif()

        file(ARCHIVE_EXTRACT INPUT "${output_path}" DESTINATION "${extract_path}")
    else()
        message(STATUS "Model already exists: ${base_name}, skipping download.")
    endif()
endforeach()
