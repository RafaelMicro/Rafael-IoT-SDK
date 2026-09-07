# Post-build flash image size check.
#
# Run with `cmake -P`, never included. Reports how much flash the generated
# .bin occupies and warns when its end address passes CONFIG_IMAGE_END_ADDR_LIMIT.
# Deliberately never fails: the build stays green, the developer just gets told.
#
# Inputs (-D):
#   BIN_FILE        path to the .bin produced by objcopy
#   IMAGE_ORIGIN    ORIGIN of the flash region the image is linked into
#   REGION_NAME     that region's name, for the message only
#   REGION_END      ORIGIN + LENGTH of that region
#   IMAGE_END_LIMIT user limit; empty / n / 0 means "not configured"

# 1234567 -> "1205.6 KB". CMake math is integer only, so carry one decimal
# by hand: round to tenths of a KB, then split.
function(_fmt_kb bytes out)
  math(EXPR _tenths "(${bytes} * 10 + 512) / 1024")
  math(EXPR _int "${_tenths} / 10")
  math(EXPR _frac "${_tenths} % 10")
  set(${out} "${_int}.${_frac} KB" PARENT_SCOPE)
endfunction()

# Same trick for a percentage with one decimal.
function(_fmt_pct used total out)
  if(total LESS_EQUAL 0)
    set(${out} "?" PARENT_SCOPE)
    return()
  endif()
  math(EXPR _tenths "(${used} * 1000 + ${total} / 2) / ${total}")
  math(EXPR _int "${_tenths} / 10")
  math(EXPR _frac "${_tenths} % 10")
  set(${out} "${_int}.${_frac}%" PARENT_SCOPE)
endfunction()

function(_repeat ch n out)
  set(_s "")
  foreach(_i RANGE 1 ${n})
    set(_s "${_s}${ch}")
  endforeach()
  set(${out} "${_s}" PARENT_SCOPE)
endfunction()

# Draw an ASCII box around the report so it stands out in the build log.
#
# Always emitted with message(NOTICE): NOTICE prints verbatim, while
# message(WARNING) re-wraps and re-indents its text and would tear the frame
# apart. Callers that need warning severity emit a separate one-line WARNING.
#
# ASCII rather than Unicode box-drawing characters, because cmd.exe on a
# non-UTF-8 code page renders those as mojibake.
function(_print_box border title lines_var)
  set(_lines "${${lines_var}}")

  # Size the box to its widest line so long file names are never truncated.
  string(LENGTH "${title}" _w)
  foreach(_l IN LISTS _lines)
    string(LENGTH "${_l}" _n)
    if(_n GREATER _w)
      set(_w ${_n})
    endif()
  endforeach()
  if(_w LESS 60)
    set(_w 60)
  endif()

  math(EXPR _rule_len "${_w} + 2")
  _repeat("${border}" ${_rule_len} _rule)
  set(_out "+${_rule}+")

  set(_all "${title}" "${border}" ${_lines})
  foreach(_l IN LISTS _all)
    if(_l STREQUAL "${border}")
      set(_out "${_out}\n+${_rule}+")
    else()
      string(LENGTH "${_l}" _n)
      math(EXPR _pad_len "${_w} - ${_n}")
      set(_pad "")
      if(_pad_len GREATER 0)
        _repeat(" " ${_pad_len} _pad)
      endif()
      set(_out "${_out}\n| ${_l}${_pad} |")
    endif()
  endforeach()

  message(NOTICE "${_out}\n+${_rule}+")
endfunction()

# 269221632 -> "0x100D0000" (uppercase, padded to 8 digits so addresses line up)
function(_fmt_addr value out)
  math(EXPR _h "${value}" OUTPUT_FORMAT HEXADECIMAL)
  string(REGEX REPLACE "^0[xX]" "" _h "${_h}")
  string(TOUPPER "${_h}" _h)
  string(LENGTH "${_h}" _len)
  while(_len LESS 8)
    set(_h "0${_h}")
    math(EXPR _len "${_len} + 1")
  endwhile()
  set(${out} "0x${_h}" PARENT_SCOPE)
endfunction()

if(NOT BIN_FILE OR NOT EXISTS "${BIN_FILE}")
  return()
endif()

# Origin comes from the linker script; if parsing failed upstream there is
# nothing meaningful to report, so stay quiet rather than print garbage.
# Compare against "" and not truthiness: a valid origin can be 0 (the RT582
# bootloader is linked at 0x00000000), which CMake would treat as false.
if(IMAGE_ORIGIN STREQUAL "")
  return()
endif()

