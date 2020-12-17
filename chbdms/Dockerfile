#
# docker build -t sv-artifactory.infinera.com/chm6/x86_64/chm6_board_ms:[version] --target=chm6_board_ms_x86 .
# docker build -t sv-artifactory.infinera.com/chm6/arm64/chm6_board_ms:[version] --target=chm6_board_ms_arm64 .
#

ARG chm6devenv=V3.0.0
ARG chm6runenv=V4.0.0
ARG chm6testenv=V2.0.0
ARG chm6proto=V5.0.0
ARG cli3pp=V0.1
ARG chm6common=V1.4.0
ARG chm6_ds_apps_adapter=V2.0.0
ARG epdm=v2.0.5
ARG milb=V4.0
ARG chm6_logging=V2.0.0
ARG signing_action=sign
ARG signingkit_version=3.0

FROM sv-artifactory.infinera.com/chm6_proto:${chm6proto} as chm6_proto
FROM sv-artifactory.infinera.com/3pp/cli:${cli3pp} as cli_3pp
FROM sv-artifactory.infinera.com/chm6/arm64/chm6_common:${chm6common} as chm6_common_arm64
FROM sv-artifactory.infinera.com/chm6/x86_64/chm6_common:${chm6common} as chm6_common_x86
FROM sv-artifactory.infinera.com/chm6/arm64/chm6_ds_apps_adapter:${chm6_ds_apps_adapter} as chm6_ds_apps_adapter_arm64
FROM sv-artifactory.infinera.com/chm6/x86_64/chm6_ds_apps_adapter:${chm6_ds_apps_adapter} as chm6_ds_apps_adapter_x86
FROM sv-artifactory.infinera.com/3pp/epdm:${epdm} as epdm
FROM sv-artifactory.infinera.com/3pp/milb:${milb} as milb
FROM sv-artifactory.infinera.com/chm6/arm64/chm6_logging:${chm6_logging} as chm6_logging_arm64
FROM sv-artifactory.infinera.com/chm6/x86_64/chm6_logging:${chm6_logging} as chm6_logging_x86

#------------------------------DEV IMAGE----------------------------------------#
FROM sv-artifactory.infinera.com/infnbase/x86_64/chm6devenv:${chm6devenv} as chm6_board_ms_dev

# Copy src from repo
RUN mkdir -p /home/src
COPY src/ /home/src

RUN mkdir -p /home/src/include

# Copy interface files to gen folder
RUN mkdir -p /home/src/protogen/cpp
WORKDIR /home/src/protogen/cpp
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/board/v3", \
 "./infinera/chm6/board/v3" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_config*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_fault*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_pm*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_program_state*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/board_init_state*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/dco_card*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/sys_constants*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/tom_presence_map*", \
 "./infinera/chm6/common/v2/" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/hal/board", \
 "./infinera/hal/board" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/hal/chm6", \
 "./infinera/hal/chm6" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/hal/common", \
 "./infinera/hal/common" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/wrappers/v1", \
 "./infinera/wrappers/v1" \
 ]

# Install telnet - temporary
WORKDIR /home/src
RUN dpkg -i telnet_0.17-41.2_amd64.deb

# Add cli
RUN mkdir -p /home/src/external-repos/cli/include
COPY --from=cli_3pp [ \
  "/home/infinera/cli/include", \
  "/home/src/external-repos/cli/include" \
  ]
COPY --from=cli_3pp [ \
  "/home/infinera/cli/CMakeLists.txt", \
  "/home/infinera/cli/cliConfig.cmake.in", \
  "/home/infinera/cli/cli.pc.in", \
  "/home/src/external-repos/cli/" \
  ]

# Add Gecko Includes
WORKDIR /home/src/include
RUN wget http://sv-artifactory.infinera.com/artifactory/sv-thanos-local/security_libs/gecko_interface/hw_chm6_V0.1.1/Gecko_Interface_includes.tar
RUN tar xf Gecko_Interface_includes.tar
RUN rm Gecko_Interface_includes.tar

#------------------------------Build aarch64 ----------------------------------------#
FROM chm6_board_ms_dev as chm6_board_ms_build_aarch64

# Add common modules
RUN mkdir -p /home/src/include
COPY --from=chm6_common_arm64 [ \
  "/home/src/build/aarch64/include", \
  "/home/src/include" \
  ]
COPY --from=chm6_common_arm64 [ \
  "/home/src/build/aarch64/lib", \
  "/usr/local/lib" \
  ]

# Copy redis adapter libs
COPY --from=chm6_ds_apps_adapter_arm64 [ \
  "/home/infinera/build/aarch64/include", \
  "/home/src/include" \
  ]
  
