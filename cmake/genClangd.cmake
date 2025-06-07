# genClangd.cmake

function(generate_clangd_file OUT_PATH INCLUDE_DIRS)
  # OUT_PATH: path to write .clangd file (string)
  # INCLUDE_DIRS: list of include directories (list)

  # Start writing .clangd file
  file(WRITE "${OUT_PATH}" "CompileFlags:\n")
  file(APPEND "${OUT_PATH}" "  Add: [\n")

  # Iterate over all include dirs and append them as -I options
  foreach(dir IN LISTS INCLUDE_DIRS)
    file(APPEND "${OUT_PATH}" "    \"-I${dir}\",\n")
  endforeach()

  # Remove trailing comma from last entry
  # Trick: rewrite last line without comma

  # Read file content back to fix trailing comma
  file(READ "${OUT_PATH}" _content)
  string(REGEX REPLACE ",\n$" "\n" _content_fixed "${_content}")

  # Write fixed content back
  file(WRITE "${OUT_PATH}" "${_content_fixed}")

  # Close the list and the YAML block
  file(APPEND "${OUT_PATH}" "  ]\n")
endfunction()