get_filename_component(_bin_name "${BIN_FILE}" NAME)
file(SIZE "${BIN_FILE}" _size)

math(EXPR _origin_dec "${IMAGE_ORIGIN}")
math(EXPR _end_dec "${_origin_dec} + ${_size}")

_fmt_kb("${_size}" _size_kb)
_fmt_addr("${_origin_dec}" _origin_str)
_fmt_addr("${_end_dec}" _end_str)

set(_region_end_dec "")
if(NOT REGION_END STREQUAL "")
  math(EXPR _region_end_dec "${REGION_END}")
  _fmt_addr("${_region_end_dec}" _region_end_str)
  math(EXPR _region_len "${_region_end_dec} - ${_origin_dec}")
  math(EXPR _region_free "${_region_end_dec} - ${_end_dec}")
  _fmt_pct("${_size}" "${_region_len}" _region_pct)
endif()

# CONFIG_IMAGE_END_ADDR_LIMIT is absent -> "", or "n" when Kconfig wrote
# "# ... is not set". Both mean the check is off.
set(_limit_dec "")
if(IMAGE_END_LIMIT AND NOT IMAGE_END_LIMIT STREQUAL "n")
  math(EXPR _limit_dec "${IMAGE_END_LIMIT}")
  if(_limit_dec LESS_EQUAL 0)
    set(_limit_dec "")
  endif()
endif()

# Print the last byte actually occupied, not just the exclusive end address.
# The limit is compared against the exclusive end, so showing only that made it
# ambiguous whether the image reaches the limit address or merely touches it.
if(_size GREATER 0)
  math(EXPR _last_dec "${_end_dec} - 1")
  _fmt_addr("${_last_dec}" _last_str)
  set(_range "${_origin_str} .. ${_last_str}  (ends at ${_end_str})")
else()
  set(_range "empty")
endif()

set(_head
  "image  : ${_bin_name}"
  "size   : ${_size} bytes (${_size_kb})"
  "range  : ${_range}")

if(_limit_dec STREQUAL "")
  # No limit configured. Still print a summary every build so it is obvious
  # the check ran -- silence would be indistinguishable from a broken hook.
  set(_body ${_head} "limit  : not set (CONFIG_IMAGE_END_ADDR_LIMIT)")
  if(NOT _region_end_dec STREQUAL "")
    list(APPEND _body "${REGION_NAME} : ${_size} / ${_region_len} bytes used (${_region_pct}), ${_region_free} bytes free")
  endif()
  _print_box("-" "IMAGE SIZE" _body)
  return()
endif()

_fmt_addr("${_limit_dec}" _limit_str)

# A limit past the end of the region can never fire -- almost certainly a typo.
set(_misconfig "")
if(NOT _region_end_dec STREQUAL "" AND _limit_dec GREATER _region_end_dec)
  # No ';' in box text: the lines travel as a CMake list, which would split on it.
  set(_misconfig "NOTE   : limit is past the end of ${REGION_NAME} (${_region_end_str}) and can never trigger")
endif()

if(_end_dec GREATER _limit_dec)
  math(EXPR _over "${_end_dec} - ${_limit_dec}")
  set(_body ${_head} "limit  : ${_limit_str}  (EXCEEDED by ${_over} bytes)")
  if(NOT _region_end_dec STREQUAL "")
    list(APPEND _body "${REGION_NAME} ends at ${_region_end_str} (${_region_free} bytes still free)")
  endif()
  if(NOT _misconfig STREQUAL "")
    list(APPEND _body "${_misconfig}")
  endif()
  _print_box("=" "IMAGE SIZE WARNING" _body)
  # The box above is NOTICE, which carries no severity. Emit a real warning too
  # so IDE problem matchers and CI log scanners still flag the build.
  message(WARNING "IMAGE SIZE: ${_bin_name} exceeds the configured limit by ${_over} bytes (see the box above).")
else()
  math(EXPR _headroom "${_limit_dec} - ${_end_dec}")
  math(EXPR _budget "${_limit_dec} - ${_origin_dec}")
  _fmt_pct("${_size}" "${_budget}" _used_pct)
  set(_body ${_head} "limit  : ${_limit_str}  (${_headroom} bytes headroom, ${_used_pct} used)")
  if(NOT _region_end_dec STREQUAL "")
    list(APPEND _body "${REGION_NAME} ends at ${_region_end_str}")
  endif()
  if(NOT _misconfig STREQUAL "")
    list(APPEND _body "${_misconfig}")
  endif()
  _print_box("-" "IMAGE SIZE OK" _body)
endif()