COPY --from=chm6_ds_apps_adapter_arm64 [ \
  "/home/infinera/build/aarch64/lib/", \
  "/usr/local/lib" \
  ]

# copy Gecko Lib  
WORKDIR /usr/local/lib
RUN wget http://sv-artifactory.infinera.com/artifactory/sv-thanos-local/security_libs/gecko_interface/hw_chm6_V0.1.1/libgeckointf.a
RUN wget http://sv-artifactory.infinera.com/artifactory/sv-thanos-local/security_libs/gecko_interface/hw_chm6_V0.1.1/libgecko_access.a

# Add BearBox SDK   
COPY --from=epdm [ \
  "/home/infinera/lib", \
  "/usr/local/lib/aarch64-linux-gnu/epdm" \
  ]

COPY --from=milb [ \
  "/home/infinera/lib", \
  "/usr/local/lib/aarch64-linux-gnu/milb" \
  ]

COPY --from=epdm [ \
  "/home/infinera/include/", \
  "/usr/local/include/aarch64-linux-gnu" \
  ]

COPY --from=milb [ \
  "/home/infinera/include/", \
  "/usr/local/include/aarch64-linux-gnu" \
  ]

# Add logging
COPY --from=chm6_logging_arm64 [ \
  "/home/infinera/include", \
  "/home/src/include" \
  ]

COPY --from=chm6_logging_arm64 [ \
  "/home/infinera/lib/aarch64/libInfnLogger.a", \
  "/usr/local/lib" \
  ]

# Building src
RUN mkdir -p /home/src/compile
WORKDIR /home/src/compile

#todo: can use this to debug build by running manually, comment out once done
#FROM chm6_board_ms_build_aarch64 as chm6_build_dbg_arm64

RUN cmake -DCMAKE_TOOLCHAIN_FILE="../Toolchain-arm.cmake" -DARCH:STRING=arm64 ..
# hadolint ignore=SC2046,SC2061,SC2035
RUN make -j$(nproc) 

WORKDIR /home/src/compile/daemon
RUN aarch64-linux-gnu-strip BoardInitMs -o BoardInitMs.stripped; \
    aarch64-linux-gnu-strip BoardMs -o BoardMs.stripped

#-----------------------------ARM64 Signing stage-----------------------------------------#
FROM signingkit:${signingkit_version} as chm6_board_ms_build_aarch64_signed

COPY --from=chm6_board_ms_build_aarch64 /home/src/compile/daemon/BoardInitMs.stripped /signing_mount
COPY --from=chm6_board_ms_build_aarch64 /home/src/compile/daemon/BoardMs.stripped /signing_mount
COPY --from=chm6_board_ms_build_aarch64 /home/src/compile/daemon/BoardInitMs /signing_mount
COPY --from=chm6_board_ms_build_aarch64 /home/src/compile/daemon/BoardMs /signing_mount
# hadolint ignore=SC2154,SC2086
RUN /sign.py --sign-filesystem -w /signing_mount -c /signConfig.txt --action=${signing_action} -a arm64

#-----------------------------ARM64 Signed RUNTIME IMAGE-----------------------------------
FROM sv-artifactory.infinera.com/infnbase/arm64/chm6runenv:${chm6runenv}-signed as chm6_board_ms_arm64_signed

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_aarch64_signed [ \
  "/signing_mount/BoardInitMs.stripped", \
  "/home/bin/BoardInitMs" \
  ]
COPY --from=chm6_board_ms_build_aarch64_signed [ \
  "/signing_mount/BoardMs.stripped", \
  "/home/bin/BoardMs" \
  ]

FROM sv-artifactory.infinera.com/infnbase/arm64/chm6runenv:${chm6runenv}-signed as chm6_board_ms_arm64_sym_signed

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_aarch64_signed [ \
  "/signing_mount/BoardInitMs", \
  "/signing_mount/BoardMs", \
  "/home/bin/" \
  ]
#-------------------------------ARM64 RUNTIME IMAGE-------------------------------
FROM sv-artifactory.infinera.com/infnbase/arm64/chm6runenv:${chm6runenv} as chm6_board_ms_arm64

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_aarch64 [ \
  "/home/src/compile/daemon/BoardInitMs.stripped", \
  "/home/bin/BoardInitMs" \
  ]
COPY --from=chm6_board_ms_build_aarch64 [ \
  "/home/src/compile/daemon/BoardMs.stripped", \
  "/home/bin/BoardMs" \
  ]

FROM sv-artifactory.infinera.com/infnbase/arm64/chm6runenv:${chm6runenv} as chm6_board_ms_arm64_sym

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_aarch64 [ \
  "/home/src/compile/daemon/BoardInitMs", \
  "/home/src/compile/daemon/BoardMs", \
  "/home/bin/" \
  ]

