set(SOURCES src/log.c
            src/memguard.c
            src/cpu/nes6502/dis6502.c
            src/cpu/nes6502/nes6502.c
            src/machine/nsf.c
            src/sndhrdw/fds_snd.c
            src/sndhrdw/fmopl.c
            src/sndhrdw/mmc5_snd.c
            src/sndhrdw/nes_apu.c
            src/sndhrdw/vrc7_snd.c
            src/sndhrdw/vrcvisnd.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu/nes6502
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/machine
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/sndhrdw)

add_definitions(-DNSF_PLAYER -DNES6502_MEM_ACCESS_CTRL)

add_library(nosefart_tag SHARED ${SOURCES})
add_library(nosefart_0 SHARED ${SOURCES})
add_library(nosefart_1 SHARED ${SOURCES})
if(NOT WIN32)
  target_link_libraries(nosefart_tag m)
  target_link_libraries(nosefart_0 m)
  target_link_libraries(nosefart_1 m)
endif()
