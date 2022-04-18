if (SDRTOOLS_FM_RADIO)

  add_executable(fm-radio
    fm_radio.c
  )

  target_link_libraries(fm-radio PRIVATE
    ${RTLSDR_LIBRARIES}
  )

  target_include_directories(fm-radio PRIVATE
    ${RTLSDR_INCLUDE_DIRS}
  )
    
endif()