#ENTRYPOINT ["/home/bin/BoardMs"]

#------------------------------Build x86_64 ----------------------------------------#
FROM chm6_board_ms_dev as chm6_board_ms_build_x86

# Add common modules
RUN mkdir -p /home/src/include
COPY --from=chm6_common_x86 [ \
  "/home/src/build/x86_64/include", \
  "/home/src/include" \
  ]
COPY --from=chm6_common_x86 [ \
  "/home/src/build/x86_64/lib", \
  "/usr/local/lib" \
  ]

# Copy redis adapter libs
COPY --from=chm6_ds_apps_adapter_x86 [ \
  "/home/infinera/build/x86_64/include", \
  "/home/src/include" \
  ]
  
COPY --from=chm6_ds_apps_adapter_x86 [ \
  "/home/infinera/build/x86_64/lib/", \
  "/usr/local/lib" \
  ]
 
# Add BearBox SDK  
COPY --from=epdm [ \
  "/home/infinera/include/", \
  "/usr/local/include/x86_64-linux-gnu" \
  ]

COPY --from=milb [ \
  "/home/infinera/include/", \
  "/usr/local/include/x86_64-linux-gnu" \
  ]

# Add logging
COPY --from=chm6_logging_x86 [ \
  "/home/infinera/include", \
  "/home/src/include" \
  ]

COPY --from=chm6_logging_x86 [ \
  "/home/infinera/lib/x86_64/libInfnLogger.a", \
  "/usr/local/lib" \
  ]

RUN apt-get install -y --no-install-recommends libboost-log-dev:amd64=1.67.0.1

# Building src
RUN mkdir -p /home/src/compile
WORKDIR /home/src/compile

#todo: can use this to debug build by running manually, comment out once done
#FROM chm6_board_ms_build_x86 as chm6_build_dbg_x86

RUN cmake -DARCH:STRING=x86 ..
# hadolint ignore=SC2046,SC2061,SC2035
RUN make -j$(nproc) 

WORKDIR /home/src/compile/daemon
RUN x86_64-linux-gnu-strip BoardInitMs -o BoardInitMs.stripped; \
    x86_64-linux-gnu-strip BoardMs -o BoardMs.stripped

#-----------------------------x86 Signing stage-----------------------------------------#
FROM signingkit:${signingkit_version} as chm6_board_ms_build_x86_signed

COPY --from=chm6_board_ms_build_x86 /home/src/compile/daemon/BoardInitMs.stripped /signing_mount
COPY --from=chm6_board_ms_build_x86 /home/src/compile/daemon/BoardMs.stripped /signing_mount
COPY --from=chm6_board_ms_build_x86 /home/src/compile/daemon/BoardInitMs /signing_mount
COPY --from=chm6_board_ms_build_x86 /home/src/compile/daemon/BoardMs /signing_mount
# hadolint ignore=SC2154,SC2086
RUN /sign.py --sign-filesystem -w /signing_mount -c /signConfig.txt --action=${signing_action}

#------------------------------x86 Signed RUNTIME IMAGE------------------------------------
FROM sv-artifactory.infinera.com/infnbase/x86_64/chm6runenv:${chm6runenv}-signed as chm6_board_ms_x86_signed

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_x86_signed [ \
  "/signing_mount/BoardInitMs.stripped", \
  "/home/bin/BoardInitMs" \
  ]
COPY --from=chm6_board_ms_build_x86_signed [ \
  "/signing_mount/BoardMs.stripped", \
  "/home/bin/BoardMs" \
  ]

FROM sv-artifactory.infinera.com/infnbase/x86_64/chm6runenv:${chm6runenv}-signed as chm6_board_ms_x86_sym_signed

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_x86_signed [ \
  "/signing_mount/BoardMs", \
  "/signing_mount/BoardInitMs", \
  "/home/bin/" \
  ]

#ENTRYPOINT ["/home/bin/BoardMs"]

#-------------------------------x86 RUNTIME IMAGE-------------------------------
FROM sv-artifactory.infinera.com/infnbase/x86_64/chm6runenv:${chm6runenv} as chm6_board_ms_x86

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_x86 [ \
  "/home/src/compile/daemon/BoardInitMs.stripped", \
  "/home/bin/BoardInitMs" \
  ]
COPY --from=chm6_board_ms_build_x86 [ \
  "/home/src/compile/daemon/BoardMs.stripped", \
  "/home/bin/BoardMs" \
  ]

FROM sv-artifactory.infinera.com/infnbase/x86_64/chm6runenv:${chm6runenv} as chm6_board_ms_x86_sym

WORKDIR /home
RUN mkdir -p /home/bin
COPY --from=chm6_board_ms_build_x86 [ \
  "/home/src/compile/daemon/BoardMs", \
  "/home/src/compile/daemon/BoardInitMs", \
  "/home/bin/" \
  ]

