message(STATUS "Downloading models...")

set(MODEL_OOT "${CMAKE_BINARY_DIR}/models")
file(MAKE_DIRECTORY ${MODEL_OOT})

set(model_entries
        "https://learnopengl.com/data/models/backpack.zip"
        "backpack.zip"
        "backpack"

        "https://www.dropbox.com/scl/fi/orupih2mtai52o0ou7bjd/cs_office_csgo_real_light_version.glb?rlkey=nkes07bobyaa7imdc0mi9r4y3&st=sqnh254k&dl=0"
        "csgo.glb"
        "csgo"

        "https://www.dropbox.com/scl/fi/7m1rctjjvtsirbr85yslp/full-gameready-city-buildings.zip?rlkey=yiexa553tt043bovyum2e9lay&st=ej66p7gp&dl=0"
        "town_map.zip"
        "town_map"
)

set(archive_extensions zip tar "tar.gz" tgz "tar.bz2" tbz2)

list(LENGTH model_entries entry_count)
math(EXPR entry_iterations "${entry_count} / 3 - 1")

foreach(idx RANGE ${entry_iterations})
    math(EXPR base_idx "${idx} * 3")

    list(GET model_entries ${base_idx} url)
    math(EXPR filename_idx "${base_idx} + 1")
    list(GET model_entries ${filename_idx} zip_filename)
    math(EXPR extract_idx "${base_idx} + 2")
    list(GET model_entries ${extract_idx} extract_dir)

    message(STATUS "Processing entry ${idx}: ${url}")

    set(output_path "${MODEL_OOT}/${zip_filename}")
    set(extract_path "${MODEL_OOT}/${extract_dir}")

    if(NOT EXISTS ${output_path})
        message(STATUS "Downloading model: ${zip_filename} from ${url}")

        file(DOWNLOAD ${url} ${output_path}
                STATUS download_status
                SHOW_PROGRESS)

        list(GET download_status 0 download_result)
        if(NOT download_result EQUAL 0)
            message(FATAL_ERROR "Failed to download ${url}")
        endif()

        get_filename_component(extension ${zip_filename} EXT)
        string(REGEX REPLACE "^\\." "" extension ${extension})

        if("${extension}" IN_LIST archive_extensions)
            message(STATUS "Extracting archive: ${zip_filename} to ${extract_path}")
            file(ARCHIVE_EXTRACT INPUT "${output_path}" DESTINATION "${extract_path}")
        else()
            message(STATUS "Skipping extraction for non-archive file: ${zip_filename}")
        endif()
    else()
        message(STATUS "Model already exists: ${zip_filename}, skipping download.")
    endif()
endforeach()