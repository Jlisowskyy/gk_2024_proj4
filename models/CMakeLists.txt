message(STATUS "Downloading models...")

set(MODEL_OUT "${CMAKE_SOURCE_DIR}/models")
file(MAKE_DIRECTORY ${MODEL_OUT})

set(model_entries
        "https://learnopengl.com/data/models/backpack.zip"
        "backpack.zip"
        "backpack"

        "https://www.dropbox.com/scl/fi/go86pw2h7vpjjjykzfmrv/low_poly_light_bulb.glb?rlkey=hq4l2qot5kws9v2rsbgqxd547&st=lf858qq3&dl=0"
        "bulb.glb"
        "bulb"

        "https://www.dropbox.com/scl/fi/4dycyagrn0jjgdncig5su/ak47-counter-strike-2.zip?rlkey=w5r2hv94zwozpxwv6lajjua9l&st=9zh8wmhq&dl=0"
        "ak47.zip"
        "ak47"

        "https://www.dropbox.com/scl/fi/mtsc1xyuxepys5ozhgl9k/zsas-cs2-agent-model-blue.zip?rlkey=h0oc3gn9z3mk8ibpg4x4vgaei&st=xc1vm42j&dl=0"
        "at_model.zip"
        "at_model"

        "https://www.dropbox.com/scl/fi/mydpsbkn55rt396781gj3/professional__cs2_agent_model_miami.glb?rlkey=2qp4og3atuh3uiiwvd1a4yrzv&st=jw507sbr&dl=0"
        "tt_model.glb"
        "tt_model"

        "https://www.dropbox.com/scl/fi/p0oc2dcrg6j6pca3dxucs/cs_office_with_real_light.glb?rlkey=690ycsaqkql9aaj20g3m9oiyn&st=zn3tqyyj&dl=0"
        "office.glb"
        "office"

        "https://www.dropbox.com/scl/fi/zs5t9xiwz5h8gixlor8xo/sphere.fbx.glb?rlkey=n1xo7qek87bxody3askuh78aj&st=8kb590cs&dl=0"
        "sphere.glb"
        "sphere"

        "https://www.dropbox.com/scl/fi/xtt28kxheo1t0fevp26lz/sci-fi_modular_floor_tiles.glb?rlkey=zc4cfoy0lbg3862ij30wrgccu&st=9feey0yw&dl=0"
        "floor.glb"
        "floor"
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

    set(output_path "${MODEL_OUT}/${zip_filename}")
    set(extract_path "${MODEL_OUT}/${extract_dir}")

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