#ENTRYPOINT ["/home/bin/BoardMs"]

#------------------------------TEST IMAGE------------------------------
FROM sv-artifactory.infinera.com/infnbase/x86_64/chm6testenv:${chm6testenv} as chm6_board_ms_test

ARG prj_root=/home
ARG cov_stream=chm6_board_ms
ARG cov_emails
ARG prSrc_branch
ARG pr_url
ARG cov_build_type

# Copy src from repo
RUN mkdir -p /home/src
COPY src/ /home/src

RUN mkdir -p /home/src/include

# Copy interface files to gen folder
RUN mkdir -p /home/src/protogen/cpp
WORKDIR /home/src/protogen/cpp
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/board/v3", \
 "./infinera/chm6/board/v3" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_config*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_fault*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_pm*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/base_program_state*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/board_init_state*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/dco_card*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/sys_constants*", \
 "/home/infinera/chm6_proto/gen/cpp/infinera/chm6/common/v2/tom_presence_map*", \
 "./infinera/chm6/common/v2/" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/hal/board", \
 "./infinera/hal/board" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/hal/chm6", \
 "./infinera/hal/chm6" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/hal/common", \
 "./infinera/hal/common" \
 ]
COPY --from=chm6_proto [ \
 "/home/infinera/chm6_proto/gen/cpp/infinera/wrappers/v1", \
 "./infinera/wrappers/v1" \
 ]

# Add cli
RUN mkdir -p /home/src/external-repos/cli/include
COPY --from=cli_3pp [ \
  "/home/infinera/cli/include", \
  "/home/src/external-repos/cli/include" \
  ]
COPY --from=cli_3pp [ \
  "/home/infinera/cli/CMakeLists.txt", \
  "/home/infinera/cli/cliConfig.cmake.in", \
  "/home/infinera/cli/cli.pc.in", \
  "/home/src/external-repos/cli/" \
  ]

# Add Gecko Includes
WORKDIR /home/src/include
RUN wget http://sv-artifactory.infinera.com/artifactory/sv-thanos-local/security_libs/gecko_interface/hw_chm6_V0.1.1/Gecko_Interface_includes.tar
RUN tar xf Gecko_Interface_includes.tar
RUN rm Gecko_Interface_includes.tar

# Add common modules
COPY --from=chm6_common_x86 [ \
  "/home/src/build/x86_64/include", \
  "/home/src/include" \
  ]
COPY --from=chm6_common_x86 [ \
  "/home/src/build/x86_64/lib", \
  "/usr/local/lib" \
  ]

# Copy redis adapter libs
COPY --from=chm6_ds_apps_adapter_x86 [ \
  "/home/infinera/build/x86_64/include", \
  "/home/src/include" \
  ]
  
COPY --from=chm6_ds_apps_adapter_x86 [ \
  "/home/infinera/build/x86_64/lib/", \
  "/usr/local/lib" \
  ]
 
# Add BearBox SDK  
COPY --from=epdm [ \
  "/home/infinera/include/", \
  "/usr/local/include/x86_64-linux-gnu" \
  ]

COPY --from=milb [ \
  "/home/infinera/include/", \
  "/usr/local/include/x86_64-linux-gnu" \
  ]

# Add logging
COPY --from=chm6_logging_x86 [ \
  "/home/infinera/include", \
  "/home/src/include" \
  ]

COPY --from=chm6_logging_x86 [ \
  "/home/infinera/lib/x86_64/libInfnLogger.a", \
  "/usr/local/lib" \
  ]

RUN apt-get install -y --no-install-recommends libboost-log-dev:amd64=1.67.0.1

# Building src
RUN mkdir -p /home/src/compile
WORKDIR /home/src/compile

#todo: can use this to debug build by running manually, comment out once done
#FROM chm6_board_ms_build_x86 as chm6_build_dbg_x86

RUN cmake -DARCH:STRING=x86 ..

RUN if [ "${cov_build_type}" = "desktop" ]; then \
        python3 /opt/Coverity/bin/runcov_chm6.py -e ${cov_emails} -b ${prSrc_branch} \
                                                 -p ${pr_url} -w ${prj_root} -s ${cov_stream}; \
    else \
        /opt/Coverity/bin/cov-build --dir ./coverity_results/ make VERBOSE=1 \
        && /opt/Coverity/bin/cov-analyze --dir ./coverity_results/ --all \
        && /opt/Coverity/bin/cov-commit-defects --dir ./coverity_results/ \
                                                --host coverity.infinera.com \
                                                --stream ${cov_stream} \
                                                --user chm6_admin \
                                                --password chm6_admin; \
    fi

    
